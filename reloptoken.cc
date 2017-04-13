#include "reloptoken.h"

string relop;

RelopToken::RelopToken()
{
	attr = RELOP_NO_ATTR;
}

RelopToken::RelopToken(relop_attr_type attr)
{
	this->attr = attr;
}

RelopToken::~RelopToken()
{}

relop_attr_type RelopToken::get_attribute()
{
	return attr;
}

void RelopToken::set_attribute(relop_attr_type attr)
{
	this->attr = attr;
}

string* RelopToken::to_string() const
{
	switch(attr)
	{
		case RELOP_EQ:
			relop = "RelopToken: Equal";
		break;
	  case RELOP_NE:
	  	relop = "RelopToken: Not Equal";
	  break;
		case RELOP_GT:
			relop = "RelopToken: Greater Than";
		break;
		case RELOP_GE:
		  relop = "RelopToken: Greather than or equal";
		 break;
		case RELOP_LT:
			relop = "RelopToken: Less Than";
		break;
		case RELOP_LE:
			relop = "RelopToken: Less than or equal";
		break;
		case RELOP_NO_ATTR:
			relop = "RelopToken: None";
		break;
	}
  return &relop;
}