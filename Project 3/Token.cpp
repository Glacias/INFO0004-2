#include <sstream>

#include "Token.hpp"

/*
* positionStr returns the string "line:col" where line
* and col are the positions of the Token in the file.
*/
std::string Token::positionStr() const
{
  std::stringstream ss;
  ss << std::to_string(line) << ":" << std::to_string(column);
  return ss.str();
}

/*
* copy returns a copy of the Token.
*/
Token Token::copy() const
{
  Token c;
  c.id = id;
  c.word = word;
  c.symbol = symbol;
  c.line = line;
  c.column = column;
  return c;
}
