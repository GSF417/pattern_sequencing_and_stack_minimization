#include "util.h"
using namespace std;

class Graph {
    private:
        struct arc {
            // this boolean determines whether this arc has been erased or not
            bool active = true;
            // these integers represent what nodes this arc connects
            int node_1 = -1;
            int node_2 = -1;
        } typedef stArc;

        std::vector<stArc> existing_arcs;

        struct node {
            // stores what arcs are connected to this vector
            std::vector<int> connected_arcs;
            // this boolean determines whether this node has been erased or not
            bool active = true;
            // estimates how many stacks we would open until we reach this node in an optimal way
            int score = -1;
        } typedef stNode;

        std::vector<stNode> existing_nodes;

        struct partial_walk {
            // stores what arcs are included in this partial walk
            std::vector<int> arcs;
            // the next arc that we would run through if the next node wasn't the ending node
            int next_arc = -1;
            // stores the final node of this partial walk
            int ending_node = -1;
            // stores the pwd degree of this partial walk
            int pw_degree = -1;
            int fpw_degree = -1;
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

        // returns -1 if neither node of the arc corresponds to the node we are giving.
        // otherwise, return the node that is different from the node we are passing.
        int check_next_node (int arc, int node) {
            int next = -1;
            if (existing_arcs[arc].node_1 != node && existing_arcs[arc].node_2 != node) {
                return next;
            }
            if (existing_arcs[arc].node_1 != node) {
                next = existing_arcs[arc].node_1;
            }
            else {
                next = existing_arcs[arc].node_2;
            }
            return next;
        }

        // this function shall receive a list of partial walks from TREE3, find all that have an ending k and then join them into a new partial walk
        stPartialWalk CONSTRUCT(std::vector<stPartialWalk> pws, int k) {
            // execute CONSTRUCT algorythm
            int highest = -1;
            int second_highest = -1;
            vector<int> arcs;
            stPartialWalk result = {arcs, -1, k, node_degree(k), -1};
            // here we search for the highest and second highest pw degrees that connect to our singular node
            for (int i = 0 ; i < pws.size(); i++) {
                stPartialWalk pw = pws[i];
                if (pw.ending_node == k ||
                    existing_arcs[pw.next_arc].node_1 == k ||
                    existing_arcs[pw.next_arc].node_2 == k) {
                        if (highest == -1) {
                            highest = i;
                        }
                        if (pw.pw_degree > pws[highest].pw_degree) {
                            second_highest = highest;
                            highest = i;
                        }
                        else if (pw.pw_degree > pws[second_highest].pw_degree) {
                            second_highest = i;
                        }
                }
            }
            // now we create a new partial walk that contains all partial walks that have the node as the ending one
            // we start by adding to the resulting partial walk the highest linked partial walk
            for (int i = 0; i < pws[highest].arcs.size(); i++) {
                result.arcs.push_back(pws[highest].arcs[i]);
            }
            // along with its extended arc
            result.arcs.push_back(pws[highest].next_arc);
            // in between the highest and second highest, we add every other partial walk
            for (int i = 0; i < pws.size(); i++) {
                if (i == highest || i == second_highest) {
                    continue;
                }
                for (int j = 0; j < pws[i].arcs.size(); j++) {
                    result.arcs.push_back(pws[i].arcs[j]);
                }
            }
            // we finish by adding to the resulting partial the second highest linked partial walk, but in reverse
            // with its extended arc first
            result.arcs.push_back(pws[second_highest].next_arc);
            for (int i = pws[second_highest].arcs.size() - 1; i >= 0; i--) {
                result.arcs.push_back(pws[second_highest].arcs[i]);
            }
            return result;
        }

