#include "anagrams.hpp"

#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <limits>

using namespace std;

array<int, 27> createMap(const string& word);
bool isInMap(const array<int, 27>& small, const array<int, 27>& big);
void anagramsRecur(const array<int, 27>& inputMap,
  const vector<unsigned>& potentialWordIndex,
  const unsigned start,
  const Dictionary& dict,
  unsigned max,
  vector<unsigned>& indexes,
  vector<vector<unsigned>>& indexResults);


Dictionary create_dictionary(const string& filename)
{
 /*
  * Dictionary is vector of pairs containing a string and a "map".
  *
  * The string contains a word of the dictionnary.
  *
  * The "map" is an array of int, it contains 27 elements. The first 26st
  * are the number of times a letter appears in the word and the last element
  * is the size. The array follows the configuration :
  * "a b c d e f g h i j k l m n o p q r s t u v w x y z SIZE"
  *
  * Example : String = "Apple",
  *           Map = 1 0 0 0 1 0 0 0 0 0 0 1 0 0 0 2 0 0 0 0 0 0 0 0 0 0 5
  */
  Dictionary dict;
  ifstream infile(filename);
  string line;
  string word;

  //Reading line in dictionary file
  while (getline(infile, line))
  {
    //Reading word in dictionary file
    istringstream iss(line);
    if (!(iss >> word))
    {
      cerr << "Error : file reading" << endl;
      exit(1);
    }

    //Assembling map
    array<int, 27> wordMap = createMap(word);

    //Create pair
    pairdict pair1;
    pair1.first = word;
    pair1.second = wordMap;

    //Add pair to dictionary
    dict.push_back(pair1);
  }
  return dict;
}

/*
 * Return a map of the coresponding word
 */
array<int, 27> createMap(const string& word)
{
  array<int, 27> map;
  fill(map.begin(), map.end(), 0);
  unsigned size_of_word = 0;
  for (char letter : word)
  {
    //Ignoring spaces
    if(!isspace(letter))
    {
      //Substarct ASCII value of letter to the ASCII value of "a"
      // in order to get the position in the array
      int posInMap = int(letter) - int('a');
      if (posInMap >= 0 && posInMap < 26)
      {
        map[posInMap]++;
      }
      else
      {
        cerr << "Invalid word (in input or dictionary) : "
        << "Unauthorized character" << endl;
        exit(1);
      }
      size_of_word++;
    }
  }
  map[26] = size_of_word;
  return map;
}

/*
 * Check if a word1 is possible to write with letters from word2
 * by comparing the maps of each word.
 * Allows to see if a word is a potential part of an anagram.
 */
bool isInMap(const array<int, 27>& word1Map, // Map of word 1
   const array<int, 27>& word2Map) // Map of word 2
{
  for(int k = 26; k>=0; k--)
  {
    if(word1Map[k]>word2Map[k])
    {
      return false;
    }
  }
  return true;
}


vector<vector<string>>
anagrams(const string& input, const Dictionary& dict, unsigned max)
{
  ///Assembling inputMap
  array<int, 27> inputMap = createMap(input);

  ///Filter Dictionary
  //Create an array of index of word that are possible
  // to write with letters from the input
  vector<unsigned> acceptedWordIndex;
  for (unsigned index = 0; index < dict.size(); index++)
  {
    if(isInMap(dict[index].second, inputMap))
    {
      acceptedWordIndex.push_back(index);
    }
  }

  ///Construction of anagrams
  //Set the maximum amount of word to infinity if max = 0
  if(max==0)
  {
    max = numeric_limits<unsigned>::max();
  }
  max--;

  //Anagrams search
  vector<unsigned> indexes;
  vector<vector<unsigned>> indexResults;
  array<int, 27> inputMapChild;
  //Browse array of acceptedWordIndex and launch recursive search
  for(unsigned k = 0; k < acceptedWordIndex.size(); k++)
  {
    //Add index to the list of indexes (of chosen word making the anagram)
    indexes.push_back(acceptedWordIndex[k]);

    //Reduce map
    for(int x = 0; x<=26; x++)
    {
      inputMapChild[x] = inputMap[x] - dict[acceptedWordIndex[k]].second[x];
    }

    //Recursive search
    anagramsRecur(inputMapChild, acceptedWordIndex, k, dict, max, indexes, indexResults);

    //Remove index in order to lanch other recursive search
    indexes.pop_back();
  }

  //Convert indexes to recover the solutions
  vector<vector<string>> results;
  for(vector<unsigned> x : indexResults)
  {
    vector<string> anag;
    for(unsigned index : x)
    {
      anag.push_back(dict[index].first);
    }
    results.push_back(anag);
  }
  return results;
}

/*
 * This function constructs anagrams by recursion.
 *
 * Details :
 * One of its arguments is a list of index of word in the dictionary.
 * Thoses words are potentially a potential part of an anagram.
 * This function filter them to only take the ones still promissing
 *  (see function IsInMap).
 * It then lauches itself for each of thoses words with a updated map,
 *  the list of index it just filtered, an index of where to start on that list
 *  and an added index (in indexes) refering to the word.
 */
void
anagramsRecur(const array<int, 27>& inputMap, // map of the remaining letters
  const vector<unsigned>& potentialWordIndex, // List of index of potential word
  const unsigned start, // Index where to start on potentialWordIndex
  const Dictionary& dict, // Dictionary
  unsigned max, // Maximum numbers of words allowed
  vector<unsigned>& indexes, // List of index of chosen words
  vector<vector<unsigned>>& indexResults) // vector of index of solutions
{
  //Add indexes to solution if inputMap is empty
  if(inputMap[26] == 0)
  {
    indexResults.push_back(indexes);
  }
  else
  {
    //Continue only if maximum number of words > 0
    if(max>0)
    {
      max--;

      //Create an array of index of word that are possible
      // to write with letters from the input by browsing
      // the remaining potential words
      vector<unsigned> acceptedWordIndex;
      for(unsigned x = start; x < potentialWordIndex.size(); x++)
      {
        if(isInMap(dict[potentialWordIndex[x]].second, inputMap))
        {
          acceptedWordIndex.push_back(potentialWordIndex[x]);
        }
      }

      //Browse array of acceptedWordIndex and launch recursive search
      array<int, 27> inputMapChild;
      for(unsigned k = 0; k < acceptedWordIndex.size(); k++)
      {
        //Add index to the list of indexes (of chosen word)
        indexes.push_back(acceptedWordIndex[k]);

        //Reduce map
        for(int x = 0; x<=26; x++)
        {
          inputMapChild[x] = inputMap[x] - dict[acceptedWordIndex[k]].second[x];
        }

        //Recursive search
        anagramsRecur(inputMapChild, acceptedWordIndex, k, dict, max, indexes, indexResults);

        //Remove index in order to lanch other anagrams_rec
        indexes.pop_back();
      }
    }
  }
  return;
}
