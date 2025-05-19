%{

#include "SemanticValue.h"
#include "ParserHelper.h"

extern "C" {
int yylex();
void yyerror(const char *);
}

#define YYSTYPE scc::SemanticValue
using namespace scc;

%}

%debug

%token TK_INT
%token TK_FLOAT
%token TK_DOUBLE
%token TK_CONST
%token TK_VOLATILE
%token TK_UNSIGNED
%token TK_CHAR
%token TK_RETURN
%token TK_VOID
%token TK_STR
%token TK_FOR
%token TK_INT_CONST
%token TK_FLOAT_CONST
%token TK_CHAR_CONST
%token TK_IDENTIFIER
%token TK_ELLIPSIS
%token TK_LE
%token TK_INC
%token TK_PLUS_EQ
%token TK_IF
%token TK_ELSE

%%

root_symbol:
    translation_unit {
      std::cout << "Done parsing" << std::endl;
      if (getenv("PRINT_CST")) {
        std::cout << $1 << std::endl;
      }

      TranslationUnit& tu = $1.translation_unit;
      post_parse(tu);
    }
  ;

translation_unit:
    external_declaration {
      $$ = TranslationUnit($1.external_declaration);
    }
  | translation_unit external_declaration {
      $$ = $1.translation_unit.addItem($2.external_declaration); 
    }
  ;

external_declaration: 
    function_definition {
      $$ = ExternalDeclaration($1.function_definition);
    }
  | declaration {
      $$ = ExternalDeclaration($1.declaration);
    }
  ;

function_definition:
    declaration_specifiers declarator compound_statement {
      $$ = FunctionDefinition($1.declaration_specifiers,
        $2.declarator,
        $3.compound_statement);
    }
  ;

opt_declaration_list:
    declaration_list
  | { $$ = DeclarationList() };

declaration_list:
    declaration {
      $$ = DeclarationList($1.declaration);
    }
  | declaration_list declaration {
      $$ = $1.declaration_list.addItem($2.declaration);
    }
  ;

declaration:
    declaration_specifiers opt_init_declarator_list ';' {
      $$ = Declaration($1.declaration_specifiers, $2.init_declarator_list);
    }
  ;

declaration_specifiers:
    type_specifier opt_declaration_specifiers {
      $$ = $2.declaration_specifiers.addSpecifier($1.spec);
    }
  | type_qualifier opt_declaration_specifiers {
      $$ = $2.declaration_specifiers.addQualifier($1.qual);
    }
  ;

opt_type_qualifier_list:
    type_qualifier_list
  | { $$ = TypeQualifierList(); }
  ;

type_qualifier_list:
    type_qualifier { $$ = TypeQualifierList($1.qual); }
  | type_qualifier_list type_qualifier
    { $$ = $1.type_qualifier_list.addItem($2.qual); }
  ;

type_qualifier:
    TK_CONST {
      $$ = SemanticValue(TypeQualifier::CONST);
    }
  | TK_VOLATILE {
      $$ = SemanticValue(TypeQualifier::VOLATILE);
    }
  ;

opt_declaration_specifiers:
    declaration_specifiers
  | { $$ = SemanticValue(DeclarationSpecifiers()); }
  ;

type_specifier:
    TK_UNSIGNED
  | TK_CHAR { $$ = TypeSpecifier::CHAR; }
  | TK_INT { $$ = TypeSpecifier::INT; }
  | TK_FLOAT { $$ = TypeSpecifier::FLOAT; }
  | TK_DOUBLE { $$ = TypeSpecifier::DOUBLE; }
  | TK_VOID { $$ = TypeSpecifier::VOID; }
  ;

opt_init_declarator_list:
    init_declarator_list
  | ;

init_declarator_list:
    init_declarator_list ',' init_declarator {
      $$ = $1.init_declarator_list.addItem($3.init_declarator);
    }
  | init_declarator {
      $$ = InitDeclaratorList($1.init_declarator);
    }
  ;

init_declarator:
    declarator { $$ = InitDeclarator($1.declarator); }
  | declarator '=' initializer
    { $$ = InitDeclarator($1.declarator, $3.initializer); }
  ;

