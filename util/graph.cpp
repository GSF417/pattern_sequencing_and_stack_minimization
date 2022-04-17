#include "util.h"
using namespace std;

class Graph {
    private:
        struct arc {
            // this boolean determines whether this arc has been erased or not
            bool active;
            // these integers represent what nodes this arc connects
            int node_1;
            int node_2;
        } typedef stArc;

        std::vector<stArc> existing_arcs;

        struct node {
            // stores what arcs are connected to this vector
            std::vector<int> connected_arcs;
            // this boolean determines whether this node has been erased or not
            bool active = true;
        } typedef stNode;

        std::vector<stNode> existing_nodes;

        struct partial_walk {
            // stores what arcs are included in this partial walk
            std::vector<int> arcs;
            // the next arc that we would run through if the next node wasn't the ending node
            int next_arc;
            // stores the final node of this partial walk
            int ending_node;
            // stores the pwd degree of this partial walk
            int pw_degree;
            int fpw_degree;
        } typedef stPartialWalk;

        // this integer stores the amount of nodes created
        int node_count;
        // this integer stores how many nodes have not been erased in the current graph
        int unerased_nodes;
        // this integer stores the amount of existing arcs in the graph
        int arc_count;
    public:
        
        void disable_node(int id) {
            stNode *node = &existing_nodes[id];
            node->active = false;
            for (int i = 0; i < node->connected_arcs.size(); i++) {
                stArc *arc = &existing_arcs[node->connected_arcs[i]];
                arc->active = false;
            }
        }

        // counts the degree of a node (how many arcs connect to it)
        int node_degree(int id) {
            int count = 0;
            for (int i = 0; i < existing_nodes[id].connected_arcs.size(); i++) {
                // pick each arc of the id node in the vector of existing arcs
                stArc *arc = &existing_arcs[(existing_nodes[id]).connected_arcs[i]];
                // we only count those that are currently active
                if (arc->active) {
                    count++;
                }
            }
            return count;
        }

        // this function will pick the first active arc it spots for a given node id
        int pick_first_active_arc(int id) {
            stNode *node = &existing_nodes[id];
            for (int i = 0; i < node->connected_arcs.size(); i++) {
                stArc *arc = &(existing_arcs[node->connected_arcs[i]]);
                if (arc->active) {
                    return node->connected_arcs[i];
                }
            }
            return -1;
        }

        bool disable_tree_node(int id) {
            int adjNode = 0;
            stNode *node = &existing_nodes[id];
            //cout << "Attempting to disable node " << id << endl;
            if (node->active && node_degree(id) <= 1) {
                stArc *connected_arc = &(existing_arcs[pick_first_active_arc(id)]);
                if (connected_arc->node_1 != id) {
                    adjNode = connected_arc->node_1;
                }
                else {
                    adjNode = connected_arc->node_2;
                }
                if (node_degree(adjNode) <= 2) {
                    disable_node(id);
                    disable_tree_node(adjNode);
                }
            }
            return false;
        }

        void check_for_trees() {
            for (int i = 0; i < existing_nodes.size(); i++) {
                if (node_degree(i) <= 1) {
                    disable_tree_node(i);
                }
            }
        }

        int partial_walk(int id, int final) {
            stNode *node = &existing_nodes[id];
            if (node_degree(id) > 2) {
                final = id;
            }
            else {
                stArc *arc = &existing_arcs[pick_first_active_arc(id)];
                int adjNode = -1;
                if (arc->node_1 == id) {
                    adjNode = arc->node_1;
                }
                else {
                    adjNode = arc->node_2;
                }
                partial_walk(adjNode, final);
            }
            return final;
        }
        
