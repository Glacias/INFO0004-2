#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>
/*
* Token is a class used to define what is returned by getToken
*/
struct Token {
    int id=0; //Used to determine the type of the token
    /* ID :
    *  -1 = end of file
    *  -2 = word
    *  -3 = symbol
    */
    int line=-1;//Line where the Token is
    int column=-1;//Column where the Token is (or starts)
    char symbol = ' ';
    std::string word;

    std::string positionStr() const;
    Token copy() const;
};

#endif
