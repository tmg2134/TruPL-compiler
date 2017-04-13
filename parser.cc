#include "parser.h"

Parser::Parser(Scanner *the_scanner) 
{
  /* Initialize the parser. */
  lex = the_scanner;
  word = lex->next_token();

  // Semantic analysis initializations
  current_env = main_env = procedure_name = NULL;
  actual_parm_position = formal_parm_position = -1;
  parsing_formal_parm_list = false;

  // IR initializations
  e = new Emitter();
  allocator = new Register_Allocator();
}

Parser::~Parser() 
{
  /* Delete the parser. */
  if (lex != NULL) {
    delete lex;
  }
  if (word != NULL) {
    delete word;
  }
}

bool Parser::is_semicolon()
{
  if (word->get_token_type() == TOKEN_PUNC
        && static_cast<PuncToken *>(word)->get_attribute() == PUNC_SEMI) {
    return true;
  }
  return false;
}

bool Parser::is_identifier()
{
  if (word->get_token_type() == TOKEN_ID) {
    return true;
  }
  return false;
}

void Parser::advance()
{
  //cout << *(word->to_string())<< endl;
  //delete word;
  word = lex->next_token();
}

void Parser::parse_error(string *expected, Token *found)
{
  cerr << "Expected: " << *expected << " Found: "<< *(found->to_string()) << endl;
  delete expected;
}

// If we have parsed the entire program, then word
// should be the EOF Token.  This function tests
// that condition.
bool Parser::done_with_input()
{
  return word->get_token_type() == TOKEN_EOF;
}

void Parser::multiply_defined_identifier (string *id) const
{
  cerr << "The identifier " << *id << " has already been declared. " << endl;
  exit (-1);
}


void Parser::undeclared_identifier (string *id) const
{
  cerr << "The identifier " << *id << " has not been declared. " << endl;
  exit (-1);
}


void Parser::type_error(const expr_type expected,
       const expr_type found) const
{
  cerr << "Type error: expected " << *(stab.type_to_string (expected))
       << " found " << *(stab.type_to_string(found)) << "." << endl;
  exit (-1);
}

void Parser::type_error(const expr_type expected1,
       const expr_type expected2,
       const expr_type found) const
{
  cerr << "Type error: expected " << *(stab.type_to_string (expected1))
       << " or " << *(stab.type_to_string (expected2))
       << ", found " << *(stab.type_to_string(found)) << "." << endl;
  exit (-1);
}

bool Parser::parse_program()
{
   // PROGRAM -> program identifier ; DECL_LIST BLOCK ;
   // Predict (program identifier ; DECL_LIST BLOCK ;) == {program}

   // Match keyword program, first symbol on RHS
  if (word->get_token_type() == TOKEN_KEYWORD 
       && static_cast<KeywordToken *>(word)->get_attribute() == KW_PROGRAM) {

     /* ADVANCE - Notice that we only delete a token on an ADVANCE,
       and, if we ADVANCE, it is the ADVANCE code that is responsible
       for getting the next token.
     */
     advance();
     // Match identifier, 2nd symbol on RHS
    if (word->get_token_type() == TOKEN_ID) {    

      string *id_name = static_cast<IdToken *>(word)->get_attribute();
      string *global_env_name = new string ("_EXTERNAL");
      stab.install (id_name, global_env_name, PROGRAM_T);
      current_env = new string (*id_name);
      main_env = new string (*id_name);
      //delete global_env_name;
      ////delete id_name;

      e->emit_label (id_name);


      advance();

  // Match semicolon(;), 3rd symbol on RHS
  if (word->get_token_type() == TOKEN_PUNC
      && static_cast<PuncToken *>(word)->get_attribute() == PUNC_SEMI) {
    // ADVANCE

    advance();

    /* Match DECL_LIST, 4th bymbol on RHS.  This is an ACTION,
       not an advance, so we don't grab another token if we
       succeed.  We are just growing the parse tree at this
       point.
    */
    if (parse_decl_list()) {
      
      // Match BLOCK, 5th on RHS - ACTION
      if (parse_block()) {
        
        // Match semicolon(;), 6th and last on RHS -  
        if (word->get_token_type() == TOKEN_PUNC
      && static_cast<PuncToken *>(word)->get_attribute() == PUNC_SEMI) {
    // ADVANCE
    // Since we advanced, we matched a token so we get the next one.
    advance();

    // Parse_program succeeded.
    if(!done_with_input()){
      cerr << "The parse succeeded, but there was extra input at the end" << endl;
    }
    e->emit_halt();
    // Data directives
    stab.emit_data_directives();
    return true;
    
    // We failed to match the second semicolon
        } else {
    // Variable expected should be deleted in parse_error()
    string *expected = new string (";");
    parse_error (expected, word);
    return false;
        }
        
        /* We failed to parse BLOCK.  Don't print an error here;
     instead, print the error when you discover it,
     i.e. when you are trying to advance. */
      } else {
        return false;
      }
      
      // We failed to parse DECL_LIST
    } else {
      return false;
    }
    
    // We failed to match the first semicolon
  } else {
    string *expected = new string (";");
    parse_error (expected,word);
    return false;
  }
  
  // We failed to match an identifier
     } else {
      string *expected = new string ("identifier");
       parse_error (expected,word);
       return false;
     }
     
     // We failed to match the keyword program
   } else {
    string *expected = new string ("keyword program");
     parse_error (expected, word);
     return false;
   }
   
   /* We shouldn't reach this statement, but it is here as a defensive
      programming measure. */
   return false;
}