        // this function shall store away all trees that can be found in our graph for later use, making the graph easier to solve
        void TREE3() {
            std::vector<int> L;
            std::vector<int> scores;
            std::vector<int> scores_flag;
            std::vector<int> transversed_arcs;
            std::vector<stPartialWalk> partial_walks;
            int current_walk = 0;
            // step 0
            // initially, we create a list L that stores each node of degree 1
            for (int i = 0; i < existing_nodes.size(); i++) {
                if (node_degree(i) <= 1) {
                   L.push_back(i);
                }
                // we also define each node in the graph as having a score of -1
                scores.push_back(-1);
                scores_flag.push_back(0);
            }
            // here we initialize our transversed arcs by defining them as 1, which means we can still pass through them
            // later we define 0 as indicating we already went through this arc
            for (int i = 0; i < existing_arcs.size(); i++) {
                transversed_arcs.push_back(1);
            }
            // step 1
            // now to calculate and store each partial walk
            for (int i = 0; i < L.size(); i++) {
                // store the current node of degree 1 we are verifying
                int node = L[i];
                // initialize this partial walk in the vector
                int next_arc = 0;
                std::vector<int> arcs;
                partial_walks.push_back(stPartialWalk{arcs, next_arc, 0, 1});
                int adj_node = node;
                // while our current node hade degree of 2 or less or negative score, execute the loop
                while (node_degree(adj_node) <= 2 || scores[adj_node] < 0) {
                    // check the value in the node itself
                    stNode *node_next = &existing_nodes[adj_node];
                    for(int j = 0; j < node_next->connected_arcs.size(); j++) {
                        int comp_arc = node_next->connected_arcs[j];
                        if (existing_arcs[comp_arc].active && transversed_arcs[comp_arc] == 1) {
                            transversed_arcs[comp_arc] = 0;
                            next_arc = comp_arc;
                            break;
                        }
                        else {
                            next_arc = -1;
                        }
                    }
                    if (next_arc == -1) {
                        break;
                    }
                    partial_walks[current_walk].arcs.push_back(next_arc);
                    if (existing_arcs[next_arc].node_1 != adj_node) {
                        adj_node = existing_arcs[next_arc].node_1;
                    }
                    else {
                        adj_node = existing_arcs[next_arc].node_2;
                    }
                    partial_walks[current_walk].pw_degree = 2;
                }
                partial_walks[current_walk].ending_node = adj_node;
                partial_walks[current_walk].next_arc = next_arc;
                partial_walks[current_walk].fpw_degree = node_degree(partial_walks[current_walk].ending_node);
            }
            // this loop shall be broken once we a find a break instruction that determines we are already finished
            while (1) {
                // step 2
                // now, we erase each visited node 
                for (int i = 0; i < partial_walks.size(); i++) {
                    stPartialWalk curr_walk = partial_walks[i];
                    for (int j = 0; j < curr_walk.arcs.size(); j++) {
                        int curr_arc = curr_walk.arcs[j];
                        disable_node(existing_arcs[curr_arc].node_1);
                        disable_node(existing_arcs[curr_arc].node_2);
                    }
                    if (scores[curr_walk.ending_node] == -1) {
                        scores[curr_walk.ending_node] = curr_walk.pw_degree;
                    }
                    else if (curr_walk.pw_degree > scores[curr_walk.ending_node]) {
                        scores[curr_walk.ending_node] = curr_walk.pw_degree;
                        scores_flag[curr_walk.ending_node] = 1;
                    }
                    else {
                        scores_flag[curr_walk.ending_node] = 1;
                    }
                }
                for (int i = 0; i < scores.size(); i++) {
                    if (scores_flag[i] == 1) {
                        scores[i]++;
                    }
                }
                // once more, we create a list with all nodes of degree 1
                L.clear();
                for (int i = 0; i < existing_nodes.size(); i++) {
                    if (node_degree(i) <= 1) {
                    L.push_back(i);
                    }
                    // we also define each node in the graph as having a score of -1
                    scores.push_back(-1);
                    scores_flag.push_back(0);
                }
                // here we initialize our transversed arcs by defining them as 1, which means we can still pass through them
                // later we define 0 as indicating we already went through this arc
                transversed_arcs.clear();
                for (int i = 0; i < existing_arcs.size(); i++) {
                    transversed_arcs.push_back(1);
                }
                // step 3
            }
        }

        // verifies whether an arc that connects the nodes x and y already exists or not
        bool arc_exists(int x, int y) {
            for (int i = 0; i < existing_arcs.size(); i++) {
                // this is the current arc we shall be comparing to
                stArc *comp = &existing_arcs[i];
                if ((comp->node_1 == x || comp->node_2 == x) && (comp->node_1 == y || comp->node_2 == y)) {
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

        // initializes a new arc and adds it to each of its corresponding nodes
        void add_arc_to_nodes(int n_1, int n_2) {
            Graph::stArc new_arc = {true, n_1, n_2};
            existing_nodes[n_1].connected_arcs.push_back(arc_count);
            existing_nodes[n_2].connected_arcs.push_back(arc_count);
            arc_count++;
            existing_arcs.push_back(new_arc);
        }

        void print_active_nodes() {
            cout << "Active Nodes" << endl;
            for (int i = 0; i < existing_nodes.size(); i++) {
                stNode *node = &existing_nodes[i];
                if (node->active) {
                    cout << "Node[" << i << "]" << endl << "Arcs: ";
                    for (int j = 0; j < node->connected_arcs.size(); j++) {
                        if (existing_arcs[node->connected_arcs[j]].active) {
                            cout << node->connected_arcs[j] << " ";
                        }
                    }
                    cout << endl;
                }
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
                                add_arc_to_nodes(j, asoc[k]);
                            }
                        }
                    }
                }
                asoc.clear();
            }
        }
};