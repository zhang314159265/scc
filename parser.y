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
%token TK_CONST
%token TK_VOLATILE
%token TK_UNSIGNED
%token TK_CHAR
%token TK_RETURN
%token TK_VOID
%token TK_STR
%token TK_INT_CONST
%token TK_CHAR_CONST
%token TK_IDENTIFIER

%%

root_symbol:
    translation_unit {
      std::cout << "Done parsing" << std::endl;
      #if 0
      std::cout << $1 << std::endl;
      #endif

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
    { $$ = InitDeclarator($1.declarator); }
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
  ;

direct_declarator:
    TK_IDENTIFIER { $$ = DirectDeclarator($1.str); }
  | direct_declarator '(' parameter_type_list ')' {
      $$ = $1.direct_declarator.addDeclarationList($3.declaration_list);
    }
  | direct_declarator '(' ')' {
      $$ = $1.direct_declarator.addDeclarationList(DeclarationList());
    }
  ;

parameter_type_list:
    parameter_list
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
      $$ = Declaration($1.declaration_specifiers, InitDeclaratorList());
    }
  ;

opt_abstract_declarator:
    abstract_declarator
  | ;

abstract_declarator:
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
  | ;

expression:
    assignment_expression {
      $$ = Expression($1.assignment_expression);
    }
  | expression ',' assignment_expression
  ;

assignment_expression:
    conditional_expression {
      assert($1.tag == SV_POSTFIX_EXPRESSION);
      $$ = AssignmentExpression($1.postfix_expression);
    }
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
    shift_expression
  ;

shift_expression:
    additive_expression
  ;

additive_expression:
    multiplicative_expression
  ;

multiplicative_expression:
    cast_expression
  ;

cast_expression:
    unary_expression
  ;

unary_expression:
    postfix_expression
  ;

postfix_expression:
    primary_expression { $$ = PostfixExpression($1.primary_expression); }
  | postfix_expression '(' opt_argument_expression_list ')' {
      $$ = $1.postfix_expression.addPostfix($3.argument_expression_list);
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
  ;


%%

#include "tokenizer.cpp"

int main(void) {
  if (getenv("DEBUG_PARSER")) {
    yydebug = 1;
  }
  return yyparse();
}
