#include "anagrams.cpp"
//#include <string>
//#include <vector>

#include <iostream>
#include <fstream>

#include <chrono>

using namespace std;

int main() {
    // Variable declaration
    Dictionary dict;

    string input;
    unsigned max;

    vector<vector<string>> results;

    // Retrieving parameters
    cout << "Enter a string : ";
    getline(cin, input);

    cout << "Enter the maximum number of words (0 for no restriction) : ";
    cin >> max;

    // Dictionary creation
    auto start = chrono::steady_clock::now();

    dict = create_dictionary("sowpods.txt");

    auto end = chrono::steady_clock::now();
    auto diff = end - start;
    auto time_dict = chrono::duration <double, milli> (diff).count();

    // Finding anagrams
    start = chrono::steady_clock::now();

    results = anagrams(input, dict, max);

    end = chrono::steady_clock::now();
    diff = end - start;
    auto time_results = chrono::duration <double, milli> (diff).count();

    // Showing results
    cout << "Number of anagrams : " << results.size() << endl;
    cout << "Time (create_dictionary) : " << time_dict << " ms" << endl;
    cout << "Time (anagrams) : " << time_results << " ms" << endl;
    cout << "Time (total) : " << time_dict + time_results << " ms" << endl;

    // Exporting results
    ofstream out("anagrams/" + string(input) + "-" + to_string(max) + ".txt");

    for(vector<string> x : results) {
        for(string y : x)
            out << y << " ";

        out << "\n";
    }

    return 0;
}