        stPartialWalk CONSTRUCT1(std::vector<stPartialWalk> pws, int k) {
            // execute CONSTRUCT1 algorythm
            int highest = -1;
            int second_highest = -1;
            vector<int> arcs;
            stPartialWalk result = {arcs, -1, k, node_degree(k), -1};
            // here we search for the highest and second highest pw degrees that connect to our singular node
            for (int i = 0 ; i < pws.size(); i++) {
                stPartialWalk pw = pws[i];
                if (pw.ending_node == k ||
                    existing_arcs[pw.next_arc].node_1 == k ||
                    existing_arcs[pw.next_arc].node_2 == k) {
                        if (highest == -1) {
                            highest = i;
                        }
                        if (pw.pw_degree > pws[highest].pw_degree) {
                            second_highest = highest;
                            highest = i;
                        }
                        else if (pw.pw_degree > pws[second_highest].pw_degree) {
                            second_highest = i;
                        }
                }
            }
            // now we create a new partial walk that contains all partial walks that have the node as the ending one
            // we start by adding to the resulting partial walk the highest linked partial walk
            for (int i = 0; i < pws[highest].arcs.size(); i++) {
                result.arcs.push_back(pws[highest].arcs[i]);
            }
            // along with its extended arc
            result.arcs.push_back(pws[highest].next_arc);
            // in between the highest and second highest, we add every other partial walk
            for (int i = 0; i < pws.size(); i++) {
                if (i == highest || i == second_highest) {
                    continue;
                }
                for (int j = 0; j < pws[i].arcs.size(); j++) {
                    result.arcs.push_back(pws[i].arcs[j]);
                }
            }
            // we finish by adding to the resulting partial the second highest linked partial walk, but in reverse
            // with its extended arc first
            for (int i = 0; i < pws[second_highest].arcs.size(); i++) {
                result.arcs.push_back(pws[second_highest].arcs[i]);
            }
            return result;
        }
        
        // this function shall store away all trees that can be found in our graph for later use, making the graph easier to solve
        void TREE3() {
            std::vector<int> L;
            std::vector<int> scores;
            std::vector<int> scores_flag;
            std::vector<int> transversed_arcs;
            std::vector<int> visited_nodes;
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
                cout << "S1 [" << node << "] ";
                // initialize this partial walk in the vector
                int next_arc = 0;
                std::vector<int> arcs;
                partial_walks.push_back(stPartialWalk{arcs, next_arc, 0, 1, 1});
                int adj_node = node;
                // while our current node have degree of 2 or less or negative score, execute the loop
                while (node_degree(adj_node) <= 2 && scores[adj_node] < 0) {
                    // check the value in the node itself
                    cout << adj_node << "(" << node_degree(adj_node) << ") ";
                    visited_nodes.push_back(adj_node);
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
                    int next_node = check_next_node(next_arc, adj_node);
                    if (node_degree(next_node) <= 2) {
                        partial_walks[current_walk].arcs.push_back(next_arc);
                        partial_walks[current_walk].pw_degree = 2;
                    }
                    else {
                        partial_walks[current_walk].ending_node = next_node;
                        partial_walks[current_walk].next_arc = next_arc;
                    }
                    adj_node = next_node;
                }
                cout << endl;
                partial_walks[current_walk].fpw_degree = node_degree(partial_walks[current_walk].ending_node);
            }
            // this loop shall be broken once we a find a break instruction that determines we are already finished
            vector<stPartialWalk> resulting_walks;
            while (1) {
                // step 2
                // now, we disable each visited node 
                for (int j = 0; j < visited_nodes.size(); j++) {
                    int v_node = visited_nodes[j];
                    cout << v_node << " ";
                    disable_node(v_node);
                }
                cout << endl;
                visited_nodes.clear();
                for (int i = 0; i < partial_walks.size(); i++) {
                    stPartialWalk curr_walk = partial_walks[i];
                    cout << "S2 [" << i << "] Ends at " << curr_walk.ending_node << endl;
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
                    if (node_degree(i) <= 1 && existing_nodes[i].active) {
                        L.push_back(i);
                    }
                }
                // step 3
                if (L.size() <= 1) {
                    resulting_walks.push_back(CONSTRUCT(partial_walks, L[0]));
                }
                else {
                    int lowest_score_node = 0;
                    stPartialWalk result;
                    for (int i = 0; i < L.size(); i++) {
                        if (scores[i] < scores[lowest_score_node]) {
                            lowest_score_node = i;
                        }
                    }
                    result = CONSTRUCT1(partial_walks, lowest_score_node);
                    // execute WALK algorithm but adapted to this case
                    int next_node = lowest_score_node;
                    int comp_arc = -1;
                    while (node_degree(next_node) <= 2) {
                        stNode *curr_node = &existing_nodes[next_node];
                        for(int j = 0; j < curr_node->connected_arcs.size(); j++) {
                            comp_arc = curr_node->connected_arcs[j];
                            if (existing_arcs[comp_arc].active && transversed_arcs[comp_arc] == 1) {
                                transversed_arcs[comp_arc] = 0;
                                result.arcs.push_back(comp_arc);
                                next_node = check_next_node(comp_arc, next_node);
                            }
                        }
                    }
                    result.ending_node = next_node;
                    result.fpw_degree = scores[lowest_score_node];
                    partial_walks.push_back(result);
                    result = CONSTRUCT(partial_walks, lowest_score_node);
                    resulting_walks.push_back(result);
                    for (int i = 0; i < result.arcs.size(); i++) {
                        int sel_arc = i;
                        disable_tree_node(existing_arcs[i].node_1);
                        disable_tree_node(existing_arcs[i].node_2);
                    }
                }
            }
        }