declarator:
    opt_pointer direct_declarator {
      $$ = Declarator($1.pointer, $2.direct_declarator);
    }
  ;

opt_pointer:
    pointer
  | ;

pointer:
    '*' opt_type_qualifier_list pointer {
      $$ = $2.pointer.addTypeQualifierList($2.type_qualifier_list);
    }
  | '*' opt_type_qualifier_list {
      $$ = Pointer($2.type_qualifier_list);
    }
  ;

initializer:
    assignment_expression {
      $$ = Initializer($1.assignment_expression);
    }
  ;

direct_declarator:
    TK_IDENTIFIER { $$ = DirectDeclarator($1.str); }
  | direct_declarator '[' opt_constant_expression ']' {
      assert($3.tag == SV_RELATIONAL_EXPRESSION);
      $$ = $1.direct_declarator.addConstantExpression($3.relational_expression);
    }
  | direct_declarator '(' parameter_type_list ')' {
      $$ = $1.direct_declarator.addDeclarationList($3.declaration_list);
    }
  | direct_declarator '(' ')' {
      $$ = $1.direct_declarator.addDeclarationList(DeclarationList());
    }
  ;

parameter_type_list:
    parameter_list
  | parameter_list ',' TK_ELLIPSIS {
      $1.declaration_list.hasEllipsis = true;
      $$ = $1;
    }
  ;

parameter_list:
    parameter_declaration {
      $$ = DeclarationList($1.declaration);
    }
  | parameter_list ',' parameter_declaration {
      $$ = $1.declaration_list.addItem($3.declaration);
    }

parameter_declaration:
    declaration_specifiers declarator {
      $$ = Declaration($1.declaration_specifiers, InitDeclaratorList(
        InitDeclarator($2.declarator)));
    }
  | declaration_specifiers opt_abstract_declarator {
      $$ = Declaration($1.declaration_specifiers, $2.init_declarator_list);
    }
  ;

opt_abstract_declarator:
    abstract_declarator
  | { $$ = InitDeclaratorList(); };

abstract_declarator:
    pointer {
      $$ = InitDeclaratorList(InitDeclarator(
        Declarator($1.pointer, DirectDeclarator())));
    }
  ;

// Begin Statements

opt_statement_list:
    statement_list
  | ;

statement_list:
    statement {
      $$ = StatementList($1.statement);
    }
  | statement_list statement {
      $$ = $1.statement_list.addItem($2.statement);
    }
  ;

statement:
    expression_statement {
      $$ = Statement($1.expression_statement);
    }
  | jump_statement {
      $$ = Statement($1.jump_statement);
    }
  | compound_statement {
      $$ = Statement($1.compound_statement);
    }
  | iteration_statement {
      $$ = Statement($1.iteration_statement);
    }
  | selection_statement {
      $$ = Statement($1.selective_statement);
    }
  ;

selection_statement:
    TK_IF '(' expression ')' statement TK_ELSE statement {
      $$ = SelectiveStatement(SelectiveStatement_IFELSE,
        $3.expression,
        $5.statement,
        $7.statement);
    }
  ;

iteration_statement:
    TK_FOR '(' opt_expression ';' opt_expression ';' opt_expression ')' statement {
      $$ = IterationStatement(IterationStatement_FOR, $3.expression, $5.expression, $7.expression, $9.statement);
    }
  ;

jump_statement:
    TK_RETURN opt_expression ';' {
      $$ = JumpStatement($2.expression);
    }
  ;

expression_statement:
    expression ';' { $$ = ExpressionStatement($1.expression); }
  ;

compound_statement:
    '{' opt_declaration_list opt_statement_list '}' {
      $$ = CompoundStatement($2.declaration_list, $3.statement_list);
    }
  ;

// End Statements

// Begin Expressions

opt_expression:
    expression
  | { $$ = Expression(); }
  ;

expression:
    assignment_expression {
      $$ = Expression($1.assignment_expression);
    }
  | expression ',' assignment_expression
  ;

assignment_expression:
    conditional_expression {
      assert($1.tag == SV_RELATIONAL_EXPRESSION);
      $$ = AssignmentExpression($1.relational_expression);
    }
  | unary_expression assignment_operator assignment_expression {
      $$ = $3.assignment_expression.addItem($1.unary_expression, $2.assignment_operator);
    }
  ;

