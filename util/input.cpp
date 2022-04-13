#include "util.h"
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

// this function translates the contents of the csv input to a string we can utilize
string get_csv_contents(const string file_name) {
    // the variable ss is stream we utilize to manipulate strings
    auto ss = ostringstream{};
    // the ifstream is an input stream to manipulate files, which we utilize to open the file we wish and extract its text
    std::ifstream input_file (file_name);
    if (input_file.is_open()) {
        // here we copy the text from the file we opened to the ss stream
        ss << input_file.rdbuf();
        // finally we return the text in the form of a string
        return ss.str();
    }
    else {
        // in case we failed to open the file, alert the user and then close the program
        cout << "Please state the csv file to read the inputs from (eg. main.exe input.csv)\n";
        std::exit(EXIT_FAILURE);
    }
}

// this function will read the string and convert each of its elements to an integer array
std::vector<std::vector<int>> make_pattern_array(const string file_name) {
    std::vector<int> csv_line;
    std::vector<std::vector<int>> patterns;
    string record;
    string file_contents = get_csv_contents(file_name);
    istringstream sstream(file_contents);
    for (int i = 0; std::getline(sstream, record); i++) {
        istringstream line(record);
        while (std::getline(line, record, ',')) {
            csv_line.push_back(stoi(record));
        }
        patterns.push_back(csv_line);
        csv_line.clear();
    }
    return patterns;
}