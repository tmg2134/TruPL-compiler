#include "keywordtoken.h"

string keyword;

KeywordToken::KeywordToken()
{
  attribute = KW_NO_ATTR;
}

KeywordToken::KeywordToken(keyword_attr_type attr)
{
  attribute = attr;
}

KeywordToken::~KeywordToken()
{}

keyword_attr_type KeywordToken::get_attribute()
{
  return attribute;
}

void KeywordToken::set_attribute (keyword_attr_type attr)
{
  attribute = attr;
}

string * KeywordToken::to_string() const
{
	switch(attribute)
	{
		case KW_PROGRAM   :
			keyword = "KeywordToken: program";
		break; 
		case KW_PROCEDURE :
			keyword = "KeywordToken: procedure";
		break;
		case KW_INT       :
			keyword = "KeywordToken: int";
		break;  
		case KW_BOOL      :
			keyword = "KeywordToken: bool";
		break; 
		case KW_BEGIN     :
			keyword = "KeywordToken: begin";
		break;  
		case KW_END       :
			keyword = "KeywordToken: end";
		break; 
		case KW_IF        :
			keyword = "KeywordToken: if";
		break;  
		case KW_THEN      :
			keyword = "KeywordToken: then";
		break; 
		case KW_ELSE      :
			keyword = "KeywordToken: else";
		break;  
		case KW_WHILE     :
			keyword = "KeywordToken: while";
		break; 
		case KW_LOOP      :
			keyword = "KeywordToken: loop";
		break; 
		case KW_PRINT     :
			keyword = "KeywordToken: print";
		break;  
		case KW_NOT       :
			keyword = "KeywordToken: not";
		break; 
		case KW_NO_ATTR   :
			keyword = "KeywordToken: None";
		break; 	
	}
  return &keyword;
}

