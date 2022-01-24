#include <iostream>
#include <string>

#include "task.h"

#define NMAX 50

using namespace std;

ofstream out_test;

class Task3 : public Task {
  public:

    int n;
    int m;
    int k;

    // matrix used for storing the graph
    // graph[i][j] = 1, if there is an edge between i an j
    //               0, otherwise
    bool graph[NMAX + 1][NMAX + 1] = {false};
    
    void read_problem_data() {
        cin >> n >> m >> k;
        for (int i = 1; i <= m; ++i) {
            int u, v;
            cin >> u >> v;
            create_edge(u, v);
        }
    }

    void formulate_oracle_question() {

        int variable_number = k * n;
        int clause_number = 0;

        // build the first section of clauses
        // x_i1 V x_i2 V ... V x_in, with 1 <= i <= k  
        for (int v = 1; v <= n; ++v) {
            for (int i = 1; i <= k; ++i) {
                add_variable(i, v, 1);
            }
            new_clause(clause_number);
        }

        // build the second section of clauses
        // for every edge (v, w), v and w CANNOT be stored in the same register i
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
            // var represents an element x_iv, which is true if the variable v can be stored
            // in the register i and false otherwise

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

        // if x_iv = false <=> it means that the register i can't be assigned
        // to v, so we need to find a register which can be assigned to v
        //(basically search for a variable x_iv = true, with 1 <= i <= k)
        for (int v = 1; v <= n; ++v) {
            for (int i = 1; i <= k; ++i) {
                // if a register i can be assigned to it, print it
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
};

int main() {

    Task3 task3;

    task3.solve();

    return 0;
}