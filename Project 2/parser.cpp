#include <iostream>
#include <string>
#include <fstream>
#include <vector>

#include "Token.hpp"

using namespace std;

static vector<Token> listShapeName;//List of shape that are defined
static vector<Token> listColorName;//List of color that are defined

void errorInFile(string file, Token t, string error);
Token getToken(ifstream& ifs, int& line, int& column, string file);
bool parsePoint(ifstream& ifs, int& line, int& column, string file, Token& curToken, double& x, double& y);
bool parseName(Token& curToken, vector<Token>& list);
bool validName(Token curToken);

/*
* errorInFile sends a personnalised error message depending
* on the related Token (pos:line) and the given error message.
* It then stops the programs.
*
* Arguments :
*  - file : is the path to the file .paint
*  - t : is the error related token
*  - error : is the error message
*/
void errorInFile(string file, Token t, string error)
{
  string e = " ";
  if(t.id == -2)
  {
    e = ", got : \"" + t.word + "\"";
  }
  else if(t.id == -3)
  {
    e = ", got : \"";
    e += t.symbol;
    e += "\"";
  }
  cerr << file << ":" << t.positionStr() << ": error:" << error << e << endl;
  exit(EXIT_FAILURE);
}

/*
* getToken reads character in the stream and returns the corresponding
* token (defined in Token.hpp)
*
* Arguments :
*  - ifs : is the stream of character (that where read in the file)
*  - line : is the adress of the counter of the current line
*  - column : is the adress of the counter of the current column
*/
Token getToken(ifstream& ifs, int& line, int& column, string file)
{
  Token t;
  char lastChar = ifs.get();

  // Skip any whitespace.
  while (isspace(lastChar))
  {
    if(lastChar == '\n')
    {
      //Add line to counter
      line++;
      column = 1;
    }
    else
    {
      //Add column to counter
      column++;
    }
    lastChar = ifs.get();
  }

  //Skip comment
  if (lastChar == '#')
  {
    while (!ifs.eof() && lastChar != '\n')
    {
      lastChar = ifs.get();
    }

    //Add line to counter
    line++;
    column = 1;

    if (!ifs.eof())
    {
      return getToken(ifs,line,column, file);
    }
  }

  //Setup line and columns
  t.line = line;
  t.column = column;

  //Used to strore name and numbers
  string word;

  //In the case that a number start with '+' or '-'
  if(lastChar == '+' || lastChar == '-')
  {
    //Save lastChar
    char ls = lastChar;
    //Add '+' or '-' to word
    word += lastChar;
    //Get next character
    lastChar = ifs.get();

    //If '+' or '-' seems linked to a number
    if(isdigit(lastChar) || lastChar == '.')
    {
      //Count the '+' or '-'
      column++;
    }
    else
    {
      //Re-insert the charcater in the stream
      ifs.unget();
      //Get back the '+' or '-'
      lastChar = ls;
      //Reset the string word
      string word;
    }
  }

  ///Word detection///
  //A word is a sequence of alphanumerical character
  //including '.' and non strating '_'
  if(isalnum(lastChar) || lastChar == '.')
  {
    //Store first character
    word += lastChar;
    //Add column to counter
    column++;
    //Get next character
    lastChar = ifs.get();
    while(isalpha(lastChar) || isdigit(lastChar) || lastChar == '_' || lastChar == '.')
    {
      word += lastChar;
      column++;
      //Get next character
      lastChar = ifs.get();
    }

    //If the next character is important, re-insert it in the stream
    if(!isspace(lastChar) && !ifs.eof())
    {
      ifs.unget();
      column--;
    }
    //Create token
    t.id = -2;//Word id
    t.word = word;

    //Update line and columns
    if(lastChar == '\n')
    {
      line++;
      column = 1;
    }
    else
    {
      column++;
    }
    return t;
  }

  ///Symbol detection///
  //A symbol can be a '(',')','{','}','+','-','*' or '/'.
  if(lastChar == '(' || lastChar == ')' || lastChar == '{' || lastChar == '}'
  || lastChar == '+' || lastChar == '-' || lastChar == '*' || lastChar == '/')
  {
    t.id = -3;//Symbol id
    t.symbol = lastChar;
    column++;
    return t;
  }

  //Check for end of file
  if(ifs.eof())
  {
    t.id = -1;
    return t;
  }

  ///Error : a character was not recognised///
  errorInFile(file, t, "Invalid character");
  return t;
}

