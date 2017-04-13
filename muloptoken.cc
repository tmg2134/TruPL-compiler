#include "muloptoken.h"

string mulop;

MulopToken::MulopToken()
{
  attr = MULOP_NO_ATTR;
}

MulopToken::MulopToken(mulop_attr_type attr)
{
  this->attr = attr;
}

MulopToken::~MulopToken()
{}

mulop_attr_type MulopToken::get_attribute()
{
  return attr;
}

void MulopToken::set_attribute (mulop_attr_type attr)
{
  this->attr = attr;
}

string* MulopToken::to_string() const
{
	switch(attr)
	{
		case MULOP_MUL:
			mulop = "MulopToken: multiply";
		break;
		case MULOP_DIV: 
			mulop = "MulopToken: divide";
		break;
		case MULOP_AND: 
			mulop = "MulopToken: and";
		break;
		case MULOP_NO_ATTR: 
			mulop = "MulopToken: None";
		break;
	}
  return &mulop;
}