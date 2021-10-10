#include <iostream>
#include <string>
#include <fstream>
#include <memory>
#include <vector>
#include <map>
#include <utility>

#include "Token.hpp"
#include "image.hh"
#include "shape.hpp"
#include "color.hh"
#include "point.hpp"
#include "circle.hpp"
#include "rectangle.hpp"
#include "ellipse.hpp"
#include "triangle.hpp"
#include "shift.hpp"
#include "rotation.hpp"
#include "union.hpp"
#include "difference.hpp"
#include "fill.hpp"

using namespace std;

static map<string,pair<Token,shared_ptr<Shape>>> listShapeName;//List of shape that are defined
static map<string,pair<Token,shared_ptr<Color>>> listColorName;//List of color that are defined
static vector<shared_ptr<Fill>> listOfFill;//List of fills that are defined

void errorInFile(string file, Token t, string error);
Token getToken(ifstream& ifs, int& line, int& column, string file);
bool parsePoint(ifstream& ifs, int& line, int& column, string file, Token& curToken, double& x, double& y);
bool parseNewShapeName(string file, Token& curToken, map<string,pair<Token,shared_ptr<Shape>>>& list);
bool parseNewColorName(string file, Token& curToken, map<string,pair<Token,shared_ptr<Color>>>& list);
bool parseShapeName(Token& curToken, map<string,pair<Token,shared_ptr<Shape>>>& list, shared_ptr<Shape>& s);
bool parseColorName(Token& curToken, map<string, pair<Token,shared_ptr<Color>>>& list, shared_ptr<Color>& c);
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

    shared_ptr<Shape> s;
    //Verify that SHAPE_NAME is valid
    if(!parseShapeName(tokShapeName, listShapeName, s))
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
      errorInFile(file, tokPointName, "Invalid point : Wrong point name");
    }

    //Get named Point
    Point np = s->namedPoint(pointName);
    //Check that it is a valid point name
    double inf = std::numeric_limits<double>::infinity();
    if(np.x()==inf && np.y()==inf)
    {
      errorInFile(file, tokPointName, "Invalid point : Wrong point name");
    }
    x = np.x();
    y = np.y();
    return true;
  }

  //Point expression
  if(curToken.symbol == '(')
  {
    curToken = getToken(ifs,line,column, file);
    //Addition
    if(curToken.symbol == '+')
    {
      curToken = getToken(ifs,line,column, file);
      double cx;
      double cy;
      if(parsePoint(ifs,line,column, file, curToken, cx, cy))
      {
        double addx = cx;
        double addy = cy;
        curToken = getToken(ifs,line,column, file);
        while(curToken.symbol != ')')
        {
          if(!parsePoint(ifs,line,column, file, curToken, cx, cy))
          {
            errorInFile(file, curToken, "Invalid point expression : expected \")\" or point");
          }
          addx += cx;
          addy += cy;
          curToken = getToken(ifs,line,column, file);
        }
        x = addx;
        y = addy;
        return true;
      }
      else
      {
        errorInFile(file, curToken, "Invalid point expression : expected point");
      }
    }
    //Substraction
    else if(curToken.symbol == '-')
    {
      curToken = getToken(ifs,line,column, file);
      double cx;
      double cy;
      if(parsePoint(ifs,line,column, file, curToken, cx, cy))
      {
        double subx = cx;
        double suby = cy;
        curToken = getToken(ifs,line,column, file);
        while(curToken.symbol != ')')
        {
          if(!parsePoint(ifs,line,column, file, curToken, cx, cy))
          {
            errorInFile(file, curToken, "Invalid point expression : expected \")\" or point");
          }
          subx -= cx;
          suby -= cy;
          curToken = getToken(ifs,line,column, file);
        }
        x = subx;
        y = suby;
        return true;
      }
      else
      {
        errorInFile(file, curToken, "Invalid point expression : expected point");
      }
    }
    //Multiplication
    else if(curToken.symbol == '*')
    {
      curToken = getToken(ifs,line,column, file);
      double cx;
      double cy;
      double number;
      if(parsePoint(ifs,line,column, file, curToken, cx, cy))
      {
        curToken = getToken(ifs,line,column, file);
        if(parseNumber(ifs,line,column, file, number, curToken))
        {
          curToken = getToken(ifs,line,column, file);
          if(curToken.symbol == ')')
          {
            x = cx*number;
            y = cy*number;
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
    //Division
    else if(curToken.symbol == '/')
    {
      curToken = getToken(ifs,line,column, file);
      double cx;
      double cy;
      double number;
      if(parsePoint(ifs,line,column, file, curToken, cx, cy))
      {
        curToken = getToken(ifs,line,column, file);
        if(parseNumber(ifs,line,column, file, number, curToken))
        {
          if(number==0)
          {
            errorInFile(file, curToken, "Invalid point expression : can not divide by 0 \")\"");
          }
          curToken = getToken(ifs,line,column, file);
          if(curToken.symbol == ')')
          {
            x = cx/number;
            y = cy/number;
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
* parseNewShapeName returns
*  - True if the curToken is recognised as a name that isn't already taken
*  - False otherwise
*
* Arguments :
*  - file : is the path to the file .paint
*  - curToken : is the current Token
*  - list : is the map of Token that contains name that were defined (shapes or colors)
* parseNewName can send personnalised errors
*/
bool parseNewShapeName(string file, Token& curToken, map<string,pair<Token,shared_ptr<Shape>>>& list)
{
  //Check for valid name
  if(validName(curToken))
  {
    auto x = list.find(curToken.word);
    //Check that the name is not already defined
    if(x!=list.end())
    {
      Token t = x->second.first;
      string error =  " Shape name \"" + t.word + "\" already taken at " + t.positionStr();
      cerr << file << ":" << t.positionStr() << ": error:" << error << endl;
      exit(EXIT_FAILURE);
    }
    return true;
  }
  return false;
}

/*
* parseNewColorName returns
*  - True if the curToken is recognised as a color name that isn't already taken
*  - False otherwise
*
* Arguments :
*  - file : is the path to the file .paint
*  - curToken : is the current Token
*  - list : is the map of Token that contains name that were defined (shapes or colors)
*
* parseNewColorName can send personnalised errors
*/
bool parseNewColorName(string file, Token& curToken, map<string,pair<Token,shared_ptr<Color>>>& list)
{
  //Check for valid name
  if(validName(curToken))
  {
    auto x = list.find(curToken.word);
    //Check that the name is not already defined
    if(x!=list.end())
    {
      Token t = x->second.first;
      string error = " Color name \"" + t.word + "\" already taken at " + t.positionStr();
      cerr << file << ":" << t.positionStr() << ": error:" << error << endl;
      exit(EXIT_FAILURE);
    }
    return true;
  }
  return false;
}

/*
* parseShapeName returns
*  - True if the curToken is recognised as a name that is already defined
*  - False otherwise
*
* Arguments :
*  - curToken : is the current Token
*  - list : is the map of Token that contains name that were defined (shapes or colors)
*/
bool parseShapeName(Token& curToken, map<string,pair<Token,shared_ptr<Shape>>>& list, shared_ptr<Shape>& s)
{
  //Check for valid name
  if(validName(curToken))
  {
    auto x = list.find(curToken.word);
    //Check that the name is already defined
    if(x!=list.end())
    {
      s = x->second.second;
      return true;
    }
  }
  return false;
}

/*
* parseColorName returns
*  - True if the curToken is recognised as a name that is already defined
*  - False otherwise
*
* Arguments :
*  - curToken : is the current Token
*  - list : is the map of Token that contains name that were defined (shapes or colors)
*/
bool parseColorName(Token& curToken, map<string,pair<Token,shared_ptr<Color>>>& list, shared_ptr<Color>& c)
{
  //Check for valid name
  if(validName(curToken))
  {
    auto x = list.find(curToken.word);
    //Check that the name is already defined
    if(x!=list.end())
    {
      c = x->second.second;
      return true;
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
bool parseColor(ifstream& ifs, int& line, int& column, string file, Token& curToken, shared_ptr<Color>& c)
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
      if(r<0.0 || r>1.0)
      {
        errorInFile(file, curToken, "Invalid color : red should be 0<=red<=1 ");
      }
      curToken = getToken(ifs,line,column, file);
      if(parseNumber(ifs,line,column, file, g, curToken))
      {
        if(g<0.0 || g>1.0)
        {
          errorInFile(file, curToken, "Invalid color : green should be 0<=green<=1 ");
        }
        curToken = getToken(ifs,line,column, file);
        if(parseNumber(ifs,line,column, file, b, curToken))
        {
          if(b<0.0 || b>1.0)
          {
            errorInFile(file, curToken, "Invalid color : blue should be 0<=blue<=1 ");
          }
          curToken = getToken(ifs,line,column, file);
          if(curToken.symbol == '}')
          {
            c = make_shared<Color>(uint8_t(std::round(r * 255)), uint8_t(std::round(g * 255)), uint8_t(std::round(b * 255)));
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
  if(parseColorName(curToken, listColorName, c))
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

  if(inputFile.rfind(".paint")==string::npos)
  {
    cerr << "Error : Invalid path to file : Expected .paint" << endl;
    return 1;
  }
  ifs.open(inputFile, ifstream::in);
  if(ifs.fail())
  {
    cerr << "Error : Invalid path to file" << endl;
    return 1;
  }

  inputFile.resize(inputFile.size()-5);

  //Write a file
  ofstream ofs;
  ofs.open(inputFile + "ppm", std::ios::binary);
  if(ofs.fail())
  {
    cerr << "Error : Unable to write" << endl;
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
  double w;
  double wint;
  curToken = getToken(ifs,line,column, inputFile);
  if(!parseNumber(ifs,line,column, inputFile, w, curToken))
  {
    errorInFile(inputFile, curToken, "WIDTH incorrect or not found in size_command");
  }

  if(modf(w,&wint)!=0.0)
  {
    errorInFile(inputFile, curToken, "WIDTH must be an integer");
  }
  size_t width = (size_t) wint;

  //Check for height
  double h;
  double hint;
  curToken = getToken(ifs,line,column, inputFile);
  if(!parseNumber(ifs,line,column, inputFile, h, curToken))
  {
    errorInFile(inputFile, curToken, "HEIGHT incorrect or not found in size_command");
  }

  if(modf(h,&hint)!=0.0)
  {
    errorInFile(inputFile, curToken, "HEIGHT must be an integer");
  }
  size_t height = (size_t) hint;

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
      shared_ptr<Color> c;
      //Get the next token
      curToken = getToken(ifs,line,column, inputFile);
      //Save color name
      Token colorName = curToken.copy();
      //Register new color name
      if(parseNewColorName(inputFile, curToken, listColorName))
      {
        curToken = getToken(ifs,line,column, inputFile);
        if(!parseColor(ifs,line,column, inputFile, curToken, c))
        {
          errorInFile(inputFile, curToken, "Color definition failed : Invalid color");
        }
      }
      else
      {
        errorInFile(inputFile, curToken, "Color definition failed : Invalid color name");
      }

      //Add color name to to the list of existing color
      pair<Token,shared_ptr<Color>> p(colorName, c);
      listColorName[colorName.word] = p;
      nbrColor++;
    }
    ///Shape def///
    else if(curToken.word == "circ")
    {
      shared_ptr<Shape> circ;
      //Get new token
      curToken = getToken(ifs,line,column, inputFile);
      //Save shape name
      Token shapeName = curToken.copy();
      //Register new shape name
      if(parseNewShapeName(inputFile, curToken, listShapeName))
      {
        //Get new token
        curToken = getToken(ifs,line,column, inputFile);
        //Recover point absicca and ordiante
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
          if(number<0)
          {
            errorInFile(inputFile, curToken, "Circle definition failed : Invalid number : Radius should be >= 0");
          }
          else
          {
            //Create circle
            circ = make_shared<Circle> (Point(x,y), number);
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
      pair<Token,shared_ptr<Shape>> p(shapeName, circ);
      listShapeName[shapeName.word]=p;
      nbrShape++;
    }
    else if(curToken.word == "elli")
    {
      shared_ptr<Shape> elli;
      //Get new token
      curToken = getToken(ifs,line,column, inputFile);
      //Save shape name
      Token shapeName = curToken.copy();
      //Register new shape name
      if(parseNewShapeName(inputFile, curToken, listShapeName))
      {
        //Get new token
        curToken = getToken(ifs,line,column, inputFile);
        //Recover point absicca and ordiante
        double x;
        double y;
        //Check for point (center)
        if(parsePoint(ifs,line,column, inputFile, curToken, x, y))
        {
          //Get new token
          curToken = getToken(ifs,line,column, inputFile);
          double ra;
          //Check for number (semi-major radius a)
          if(parseNumber(ifs,line,column, inputFile, ra, curToken))
          {
            if(ra<0)
            {
              errorInFile(inputFile, curToken, "Ellipse definition failed : semi-major radius a should be >=0");
            }
            //Get new token
            curToken = getToken(ifs,line,column, inputFile);
            double rb;
            //Check for number (semi-major radius b)
            if(!parseNumber(ifs,line,column, inputFile, rb, curToken))
            {
              errorInFile(inputFile, curToken, "Ellipse definition failed : Invalid number : semi-major radius b");
            }
            if(rb<0)
            {
              errorInFile(inputFile, curToken, "Ellipse definition failed : semi-major radius b should be >=0");
            }
            if(ra<rb)
            {
              errorInFile(inputFile, curToken, "Ellipse definition failed : semi-major radius a should >= than semi-major radius b");
            }
            else
            {
              //Create rectangle
              elli = make_shared<Ellipse> (Point(x,y), ra, rb);
            }
          }
          else
          {
            errorInFile(inputFile, curToken, "Ellipse definition failed : Invalid number : semi-major radius a");
          }
        }
        else
        {
          errorInFile(inputFile, curToken, "Ellipse definition failed : Invalid point : Center");
        }
      }
      else
      {
        errorInFile(inputFile, curToken, "Ellipse definition failed : Invalid shape name");
      }

      //Add shape name to to the list of existing shape
      pair<Token,shared_ptr<Shape>> p(shapeName, elli);
      listShapeName[shapeName.word]=p;
      nbrShape++;
    }
    else if(curToken.word == "rect")
    {
      shared_ptr<Shape> rect;
      //Get new token
      curToken = getToken(ifs,line,column, inputFile);
      //Save shape name
      Token shapeName = curToken.copy();
      //Register new shape name
      if(parseNewShapeName(inputFile, curToken, listShapeName))
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
            if(width<0)
            {
              errorInFile(inputFile, curToken, "Rectangle definition failed : width should be >=0");
            }
            //Get new token
            curToken = getToken(ifs,line,column, inputFile);
            double height;
            //Check for number (height)
            if(!parseNumber(ifs,line,column, inputFile, height, curToken))
            {
              errorInFile(inputFile, curToken, "Rectangle definition failed : Invalid number : Height");
            }
            if(height<0)
            {
              errorInFile(inputFile, curToken, "Rectangle definition failed : height should be >=0");
            }
            else
            {
              //Create rectangle
              rect = make_shared<Rectangle> (Point(x,y), width, height);
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
      pair<Token,shared_ptr<Shape>> p(shapeName, rect);
      listShapeName[shapeName.word]=p;
      nbrShape++;
    }
    else if(curToken.word == "tri")
    {
      shared_ptr<Shape> tri;
      //Get new token
      curToken = getToken(ifs,line,column, inputFile);
      //Save shape name
      Token shapeName = curToken.copy();
      //Register new shape name
      if(parseNewShapeName(inputFile, curToken, listShapeName))
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

            //Create triangle
            tri = make_shared<Triangle> (Point(P1_x,P1_y), Point(P2_x,P2_y), Point(P3_x,P3_y));
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
      pair<Token,shared_ptr<Shape>> p(shapeName, tri);
      listShapeName[shapeName.word]=p;
      nbrShape++;
    }
    else if(curToken.word == "shift")
    {
      shared_ptr<Shape> shift;
      shared_ptr<Shape> toShift;
      //Get new token
      curToken = getToken(ifs,line,column, inputFile);
      //Save shape name
      Token shapeName = curToken.copy();
      //Register new shape name
      if(parseNewShapeName(inputFile, curToken, listShapeName))
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
          if(!parseShapeName(curToken, listShapeName, toShift))
          {
            errorInFile(inputFile, curToken, "Shift definition failed : Invalid original shape name");
          }
          //Create shift
          shift = make_shared<Shift> (Point(x,y), toShift);
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
      pair<Token,shared_ptr<Shape>> p(shapeName, shift);
      listShapeName[shapeName.word]=p;
      nbrShape++;
    }
    else if(curToken.word == "rot")
    {
      shared_ptr<Shape> rot;
      shared_ptr<Shape> toRot;
      //Get new token
      curToken = getToken(ifs,line,column, inputFile);
      //Save shape name
      Token shapeName = curToken.copy();
      //Register new shape name
      if(parseNewShapeName(inputFile, curToken, listShapeName))
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
            if(!parseShapeName(curToken, listShapeName, toRot))
            {
              errorInFile(inputFile, curToken, "Rotation definition failed : Invalid original shape name");
            }
            //Create rotation
            rot = make_shared<Rotation> (degrees, Point(x,y), toRot);
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
      pair<Token,shared_ptr<Shape>> p(shapeName, rot);
      listShapeName[shapeName.word]=p;
      nbrShape++;
    }
    else if(curToken.word == "union")
    {
      shared_ptr<Shape> un;
      vector<shared_ptr<Shape>> group;
      //Get new token
      curToken = getToken(ifs,line,column, inputFile);
      //Save shape name
      Token shapeName = curToken.copy();
      //Register new shape name
      if(parseNewShapeName(inputFile, curToken, listShapeName))
      {
        //Get new token
        curToken = getToken(ifs,line,column, inputFile);
        //Check for {
        if(curToken.symbol == '{')
        {
          int nbrShape = 0;
          group.resize(nbrShape+1);
          //Get new token
          curToken = getToken(ifs,line,column, inputFile);
          //Check for first shape name (first shape to unite)
          if(parseShapeName(curToken, listShapeName, group.at(nbrShape)))
          {
            nbrShape++;
            //Get new token
            curToken = getToken(ifs,line,column, inputFile);
            //Check for end of union
            while(curToken.symbol != '}')
            {
              group.resize(nbrShape+1);
              //Check for valid shape name
              if(!parseShapeName(curToken, listShapeName, group.at(nbrShape)))
              {
                errorInFile(inputFile, curToken, "Union definition failed : Expected valid shape name or \"}\"");
              }
              nbrShape++;
              //Get new token
              curToken = getToken(ifs,line,column, inputFile);
            }
            //Create union
            un = make_shared<Union> (group);
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
      pair<Token,shared_ptr<Shape>> p(shapeName, un);
      listShapeName[shapeName.word]=p;
      nbrShape++;
    }
    else if(curToken.word == "diff")
    {
      shared_ptr<Shape> diff;
      shared_ptr<Shape> s1;
      shared_ptr<Shape> s2;
      //Get new token
      curToken = getToken(ifs,line,column, inputFile);
      //Save shape name
      Token shapeName = curToken.copy();
      //Register new shape name
      if(parseNewShapeName(inputFile, curToken, listShapeName))
      {
        //Get new token
        curToken = getToken(ifs,line,column, inputFile);
        //Save first shape
        Token firstShape = curToken.copy();
        //Check first shape name (which to subtract from)
        if(parseShapeName(curToken, listShapeName, s1))
        {
          //Get new token
          curToken = getToken(ifs,line,column, inputFile);
          //Check first shape name (which to subtract from)
          if(parseShapeName(curToken, listShapeName, s2))
          {
            //Check that the two shapes are different
            if(firstShape.word == curToken.word)
            {
              errorInFile(inputFile, curToken, "Difference definition failed : Shapes must be different (Can not substract itself)");
            }
            //Create difference
            diff = make_shared<Difference> (s1,s2);
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
      pair<Token,shared_ptr<Shape>> p(shapeName, diff);
      listShapeName[shapeName.word]=p;
      nbrShape++;
    }
    ///Paint command///
    else if(curToken.word == "fill")
    {
      shared_ptr<Shape> s;
      shared_ptr<Color> c;
      curToken = getToken(ifs,line,column, inputFile);
      //Register new shape name
      if(parseShapeName(curToken, listShapeName, s))
      {
        curToken = getToken(ifs,line,column, inputFile);
        if(!parseColor(ifs,line,column, inputFile, curToken, c))
        {
          errorInFile(inputFile, curToken, "Painting command failed : Invalid color");
        }
        listOfFill.push_back(make_shared<Fill>(s,c));
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

  //Close input file
  ifs.close();

  //Construction of image
  Image img = Image(width,height);
  //Matrix of boolean to check if a pixel has already been painted
  auto _pixelDone = std::make_unique<bool[]>(width * height);

  /// Fill the set of pixels
  // The goal here is to paint the fills in opposite order of
  // apperance, this allows us to ignore pixels that were
  // already painted.
  // We also make sure to browse only a square framing
  // the shape in order to avoid uselesss iterations.
  for(int t = listOfFill.size()-1; t>=0; t--)
  {
    //Make sure that pixels coordinates are in the square
    //framing the shape and also in the canvas.
    auto sha = (*listOfFill[t]).sha;
    size_t i = std::max(0,(int) round(sha->min_x()-1.0));
    size_t k = std::max(0,(int) round(sha->min_y()-1.0));
    size_t fi = std::min((int) width,(int) round(sha->max_x()+1.0));
    size_t fj = std::min((int) height,(int) round(sha->max_y()+1.0));

    //Browse in frame pixels
    while(i<fi)
    {
      size_t j = k;
      while(j<fj)
      {
        if(_pixelDone[i*height+j]==false)
        {
          if(sha->inside(Point(((double)i)+0.5, ((double)j)+0.5)))
          {
            img.setPixel(i,j,(*listOfFill[t]).col);
            _pixelDone[i*height+j]=true;
          }
        }
        j++;
      }
      i++;
    }
  }

  //Write ppm
  ofs << img;
  //Close output file
  ofs.close();
  return 0;
}
