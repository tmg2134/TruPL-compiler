#include "numtoken.h"
#include <iostream>

NumToken::NumToken()
{
  attribute = NULL;
}

NumToken::NumToken(const string& attr)
{
  attribute = new string(attr);
}

NumToken::~NumToken()
{
	delete attribute;
}

string* NumToken::get_attribute()
{
  return attribute;
}

void NumToken::set_attribute(const string& attr)
{
  attribute = new string(attr);
}

string* NumToken::to_string() const
{
	cout<<"NumToken: ";
  return attribute;
}