assignment_operator:
    TK_PLUS_EQ { $$ = AssignmentOperator(AssignmentOperator::PLUS_EQ); }
  | '=' { $$ = AssignmentOperator(AssignmentOperator::EQ); }
  ;

opt_constant_expression:
    constant_expression
  | { assert(0); };

constant_expression:
    conditional_expression
  ;

conditional_expression:
    logical_or_expression
  ;

logical_or_expression:
    logical_and_expression
  ;

logical_and_expression:
    inclusive_or_expression
  ;

inclusive_or_expression:
    exclusive_or_expression
  ;

exclusive_or_expression:
    and_expression
  ;

and_expression:
    equality_expression
  ;

equality_expression:
    relational_expression;
  ;

relational_expression:
    shift_expression {
      assert($1.tag == SV_ADDITIVE_EXPRESSION);
      $$ = RelationalExpression($1.additive_expression);
    }
  | relational_expression TK_LE shift_expression {
      assert($3.tag == SV_ADDITIVE_EXPRESSION);
      $$ = $1.relational_expression.addItem(RelationalOp_LE, $3.additive_expression);
    }
  | relational_expression '<' shift_expression {
      assert($3.tag == SV_ADDITIVE_EXPRESSION);
      $$ = $1.relational_expression.addItem(RelationalOp_LT, $3.additive_expression);
    }
  | relational_expression '>' shift_expression {
      assert($3.tag == SV_ADDITIVE_EXPRESSION);
      $$ = $1.relational_expression.addItem(RelationalOp_GT, $3.additive_expression);
    }
  ;

shift_expression:
    additive_expression
  ;

additive_expression:
    multiplicative_expression {
      assert($1.tag == SV_UNARY_EXPRESSION);
      $$ = AdditiveExpression($1.unary_expression);
    }
  | additive_expression '+' multiplicative_expression {
      assert($3.tag == SV_UNARY_EXPRESSION);
      $$ = $1.additive_expression.addItem(AdditiveOp_ADD, $3.unary_expression);
    }
  | additive_expression '-' multiplicative_expression {
      assert($3.tag == SV_UNARY_EXPRESSION);
      $$ = $1.additive_expression.addItem(AdditiveOp_SUB, $3.unary_expression);
    }
  ;

multiplicative_expression:
    cast_expression
  ;

cast_expression:
    unary_expression
  ;

unary_expression:
    postfix_expression {
      $$ = UnaryExpression($1.postfix_expression);
    }
  | TK_INC unary_expression {
      $$ = UnaryExpression(UnaryExpression_PREINC, $2.unary_expression);
    }
  ;

postfix_expression:
    primary_expression { $$ = PostfixExpression($1.primary_expression); }
  | postfix_expression '(' opt_argument_expression_list ')' {
      $$ = $1.postfix_expression.addPostfix($3.argument_expression_list);
    }
  | postfix_expression '[' expression ']' {
      $$ = $1.postfix_expression.addPostfix($3.expression);
    }
  ;

opt_argument_expression_list:
    argument_expression_list
  | { $$ = ArgumentExpressionList(); }
  ;

argument_expression_list:
    assignment_expression {
      $$ = ArgumentExpressionList($1.assignment_expression);
    }
  | argument_expression_list ',' assignment_expression {
      $$ = $1.argument_expression_list.addItem($3.assignment_expression);
    }
  ;

primary_expression:
    TK_IDENTIFIER {
      $$ = PrimaryExpression($1.str, PrimaryExpression_IDENTIFIER);
    }
  | TK_STR {
      $$ = PrimaryExpression($1.str, PrimaryExpression_STR);
    }
  | constant {
      $$ = PrimaryExpression($1.constant);
    }
  // | '(' expression ')'
  ;

// End Expressions

constant:
    TK_INT_CONST { $$ = Constant($1.ival); }
  | TK_CHAR_CONST { $$ = Constant((char) $1.ival); }
  | TK_FLOAT_CONST { $$ = Constant($1.dval); }
  ;


%%

#include "tokenizer.cpp"

int main(void) {
  if (getenv("DEBUG_PARSER")) {
    yydebug = 1;
  }
  return yyparse();
}
