#include <iostream>
#include <string>

#include "task.h"

#define NMAX 50

using namespace std;

ofstream out_test;

/**
 * Class for solving the k-clique problem
 */
class Task1 : public Task {
  public:

    /**
     * Reads the problem data from STDIN
     */
    void read_problem_data() {
        // read the number of nodes, edges and the size of the searched clique
        cin >> n >> m >> k;
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

        int variable_number = k * n;
        int clause_number = 0;

        /**
        * x_iv = true <=> Vertex v is on the i-th position in the k-Clique
        */ 

        // Build the first section of clauses
        // For every index i, there must exist an i-th vertex in the clique
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
        // For every NON-EDGE (v, w), v and w cannot both be in the clique, since between any
        // pair of vertices (v, w) from the clique, there must exist an edge.
        // not(x_iv) V not(x_jw), with 1 <= i < j <= k
        //                             1 <= v,w <= n, (v,w) is not an edge and v != w
        for (int v = 1; v <= n; ++v) {
            for (int w = 1; w <= n; ++w) {
                // get only the pairs of vertices which are not connected by an edge
                if (v != w && !is_edge(v, w)) {
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
        // Two different vertices cannot both be the i-th vertex in the clique
        // not(x_iv) V not(x_iw), with 1 <= i <= k; 1 <= v,w <= n, v != w
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

        // Build the fourth section of clauses
        // Each vertex v cannot be both the i-th and the j-th vertex in the clique
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
        
        // create the string which contains the question in DIMACS CNF format
        result = "p cnf ";
        result += to_string(variable_number) + " ";
        result += to_string(clause_number) + "\n";
        result += clauses;

        ofstream out_file;

        // write the string in the sat.cnf file
        out_file.open(oracle_in_filename);
        out_file << result;
    }

    void solve() {
        read_problem_data();
        formulate_oracle_question();
        ask_oracle();
        decipher_oracle_answer();
        write_answer();
    }

    /**
     * Reads the SAT solver's result and parses it
     */
    void decipher_oracle_answer() {
        
        ifstream in_file;
        in_file.open(oracle_out_filename);

        string boolean_value;
        in_file >> boolean_value;

        // if the clauses can't be solved, print "False"
        if (boolean_value == "False") {
            result = "False";
            return;
        }

        // else, the oracle (SAT solver) gives us "True" and a solution for the SAT problem
        result  = "True\n";
        int var_num;

        // read the number of variables and parse them
        in_file >> var_num;
        for (int i = 1; i <= var_num; ++i) {
            int var;
            // var represents an element x_iv, which is true if v is the i-th vertex in the clique
            // and false if it isn't
            // var is negative <=> x_iv = false, positive <=> x_iv = true
            in_file >> var;
            if (var < 0) {
                // if it is false, x_iv = false <=> it means the vertex associated to
                // this variable is not in the found clique
                continue;
            }
            // if it is true, we must determine the value of v (the vertex) based on the associated
            // variable number (number associated to each x_iv)
            int v = get_node(var);
            result += to_string(v) + " ";
        }

    }
    /**
     * Writes the result to STDOUT
     */
    void write_answer() {
        cout << result;
    }
};

int main() {

    Task1 task1;

    task1.solve();

    return 0;
}