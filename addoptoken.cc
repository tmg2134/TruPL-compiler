#include "addoptoken.h"

string addop = "";

AddopToken::AddopToken()
{
  attr = ADDOP_NO_ATTR;
}

AddopToken::AddopToken(addop_attr_type attr)
{
  this->attr = attr;
}

AddopToken::~AddopToken()
{}

addop_attr_type AddopToken::get_attribute()
{
  return attr;
}

void AddopToken::set_attribute (addop_attr_type attr)
{
  this->attr = attr;
}

string* AddopToken::to_string() const
{
  if(attr == ADDOP_ADD)
  {
    addop = "AddopToken: add";
  }else if(attr == ADDOP_SUB)
  {
    addop = "AddopToken: sub";
  }else if(attr == ADDOP_OR)
    addop = "AddopToken: or";
  return &addop;
}
