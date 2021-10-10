#include "anagrams.hpp"

#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <limits>

using namespace std;

bool isInMap(const array<int, 27>& small,const array<int, 27>& big);

void
anagramsRecur(const array<int, 27>& inputMap,
  const vector<unsigned>& potentialWordIndex,
  const unsigned start,
  const Dictionary& dict,
  unsigned max,
  vector<string>& chosenWord,
  vector<vector<string>>& results);

Dictionary create_dictionary(const string& filename)
{
  Dictionary dict;

  ifstream infile(filename);
  string line;

  while (getline(infile, line))
  {
    //Creating pair members
    string word;
    array<int, 27> mapAlpha;
    fill(mapAlpha.begin(), mapAlpha.end(), 0);

    //Reading file setup
    istringstream iss(line);
    if (!(iss >> word)) { break; } // error

    //Assembling map
    int size_of_word = 0;
    for (char letter : word)
    {
      int OrderInAlpha = int(letter) - int('a');
      if (OrderInAlpha >= 0 && OrderInAlpha < 26)
      {
        mapAlpha[OrderInAlpha]++;
      }
      else { break; }//Error
      size_of_word++;
    }
    mapAlpha[26] = size_of_word;

    //Creating pair
    pairdict pair1;
    pair1.first = word;
    pair1.second = mapAlpha;

    //Add pair to dictionary
    dict.push_back(pair1);
  }
  return dict;
}


vector<vector<string>>
anagrams(const string& input, const Dictionary& dict, unsigned max)
{
  ///PROCESSING INPUT
  //Creating inputMap
  array<int, 27> inputMap;
  fill(inputMap.begin(), inputMap.end(), 0);

  //Assembling inputMap
  int size_of_input = 0;
  for (char letter : input)
  {
    if(!isspace(letter))
    {
      int OrderInAlpha = int(letter) - int('a');
      if (OrderInAlpha >= 0 && OrderInAlpha < 26)
      {
        inputMap[OrderInAlpha]++;
      }
      else { break; }//Error
      size_of_input++;
    }
  }
  inputMap[26] = size_of_input;

  //FILTERING DICTIONARY
  //Creating Filtered Dictionary
  vector<unsigned> acceptedWordIndex;
  //Filtering
  for (unsigned index = 0; index < dict.size(); index++)
  {
    if(isInMap(dict[index].second, inputMap))
    {
      acceptedWordIndex.push_back(index);
    }
  }

  ///ANAGRAMS CONSTRUCTION
  //Set max to infinity if max = 0
  if(max==0)
  {
    max = numeric_limits<unsigned>::max();
  }
  max--;
  /*
  //Creating results
  vector<unsigned> indexes;
  vector<vector<unsigned>> indexResults;
  */
  vector<string> chosenWord;
  vector<vector<string>> results;
  //Lanching
  array<int, 27> inputMapChild;
  for(unsigned k = 0; k < acceptedWordIndex.size(); k++)
  {
    //Add index to the list of indexes (of chosen word)
    chosenWord.push_back(dict[acceptedWordIndex[k]].first);

    //Reduce map
    for(int x = 0; x<=26; x++)
    {
      inputMapChild[x] = inputMap[x] - dict[acceptedWordIndex[k]].second[x];
    }

    anagramsRecur(inputMapChild, acceptedWordIndex, k, dict, max, chosenWord, results);

    //Remove index in order to lanch other anagrams_rec
    chosenWord.pop_back();
  }

  /*
  //Converting solution
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
  */
  return results;
}

void
anagramsRecur(const array<int, 27>& inputMap,
  const vector<unsigned>& potentialWordIndex,
  const unsigned start,
  const Dictionary& dict,
  unsigned max,
  vector<string>& chosenWord,
  vector<vector<string>>& results)
{
  if(inputMap[26] == 0)
  {
    results.push_back(chosenWord);
  }
  else
  {
    if(max>0)
    {
      max--;
      //Filter potentialWordIndex
      vector<unsigned> acceptedWordIndex;
      for(unsigned x = start; x < potentialWordIndex.size(); x++)
      {
        if(isInMap(dict[potentialWordIndex[x]].second, inputMap))
        {
          acceptedWordIndex.push_back(potentialWordIndex[x]);
        }
      }
      //Lanching
      array<int, 27> inputMapChild;
      for(unsigned k = 0; k < acceptedWordIndex.size(); k++)
      {
        //Add index to the list of indexes (of chosen word)
        chosenWord.push_back(dict[acceptedWordIndex[k]].first);

        //Reduce map
        for(int x = 0; x<=26; x++)
        {
          inputMapChild[x] = inputMap[x] - dict[acceptedWordIndex[k]].second[x];
        }

        anagramsRecur(inputMapChild, acceptedWordIndex, k, dict, max, chosenWord, results);

        //Remove index in order to lanch other anagrams_rec
        chosenWord.pop_back();
      }
    }
  }
  return;
}

bool isInMap(const array<int, 27>& small,const array<int, 27>& big)
{
  //Checking if word is acceptable
  //Checking size and letters
  for(int k = 26; k>=0; k--)
  {
    if(small[k]>big[k])
    {
      return false;
    }
  }
  return true;
}