        // checks if in the current graph every node is connected to every other existing node
        bool graph_is_complete() {
            bool complete = true;
            int active_nodes = 0;
            // count how many active nodes exist
            for (int i = 0; i < existing_nodes.size(); i++) {
                if (existing_nodes[i].active) {
                    active_nodes++;
                }
            }
            // now check if every active node connects to every other active node
            for (int i = 0; i < existing_nodes.size(); i++) {
                if (existing_nodes[i].active) {
                    int arc_count = 0;
                    for (int j = 0; j < existing_nodes[i].connected_arcs.size(); j++) {
                        if (existing_arcs[existing_nodes[i].connected_arcs[j]].active) {
                            arc_count++;
                        }
                    }
                    if (arc_count != active_nodes - 1) {
                        complete = false;
                    }
                }
                if (!complete) {
                    break;
                }
            }
            return complete;
        }

        // here we deactivate both vertices v1 and v2 and then create a new one whose arcs point to all the nodes it predecessors had.
        int collapse_arcs(int v1, int v2) {
            vector<int> stored_arcs;
            vector<int> created_arcs;
            // first we store all arcs that belong to the two vertices we are trying to collapse
            for (int i = 0; i < existing_nodes[v1].connected_arcs.size(); i++) {
                stored_arcs.push_back(existing_nodes[v1].connected_arcs[i]);
            }
            for (int i = 0; i < existing_nodes[v2].connected_arcs.size(); i++) {
                int current_arc = existing_nodes[v2].connected_arcs[i];
                int found = 0;
                for (int j = 0; j < stored_arcs.size(); j++) {
                    if (current_arc == stored_arcs[j]) {
                        found = 1;
                        break;
                    }
                }
                if (found) {
                    continue;
                }
                stored_arcs.push_back(existing_nodes[v2].connected_arcs[i]);
            }
            // now we create new arcs based on the ending points of the ones we stored
            for (int i = 0; i < stored_arcs.size(); i++) {
                int ending_point = -1;
                int check = check_next_node(stored_arcs[i], v1);
                if (check != v2 && check != -1) {
                    ending_point = check;
                }
                else {
                    check = check_next_node(stored_arcs[i], v2);
                    if (check != v1 && check != -1) {
                        ending_point = check;
                    }
                }
                if (ending_point != -1) {
                    stArc new_arc = {1, existing_nodes.size(), ending_point};
                    created_arcs.push_back(existing_arcs.size());
                    existing_nodes[ending_point].connected_arcs.push_back(existing_arcs.size());
                    existing_arcs.push_back(new_arc);
                }
            }
            stNode new_node = {created_arcs, true, -1};
            existing_nodes.push_back(new_node);
        }

        // to find the lower bound of the graph, we shall recursively execute arc collapsing until it is simple enough to state its lower bound.
        int find_lower_bound() {
            int lower_bound = 0;
            // we shall execute until every node in the graph is connected with the other nodes that exist in the graph
            while (!graph_is_complete) {
                // initially we just collapse arcs by merging two adjacent nodes together
            }
            return lower_bound;
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