bool Parser::parse_decl_list()
{
   /* DECL_LIST -> VARIABLE_DECL_LIST PROCEDURE_DECL_LIST 

      Predict(VARIABLE_DECL_LIST PROCEDURE_DECL_LIST) 
        = First(VARIABLE_DECL_LIST) 
       union First (PROCEDURE_DECL_LIST) 
       union Follow (DECL_LIST) = {identifier, procedure, begin}

       Note that we don't actually need to check the predict set
       here.  The predict set is used to choose the correct
       production, but there isn't a choice here.

       In addition, we take advantage of C++'s short circuit
       evaluation of Boolean expressions. */
  return parse_variable_decl_list() && parse_procedure_decl_list();
}

bool Parser::parse_variable_decl_list()
{
  // Need predict for variable_decl
  if (is_identifier()) {
    if (parse_variable_decl()) {
      // Match semicolon(;) 
      if (is_semicolon()) {
        advance();
        //variable_decl_list, recurse
        return(parse_variable_decl_list());
      // No Semicolon xD
      }else{
        string *expected = new string ("';'");
        parse_error (expected, word);
        return false;
      }
    // Couldn't parse variable_decl
    }else{
      return false;
    }
  // Lambda case
  }else{
    return true;
  }
  // Defense
  return false;
}

bool Parser::parse_variable_decl()
{
  expr_type standard_type_type = GARBAGE_T;
  // Parse ID_LIST : STANDARD_TYPE
  if(parse_identifier_list()){
    //No advnacerino
    // Match colon(:)  
    if (word->get_token_type() == TOKEN_PUNC
        && static_cast<PuncToken *>(word)->get_attribute() == PUNC_COLON) {
      advance();
      if(parse_standard_type(standard_type_type)){
        // foreach (identifier i in stab such that i.type == unknown t)
        //   stab.update type (i, STANDARD TYPE.type);
        stab.update_type(standard_type_type);
        return true;
      }else{
        return false;
      }
    // No colon LUL
    }else{
      string *expected = new string ("';'");
      parse_error (expected, word);
      return false;
    }
  }else{
    return false;
  }
  // Defense
  return false;
}

bool Parser::parse_procedure_decl_list()
{
  // PROCEDURE_DECL ; PROCEDURE_DECL_LIST | lambda
  //Need predict of procedure_decl
  if(word->get_token_type() == TOKEN_KEYWORD 
       && static_cast<KeywordToken *>(word)->get_attribute() == KW_PROCEDURE) {
    if(parse_procedure_decl()){
      if(is_semicolon()){
        advance();
        return(parse_procedure_decl_list());
      //No semi
      }else{
        string *expected = new string ("';'");
        parse_error (expected, word);
        return false;
      }
    // procedure decl failed
    }else{
      return false;
    }
  //lambda case
  }else{
    return true;
  }
  // Defense
  return false;
}

bool Parser::parse_identifier_list()
{
  // identifier IDENTIFIER_LIST_PRM
  if(is_identifier()){
      // {if (is declared (identifier.attr, current env))
      // multiple definition error();
      // else
      // stab.install (identifier.attr, current env, unknown t}
      string *id_name = static_cast<IdToken *>(word)->get_attribute();
      if( stab.is_decl(id_name, current_env)){
        multiply_defined_identifier(id_name);
      }else{
        stab.install(id_name, current_env, UNKNOWN_T);
      }
      //delete id_name;
    advance();
    if(parse_identifier_list_prm()){
      return true;
    }else{
      return false;
    }
  // Not an id list 
  }else{
    string *expected = new string ("identifier");
    parse_error (expected, word);
    return false;
  }
  // Defense
  return false;
}

bool Parser::parse_identifier_list_prm()
{
  // , identifier IDENTIFIER_LIST_PRM | lambda
  if(word->get_token_type() == TOKEN_PUNC
        && static_cast<PuncToken *>(word)->get_attribute() == PUNC_COMMA){
    advance();
    if(is_identifier()){
      /*{if (is declared (identifier.attr, current env))
        multiple definition error();
        else
        if (parsing formal parm list)
        stab.install(identifier.attr, current env, unknown t, formal parm position);
        formal parm position++;
        else
        stab.install (identifier.attr, current env, unknown t);}
      */
      string *id_name = static_cast<IdToken *>(word)->get_attribute();
      if( stab.is_decl(id_name, current_env)){
        multiply_defined_identifier(id_name);
      }else{
        if(parsing_formal_parm_list){
          stab.install(id_name, current_env, UNKNOWN_T, formal_parm_position);
          formal_parm_position++;
        }else{
          stab.install(id_name, current_env, UNKNOWN_T);
        }
      }
      //delete id_name;
      advance();
      return(parse_identifier_list_prm());
    // Identifier Kappa
    }else{
      string *expected = new string ("identifier");
      parse_error (expected, word);
      return false;
    }
  // lambda case
  }else{
    return true;
  }
  // Defense
  return false;
}

