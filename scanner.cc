//Thomas Goodman
#include "scanner.h"

Scanner::Scanner (char *filename)
{
  // Create and fill the buffer.
  buf = new Buffer (filename);
}

Scanner::~Scanner ()
{
  delete buf;
}

void Scanner::loopCase()
{
  if(is_alphanum(c))
  {
    state = 19; // The IdToken State
    attr += c;
  }else if(is_space(c))
  {
    lexeme = new IdToken(attr);
    lexeme->set_token_type(TOKEN_ID);
    state = done;
  }else
  {
    lexeme = new IdToken(attr);
    lexeme->set_token_type(TOKEN_ID);
    state = done;
    buf->unread_char(c);
  }
}

// Case statement for moving to the next part of a
// KeywordToken, or identifies and IdToken
void Scanner::alphanumCase(char theLetter, int newState)
{
  if(c == theLetter)
  {
    state = newState; 
    attr += c;
  }else if(is_alphanum(c))
  {
    state = 19; // The IdToken State
    attr += c;
  }else if(is_space(c))
  {
    lexeme = new IdToken(attr);
    lexeme->set_token_type(TOKEN_ID);
    state = done;
  }else
  {
    lexeme = new IdToken(attr);
    lexeme->set_token_type(TOKEN_ID);
    state = done;
    buf->unread_char(c);
  }
}

// Takes 2 parameters of each instead of 1 each
void Scanner::alphanumCase2P(char theLetter1, char theLetter2, int newState1, int newState2)
{
  if(c == theLetter1) 
  {
    state = newState1; 
    attr += c;
  }else if(c == theLetter2) 
  {
    state = newState2;
    attr += c;
  }
  else if(is_alphanum(c))
  {
    state = 19;
    attr += c;
  }else if(is_space(c))
  {
    lexeme = new IdToken(attr);
    lexeme->set_token_type(TOKEN_ID);
    state = done;
  }else
  {
    lexeme = new IdToken(attr);
    lexeme->set_token_type(TOKEN_ID);
    state = done;
    buf->unread_char(c);
  }
}

// For possible new Keyword
void Scanner::newKeywordToken(keyword_attr_type KW)
{
	attr += c;
  if(is_alphanum(c))
  {
    state = 19;
  }else if(is_space(c))
  {
    lexeme = new KeywordToken(KW);
    lexeme->set_token_type(TOKEN_KEYWORD);
    state = done;
  }else 
  {
    lexeme = new KeywordToken(KW);
    lexeme->set_token_type(TOKEN_KEYWORD);
    state = done;
    buf->unread_char(c);
  }
}

//For possible new Punc
void Scanner::newPuncToken(punc_attr_type PUNC)
{
	if(is_space(c))
  {
    lexeme = new PuncToken(PUNC);
    lexeme->set_token_type(TOKEN_PUNC);
    state = done;
  }else if(c == '=' && state == 12) // Special case
  {
    state = 35; 
  }else
  {
    lexeme = new PuncToken(PUNC);
    lexeme->set_token_type(TOKEN_PUNC);
    state = done;
    buf->unread_char(c);
  }
}
// Checks if it needs to unread a character
void Scanner::checkUnread()
{
  if(!(is_space(c)))
  {
    buf->unread_char(c);
  }
}