/*
* parseNumber returns
*  - True if the following token or group of tokens are recognised as a number
*  - False otherwise
*
* Arguments :
*  - ifs : is the stream of character (that where read in the file)
*  - line : is the adress of the counter of the current line
*  - column : is the adress of the counter of the current column
*  - file : is the path to the file .paint
*  - value : is the adress of the value to return
*  - curToken : is the current Token
*
* parseNumber also updates the variable 'value' by the value of the number
* that was read (if it was correctly read)
*/
bool parseNumber(ifstream& ifs, int& line, int& column, string file, double& value, Token& curToken)
{
  //Word token that is possibly a number
  if(curToken.id == -2 && (isdigit(curToken.word[0]) || curToken.word[0] == '.'
    || curToken.word[0] == '+' || curToken.word[0] == '-'))
  {
    //Used to remember the index of the word we are looking at
    size_t i=0;

    //In case of + or -
    string num;
    if(curToken.word[0] == '+' || curToken.word[0] == '-')
    {
      num =+ curToken.word[0];
      i++;
    }

    //Case with .6 for exemple
    bool donePoint = false; //Used for checking that a number contains only one point
    if(curToken.word[0] == '.')
    {
      num += "0.";
      i++;
      donePoint = true;
    }

    //Get the size of the token (word)
    string::size_type size = curToken.word.length();
    while(i<size)
    {
      //Check that the word only contains digits and '.'
      if(isdigit(curToken.word[i]) || curToken.word[i] == '.')
      {
        num += curToken.word[i];
        if(curToken.word[i] == '.' && donePoint)
        {
          //Update the columns to point to the invalid character
          curToken.column += i;
          return false;
        }
        if(curToken.word[i] == '.' && !donePoint)
        {
          donePoint = true;
        }
      }
      else
      {
        //Update the columns to point to the invalid character
        curToken.column += i;
        return false;
      }
      i++;
    }
    //Convert the string into a double
    value = stod(num);
    return true;
  }


  //Point (that possibly is a named point) abscissa or ordinate
  //We do that to avoid sending ".x" or ".y" to a point
  if(curToken.id == -2)
  {
    double x;
    double y;
    string::size_type size = curToken.word.length();
    if(curToken.word[size-1] == 'x')
    {
      if(curToken.word[size-2] == '.')
      {
        //Create a new token
        string word = curToken.word;
        word.erase(size-2,2);
        Token t;
        t.id = -2;
        t.word = word;
        //Check that it's a valid point name
        if(parsePoint(ifs,line,column, file, t, x, y))
        {
          value = x;
          return true;
        }
      }
    }
    if(curToken.word[size-1] == 'y')
    {
      if(curToken.word[size-2] == '.')
      {
        //Create a new token
        string word = curToken.word;
        word.erase(size-2,2);
        Token t;
        t.id = -2;
        t.word = word;
        //Check that it's a valid point name
        if(parsePoint(ifs,line,column, file, t, x, y))
        {
          value = y;
          return true;
        }
      }
    }
  }

  //Point (that isn't a named point) abscissa or ordinate
  double x;
  double y;
  //Check if the token is a point
  if(parsePoint(ifs,line,column, file, curToken, x, y))
  {
    //Get next token
    curToken = getToken(ifs,line,column, file);
    if(curToken.word == ".x")
    {
      value = x;
      return true;
    }
    else if(curToken.word == ".y")
    {
      value = y;
      return true;
    }
  }

  return false;
}