bool Parser::parse_standard_type(expr_type &standard_type_type)
{
  // int | bool
  if((word->get_token_type() == TOKEN_KEYWORD
        && static_cast<KeywordToken *>(word)->get_attribute() == KW_INT)){
    standard_type_type = INT_T;
    advance();
    return true;
  }else if((word->get_token_type() == TOKEN_KEYWORD
        && static_cast<KeywordToken *>(word)->get_attribute() == KW_BOOL)){
    standard_type_type = BOOL_T;
    advance();
    return true;
  }else{
    string *expected = new string ("int or bool");
    parse_error (expected, word);
    return false;
  }
  // Defense
  return false;
}

bool Parser::parse_block()
{
  // begin STMT_LIST end
  if(word->get_token_type() == TOKEN_KEYWORD
        && static_cast<KeywordToken *>(word)->get_attribute() == KW_BEGIN){
    advance();
    // action
    if(parse_stmt_list()){
      if(word->get_token_type() == TOKEN_KEYWORD
        && static_cast<KeywordToken *>(word)->get_attribute() == KW_END){
        advance();
        return true;
      // end 4Head
      }else{
        string *expected = new string ("end");
        parse_error (expected, word);
        return false;
      }
    // parse_stmt_list
    }else{
      return false;
    }
  // begin FeelsBadMan
  }else{
    string *expected = new string ("begin");
    parse_error (expected, word);
    return false;
  }
  // Defense
  return false;
}

bool Parser::parse_procedure_decl()
{
  // procedure identifier ( PROCEDURE_ARGS ) VARIABLE_DECL_LIST BLOCK
  if(word->get_token_type() == TOKEN_KEYWORD 
       && static_cast<KeywordToken *>(word)->get_attribute() == KW_PROCEDURE){
    advance();
    if(is_identifier()){
      string *id_name = static_cast<IdToken *>(word)->get_attribute();
      if(stab.is_decl(id_name, current_env)){
        multiply_defined_identifier(id_name);
      }else{
        stab.install(id_name, current_env, PROCEDURE_T);
        current_env = new string(*id_name);
        formal_parm_position = 0;
        //delete id_name;
      }
      advance();
      if(word->get_token_type() == TOKEN_PUNC
        && static_cast<PuncToken *>(word)->get_attribute() == PUNC_OPEN){
        advance();
        if(parse_procedure_args()){
          if(word->get_token_type() == TOKEN_PUNC
            && static_cast<PuncToken *>(word)->get_attribute() == PUNC_CLOSE){
            advance();
            if(parse_variable_decl_list()){
              if(parse_block()){
                current_env = main_env;
                return true;
              // RIP
              }else{
                return false;
              }
            // Decl_list
            }else{
              return false;
            }
          // ')'
          }else{
            string *expected = new string ("')'");
            parse_error (expected, word);
            return false;
          }
        // procedure_args
        }else{
          return false;
        }
      // '('
      }else{
        string *expected = new string ("'('");
        parse_error (expected, word);
        return false;
      }
    // identifier
    }else{
      string *expected = new string ("identifier");
      parse_error (expected, word);
      return false;
    }
  // procedure
  }else{
    string *expected = new string ("procedure");
    parse_error (expected, word);
    return false;
  }
  // Defense
  return false;
}

bool Parser::parse_procedure_args()
{
  // FORMAL_PARM_LIST | lambda
  parsing_formal_parm_list = true;
  if(parse_formal_parm_list()){
    parsing_formal_parm_list = false;
    return true;
  // lambda case
  }else{
    return true;
  }
  // Defense
  return false;
}

bool Parser::parse_formal_parm_list()
{
  expr_type standard_type_type = GARBAGE_T;
  // identifier IDENTIFIER_LIST_PRM : STANDARD_TYPE FORMAL_PARM_LIST_HAT
  if(is_identifier()){
    if(is_identifier()){
      string *id_name = static_cast<IdToken *>(word)->get_attribute();
      if(stab.is_decl(id_name, current_env)){
        multiply_defined_identifier(id_name);
      }else{
        stab.install(id_name, current_env, UNKNOWN_T, formal_parm_position);
        formal_parm_position++;
        //delete id_name;
      }
      advance();
      if(parse_identifier_list_prm()){
        if(word->get_token_type() == TOKEN_PUNC
              && static_cast<PuncToken *>(word)->get_attribute() == PUNC_COLON){
          advance();
          if(parse_standard_type(standard_type_type)){
            /*
            {foreach (identifier i in stab such that i.type == unknown t)
            stab.update type (i, current env, STANDARD TYPE.type);}
            */
            stab.update_type(standard_type_type);
            return(parse_formal_parm_list_hat());
          // Didn't get em
          }else{
            return false;
          }
        // colon
        }else{
          string *expected = new string ("':'");
          parse_error (expected, word);
          return false;
        }
      // id_list_prm
      }else{
        return false;
      }
    // id
    }else{
      return false;
    }
    return false;
  }else{
    return false;
  }
  // Defense
  return false;
}

bool Parser::parse_formal_parm_list_hat()
{
  // ; FORMAL_PARM_LIST | lambda
  if(is_semicolon()){
    advance();
    return(parse_formal_parm_list());
  // lambda case
  }else{
    return true;
  }
  return false;
}

