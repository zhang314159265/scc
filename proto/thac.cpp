#include "thac.h"
#include "util.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <fmt/format.h>
#include <unordered_map>

// one 3 address code with optional labels
class Instruction {
 public:
  explicit Instruction(const std::string& line, std::vector<std::string>&& labels)
    : line_(line), labels_(std::move(labels)) {
  }

  bool isJump() const {
    return line_.find("goto ") != std::string::npos;
  }

  bool isCondJump() const {
    return isJump() && line_.find("if") == 0;
  }

  bool isAbsJump() const {
    return isJump() && line_.find("  goto") == 0;
  }

  std::string destLabel() const {
    assert(isJump());
    std::string needle = "goto ";
    auto idx = line_.find(needle);
    assert(idx != std::string::npos);
    return line_.substr(idx + needle.size());
  }

  bool hasLabel() const {
    return labels_.size() > 0;
  }

  friend std::ostream& operator<<(std::ostream& os, const Instruction& instr) {
    if (instr.labels_.size() > 0) {
      for (auto& lab : instr.labels_) {
        os << lab;
      }
      os << std::endl;
    }
    os << instr.line_ << std::endl;
    return os;
  }
 private:
  std::string line_;
  std::vector<std::string> labels_; // at most 1 label in most of the cases

  friend class FlowGraph;
};

class BasicBlock {
 public:
  explicit BasicBlock(std::vector<Instruction>&& instrs) : instrs_(std::move(instrs)) {
    assert(instrs_.size() > 0);
  }

  std::string dotNodeName() {
    return fmt::format("Blk_{}", blockIdx_);
  }

  friend std::ostream& operator<<(std::ostream& os, const BasicBlock& bb) {
    os << "Block " << bb.blockIdx_ << std::endl;
    for (const auto& instr : bb.instrs_) {
      os << instr;
    }
    if (bb.nextBlocks_.size() > 0) {
      os << "Next blocks:";
      for (int i = 0; i < bb.nextBlocks_.size(); ++i) {
        os << " " << bb.nextBlocks_[i]->blockIdx_;
      }
      os << std::endl;
    }
    return os;
  }

  Instruction& firstInstr() {
    return instrs_.front();
  }

  Instruction& lastInstr() {
    return instrs_.back();
  }

  void addNext(BasicBlock* next) {
    nextBlocks_.push_back(next);
    // std::cout << "add an edge " << blockIdx_ << " -> " << next->blockIdx_ << std::endl;
  }
 private:
  std::vector<Instruction> instrs_;
  std::vector<BasicBlock*> nextBlocks_;
  int blockIdx_;

  friend class FlowGraph;
};

class FlowGraph {
 public:
  static FlowGraph fromLines(const std::vector<std::string> lines) {
    FlowGraph graph;
    std::vector<Instruction> instrs;
    /*
     * For each line, need distinguish a few cases
     * 1. label
     * 2, goto
     * 3, conditional goto
     * 4, others
     */
    std::vector<std::string> curlabels;
    for (const auto& line : lines) {
      if (line.back() == ':') { // label ends with ':'
        curlabels.push_back(line);
        continue;
      }
      Instruction curinstr(line, std::move(curlabels));

      if (curinstr.hasLabel()) {
        if (instrs.size() > 0) {
          // build a BasicBlock from instrs
          graph.addBlock(BasicBlock(std::move(instrs)));
        }
      }
      instrs.push_back(curinstr);
      if (curinstr.isJump()) {
        graph.addBlock(BasicBlock(std::move(instrs)));
      }
    }

    if (instrs.size() > 0) {
      graph.addBlock(BasicBlock(std::move(instrs)));
    }

    if (curlabels.size() > 0) {
      Instruction lastinstr("  nop", std::move(curlabels));
      graph.addBlock(BasicBlock({lastinstr}));
    }

    // add edges
    graph.setupEdges();
    return graph;
  }