Token *Scanner::next_token()
{
  lexeme = NULL;
  attr = "";
  state = 0;
  done = -1;
  // done cannot be a state
  while(state != done)
  {
    if (buf->empty())
    { 
      //yo
      buf->fill_buffer();
      //cout<<"Buffer Filled \n";
    }
    c = buf->next_char();
    switch(state)
    {
      case 0: // What might come first
        if(c == 'p')
        {
          state = 1;
          attr += c;
        }else if(c == 'o')
        {
          state = 2;
          attr += c;
        }else if(c == 'a')
        {
          state = 3;
          attr += c;
        }else if(c == 'e')
        {
          state = 4;
          attr += c;
        }else if(c == 'i')
        {
          state = 5;
          attr += c;
        }else if(c == 'n')
        {
          state = 6;
          attr += c;
        }else if(c == 't')
        {
          state = 7;
          attr += c;
        }else if(c == 'b')
        {
          state = 8;
          attr += c;
        }else if(c == 'l')
        {
          state = 9;
          attr += c;
        }else if(c == 'w')
        {
          state = 10;
          attr += c;
        }else if(c == ';')
        {
          state = 11;
        }else if(c == ':')
        {
          state = 12;
        }else if(c == ',')
        {
          state = 13;
        }else if(c == '=')
        {
          state = 14;
        }else if(c == '*')
        {
          state = 15;
        }else if(c == '<')
        {
          state = 16;
        }else if(c == '>')
        {
          state = 17;
        }else if(c == '/')
        {
          state = 18;
        }else if(is_alpha(c))
        {
          attr += c;
          state = 19;
        }else if(is_digit(c))
        {
          attr += c;
          state = 20;
        }else if(c == '-')
        {
          state = 76;
        }else if(c == '+')
        {
          state = 77;
        }else if(c == '$')
        {
          state = 78;
        }else if(c == '(')
        {
          state = 79;
        }else if(c == ')')
        {
          state = 80;
        }
        break;
      case 1:
        alphanumCase('r', 21);
        break;
      case 2:
        alphanumCase('r', 22);
        break;
      case 3:
        alphanumCase('n', 23);
        break;
      case 4:
        alphanumCase2P('n', 'l', 24, 25);
        break;
      case 5:
        alphanumCase2P('n', 'f', 26, 27);
        break;
      case 6:
        alphanumCase('o', 28);
        break;
      case 7:
        alphanumCase('h', 29);
        break;
      case 8:
        alphanumCase2P('o', 'e', 30, 31);
        break;
      case 9:
        alphanumCase('o', 32);
        break;
      case 10:
        alphanumCase('h', 33);
        break;
      case 11: //Semi colon
        newPuncToken(PUNC_SEMI);
        break;
      case 12: // Colon
        newPuncToken(PUNC_COLON);
        break;
      case 13: //Comma
        newPuncToken(PUNC_COMMA);
        break;
      case 14: // Equals
        lexeme = new RelopToken(RELOP_EQ);
        lexeme->set_token_type(TOKEN_RELOP);
        state = done;
        checkUnread();
        break;
      case 15: //Multiply
        lexeme = new MulopToken(MULOP_MUL);
        lexeme->set_token_type(TOKEN_MULOP);
        state = done;
        checkUnread();
        break;
      case 16: // Less than
        if(is_space(c))
        {
          lexeme = new RelopToken(RELOP_LT);
          lexeme->set_token_type(TOKEN_RELOP);
          state = done;
        }else if(c == '=')
        {
          state = 36;
        }else if(c == '>')
        {
          state = 37;
        }else
        {
          lexeme = new RelopToken(RELOP_LT);
          lexeme->set_token_type(TOKEN_RELOP);
          state = done;
          buf->unread_char(c);
        }
        break;
      case 17: //Greater than
        if(is_space(c))
        {
          lexeme = new RelopToken(RELOP_GT);
          lexeme->set_token_type(TOKEN_RELOP);
          state = done;
        }else if(c == '=')
        {
          state = 38;
        }else
        {
          lexeme = new RelopToken(RELOP_GT);
          lexeme->set_token_type(TOKEN_RELOP);
          state = done;
          buf->unread_char(c);
        }
        break;
      case 18: //Division
        lexeme = new MulopToken(MULOP_DIV);
        lexeme->set_token_type(TOKEN_MULOP);
        state = done;
        if(!(is_space(c)))
        checkUnread();
        break;
      case 19: // IdToken
        loopCase();
        break;
      case 20: // NumToken
        if(is_space(c))
        {
          lexeme = new NumToken(attr);
          lexeme->set_token_type(TOKEN_NUM);
          state = done;
        }else if(is_digit(c))
        {
          attr += c;
          state = 20;
        }else
        {
          lexeme = new NumToken(attr);
          lexeme->set_token_type(TOKEN_NUM);
          state = done;
          buf->unread_char(c);
        }
        break;
      case 21:
        alphanumCase2P('o', 'i', 42, 43);
        break;
      case 22: // or
        attr += c;
        // if(c == 'r')
        // {
          // lexeme = new AddopToken(ADDOP_OR);
          // lexeme->set_token_type(TOKEN_ADDOP);
          // return lexeme;
        // }else 
        if(is_alphanum(c))
        {
          state = 19;
        }else if(is_space(c))
        {
          lexeme = new AddopToken(ADDOP_OR);
          lexeme->set_token_type(TOKEN_ADDOP);
          return lexeme;
          // lexeme = new IdToken(attr);
          // lexeme->set_token_type(TOKEN_ID);
          // state = done;
        }else 
        {
          lexeme = new AddopToken(ADDOP_OR);
          lexeme->set_token_type(TOKEN_ADDOP);
          state = done;
          buf->unread_char(c);
        }
        break;
      case 23:
        alphanumCase('d', 45);
        break;
      case 24:
        alphanumCase('d', 46);
        break;
      case 25:
        alphanumCase('s', 47);
        break;
      case 26:
        alphanumCase('t', 48);
        break;
      case 27: // if
        newKeywordToken(KW_IF);
        break;
      case 28:
        alphanumCase('t', 49);
        break;
      case 29:
        alphanumCase('e', 50);
        break;
      case 30:
        alphanumCase('o', 51);
        break;
      case 31:
        alphanumCase('g', 52);
        break;
      case 32:
        alphanumCase('o', 53);
        break;
      case 33:
        alphanumCase('i', 54);
        break;
      case 35: //Assign
        newPuncToken(PUNC_ASSIGN);
        break;
      case 36: //Less than
        checkUnread();
        lexeme = new RelopToken(RELOP_LE);
        lexeme->set_token_type(TOKEN_RELOP);
        state = done;
        break;
      case 37: //Not equal
        checkUnread();
        lexeme = new RelopToken(RELOP_NE);
        lexeme->set_token_type(TOKEN_RELOP);
        state = done;
        break;
      case 38: //Greater than
        checkUnread();
        lexeme = new RelopToken(RELOP_GE);
        lexeme->set_token_type(TOKEN_RELOP);
        state = done;
        break;
      case 42:
        alphanumCase2P('g','c',56,57);
        break;
      case 43:
        alphanumCase('n',58);
        break;
      case 45: //and
        attr += c;
        if(is_alphanum(c))
        {
          state = 19;
        }else if(is_space(c))
        {
          lexeme = new MulopToken(MULOP_AND);
          lexeme->set_token_type(TOKEN_MULOP);
          state = done;
        }else 
        {
          lexeme = new MulopToken(MULOP_AND);
          lexeme->set_token_type(TOKEN_MULOP);
          state = done;
          buf->unread_char(c);
        }
        break;
      case 46://end
        newKeywordToken(KW_END);
        break;
      case 47:
        alphanumCase('e',59);
        break;
      case 48: //int
        newKeywordToken(KW_INT);
        break;
      case 49: //not
        newKeywordToken(KW_NOT);
        break;
      case 50:
        alphanumCase('n',60);
        break;
      case 51: //bool
        alphanumCase('l',61);
        break;
      case 52:
        alphanumCase('i',62);
        break;
      case 53:
        alphanumCase('p',63);
        break;
      case 54:
        alphanumCase('l',64);
        break;
      case 56:
        alphanumCase('r',65);
        break;
      case 57:
        alphanumCase('e',66);
        break;
      case 58:
        alphanumCase('t',67);
        break;
      case 59: //else
        newKeywordToken(KW_ELSE);
        break;
      case 60://then
        newKeywordToken(KW_THEN);
        break;
      case 61://bool
        newKeywordToken(KW_BOOL);
        break;
      case 62:
        alphanumCase('n',68);
        break;
      case 63://loop
        newKeywordToken(KW_LOOP);
        break;
      case 64:
        alphanumCase('e',69);
        break;
      case 65:
        alphanumCase('a',70);
        break;
      case 66:
        alphanumCase('d',71);
        break;
      case 67://print
        newKeywordToken(KW_PRINT);
        break;
      case 68://begin
        newKeywordToken(KW_BEGIN);
        break;
      case 69://while
        newKeywordToken(KW_WHILE);
        break;
      case 70:
        alphanumCase('m',72);
        break;
      case 71:
        alphanumCase('u',73);
        break;
      case 72://program
        newKeywordToken(KW_PROGRAM);
        break;
      case 73:
        alphanumCase('r',74);
        break;
      case 74:
        alphanumCase('e',75);
        break;
      case 75://procedure
        newKeywordToken(KW_PROCEDURE);
        break;
      case 76://subtract
        lexeme = new AddopToken(ADDOP_SUB);
        lexeme->set_token_type(TOKEN_ADDOP);
        state = done;
        checkUnread();
        break;
      case 77://add
        lexeme = new AddopToken(ADDOP_ADD);
        lexeme->set_token_type(TOKEN_ADDOP);
        state = done;
        checkUnread();
        break;
      case 78://eof
        lexeme = new EofToken();
        lexeme->set_token_type(TOKEN_EOF);
        state = done;
        break;
      case 79:
        newPuncToken(PUNC_OPEN);
        break;
      case 80:
        newPuncToken(PUNC_CLOSE);
        break;
    } // Switch
  } // While
  return lexeme;
}

void Scanner::scanner_fatal_error(const string& message)
{
  cerr << "Exiting on Scanner Fatal Error: " << message << endl;
  exit (-1);
}


