#ifndef EOFTOKEN_H
#define EOFTOKEN_H

using namespace std;

#include "token.h"

// The EofToken has no attributes.

class EofToken : public Token
{
 public:
  EofToken ();
  ~EofToken ();

  string *to_string() const;

};

#endif