  void setupEdges() {
    for (int i = 0; i < blocks_.size(); ++i) {
      auto& curBlock = blocks_[i];

      auto& lastInstr = curBlock->lastInstr(); 

      if (!lastInstr.isAbsJump() && i != blocks_.size() - 1) {
        // add edge to next
        curBlock->addNext(blocks_[i + 1].get());
      }
      if (lastInstr.isJump()) {
        // add edge to jump target
        auto label = lastInstr.destLabel();
        auto dest_block_idx = getDestBlockIdx(label);
        curBlock->addNext(blocks_[dest_block_idx].get());
      }
    }
  }

  friend std::ostream& operator<<(std::ostream& os, const FlowGraph& graph) {
    os << "The graph contains " << graph.blocks_.size() << " blocks" << std::endl;
    for (int bbidx = 0; bbidx < graph.blocks_.size(); ++bbidx) {
      auto& block = graph.blocks_[bbidx];

      os << *block << std::endl; 
    }
    return os;
  }

  void addBlock(BasicBlock bb) {
    bb.blockIdx_ = blocks_.size();
    blocks_.push_back(std::make_unique<BasicBlock>(bb));

    for (auto label : bb.firstInstr().labels_) {
      if (label.back() == ':') {
        label.pop_back();
      }
      labelToBlkIdx_[label] = blocks_.size() - 1;
    }
  }

  void exportToDot(const std::string& dotPath) {
    std::ofstream os(dotPath, std::ofstream::out);
    os << "digraph {" << std::endl;
    for (auto& curBlock : blocks_) {
      // attributes for block
      std::ostringstream ss;
      ss << *curBlock;

      std::string label = ss.str();
      // replace \n to \l to achieve left alignment
      // Check: https://marc.info/?l=graphviz-interest&m=115548621302054
      Util::replaceAll(label, "\n", "\\l");
      os << "  " << curBlock->dotNodeName() << " [label=\"" << label << "\" shape=box style=filled fillcolor=\"#CCCCCC\" color=black fontcolor=green]" << std::endl;
      
      // attributes for edges
      for (auto& nextBlock : curBlock->nextBlocks_) {
        os << "  " << curBlock->dotNodeName() << " -> " << nextBlock->dotNodeName() << std::endl; 
      }
    }

    os << "}" << std::endl;
    os.close();
  }
 private:
  int getDestBlockIdx(const std::string& label) const {
    auto itr = labelToBlkIdx_.find(label);
    assert(itr != labelToBlkIdx_.end());
    return itr->second;
  }

  std::vector<std::unique_ptr<BasicBlock>> blocks_;
  std::unordered_map<std::string, int> labelToBlkIdx_;
};

std::vector<std::string> readLines(const std::string& thacPath) {
  std::ifstream ifs(thacPath);
  std::vector<std::string> lines;
  std::string line;
  while (getline(ifs, line)) {
    lines.push_back(line);
  }
  return lines;
}

static void runCommand(const std::string& cmd) {
  int status = system(cmd.c_str());
  assert(status == 0);
}

static void convertDotToSvg(const std::string& dotPath, const std::string& svgPath) {
  runCommand(fmt::format("cat {} | dot -Tsvg > {}", dotPath, svgPath));
}

static void openSvg(const std::string& svgPath) {
  runCommand(fmt::format("open {}", svgPath));
}

void handleThac(const std::string& wsdir, const std::string& thacPath) {
  std::string dotPath = fmt::format("{}/flow_graph.gv", wsdir);
  std::cout << "Handle three address code in file " << thacPath << std::endl;
  std::vector<std::string> lines = readLines(thacPath);
  std::cout << "Read " << lines.size() << " lines of three address code." << std::endl;
  FlowGraph graph = FlowGraph::fromLines(lines);
  std::cout << graph << std::endl;

  std::cout << "Exporting the flow graph to a dot file " << dotPath << std::endl;
  graph.exportToDot(dotPath);

  std::string svgPath = fmt::format("{}/flow_graph.svg", wsdir);
  std::cout << "Convert dot to svg file: " << svgPath << std::endl;
  convertDotToSvg(dotPath, svgPath);
  std::cout << "Open the svg file..." << std::endl;
  openSvg(svgPath);
}
