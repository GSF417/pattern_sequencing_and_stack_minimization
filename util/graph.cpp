#include "util.h"
using namespace std;

class Graph {
    private:
        struct arc {
            // this is a unique integer to represent this arc
            int id;
            // this boolean determines whether this arc has been erased or not
            bool active;
            // these integers represent what nodes this arc connects
            int node_1;
            int node_2;
        } typedef stArc;

        std::vector<stArc> existing_arcs;

        struct node {
            // this is a unique integer to represent this node
            int id;
            // this boolean determines whether this node has been erased or not
            bool active;
        } typedef stNode;

        std::vector<stNode> existing_nodes;

        // this integer stores the amount of nodes created
        int node_count;
        // this integer stores how many nodes have not been erased in the current graph
        int unerased_nodes;
        // this integer stores the amount of existing arcs in the graph
        int arc_count;
    public:
        

        // verifies whether an arc that connects the nodes x and y already exists or not
        bool arc_exists(int x, int y) {
            for (int i = 0; i < existing_arcs.size(); i++) {
                // this is the current arc we shall be comparing to
                stArc comp = existing_arcs[i];
                if ((comp.node_1 == x || comp.node_2 == x) && (comp.node_1 == y || comp.node_2 == y)) {
                    return true;
                }
            }
            return false;
        }

        // walks through our existing arc vector and prints in the screen the existing arcs
        void print_arcs() {
            for (int i = 0; i < existing_arcs.size(); i++) {
                cout << existing_arcs[i].node_1 << " " << existing_arcs[i].node_2 << endl;
            }
        }

        // constructs a new graph that is initially empty
        Graph() {
            // initialize each of the graph variables
            node_count = 0;
            unerased_nodes = 0;
            arc_count = 0;
        }

        // constructs a new graph based on a input array of patterns x objects
        Graph(std::vector<std::vector<int>> patterns) {
            // initialize each of the graph variables
            node_count = 0;
            unerased_nodes = 0;
            arc_count = 0;
            // our existing nodes vector shall initially store as many elements as the number of objects in the array
            existing_nodes.resize(patterns[0].size());
            // here, we initialize our vector of existing nodes
            for (int i = 0; i < patterns[0].size(); i++) {
                existing_nodes[i] = {node_count, true};
                node_count++;
            }
            // now, we check for arcs we should create
            // this vector stores what objects are processed in the current pattern
            std::vector<int> asoc;
            for (int i = 0; i < patterns.size(); i++) {
                for (int j = 0; j < patterns[i].size(); j++) {
                    if (patterns[i][j] == 1) {
                        asoc.push_back(j);
                        for (int k = 0; k < asoc.size(); k++) {
                            if (j == asoc[k]) {
                                continue;
                            }
                            if (!arc_exists(j, asoc[k])) {
                                Graph::stArc new_arc = {arc_count, true, asoc[k], j};
                                arc_count++;
                                existing_arcs.push_back(new_arc);
                            }
                        }
                    }
                }
                asoc.clear();
            }
        }
};