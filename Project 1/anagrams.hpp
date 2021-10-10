#ifndef ANAGRAMS_HPP
#define ANAGRAMS_HPP

#include <string>
#include <vector>
#include <array>
#include <utility>

typedef std::pair <std::string, std::array<int, 27>> pairdict;
typedef std::vector<pairdict> Dictionary;

Dictionary create_dictionary(const std::string& filename);

std::vector< std::vector<std::string> > anagrams(const std::string& input, const Dictionary& dict, unsigned max);

#endif