bool Parser::parse_stmt_list()
{
  // STMT ; STMT_LIST_PRM | ; STMT_LIST_PRM
  if(parse_stmt()){
    if(is_semicolon()){
      advance();
      return(parse_stmt_list_prm());
    // semi
    }else{
      string *expected = new string ("';'");
      parse_error (expected, word);
      return false;
    }
  }else if(is_semicolon()){
    return(parse_stmt_list_prm());
  }
  return false;
}

bool Parser::parse_stmt_list_prm()
{
  // STMT ; STMT_LIST_PRM | lambda
  if(parse_stmt()){
    if(is_semicolon()){
      advance();
      return(parse_stmt_list_prm());
    }else{
      string *expected = new string ("';'");
      parse_error (expected, word);
      return false;
    }
  // lambda
  }else{
    return true;
  }
  return false;
}

bool Parser::parse_stmt()
{
  Operand *expression;
  // if | while | print | id AD_HOC
  if(parse_if_stmt()){
    return true;
  }else if(parse_while_stmt()){
    return true;
  }else if(parse_print_stmt()){
    return true;
  }else if(is_identifier()){
    if(is_identifier()){
      string *id_name = static_cast<IdToken *>(word)->get_attribute();
      if(!stab.is_decl(id_name, current_env)){
        undeclared_identifier(id_name);
      }else{
        procedure_name = id_name;
      }
      advance();
      expr_type id_type = stab.get_type(id_name,current_env);
      expr_type ad_hoc_as_pc_tail_type = GARBAGE_T;
      if(parse_ad_hoc_as_pc_tail(ad_hoc_as_pc_tail_type, expression)){ // ASSIGN
        if(ad_hoc_as_pc_tail_type != id_type){
          type_error(ad_hoc_as_pc_tail_type, id_type);
        }
        /* IR actions 
        need to move id into expr
        */

        Register *expression_register = NULL;
        if (expression->get_type() == OPTYPE_REGISTER) {
            expression_register = expression->get_r_value();
        } else {
          /* Allocate a register */
          expression_register = allocator->allocate_register(); //
          if (expression->get_type() == OPTYPE_IMMEDIATE) {
            e->emit_move (expression_register, expression->get_i_value());
          } else if (expression->get_type() == OPTYPE_MEMORY) {
            e->emit_move (expression_register, expression->get_m_value());
          }
        }
        // Generate labels
        // string *assign = id_name;
        // e->emit_branch (INST_MOVE, expression_register, assign);
        
        // Deallocate
        allocator->deallocate_register (expression_register);
        delete expression; //MEGADELETE


        actual_parm_position = 0;
        return true;
      }else{
        return false;
      }
    // None
    }else{
      return false;
    }
    return false;
  }else{
    return false;
  }
  return false;
}

bool Parser::parse_ad_hoc_as_pc_tail(expr_type &ad_hoc_as_pc_tail_type, Operand *&expression)
{
  expr_type expr_type_result = GARBAGE_T;
  // := EXPR | (EXPR_LIST)
  if(word->get_token_type() == TOKEN_PUNC
     && static_cast<PuncToken *>(word)->get_attribute() == PUNC_ASSIGN){
    advance();
    //semantics
    if(parse_expr(expr_type_result, expression)){
      ad_hoc_as_pc_tail_type = expr_type_result;
      return true;
    }else{
      return false;
    }
  }else if(word->get_token_type() == TOKEN_PUNC
          && static_cast<PuncToken *>(word)->get_attribute() == PUNC_OPEN){
    advance();
    //semantic
    if(stab.get_type(procedure_name,main_env) != PROCEDURE_T){
      type_error(PROCEDURE_T, stab.get_type(procedure_name,main_env));
    }
    actual_parm_position = 0;
    if(parse_expr_list()){
      if(word->get_token_type() == TOKEN_PUNC
         && static_cast<PuncToken *>(word)->get_attribute() == PUNC_CLOSE){
        advance();
        ad_hoc_as_pc_tail_type = PROCEDURE_T;
        return true;
        // not close
      }else{
        string *expected = new string ("')'");
        parse_error (expected, word);
        return false;
      }
    // expr_list
    }else{
      return false;
    }
  // open
  }else{
    string *expected = new string ("':=' or '('");
    parse_error (expected, word);
    return false;
  }
  return false;
}

