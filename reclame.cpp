#include <iostream>
#include <string>
#include <unordered_set>

#include "task.h"

#define NMAX 50

using namespace std;

/**
 * Class used for solving the second task (maximum independent set)
 */
class Task2 : public Task {
  public:
  
    /**
     * Reads the problem data from STDIN
     */
    void read_problem_data() {
        cin >> n >> m;
        for (int i = 1; i <= m; ++i) {
            int u, v;
            cin >> u >> v;
            create_edge(u, v);
        }
    }
   
    /**
     * Formulates a question in CNF format for the oracle (SAT solver)
     */
    void formulate_oracle_question() {

        result = "p cnf ";
        clauses = "";

        int variable_number = k * n;
        int clause_number = 0;

        /**
        * x_iv = true <=> Vertex v is on the i-th position in the maximum independent set
        */

        // Build the first section of clauses
        // For every index i, there must exist an i-th vertex in the independent set
        // x_i1 V x_i2 V ... V x_in, with 1 <= i <= k 
        for (int i = 1; i <= k; ++i) {
            for (int v = 1; v <= n; ++v) {
                // add x_iv to the current clause
                add_variable(i, v, 1);
            }
            // each x_i1 V x_i2 V ... V x_in is a new clause
            new_clause(clause_number);
        }

        // Build the second section of clauses
        // For every EDGE (v, w), v and w cannot both be in the independent set, since between any
        // pair of vertices (v, w) from the independent set, there must NOT be an edge.
        // not(x_iv) V not(x_jw), with 1 <= i < j <= k
        //                             1 <= v,w <= n, (v,w) is an edge and v != w
        for (int v = 1; v <= n; ++v) {
            for (int w = 1; w <= n; ++w) {
                // get only the pairs of vertices which are connected by an edge
                if (v != w && is_edge(v, w)) {
                    for (int i = 1; i <= k; ++i) {
                        for (int j = i + 1; j <= k; ++j) {
                            // the variables are negated, so according to DIMACS CNF,
                            // they are negative
                            // add not(x_iv)
                            add_variable(i, v, -1);
                            // add not(x_jw)
                            add_variable(j, w, -1);
                            // each expression not(x_iv) V not(x_jw) is a new clause
                            new_clause(clause_number);
                        }
                    }
                }
            }
        }

        // Build the third section of clauses
        // Each vertex v cannot be both the i-th and the j-th vertex in the independent set
        // not(x_iv) V not(x_jv), with 1 <= i < j <= k; 1 <= v <= n
        for (int v = 1; v <= n; ++v) {
            for (int i = 1; i <= k; ++i) {
                for (int j = i + 1; j <= k; ++j) {
                    // the variables are negated, so according to DIMACS CNF,
                    // they are negative
                    // add not(x_iv)
                    add_variable(i, v, -1);
                    // add not(x_jv)
                    add_variable(j, v, -1);
                    // each expression not(x_iv) V not(x_jv) is a new clause
                    new_clause(clause_number);
                }
            }
        }

        // Build the fourth section of clauses
        // Two different vertices cannot both be the i-th vertex in the independent set
        // not(x_iv) V not(x_iw), with 1 <= i <= k; 1 <= v, w <= n
        for (int v = 1; v <= n; ++v) {
            for (int w = 1; w <= n; ++w) {
                if (v != w) {
                    for (int i = 1; i <= k; ++i) {
                        // the variables are negated, so according to DIMACS CNF,
                        // they are negative
                        // add not(x_iv)
                        add_variable(i, v, -1);
                        // add not(x_iw)
                        add_variable(i, w, -1);
                        // each expression not(x_iv) V not(x_iw) is a new clause
                        new_clause(clause_number);
                    }
                }
            }
        }

        // create the string which contains the question in DIMACS CNF format
        result += to_string(variable_number) + " ";
        result += to_string(clause_number) + "\n";
        result += clauses;

        ofstream out_file;
        out_file.open(oracle_in_filename);
        out_file << result;
    }

    void solve() {
        string final_result = "";

        read_problem_data();

        // Apply a binary search to find a maximal independent set with k vertices
        int left = 1, right = n;
        while (left <= right) {
            int mid = (left + right) / 2;

            // try to find an independent set with mid vertices
            k = mid;

            formulate_oracle_question();
            ask_oracle();
            decipher_oracle_answer();

            if (oracle_verdict == true) {
                // save the result as this may be the maximum independent set
                final_result = result;
                // this is an independent set, but try to find a bigger one
                left = mid + 1;
            } else {
                // if an independent set can't be formed with mid vertices, then evidently a bigger
                // number of vertices can't form a set, so go down
                right = mid - 1;
            }
        }

        cout << final_result;
    }

    void decipher_oracle_answer() {
        
        ifstream in_file;
        in_file.open(oracle_out_filename);

        string boolean_value;
        in_file >> boolean_value;

        result = "";

        // if the clauses can't be solved, print "False"
        if (boolean_value == "False") {
            oracle_verdict = false;
            return;
        }

        oracle_verdict = true;

        // data structure to store the vertices from the found independent set
        unordered_set<int> independent_set;

        // else, the oracle (SAT solver) gives us "True" and a solution for the SAT problem
        int var_num;

        // read the number of variables and parse them
        in_file >> var_num;
        for (int i = 1; i <= var_num; ++i) {
            int var;
            // var represents an element x_iv, which is true if v is the i-th vertex in the
            // independent set and false if it isn't
            // var is negative <=> x_iv = false, positive <=> x_iv = true
            in_file >> var;
            if (var < 0) {
                // if it is false, x_iv = false <=> it means the vertex associated to
                // this variable is not in the found independent set
                continue;
            }
            // if it is true, we must determine the value of v (the vertex) based on the associated
            // variable number (number associated to each x_iv)
            int v = get_node(var);
            // add the vertex to the list of vertices in the independent set
            independent_set.insert(v);
        }

        // the result consists of the vertices which are not in the independent set
        for (int v = 1; v <= n; ++v) {
            if (independent_set.find(v) == independent_set.end()) {
                result += to_string(v) + " ";
            }
        }

    }
    void write_answer() {
        cout << result;
    }

    /**
     * Function which determines if a vertex is isolated (has no edges) or not
     * @param v the vertex
     * @return true if the vertex v is isolated, false otherwise
     */
    bool is_isolated(int v) {
        for (int j = 1; j <= n; ++j) {
            if (graph[v][j] == true) {
                return false;
            }
        }
        return true;
    }
};


 int main() {

    Task2 task2;

    task2.solve();

    return 0;
}
