#include "eoftoken.h"

string eofToken = "$";

EofToken::EofToken()
{}

EofToken::~EofToken()
{}

string* EofToken::to_string() const
{
  return &eofToken;
}