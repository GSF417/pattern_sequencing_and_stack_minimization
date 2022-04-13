#include "util.h"

// this phase shall check if completely useless lines or collumns exist (eg. a line or collumn filled with zeroes) and then shut down the program with an alert
void pre_process_step_1(const std::vector<std::vector<int>> patterns){
    // this loop checks each line
    for (int i = 0; i < patterns.size(); i++) {
        // we start the useless flag as true, assuming that the line will be filled with zeroes
        bool useless = true;
        for (int j = 0; j < patterns[i].size(); j++) {
            // if at least one element isn't 0, then we confirm this line is not useless
            if (patterns[i][j] == 1) {
                useless = false;
            }
        }
        if (useless) {
            // alerts the user that a useless line was found and then shut downs the program
            cout << "Standard with no corresponding part detected in the csv line " << i+1 << ".\nPlease alter or remove such line.";
            std::exit(EXIT_FAILURE);
        }
    }
    // this loop checks each collumn
    for (int i = 0; i < patterns[0].size(); i++) {
        // we start the useless flag as true, assuming that the collumn will be filled with zeroes
        bool useless = true;
        for (int j = 0; j < patterns.size(); j++) {
            // if at least one element isn't 0, then we confirm this collumn is not useless
            if (patterns[j][i] == 1) {
                useless = false;
            }
        }
        if (useless) {
            // alerts the user that a useless collumn was found and then shut downs the program
            cout << "Part with no corresponding standard detected in the csv collumn " << i+1 << ".\nPlease alter or remove such collumn.";
            std::exit(EXIT_FAILURE);
        }
    }
}

// the second pre process step will create an array that relates patterns included in another
std::vector<std::vector<int>> pre_process_step_2(const std::vector<std::vector<int>> patterns) {
    // this array shall store what patterns are included in others
    // in this case, line i collumn j having a 1 indicates that pattern i is included in pattern j
    // otherwise 0
    std::vector<std::vector<int>> includes;
    // resize the array to suit our needs
    includes.resize(patterns.size());
    // initializes each line of the array with a size that matches our number of patterns
    for (int i = 0; i < patterns.size(); i++) {
        includes[i].resize(patterns.size());
    }
    // now to check whether patterns included in others exist, we execute this loop
    for (int i = 0; i < patterns.size(); i++) {
        for (int j = i; j < patterns.size(); j++) {
            // if we are checking whether the pattern is included in itself, skip it
            if (j == i) {
                continue;
            }
            bool i_contains_j = true;
            bool j_contains_i = true;
            for (int k = 0; k < patterns[i].size(); k++) {
                // if pattern i has an object at collumn k, but pattern j does not, then pattern j does not contain pattern i
                if (patterns[i][k] == 1 && patterns[j][k] == 0) {
                    j_contains_i = false;
                }
                // if pattern j has an object at collumn k, but pattern i does not, then pattern i does not contain pattern j
                else if (patterns[j][k] == 1 && patterns[i][k] == 0) {
                    i_contains_j = false;
                }
                // if we already confirmed that neither patterns contain each other, we are done here
                if (!i_contains_j && !j_contains_i) {
                    break;
                }
            }
            // this step writes to our array what patterns are contained in another
            if (i_contains_j) {
                includes[j][i] = 1;
            }
            // if they j is not included in i, then write 0 to it as the array has not yet been initialized
            else {
                includes[j][i] = 0;
            }
            if (j_contains_i) {
                includes[i][j] = 1;
            }
            else {
                includes[i][j] = 0;
            }
        }
    }
    return includes;
}