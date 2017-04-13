#ifndef PARSER_H
#define PARSER_H

// The parser needs to access token attributes.
#include "token.h"
#include "keywordtoken.h"
#include "punctoken.h"
#include "reloptoken.h"
#include "addoptoken.h"
#include "muloptoken.h"
#include "idtoken.h"
#include "numtoken.h"
#include "eoftoken.h"
#include "scanner.h"
#include "symbol_table.h"
// Code generation
#include "register.h"
#include "register_allocator.h"
#include "emitter.h"
#include "operand.h"

// To print error messages.
#include <iostream>

using namespace std;

class Parser {
   public:
      Parser (Scanner *the_scanner);
      ~Parser();
  
      bool parse_program();

      // Return true if current token is EOF
      bool done_with_input();

  
   private:

      void advance();
      bool is_semicolon();
      bool is_identifier();

      bool parse_decl_list();
      bool parse_variable_decl_list();
      bool parse_variable_decl();
      bool parse_procedure_decl_list();
      bool parse_identifier_list();
      bool parse_identifier_list_prm();
      bool parse_standard_type(expr_type &standard_type_type);
      bool parse_block();
      bool parse_procedure_decl();
      bool parse_procedure_args();
      bool parse_formal_parm_list();
      bool parse_formal_parm_list_hat();
      bool parse_stmt_list();
      bool parse_stmt_list_prm();
      bool parse_stmt(); //
      bool parse_ad_hoc_as_pc_tail(expr_type &ad_hoc_as_pc_tail_type, Operand *&expression);
      bool parse_if_stmt(); // done
      bool parse_if_stmt_hat(); // done
      bool parse_while_stmt();
      bool parse_print_stmt();
      bool parse_expr_list();
      bool parse_actual_parm_list();
      bool parse_actual_parm_list_hat();
      bool parse_expr(expr_type &expr_type_result, Operand *&op);
      bool parse_expr_hat(expr_type &expr_hat_type, Operand *&op);
      bool parse_simple_expr(expr_type &simple_expr_type, Operand *&op);
      bool parse_simple_expr_prm(expr_type &simple_expr_prm_type, Operand *&op);
      bool parse_term(expr_type &term_type, Operand *&op); // done 
      bool parse_term_prm(expr_type &term_prm_type, Operand *&left_op); // done
      bool parse_factor(expr_type &factor0_type, Operand *&op); // done
      bool parse_sign(expr_type &sign_type);
  
      // The lexical analyzer
      Scanner *lex;
      // The current token the parser is looking at
      Token *word;
      
      /* Print out a parse error message:
	 
	 "Parse error: Expected *expected*, found *found*."

	 This method should delete the string after it has printed the
	 error message. */
      void parse_error (string *expected, Token *found);

      // Other helper functions that you may define

      /*********** Semantial Analysis stuff **********/
      /* See grammar 3.0 for an explanation of these. */
      string *current_env, *main_env, *procedure_name;
      int actual_parm_position, formal_parm_position;
      bool parsing_formal_parm_list;

      Symbol_Table stab;

            /************** IR stuff *******************/
      Register_Allocator *allocator;
      Emitter *e;

      /* These functions are for signalling semantic errors.  None of
       them return - they exit and terminate the compilation.

       Identifier has been define twice.
      */
      void multiply_defined_identifier (string *id) const;
                                                
      // Identifier is undeclared.
      void undeclared_identifier (string *id) const;

      // Type error:  a single type was expected.
      void type_error (const expr_type expected, 
                   const expr_type found) const;

      // Type error: one of two types was expected.
      void type_error (const expr_type expected1,
                  const expr_type expected2,
                  const expr_type found) const;
  
  
};

#endif
