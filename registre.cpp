#include <iostream>
#include <string>

#include "task.h"

#define NMAX 50

using namespace std;

ofstream out_test;

/**
 * Class for the third task (k-Coloring)
 */
class Task3 : public Task {
  public:
    
    /**
     * For this task, the VARIABLES can be viewed as VERTICES and the RELATIONS between them
     * as EDGES. This way, the register allocation problem is basically equivalent to the
     * k-Coloring problem on the graph.
     * 
     * x_iv = true <=> Variable v is stored in register i <=> Vertex v is colored with color i
     */

    /**
     * Reads the problem data from STDIN
     */
    void read_problem_data() {
        // Read the number of variables, the number of relations between them and the number
        // of registers that can be used
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

        // Build the first section of clauses
        // Every vertex v MUST be colored with a color i
        // x_1v V x_2v V ... V x_kv, with 1 <= v <= n  
        for (int v = 1; v <= n; ++v) {
            for (int i = 1; i <= k; ++i) {
                // add x_iv to the current clause
                add_variable(i, v, 1);
            }
            // each x_1v V x_2v V ... V x_kv is a new clause
            new_clause(clause_number);
        }

        // Build the second section of clauses
        // For every edge (v, w), v and w CANNOT be colored with the same color i
        // not(x_iv) V not(x_iw), with 1 <= i < j <= k
        //                             1 <= v,w <= n, (v, w) is an edge and v != w
        for (int v = 1; v <= n; ++v) {
            for (int w = 1; w <= n; ++w) {
                // get only the pairs of vertices which are connected by an edge
                if (v != w && is_edge(v, w)) {
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

        // Build the third section of clauses
        // Each vertex v has a single color, so it cannot be colored with both
        // the i and the j color
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

        // use a matrix to store the boolean values of the SAT variables x_iv
        // x[i][v] = true, if x_iv = true
        //           false, if x_iv = false
        bool x[NMAX][NMAX] = {false};

        // else, the oracle (SAT solver) gives us "True" and a solution for the SAT problem
        result  = "True\n";
        int var_num;

        // read the number of variables and parse them
        in_file >> var_num;
        for (int i = 1; i <= var_num; ++i) {
            int var;
            // var represents an element x_iv, which is true if the variable v can colored with
            // color i and false otherwise

            in_file >> var;

            int register_number = get_register(abs(var));
            int variable_number = get_node(abs(var));

            // var is negative <=> x_iv = false
            if (var < 0) {
                x[register_number][variable_number] = false;
                continue;
            }
            // var is positive <=> x_iv = true
            x[register_number][variable_number] = true;
        }

        // if x_iv = false <=> it means that the color i can't be assigned
        // to v, so we need to find a color which can be assigned to v
        //(basically search for a variable x_iv = true, with 1 <= i <= k)
        for (int v = 1; v <= n; ++v) {
            for (int i = 1; i <= k; ++i) {
                // if a color (register) i can be assigned to v, print it
                if (x[i][v] == true) {
                    result += to_string(i) + " ";
                    break;
                }
            }
        }
    }
    
    void write_answer() {
        cout << result;
    }

    int get_register(int var) {
        if (var % k == 0) {
            return k;
        } else {
            return var % k;
        }
    }
};

int main() {

    Task3 task3;

    task3.solve();

    return 0;
}