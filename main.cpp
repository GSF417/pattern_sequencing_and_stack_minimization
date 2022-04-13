#include "util/util.h"
#include <ctime>
#include <fstream>
#include <string>
#include <sstream>
#include <new>

using namespace std;

int main(int argc, char *argv[]) {
    time_t start_time;
    time(&start_time);
    // INPUT
    // input must be an array that will associate each object to the standard it must be cut
    // such array shall be obtained from an input csv file
    string file_name = argv[1];
    std::vector<std::vector<int>> patterns = make_pattern_array(file_name);

    // PRE PROCESSING
    // first step checks if lines or collumns filled with zeroes exist
    pre_process_step_1(patterns);

    // second step creates an array that represents what lines are included in others
    // if line x is included in line y, then the array includes shall have a 1 at line x and collumn y, otherwise 0
    std::vector<vector<int>> includes = pre_process_step_2(patterns);
    
    /* this commented code section was used to debug whether the input array is being processed correctly or not
    for (int i = 0; i < patterns.size(); i++) {
        for (int j = 0; j < patterns[i].size(); j++) {
            cout << patterns[i][j] << " ";
        }
        cout << endl;
    }
    */
    // GRAPH GENERATION
    Graph patterns_graph = Graph(patterns);
    //patterns_graph.print_arcs();

    // GRAPH VERIFICATION
    // if trees exist within the graph, process them first

    // COLLAPSE VERTICES

    time_t end_time;
    time(&end_time);
    cout << end_time - start_time << " seconds of execution";
    return 0;
}