bool Parser::parse_if_stmt()
{
  // if EXPR then BLOCK IF_STMT_HAT
  if(word->get_token_type() == TOKEN_KEYWORD
     && static_cast<KeywordToken *>(word)->get_attribute() == KW_IF){
    advance();
    expr_type expr_type_result = GARBAGE_T;
    Operand *expression;
    if(parse_expr(expr_type_result, expression)){
      if(expr_type_result != BOOL_T){
        type_error(BOOL_T, expr_type_result);
      }
      /* IR actions
        Make sure operand that holds the expr value is in a
        register.
      */
      Register *expression_register = NULL;
      if (expression->get_type() == OPTYPE_REGISTER) {
          expression_register = expression->get_r_value();
      } else {
        /* 
           Allocate a register for the expression value and move it
           there. 
        */
        expression_register = allocator->allocate_register(); // SEG FAULT
        if (expression->get_type() == OPTYPE_IMMEDIATE) {
          e->emit_move (expression_register, expression->get_i_value());
        } else if (expression->get_type() == OPTYPE_MEMORY) {
          e->emit_move (expression_register, expression->get_m_value());
        }
      }
      /* 
         Generate labels of else part (even if it doesn't exist) and
         the next statement after the if. 
      */
      string *else_part = e->get_new_label ("else");
      string *if_done = e->get_new_label ("if_done");

      /* 
         Test the register that holds the value of the expression.  If
         it is false, jump to the else part. 
      */
      
      e->emit_branch (INST_BREZ, expression_register, else_part);
      
      /* 
        We are done with the expresion operand and the register in
        which it resides.  Deallocate both. 
      */
      allocator->deallocate_register (expression_register);
      delete expression; //MEGADELETE
      // Continue Semantic Anaylsis
      if(word->get_token_type() == TOKEN_KEYWORD
        && static_cast<KeywordToken *>(word)->get_attribute() == KW_THEN){
        advance();
        if(parse_block()){
          // IR - Skip over else part
          e->emit_branch (if_done);
          e->emit_label (else_part);
    
          /* IR - 
            if there is an else part to the if statment, the
            code for the else part code will be generated by
            parse_if_stmt_hat(). 
          */

          if(parse_if_stmt_hat()){
            // IR - emit label for statement following if statment
            e->emit_label (if_done);
            return true;
          }else{
            return false;
          }
        }else{
          return false;
        }
      // Then
      }else{
        string *expected = new string ("then");
        parse_error (expected, word);
        return false;
      }
    // expr
    }else{
      return false;
    }
  // if 
  }else{
    // string *expected = new string ("if");
    // parse_error (expected, word);
    return false;
  }
  return false;
}

bool Parser::parse_if_stmt_hat()
{
  if(word->get_token_type() == TOKEN_KEYWORD
        && static_cast<KeywordToken *>(word)->get_attribute() == KW_ELSE){
    advance();
    return(parse_block());
  // lambda
  }else{
    return true;
  }
  return false;
}

bool Parser::parse_while_stmt()
{
  expr_type expr_type_result = GARBAGE_T;
  Operand *expression; // = NULL;
  // while EXPR loop BLOCK
  if(word->get_token_type() == TOKEN_KEYWORD
        && static_cast<KeywordToken *>(word)->get_attribute() == KW_WHILE){
    advance();
    if(parse_expr(expr_type_result, expression)){
      //semantic
      if(expr_type_result != BOOL_T){
        type_error(BOOL_T,expr_type_result);
      }
      /* IR actions */
      Register *expression_register = NULL;
      string *while_ = e->get_new_label ("while"); 
      if (expression->get_type() == OPTYPE_REGISTER) {
          expression_register = expression->get_r_value();
      } else {
        /* Allocate a register */
        expression_register = allocator->allocate_register(); //
        if (expression->get_type() == OPTYPE_IMMEDIATE) {
          e->emit_label (while_);
          e->emit_move (expression_register, expression->get_i_value());
        } else if (expression->get_type() == OPTYPE_MEMORY) {
          e->emit_label (while_);
          e->emit_move (expression_register, expression->get_m_value());
        }
      }
      /* Generate labels */
      string *while_done = e->get_new_label ("while_done");
      string *block = e->get_new_label("block");

      /* Test the register that holds the value of the expression. */
      
      e->emit_branch (INST_BREZ, expression_register, while_done);
      e->emit_branch (INST_BRPO, expression_register, while_done);
      
      // Continue Semantic Anaylsis
      if(word->get_token_type() == TOKEN_KEYWORD
        && static_cast<KeywordToken *>(word)->get_attribute() == KW_LOOP){
        advance();
        e->emit_label(block);
        if(parse_block()){
          // IR 
          e->emit_branch (INST_BRUN, expression_register, while_);
          e->emit_label (while_done);
          /*  Deallocate  */
          allocator->deallocate_register (expression_register);
          delete expression; //MEGADELETE
          /* IR - 
            if there is an else part to the if statment, the
            code for the else part code will be generated by
            parse_if_stmt_hat(). 
          */
      // if(word->get_token_type() == TOKEN_KEYWORD
      //   && static_cast<KeywordToken *>(word)->get_attribute() == KW_LOOP){
      //   advance();
      //   return(parse_block());
        }
      return true;
      // conintue semantic
      // loop
      }else{
        string *expected = new string ("loop");
        parse_error (expected, word);
        return false;
      }
    // parse expr
    }else{
      return false;
    }
  // while
  }else{
    // string *expected = new string ("while");
    // parse_error (expected, word);
    return false;
  }
  return false;
}