/*
* parsePoint returns
*  - True if the following token or group of tokens are recognised as a point
*  - False otherwise (or may send errors)
*
* Arguments :
*  - ifs : is the stream of character (that where read in the file)
*  - line : is the adress of the counter of the current line
*  - column : is the adress of the counter of the current column
*  - file : is the path to the file .paint
*  - curToken : is the current Token
*  - x : is the adress of the absicca to return
*  - y : is the adress of the ordinate to return
*
* parsePointr also updates the variables 'x' and 'y' by the value of the absicca
* or ordinate that was read (if it was correctly read).
*
* parsePoint can send errors (that will stop the program)
*/
bool parsePoint(ifstream& ifs, int& line, int& column, string file, Token& curToken, double& x, double& y)
{
  //Normal point
  if(curToken.symbol == '{')
  {
    curToken = getToken(ifs,line,column, file);
    if(parseNumber(ifs,line,column, file, x, curToken))
    {
      curToken = getToken(ifs,line,column, file);
      if(parseNumber(ifs,line,column, file, y, curToken))
      {
        curToken = getToken(ifs,line,column, file);
        if(curToken.symbol == '}')
        {
          return true;
        }
      }
    }
  }

  //Named point that looks like a name
  if(curToken.id == -2 && isalpha(curToken.word[0]))
  {
    string::size_type size = curToken.word.length();
    string shapeName;
    string pointName;
    size_t i=0;
    size_t k=0;

    //Get SHAPE_NAME
    while(i<size)
    {
      if(curToken.word[i] == '.')
      {
        i++;
        k=i;
        break;
      }
      else
      {
        shapeName += curToken.word[i];
      }
      i++;
    }

    //create token of SHAPE_NAME
    Token tokShapeName;
    tokShapeName.id = -2;
    tokShapeName.word = shapeName;
    tokShapeName.line = curToken.line;
    tokShapeName.column = curToken.column;

    //Verify that SHAPE_NAME is valid
    if(!parseName(tokShapeName, listShapeName))
    {
      errorInFile(file, tokShapeName, "Invalid point : Wrong shape name");
    }

    //Get POINT_NAME
    while(i<size)
    {
      pointName += curToken.word[i];
      i++;
    }

    //create token of POINT_NAME
    Token tokPointName;
    tokPointName.id = -2;
    tokPointName.word = pointName;
    tokPointName.line = curToken.line;
    tokPointName.column = curToken.column + k;

    //Verify that POINT_NAME is a name
    if(!validName(tokPointName))
    {
      cout << tokPointName.word << endl;
      errorInFile(file, tokPointName, "Invalid point : Wrong point name");
    }
    return true;
  }

  //Point expression
  if(curToken.symbol == '(')
  {
    curToken = getToken(ifs,line,column, file);
    //Addition or substraction
    if(curToken.symbol == '+' || curToken.symbol == '-')
    {
      curToken = getToken(ifs,line,column, file);
      double x;
      double y;
      if(parsePoint(ifs,line,column, file, curToken, x, y))
      {
        curToken = getToken(ifs,line,column, file);
        while(curToken.symbol != ')')
        {
          if(!parsePoint(ifs,line,column, file, curToken, x, y))
          {
            errorInFile(file, curToken, "Invalid point expression : expected \")\" or point");
          }
          curToken = getToken(ifs,line,column, file);
        }
        return true;
      }
      else
      {
        errorInFile(file, curToken, "Invalid point expression : expected point");
      }
    }
    //Multiplication or division
    else if(curToken.symbol == '*' || curToken.symbol == '/')
    {
      curToken = getToken(ifs,line,column, file);
      double x;
      double y;
      double number;
      if(parsePoint(ifs,line,column, file, curToken, x, y))
      {
        curToken = getToken(ifs,line,column, file);
        if(parseNumber(ifs,line,column, file, number, curToken))
        {
          curToken = getToken(ifs,line,column, file);
          if(curToken.symbol == ')')
          {
            return true;
          }
          else
          {
            errorInFile(file, curToken, "Invalid point expression : expected \")\"");
          }
        }
        else
        {
          errorInFile(file, curToken, "Invalid point expression : expected number");
        }
      }
      else
      {
        errorInFile(file, curToken, "Invalid point expression : expected point");
      }
    }
    else
    {
      errorInFile(file, curToken, "Invalid point expression : expected operator (+,-,* or /)");
    }
  }

  return false;
}

/*
* validName returns
*  - True if the curToken is are recognised as a name
*  - False otherwise
*
* Arguments :
*  - curToken : is the current Token
*/
bool validName(Token curToken)
{
  //Check that token is word
  if(curToken.id == -2)
  {
    //get word size
    string::size_type size = curToken.word.length();
    size_t i=0;
    //Check that name starts with a letter
    if(isalpha(curToken.word[0]))
    {
      i++;
      while(i<size)
      {
        //Check that name contains only letter, digits or '_'
        if(!isalpha(curToken.word[i]) && !isdigit(curToken.word[i]) && !(curToken.word[i] == '_'))
        {
          //Update the columns to point to the invalid character
          curToken.column +=i;
          return false;
        }
        i++;
      }
      return true;
    }
  }
  return false;
}

