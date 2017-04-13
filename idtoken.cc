#include "idtoken.h"
#include <iostream>

IdToken::IdToken()
{
	attribute = NULL;
}

IdToken::IdToken(const string& attr)
{
	attribute = new string(attr);
}

IdToken::~IdToken()
{
	delete attribute;
}

string* IdToken::get_attribute() // I'm unsure
{
	return attribute;
}

void IdToken::set_attribute(const string& attr)
{
	attribute = new string(attr);
}

string* IdToken::to_string() const
{ 
	cout<<"IdToken: ";
  return attribute;
}