bool Parser::parse_print_stmt()
{
  expr_type expr_type_result = GARBAGE_T;
  Operand *expression;
  // print EXPR
  if(word->get_token_type() == TOKEN_KEYWORD
        && static_cast<KeywordToken *>(word)->get_attribute() == KW_PRINT){
    advance();
    if(parse_expr(expr_type_result, expression)){
      //semantic
      if(expr_type_result != INT_T && expr_type_result != BOOL_T){
        type_error(INT_T,BOOL_T,expr_type_result);
      }
      /* IR actions
        Make sure operand that holds the expr value is in a
        register.
      */
      Register *expression_register = NULL;
      if (expression->get_type() == OPTYPE_REGISTER) {
          expression_register = expression->get_r_value();
      } else {
        /* 
           Allocate a register for the expression value and move it
           there. 
        */
        expression_register = allocator->allocate_register();
        if (expression->get_type() == OPTYPE_IMMEDIATE) {
          e->emit_move (expression_register, expression->get_i_value());
        } else if (expression->get_type() == OPTYPE_MEMORY) {
          e->emit_move (expression_register, expression->get_m_value());
        }
      }
      /* 
         Generate labels 
      */
      //string *print = e->get_new_label ("print");

      /* 
         Test the register that holds the value of the expression.  If
         it is false, jump to the else part. 
      */
      
      e->emit_1addr (INST_OUTB, expression_register);
      
      /* 
        We are done with the expresion operand and the register in
        which it resides.  Deallocate both. 
      */
      allocator->deallocate_register (expression_register);
      delete expression; //MEGADELETE
    }
  // Continue Semantic Anaylsis
    return true;
  }else{
    // string *expected = new string ("print");
    // parse_error (expected, word);
    return false;
  }
  return false;
}

bool Parser::parse_expr_list()
{
  // actual_parm_list | lambda
  if(parse_actual_parm_list()){
    return true;
  }else{
    return true;
  }
  return false;
}

bool Parser::parse_actual_parm_list()
{
  expr_type expr_type_result = GARBAGE_T;
  Operand *op;// = NULL;
  // expr actual_hat
  if(parse_expr(expr_type_result, op)){
    if (stab.get_type(procedure_name, main_env, actual_parm_position) != expr_type_result){
      type_error(expr_type_result, stab.get_type(procedure_name, main_env, actual_parm_position));
    }
    actual_parm_position++;
    return(parse_actual_parm_list_hat());
  }else{
    return false;
  }
  return false;
}

bool Parser::parse_actual_parm_list_hat()
{ 
  // , actual_hat | lambda
  if(word->get_token_type() == TOKEN_PUNC
            && static_cast<PuncToken *>(word)->get_attribute() == PUNC_COMMA){
    advance();
    return(parse_actual_parm_list_hat());
  }else{
    return true;
  }
  return false;
}

bool Parser::parse_expr(expr_type &expr_type_result, Operand *&op)
{
  expr_type simple_expr_type = GARBAGE_T;
  expr_type expr_hat_type = GARBAGE_T;
  // simple_expr expr_hat
  if(parse_simple_expr(simple_expr_type, op) && parse_expr_hat(expr_hat_type, op)){
    // Semantic
    if (expr_hat_type == NO_T) {
      expr_type_result = simple_expr_type;
    } else if (simple_expr_type == INT_T && expr_hat_type == INT_T){
      expr_type_result = BOOL_T;
    } else {
      type_error(INT_T, NO_T, simple_expr_type);
    }
    return true;
  }else{
    return false;
  }
  return false;
}

bool Parser::parse_expr_hat(expr_type &expr_hat_type, Operand *&op)
{
  expr_type simple_expr_type = GARBAGE_T;
  // relop SIMPLE_EXPR
  if(word->get_token_type() == TOKEN_RELOP){
    advance();
    //semantic
    if(parse_simple_expr(simple_expr_type, op)){
      if(simple_expr_type == INT_T){
        expr_hat_type = INT_T;
      }else{
        type_error(INT_T, simple_expr_type);
      }
      return true;
    }else{
      return false;
    }
  // lambda
  }else{
    expr_hat_type = NO_T;
    return true;
  }
  return false;
}

bool Parser::parse_simple_expr(expr_type &simple_expr_type, Operand *&op)
{
  expr_type term_type = GARBAGE_T;
  expr_type simple_expr_prm_type = GARBAGE_T;
  // term simple_expr_prm
  if(parse_term(term_type, op) && parse_simple_expr_prm(simple_expr_prm_type, op)){
    // Semantic
    if (simple_expr_prm_type == NO_T) {
      simple_expr_type = term_type;
    }else if(term_type == simple_expr_prm_type){
      simple_expr_type = term_type;
    } else {
      type_error(term_type, simple_expr_type);
    }
    return true; 
  }else{
    return false;
  }
  return false;
}

bool Parser::parse_simple_expr_prm(expr_type &simple_expr_prm_type0, Operand *&op)
{
  expr_type term_type = GARBAGE_T;
  expr_type simple_expr_prm_type1 = GARBAGE_T;
  // addop TERM SIMPLE_EXPR_PRM | lambda
  if(word->get_token_type() == TOKEN_ADDOP){
    /* Semantic Analysis */
    expr_type addop_type = GARBAGE_T;
    addop_attr_type addop_attr = ADDOP_NO_ATTR;
    addop_attr = static_cast<AddopToken *>(word)->get_attribute();
    if (addop_attr == ADDOP_SUB || addop_attr == ADDOP_ADD) {
      addop_type = INT_T;
    } else {
      addop_type = BOOL_T;
    }
    advance();
    if(parse_term(term_type, op)){
      if(parse_simple_expr_prm(simple_expr_prm_type1, op)){
        if(simple_expr_prm_type1 == NO_T){
          if(addop_type == term_type){
            simple_expr_prm_type0 = addop_type;
          }else{
            type_error(INT_T, BOOL_T, simple_expr_prm_type0);
          }
        }else if(addop_type == term_type && term_type == simple_expr_prm_type1){
          simple_expr_prm_type0 = addop_type;
        }else{
          type_error(term_type, simple_expr_prm_type0);
        }
        return true;
      }else{
        return false;
      }
    }else{
      return false;
    }
  // lambda Gachigasm
  }else{
    simple_expr_prm_type0 = NO_T;
    return true;
  }
  return false;
}