/*
* parseNewName returns
*  - True if the curToken is recognised as a name that isn't already taken
*  - False otherwise
*
* Arguments :
*  - file : is the path to the file .paint
*  - curToken : is the current Token
*  - list : is the list of Token that contains name that were defined (shapes or colors)
*  - isShapeName : is a boolean that is
*                       - true if the list is the one for shapes
*                       - false if the list is the one for colors
*
* parseNewName can send personnalised errors
*/
bool parseNewName(string file, Token& curToken, vector<Token>& list, bool isShapeName)
{
  //Check for valid name
  if(validName(curToken))
  {
    //Check that the name is not already defined
    for(Token t : list)
    {
      if(t.word == curToken.word)
      {
        if(isShapeName)
        {
          string error =  " Shape name \"" + t.word + "\" already taken at " + t.positionStr();
          cerr << file << ":" << t.positionStr() << ": error:" << error << endl;
          exit(EXIT_FAILURE);
        }
        else
        {
          string error = " Color name \"" + t.word + "\" already taken at " + t.positionStr();
          cerr << file << ":" << t.positionStr() << ": error:" << error << endl;
          exit(EXIT_FAILURE);
        }
      }
    }
    return true;
  }
  return false;
}

/*
* parseName returns
*  - True if the curToken is recognised as a name that is already defined
*  - False otherwise
*
* Arguments :
*  - curToken : is the current Token
*  - list : is the list of Token that contains name that were defined (shapes or colors)
*/
bool parseName(Token& curToken, vector<Token>& list)
{
  //Check for valid name
  if(validName(curToken))
  {
    //Check that the name is already defined
    for(Token t : list)
    {
      if(t.word == curToken.word)
      {
        return true;
      }
    }
  }
  return false;
}

/*
* parseColor returns
*  - True if the following token or group of tokens are recognised as a color
*  - False otherwise (or may send errors)
*
* Arguments :
*  - ifs : is the stream of character (that where read in the file)
*  - line : is the adress of the counter of the current line
*  - column : is the adress of the counter of the current column
*  - file : is the path to the file .paint
*  - curToken : is the current Token
*
* parseColor can send errors (that will stop the program)
*/
bool parseColor(ifstream& ifs, int& line, int& column, string file, Token& curToken)
{
  //Check for a new definition of a color
  if(curToken.symbol == '{')
  {
    double r;
    double g;
    double b;
    curToken = getToken(ifs,line,column, file);
    if(parseNumber(ifs,line,column, file, r, curToken))
    {
      curToken = getToken(ifs,line,column, file);
      if(parseNumber(ifs,line,column, file, g, curToken))
      {
        curToken = getToken(ifs,line,column, file);
        if(parseNumber(ifs,line,column, file, b, curToken))
        {
          curToken = getToken(ifs,line,column, file);
          if(curToken.symbol == '}')
          {
            return true;
          }
          else
          {
            errorInFile(file, curToken, "Invalid color : expected \"}\"");
          }
        }
      }
    }
    errorInFile(file, curToken, "Invalid color : expected number in color : {number,number,number}");
  }

  //Check if current token is the name of an existing color
  if(parseName(curToken, listColorName))
  {
    return true;
  }
  return false;
}



