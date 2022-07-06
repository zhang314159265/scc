%{
#include <cassert>
#include <memory>
#include "ast/node.h"
#include "ast/stmt.h"
#include "ast/expr.h"
#include "emitter.h"

using namespace ast;

extern "C" {
int yylex();
void yyerror(const char*);
int yyparse();
}

enum {
  SV_NONE,
  SV_ASTNODE,
};

class SemanticValue {
 public:
  explicit SemanticValue() : tag_(SV_NONE) { }
  explicit SemanticValue(Node* astNode) : tag_(SV_ASTNODE), astNode_(astNode) { }

  SemanticValue& operator=(std::unique_ptr<Node>&& node) {
     std::unique_ptr<Node> copy = std::move(node);
     tag_ = SV_ASTNODE;
     astNode_ = copy.release();
     return *this;
  }

  Node*& astNode() {
    assert(tag_ == SV_ASTNODE);
    return astNode_;
  }

 private:
  int tag_;
  union {
    Node* astNode_;
  };
};

namespace {
// utilities used in semantic actions
template <typename ListType>
static SemanticValue createNodeList(SemanticValue onlyElem) {
  typename ListType::ElemType* elem = onlyElem.astNode()->to<typename ListType::ElemType>();
  auto nodeList = std::make_unique<ListType>();
  nodeList->addElem(elem);
  return SemanticValue(nodeList.release());
}

template <typename ListType>
static SemanticValue appendNodeList(SemanticValue origList, SemanticValue extraItem) {
  using ElemType = typename ListType::ElemType;
  auto listptr = origList.astNode()->to<ListType>();
  auto itemPtr = extraItem.astNode()->to<ElemType>();
  listptr->addElem(itemPtr);
  return SemanticValue(listptr);
}

static SemanticValue semanticValueForNullNode() {
  return SemanticValue((Node*) nullptr);
}

template <typename DstType, typename SrcType>
static SemanticValue wrapNode(SemanticValue nested) {
  SrcType* srcptr = nullptr;
  if (nested.astNode()) {
    srcptr = nested.astNode()->to<SrcType>();
  }
  auto dstNode = std::make_unique<DstType>(srcptr);
  return SemanticValue(dstNode.release());
}

static SemanticValue createBinOp(const std::string& opstr, SemanticValue lhs, SemanticValue rhs) {
  auto resNode = std::make_unique<BinOpExpr>(
    opstr,
    lhs.astNode()->to<ExprBase>(),
    rhs.astNode()->to<ExprBase>());
  return SemanticValue(resNode.release());
}
}

#define YYSTYPE SemanticValue

%}

%token INTEGER_CONSTANT

%%

root_symbol:
    translation_unit {
      Emitter emitter;
      $1.astNode()->dump();
      $1.astNode()->to<Stmt>()->emit(&emitter);
    }
  ;

translation_unit:
    external_declaration
  | translation_unit external_declaration
  ;

// TODO: support declaration
external_declaration:
    function_definition
  ;

// TODO support the full syntax of function definition
function_definition:
    compound_statement
  ;

// TODO support declarations
compound_statement:
    '{' opt_statement_list '}' {
      $$ = $2;
    }
  ;

opt_statement_list:
    statement_list
  | /* empty */
  ;

statement_list:
    statement {
      $$ = createNodeList<StmtList>($1);
    }
  | statement_list statement {
      $$ = appendNodeList<StmtList>($1, $2);
    }
  ;

statement:
    compound_statement
  | expression_statement
  ;

expression_statement:
    opt_expression ';' {
      $$ = wrapNode<ExprStmt, ExprBase>($1);
    }
  ;

opt_expression:
    expression
  | /* empty */ {
      $$ = semanticValueForNullNode();
    }
  ;

expression:
    assignment_expression {
      $$ = createNodeList<CommaExpr>($1);
    }
  | expression ',' assignment_expression {
      $$ = appendNodeList<CommaExpr>($1, $3);
    }
  ; 

assignment_expression:
    conditional_expression
  ;

conditional_expression:
    logical_or_expression
  ;

logical_or_expression:
    logical_and_expression
  ;

logical_and_expression:
    additive_expression
  ;

additive_expression:
    multiplicative_expression
  | additive_expression '+' multiplicative_expression {
      $$ = createBinOp("+", $1, $3);
    }
  | additive_expression '-' multiplicative_expression {
      $$ = createBinOp("-", $1, $3);
    }
  ;

multiplicative_expression:
    cast_expression
  | multiplicative_expression '*' cast_expression {
      $$ = createBinOp("*", $1, $3);
    }
  | multiplicative_expression '/' cast_expression {
      $$ = createBinOp("/", $1, $3);
    }
  | multiplicative_expression '%' cast_expression {
      $$ = createBinOp("%", $1, $3);
    }
  ;

cast_expression:
    unary_expression
  ;

unary_expression:
    INTEGER_CONSTANT
  ;

%%

#include "tokenizer.cpp"
