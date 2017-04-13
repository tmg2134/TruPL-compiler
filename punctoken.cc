#include "punctoken.h"

string punc;

PuncToken::PuncToken()
{
	attr = PUNC_NO_ATTR;
}

PuncToken::PuncToken(punc_attr_type attr)
{
	this->attr = attr;
}

PuncToken::~PuncToken()
{}

punc_attr_type PuncToken::get_attribute()
{
	return attr;
}

void PuncToken::set_attribute(punc_attr_type attr)
{
	this->attr = attr;
}

string* PuncToken::to_string() const
{
	switch(attr)
	{
		case PUNC_SEMI:
			punc = "PuncToken: SemiColon";
		break;	
		case PUNC_COLON:
			punc = "PuncToken: Colon";
		break;	
		case PUNC_COMMA:
			punc = "PuncToken: Comma";
		break;	
		case PUNC_ASSIGN:
			punc = "PuncToken: Assign";
		break;	
		case PUNC_OPEN:
			punc = "PuncToken: Open Parenthese";
		break;	
		case PUNC_CLOSE:
			punc = "PuncToken: Close Parenthese";
		break;	
		case PUNC_NO_ATTR:
			punc = "PuncToken: None";
		break;	
	}
  return &punc;
}