int main(int argc, char** argv)
{
  //Check for right number of arguments
  if(argc!= 2)
  {
    cerr << "Error : Invalid number of arguments" << endl;
    return 1;
  }

  //Recover input file
  string inputFile = argv[1];

  //Open file
  ifstream ifs;
  ifs.open(inputFile, ifstream::in);
  if(ifs.fail())
  {
    cerr << "Error : Invalid path to file" << endl;
    return 1;
  }

  //Line and column
  int line = 1;
  int column = 1;

///Check for size_command///
//Check for size
Token curToken = getToken(ifs,line,column, inputFile);
if(curToken.id != -3 && curToken.word != "size")
{
  errorInFile(inputFile, curToken, "Expected \"size\" command");
}

//Check for width
double width;
curToken = getToken(ifs,line,column, inputFile);
if(!parseNumber(ifs,line,column, inputFile, width, curToken))
{
errorInFile(inputFile, curToken, "WIDTH incorrect or not found in size_command");
}

//Check for height
double height;
curToken = getToken(ifs,line,column, inputFile);
if(!parseNumber(ifs,line,column, inputFile, height, curToken))
{
errorInFile(inputFile, curToken, "HEIGHT incorrect or not found in size_command");
}

//Setup counter of shape, color and fill
int nbrShape=0;
int nbrColor=0;
int nbrFill=0;

////Check for definitions////
while(true)
{
  //Get the next token
  curToken = getToken(ifs,line,column, inputFile);

  //Check for end of file
  if(curToken.id == -1)
  {
    break;
  }

  ///Color def///
  if(curToken.word == "color")
  {
    //Get the next token
    curToken = getToken(ifs,line,column, inputFile);
    //Save color name
    Token colorName = curToken.copy();
    //Register new color name
    if(parseNewName(inputFile, curToken, listColorName, false))
    {
      curToken = getToken(ifs,line,column, inputFile);
      if(!parseColor(ifs,line,column, inputFile, curToken))
      {
        errorInFile(inputFile, curToken, "Color definition failed : Invalid color");
      }
    }
    else
    {
      errorInFile(inputFile, curToken, "Color definition failed : Invalid color name");
    }

    //Add color name to to the list of existing color
    listColorName.push_back(colorName);
    nbrColor++;
  }
  ///Shape def///
  else if(curToken.word == "circ")
  {
    //Get new token
    curToken = getToken(ifs,line,column, inputFile);
    //Save shape name
    Token shapeName = curToken.copy();
    //Register new shape name
    if(parseNewName(inputFile, curToken, listShapeName, true))
    {
      //Get new token
      curToken = getToken(ifs,line,column, inputFile);
      //Recover point absicca and ordiante
      // (Not working yet but might be usefull for project 3)
      double x;
      double y;
      //Check for point (center)
      if(parsePoint(ifs,line,column, inputFile, curToken, x, y))
      {
        //Get new token
        curToken = getToken(ifs,line,column, inputFile);
        double number;
        //Check for number (radius)
        if(!parseNumber(ifs,line,column, inputFile, number, curToken))
        {
          errorInFile(inputFile, curToken, "Circle definition failed : Invalid number : Radius");
        }
      }
      else
      {
        errorInFile(inputFile, curToken, "Circle definition failed : Invalid point : Center");
      }
    }
    else
    {
      errorInFile(inputFile, curToken, "Circle definition failed : Invalid shape name");
    }

    //Add shape name to to the list of existing shape
    listShapeName.push_back(shapeName);
    nbrShape++;
  }
  else if(curToken.word == "rect")
  {
    //Get new token
    curToken = getToken(ifs,line,column, inputFile);
    //Save shape name
    Token shapeName = curToken.copy();
    //Register new shape name
    if(parseNewName(inputFile, curToken, listShapeName, true))
    {
      //Get new token
      curToken = getToken(ifs,line,column, inputFile);
      //Recover point absicca and ordiante
      // (Not working yet but might be usefull for project 3)
      double x;
      double y;
      //Check for point (center)
      if(parsePoint(ifs,line,column, inputFile, curToken, x, y))
      {
        //Get new token
        curToken = getToken(ifs,line,column, inputFile);
        double width;
        //Check for number (width)
        if(parseNumber(ifs,line,column, inputFile, width, curToken))
        {
          //Get new token
          curToken = getToken(ifs,line,column, inputFile);
          double height;
          //Check for number (height)
          if(!parseNumber(ifs,line,column, inputFile, height, curToken))
          {
            errorInFile(inputFile, curToken, "Rectangle definition failed : Invalid number : Height");
          }
        }
        else
        {
          errorInFile(inputFile, curToken, "Rectangle definition failed : Invalid number : Width");
        }
      }
      else
      {
        errorInFile(inputFile, curToken, "Rectangle definition failed : Invalid point : Center");
      }
    }
    else
    {
      errorInFile(inputFile, curToken, "Rectangle definition failed : Invalid shape name");
    }

    //Add shape name to to the list of existing shape
    listShapeName.push_back(shapeName);
    nbrShape++;
  }
  else if(curToken.word == "tri")
  {
    //Get new token
    curToken = getToken(ifs,line,column, inputFile);
    //Save shape name
    Token shapeName = curToken.copy();
    //Register new shape name
    if(parseNewName(inputFile, curToken, listShapeName, true))
    {
      //Get new token
      curToken = getToken(ifs,line,column, inputFile);
      //Recover point absicca and ordiante
      // (Not working yet but might be usefull for project 3)
      double P1_x;
      double P1_y;
      //Check for point1 (vertice)
      if(parsePoint(ifs,line,column, inputFile, curToken, P1_x, P1_y))
      {
        //Get new token
        curToken = getToken(ifs,line,column, inputFile);
        //Recover point absicca and ordiante
        // (Not working yet but might be usefull for project 3)
        double P2_x;
        double P2_y;
        //Check for point2 (vertice)
        if(parsePoint(ifs,line,column, inputFile, curToken, P2_x, P2_y))
        {
          //Get new token
          curToken = getToken(ifs,line,column, inputFile);
          //Recover point absicca and ordiante
          // (Not working yet but might be usefull for project 3)
          double P3_x;
          double P3_y;
          //Check for point3 (vertice)
          if(!parsePoint(ifs,line,column, inputFile, curToken, P3_x, P3_y))
          {
            errorInFile(inputFile, curToken, "Triangle definition failed : Invalid point : Vertice 3");
          }
        }
        else
        {
          errorInFile(inputFile, curToken, "Triangle definition failed : Invalid point : Vertice 2");
        }
      }
      else
      {
        errorInFile(inputFile, curToken, "Triangle definition failed : Invalid point : Vertice 1");
      }
    }
    else
    {
      errorInFile(inputFile, curToken, "Triangle definition failed : Invalid shape name");
    }

    //Add shape name to to the list of existing shape
    listShapeName.push_back(shapeName);
    nbrShape++;
  }
  else if(curToken.word == "shift")
  {
    //Get new token
    curToken = getToken(ifs,line,column, inputFile);
    //Save shape name
    Token shapeName = curToken.copy();
    //Register new shape name
    if(parseNewName(inputFile, curToken, listShapeName, true))
    {
      //Get new token
      curToken = getToken(ifs,line,column, inputFile);
      //Recover point absicca and ordiante
      // (Not working yet but might be usefull for project 3)
      double x;
      double y;
      //Check for point (translation vector)
      if(parsePoint(ifs,line,column, inputFile, curToken, x, y))
      {
        //Get new token
        curToken = getToken(ifs,line,column, inputFile);
        //Check for shape name (original shape to shift)
        if(!parseName(curToken, listShapeName))
        {
          errorInFile(inputFile, curToken, "Shift definition failed : Invalid original shape name");
        }
      }
      else
      {
        errorInFile(inputFile, curToken, "Shift definition failed : Invalid point : Translation vector");
      }
    }
    else
    {
      errorInFile(inputFile, curToken, "Shift definition failed : Invalid new shape name");
    }

    //Add shape name to to the list of existing shape
    listShapeName.push_back(shapeName);
    nbrShape++;
  }
  else if(curToken.word == "rot")
  {
    //Get new token
    curToken = getToken(ifs,line,column, inputFile);
    //Save shape name
    Token shapeName = curToken.copy();
    //Register new shape name
    if(parseNewName(inputFile, curToken, listShapeName, true))
    {
      //Get new token
      curToken = getToken(ifs,line,column, inputFile);
      double degrees;
      //Check number (degrees)
      if(parseNumber(ifs,line,column, inputFile, degrees, curToken))
      {
        //Get new token
        curToken = getToken(ifs,line,column, inputFile);
        //Recover point absicca and ordiante
        // (Not working yet but might be usefull for project 3)
        double x;
        double y;
        //Check for point (translation vector)
        if(parsePoint(ifs,line,column, inputFile, curToken, x, y))
        {
          //Get new token
          curToken = getToken(ifs,line,column, inputFile);
          //Check for shape name (original shape to shift)
          if(!parseName(curToken, listShapeName))
          {
            errorInFile(inputFile, curToken, "Rotation definition failed : Invalid original shape name");
          }
        }
        else
        {
          errorInFile(inputFile, curToken, "Rotation definition failed : Invalid point : Rotation point");
        }
      }
      else
      {
        errorInFile(inputFile, curToken, "Rotation definition failed : Invalid number : Degrees");
      }
    }
    else
    {
      errorInFile(inputFile, curToken, "Rotation definition failed : Invalid new shape name");
    }

    //Add shape name to to the list of existing shape
    listShapeName.push_back(shapeName);
    nbrShape++;
  }
  else if(curToken.word == "union")
  {
    //Get new token
    curToken = getToken(ifs,line,column, inputFile);
    //Save shape name
    Token shapeName = curToken.copy();
    //Register new shape name
    if(parseNewName(inputFile, curToken, listShapeName, true))
    {
      //Get new token
      curToken = getToken(ifs,line,column, inputFile);
      //Check for {
      if(curToken.symbol == '{')
      {
        //Get new token
        curToken = getToken(ifs,line,column, inputFile);
        //Check for first shape name (first shape to unite)
        if(parseName(curToken, listShapeName))
        {
          //Get new token
          curToken = getToken(ifs,line,column, inputFile);
          //Check for end of union
          while(curToken.symbol != '}')
          {
            //Check for valid shape name
            if(!parseName(curToken, listShapeName))
            {
              errorInFile(inputFile, curToken, "Union definition failed : Expected valid shape name or \"}\"");
            }
            //Get new token
            curToken = getToken(ifs,line,column, inputFile);
          }
        }
        else
        {
          errorInFile(inputFile, curToken, "Union definition failed : Invalid first shape name");
        }
      }
      else
      {
        errorInFile(inputFile, curToken, "Union definition failed : Expected \"{\"");
      }
    }
    else
    {
      errorInFile(inputFile, curToken, "Union definition failed : Invalid new shape name");
    }

    //Add shape name to to the list of existing shape
    listShapeName.push_back(shapeName);
    nbrShape++;
  }
  else if(curToken.word == "diff")
  {
    //Get new token
    curToken = getToken(ifs,line,column, inputFile);
    //Save shape name
    Token shapeName = curToken.copy();
    //Register new shape name
    if(parseNewName(inputFile, curToken, listShapeName, true))
    {
      //Get new token
      curToken = getToken(ifs,line,column, inputFile);
      //Save first shape
      Token firstShape = curToken.copy();
      //Check first shape name (which to subtract from)
      if(parseName(curToken, listShapeName))
      {
        //Get new token
        curToken = getToken(ifs,line,column, inputFile);
        //Check first shape name (which to subtract from)
        if(parseName(curToken, listShapeName))
        {
          //Check that the two shapes are different
          if(firstShape.word == curToken.word)
          {
            errorInFile(inputFile, curToken, "Difference definition failed : Shapes must be different (Can not substract itself)");
          }
        }
        else
        {
          errorInFile(inputFile, curToken, "Difference definition failed : Invalid second shape name (the one to be subtracted)");
        }
      }
      else
      {
        errorInFile(inputFile, curToken, "Difference definition failed : Invalid first shape name (the one of which to subtract from)");
      }
    }
    else
    {
      errorInFile(inputFile, curToken, "Difference definition failed : Invalid new shape name");
    }

    //Add shape name to to the list of existing shape
    listShapeName.push_back(shapeName);
    nbrShape++;
  }
  ///Paint command///
  else if(curToken.word == "fill")
  {
    curToken = getToken(ifs,line,column, inputFile);
    //Register new shape name
    if(parseName(curToken, listShapeName))
    {
      curToken = getToken(ifs,line,column, inputFile);
      if(!parseColor(ifs,line,column, inputFile, curToken))
      {
        errorInFile(inputFile, curToken, "Painting command failed : Invalid color");
      }
    }
    else
    {
      errorInFile(inputFile, curToken, "Painting command failed : Invalid shape name");
    }

    nbrFill++;
  }
  ///Not a valid command///
  else
  {
    errorInFile(inputFile, curToken, "Invalid command");
  }
}

//Close file
ifs.close();

//Print status
cout << "Number of shapes: " << nbrShape << endl;
cout << "Number of colors: " << nbrColor << endl;
cout << "Number of fills: " << nbrFill << endl;


return 0;
}
