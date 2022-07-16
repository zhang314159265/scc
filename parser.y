%{
#include <cassert>
#include <memory>
#include <string>
#include "ast/node.h"
#include "ast/stmt.h"
#include "ast/expr.h"
#include "emitter.h"
#include "label.h"
#include "type.h"
#include "symtab.h"

using namespace ast;

extern "C" {
int yylex();
void yyerror(const char*);
int yyparse();
}

enum {
  SV_NONE,
  SV_ASTNODE,
  SV_STR,
  SV_TYPE,
};

class SemanticValue {
 public:
  explicit SemanticValue() : tag_(SV_NONE) { }
  explicit SemanticValue(Node* astNode) : tag_(SV_ASTNODE), astNode_(astNode) { }
  explicit SemanticValue(const std::string& str) : tag_(SV_STR), str_(str) { }
  explicit SemanticValue(Type* type) : tag_(SV_TYPE), type_(type) { }

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
  const std::string& str() const {
    assert(tag_ == SV_STR);
    return str_;
  }

  Type* type() const {
    assert(tag_ == SV_TYPE);
    return type_;
  }

 private:
  int tag_;
  union {
    Node* astNode_;
    Type* type_;
  };
  std::string str_; // can not put std::string inside union. So put it outside..
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

static SemanticValue createWhileNode(SemanticValue sv_expr, SemanticValue sv_stmt) {
  auto while_node = new While(
    sv_expr.astNode()->to<ExprBase>(),
    sv_stmt.astNode()->to<Stmt>());
  return SemanticValue(while_node);
}

static SemanticValue createForNode(SemanticValue init_expr, SemanticValue cond_expr, SemanticValue post_expr, SemanticValue stmt) {
  auto init = Node::to<ExprBase>(init_expr.astNode());
  auto cond = Node::to<ExprBase>(cond_expr.astNode());
  auto post = Node::to<ExprBase>(post_expr.astNode());
  auto forNode = new For(init, cond, post, stmt.astNode()->to<Stmt>());
  return SemanticValue(forNode);
}

static SemanticValue createAssign(SemanticValue lhs, SemanticValue rhs) {
  auto assign_expr = new AssignExpr(
    lhs.astNode()->to<ExprBase>(),
    rhs.astNode()->to<ExprBase>()
  );
  return SemanticValue(assign_expr);
}

static SemanticValue createIncDecNode(SemanticValue child, bool is_inc, bool is_pre) {
  auto inc_dec = new IncDec(
    child.astNode()->to<ExprBase>(),
    is_inc,
    is_pre);
  return SemanticValue(inc_dec);
}

static SemanticValue createArrayAccessNode(SemanticValue array, SemanticValue index) {
  return SemanticValue(new ArrayAccess(
    array.astNode()->to<ExprBase>(),
    index.astNode()->to<ExprBase>()
  ));
}

}

#define YYSTYPE SemanticValue

%}

%token INTEGER_CONSTANT
%token FOR
%token WHILE
// can not create token named ID since the generated parser will contains a line like
// '#define ID 260'
// While the fmtlib has something like:
//   template <typename Context, typename ID>
// The ID parameter name collides with the ID macro and cause weird compiling errors.
%token IDENTIFIER
%token INC
%token DEC

// types
%token TK_INT
%token TK_DOUBLE

%%

root_symbol:
    translation_unit {
      Emitter emitter;
      Label next;
      $1.astNode()->dump();
      $1.astNode()->to<Stmt>()->emit(&emitter, &next);
      next.emit_if_used(&emitter);
      Symtab::cur()->dump();
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
    '{' opt_declaration_list opt_statement_list '}' {
      $$ = $3;
    }
  ;

opt_declaration_list:
    declaration_list
  | /* empty */
  ;

declaration_list:
    declaration
  | declaration_list declaration
  ;

// TODO simpifiled for now
declaration:
    type_specifier IDENTIFIER ';' {
      Symtab::cur()->reg($2.str(), $1.type());
    }
  ;

type_specifier:
    TK_INT {
      $$ = SemanticValue(&Type::INT);
    }
  | TK_DOUBLE {
      $$ = SemanticValue(&Type::DOUBLE);
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
    expression_statement
  | compound_statement
  | iteration_statement
  ;

iteration_statement:
    WHILE '(' expression ')' statement {
      $$ = createWhileNode($3, $5);
    }
  | FOR '(' opt_expression ';' opt_expression ';' opt_expression ')' statement {
      $$ = createForNode($3, $5, $7, $9);
    }
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

// TODO
assignment_expression:
    conditional_expression
  | unary_expression '=' assignment_expression {
      $$ = createAssign($1, $3);
    }
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
    postfix_expression
  | INC unary_expression {
    $$ = createIncDecNode($2, true, true);
  }
  | DEC unary_expression {
    $$ = createIncDecNode($2, false, true);
  }
  ;

postfix_expression:
    primary_expression
  | postfix_expression '[' expression ']' {
      $$ = createArrayAccessNode($1, $3);
    }
  | postfix_expression INC {
      $$ = createIncDecNode($1, true, false);
    }
  | postfix_expression DEC {
      $$ = createIncDecNode($1, false, false);
    }
  ;

primary_expression:
    INTEGER_CONSTANT
  | IDENTIFIER {
      $$ = SemanticValue(new Id($1.str()));
    }
  ;

%%

#include "tokenizer.cpp"
