#include "thac.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

// one 3 address code with optional labels
class Instruction {
 public:
  explicit Instruction(const std::string& line, std::vector<std::string>&& labels)
    : line_(line), labels_(std::move(labels)) {
  }

  bool isJump() const {
    return line_.find("goto ") != std::string::npos;
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
};

class BasicBlock {
 public:
  explicit BasicBlock(std::vector<Instruction>&& instrs) : instrs_(std::move(instrs)) { }

  friend std::ostream& operator<<(std::ostream& os, const BasicBlock& bb) {
    for (const auto& instr : bb.instrs_) {
      os << instr;
    }
    return os;
  }
 private:
  std::vector<Instruction> instrs_;
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
    return graph;
  }

  friend std::ostream& operator<<(std::ostream& os, const FlowGraph& graph) {
    os << "The graph contains " << graph.blocks_.size() << " blocks" << std::endl;
    for (int bbidx = 0; bbidx < graph.blocks_.size(); ++bbidx) {
      auto& block = graph.blocks_[bbidx];
      os << "BLOCK " << bbidx << std::endl;

      os << block << std::endl; 
    }
    return os;
  }

  void addBlock(BasicBlock bb) {
    blocks_.push_back(bb);
  }
 private:
  std::vector<BasicBlock> blocks_;
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

void handleThac(const std::string& thacPath) {
  std::cout << "Handle three address code in file " << thacPath << std::endl;
  std::vector<std::string> lines = readLines(thacPath);
  std::cout << "Read " << lines.size() << " lines of three address code." << std::endl;
  FlowGraph graph = FlowGraph::fromLines(lines);
  std::cout << graph << std::endl;
}
