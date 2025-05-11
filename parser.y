%{

#include "SemanticValue.h"

extern "C" {
int yylex();
void yyerror(const char *);
}

#define YYSTYPE scc::SemanticValue

%}

%debug

%token TK_INT
%token TK_CONST
%token TK_CHAR
%token TK_RETURN
%token TK_VOID
%token TK_STR
%token TK_INT_CONST
%token TK_IDENTIFIER

%%

root_symbol:
    translation_unit {
      std::cout << "Done parsing" << std::endl;
    }
  ;

translation_unit:
    external_declaration
  | translation_unit external_declaration
  ;

external_declaration: 
    function_definition
  | declaration
  ;

function_definition:
    declaration_specifiers declarator compound_statement
  ;

opt_declaration_list:
    declaration_list
  | ;

declaration_list:
    declaration
  | declaration_list declaration
  ;

declaration:
    declaration_specifiers opt_init_declarator_list ';'
  ;

declaration_specifiers:
    type_specifier opt_declaration_specifiers
  | type_qualifier opt_declaration_specifiers
  ;

type_qualifier:
    TK_CONST
  ;

opt_declaration_specifiers:
    declaration_specifiers
  | ;

type_specifier:
    TK_CHAR
  | TK_INT
  | TK_VOID
  ;

opt_init_declarator_list:
    init_declarator_list
  | ;

init_declarator_list:
    init_declarator_list ',' init_declarator
  | init_declarator
  ;

init_declarator:
    declarator
  | declarator '=' initializer
  ;

declarator:
    opt_pointer direct_declarator
  ;

opt_pointer:
    pointer
  | ;

pointer:
    '*' pointer
  | '*'
  ;

initializer:
  ;

direct_declarator:
    TK_IDENTIFIER
  | direct_declarator '(' parameter_type_list ')'
  | direct_declarator '(' ')'
  ;

parameter_type_list:
    parameter_list
  ;

parameter_list:
    parameter_declaration
  | parameter_list ',' parameter_declaration

parameter_declaration:
    declaration_specifiers declarator
  | declaration_specifiers opt_abstract_declarator
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
    statement
  | statement_list statement
  ;

statement:
    expression_statement
  | jump_statement
  ;

jump_statement:
    TK_RETURN opt_expression ';'
  ;

expression_statement:
    expression ';'
  ;

compound_statement:
    '{' opt_declaration_list opt_statement_list '}'
  ;

// End Statements

// Begin Expressions

opt_expression:
    expression
  | ;

expression:
    assignment_expression
  | expression ',' assignment_expression
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
    primary_expression
  | postfix_expression '(' opt_argument_expression_list ')'
  ;

opt_argument_expression_list:
    argument_expression_list
  | ;

argument_expression_list:
    assignment_expression
  | argument_expression_list ',' assignment_expression
  ;

primary_expression:
    TK_IDENTIFIER
  | TK_STR
  | constant
  ;

// End Expressions

constant:
    TK_INT_CONST
  ;


%%

#include "tokenizer.cpp"

int main(void) {
  if (getenv("DEBUG_PARSER")) {
    yydebug = 1;
  }
  return yyparse();
}