// Needs to be fixed

bool Parser::parse_term(expr_type &term_type, Operand *&op)
{
  // FACTOR TERM_PRM
  // return(parse_factor() && parse_term_prm());

  expr_type factor_type = GARBAGE_T,
  term_prm_type = GARBAGE_T;

  if (parse_factor(factor_type, op)) {
    if (parse_term_prm(term_prm_type, op)) {
      /* Semantic Analysis */
      if (term_prm_type == NO_T) {
        term_type = factor_type;
      } else if (factor_type == term_prm_type) {
        term_type = factor_type;
      } else {
        type_error (factor_type, term_prm_type);
      }
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}

bool Parser::parse_term_prm(expr_type &term_prm0_type, Operand *&left_op)
{
  expr_type factor_type = GARBAGE_T,
  term_prm1_type = GARBAGE_T;

  Operand *right_op = NULL;

  // mulop FACTOR TERM_PRM | lambda
  if(word->get_token_type() == TOKEN_MULOP){
    /* Semantic Analysis */
    expr_type mulop_type = GARBAGE_T;
    mulop_attr_type mulop_attr = MULOP_NO_ATTR;
    mulop_attr = static_cast<MulopToken *>(word)->get_attribute();
    if (mulop_attr == MULOP_MUL || mulop_attr == MULOP_DIV) {
      mulop_type = INT_T;
    } else {
      mulop_type = BOOL_T;
    }
    advance();
    if(parse_factor(factor_type, right_op)){
      /* 
        At this point, we can generate code for 
        "right_op operation left_op". 

        First we need to make sure the left operand (which
        was passed to us as a parm) is in a register.  If the left op
        is in a register, grab the register.  If the left operand is
        not in a register, allocate a register and move the operand
        there.
      */
      Register *left_op_reg;
      if (left_op->get_type() == OPTYPE_REGISTER) {
      // The left operand is already in a register.
        left_op_reg = left_op->get_r_value();
      }else{
        // Allocate a new register and move the left op into it.
        left_op_reg = allocator->allocate_register();
        if (left_op->get_type() == OPTYPE_IMMEDIATE) {
          e->emit_move (left_op_reg, left_op->get_i_value());
        } else { // left factor op is in memory
          e->emit_move (left_op_reg, left_op->get_m_value());
        }
        /* 
          We are done with the Operand object that our parent node
          gave us, and it no longer describes the left operand.
          Delete it and make a new Operand object representing the
          current state of the left operand, which is now in a register.
        */
        delete left_op; //MEGADELETE
        left_op = new Operand (OPTYPE_REGISTER, left_op_reg);
      }

      /* 
        Output IR to perform the operation.
        First, determine which mulop we the 
        program has called for.
      */
      inst_type instruction;
      if (mulop_attr == MULOP_MUL || mulop_attr == MULOP_AND) {
        instruction = INST_MUL;
      } else {
        instruction = INST_DIV;
      }

      /* 
        Now, output the appropriate instruction, depending on the
        location (immediate, register, memory) of the right hand op.
       */
      switch (right_op->get_type()){
        case OPTYPE_REGISTER:
          e->emit_2addr (instruction, left_op->get_r_value(), 
            right_op->get_r_value());
          break;
        case OPTYPE_IMMEDIATE:
          e->emit_2addr (instruction, left_op->get_r_value(), 
            right_op->get_i_value());
          break;
        case OPTYPE_MEMORY:
          e->emit_2addr (instruction, left_op->get_r_value(), 
            right_op->get_m_value());
          break;
        default:
          break;
      }
      /* Clean up.
        We are done with the right Operand object. 
      */
      if (right_op->get_type() == OPTYPE_REGISTER) {
        // IMPORTANT: don't forget to deallocate the register if
        // you are done with an operand that resides in a
        // register.  We only have 3 usuable ones.
        allocator->deallocate_register(right_op->get_r_value());
      }
      delete right_op; 

    }
    if(parse_term_prm(term_prm1_type, left_op)){

      // Semantics Analysis cont.
      if(term_prm1_type == NO_T && mulop_type == factor_type){
        term_prm0_type = mulop_type;
      }else if(mulop_type == factor_type && factor_type == term_prm1_type){
        term_prm0_type = mulop_type;
      }else{
        type_error(mulop_type, term_prm1_type);
      }
      return true;
    }else{
      return false;
    }
  // lambda
  }else{
    term_prm0_type = NO_T;
    return true;
  }
  return false;
}

bool Parser::parse_factor(expr_type &factor0_type, Operand *&op)
{
  expr_type sign_type = GARBAGE_T;
  expr_type factor1_type = GARBAGE_T;
  expr_type expr_type_result = GARBAGE_T;
  // id | num | ( EXPR ) | SIGN FACTOR
  if(is_identifier()){
    string *id_name = static_cast<IdToken *>(word)->get_attribute();
    if(!stab.is_decl(id_name,current_env)){
      undeclared_identifier(id_name);
    }else{
      factor0_type = stab.get_type(id_name,current_env);
    }
    // ir action
    op = new Operand (OPTYPE_MEMORY, id_name);
    advance();
    //delete id_name; 
    return true;
  }else if(word->get_token_type() == TOKEN_NUM){
    factor0_type = INT_T;

    /* IR action

      Make a new Operand object to represent the 
      literal we just found.
       
      There is a slight complication here.  NumToken attributes are
      stored as strings in the token, but we want them as ints in the
      operand.  We do the conversion here. 
    */
    stringstream ss (*(static_cast<NumToken *>(word)->get_attribute()));
    int op_val;
    ss >> op_val; 
    op = new Operand (OPTYPE_IMMEDIATE, op_val);

    advance();
    return true;
  }else if(word->get_token_type() == TOKEN_PUNC
    && static_cast<PuncToken *>(word)->get_attribute() == PUNC_OPEN){
    advance();
    if(parse_expr(expr_type_result, op)){
      factor0_type = expr_type_result;
      if(word->get_token_type() == TOKEN_PUNC
        && static_cast<PuncToken *>(word)->get_attribute() == PUNC_CLOSE){
        advance();
        return true;
        // not close
      }else{
        string *expected = new string ("')'");
        parse_error (expected, word);
        return false;
      }
      // expr_list
      }else{
        return false;
      }
  // open
  }else if ((word->get_token_type() == TOKEN_ADDOP
       && (static_cast<AddopToken *>(word)->get_attribute() == ADDOP_ADD
    || static_cast<AddopToken *>(word)->get_attribute() == ADDOP_SUB))
    || (word->get_token_type() == TOKEN_KEYWORD
       && static_cast<KeywordToken *>(word)->get_attribute() == KW_NOT)){

    // Hack here.  We need to save the unary operator for IR code
    // generation, later on in parse_factor().  We will
    // use an int flag to do it.  0 == '+', 1 == '-', 2 == 'not'.
    int sign_operation;
    if(word->get_token_type() == TOKEN_ADDOP
      && static_cast<AddopToken *>(word)->get_attribute() == ADDOP_ADD){
      sign_operation = 0;
    }else if(word->get_token_type() == TOKEN_ADDOP
      && static_cast<AddopToken *>(word)->get_attribute() == ADDOP_SUB){
      sign_operation = 1;
    }else{
      sign_operation = 2;
    }
    if(parse_sign(sign_type)){
      if(parse_factor(factor1_type, op)){
        if(sign_type != factor1_type){
          type_error(sign_type, factor1_type);
        }else{ // Mega else
          factor0_type = factor1_type;

          /* 
            At this point, we need to generate code to apply the sign
            operation to the operand.  If the sign is a "+", that's a
            no-op.  If it's a "-" or NOT, we need to move the operand
            into a register and then generate the instruction to peform
            the appropriate operation. 
          */
          if (sign_operation == 0) { // sign is '+'
                // do nothing
          } else {
            // make sure operand is in a register
            Register *op_register;
            if (op->get_type() == OPTYPE_REGISTER) {
              op_register = op->get_r_value();
            } else {
              op_register = allocator->allocate_register();
              // Emit instruction to move operand into register
              if (op->get_type() == OPTYPE_IMMEDIATE) {
                // move Rn, #immediate_value
                e->emit_move (op_register, op->get_i_value());
              } else if (op->get_type() == OPTYPE_MEMORY) {
                // move RN, #memory_location_name
                e->emit_move (op_register, op->get_m_value());
              }
              // The operand is now in register op_register.  Make a new
              // operand object to represent it.
              //delete op;  //MEGADELETE
              op = new Operand (OPTYPE_REGISTER, op_register);
            }  // op not in a register

            // Finally, emit the instruction to perform the SIGN
            // operation ('+' or "or" only).
            if (sign_operation == 1) { // sign is '-'
              e->emit_1addr (INST_NEG, op->get_r_value());
            } else if (sign_operation == 2) {  // sign is 'not'
              e->emit_1addr (INST_NOT, op->get_r_value());
            }
          } // operation is "-" or "NOT"
        } // Mega else
        return true;
      }else{ // parse factor
        return false;
      }
    }
  }else{
    string *expected = new string ("'id' or 'num' or '(EXPR)' or 'SIGN FACTOR'");
    parse_error (expected, word);
    return false;
  }
  return false;
}

bool Parser::parse_sign(expr_type &sign_type)
{
  // + | - | not
  if(word->get_token_type() == TOKEN_ADDOP
    && static_cast<AddopToken *>(word)->get_attribute() == ADDOP_ADD){
    sign_type = INT_T;
    advance();
    return true;
  }else if(word->get_token_type() == TOKEN_ADDOP
          && static_cast<AddopToken *>(word)->get_attribute() == ADDOP_SUB){
    sign_type = INT_T;
    advance();
    return true;
  }else if(word->get_token_type() == TOKEN_KEYWORD
           && static_cast<KeywordToken *>(word)->get_attribute() == KW_IF){
    sign_type = BOOL_T;
    advance();
    return true;
  }else{
    string *expected = new string ("'+' or '-' or 'not'");
    parse_error(expected, word);
    return false;
  }
  return false;
}
