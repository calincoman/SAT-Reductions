#include <iostream>
#include <string>

#include "task.h"

#define NMAX 50

using namespace std;

class Task1 : public Task {
  public:
  
     //string in_filename;     //the file containing the problem input
    string oracle_in_filename = "sat.cnf";  //the file containing the oracle input
    string oracle_out_filename = "sat.sol"; //the file containing the oracle output
     //string out_filename;       //the file containing the problem output
     
    string result = "p cnf ";
    string clauses = "";

    int n;
    int m;
    int k;

    ofstream out_sat_cnf;

    // matrix used for storing the graph
    // graph[i][j] = 1, if there is an edge between i an j
    //               0, otherwise
    bool graph[NMAX + 1][NMAX + 1] = {false};

    //methods

    void init_output() {
        out_sat_cnf.open(oracle_in_filename);
    }

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
        for (int i = 1; i <= k; ++i) {
            for (int v = 1; v <= n; ++v) {
                clauses += to_string(get_variable(i, v)) + " ";
            }
            ++clause_number;
            clauses += "0";
            clauses += "\n";
        }

        // build the second section of clauses
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
                            clauses += "-" + to_string(get_variable(i, v)) + " ";
                            // add not(x_jw)
                            clauses += "-" + to_string(get_variable(j, w)) + " ";
                            // each expression not(x_iv) V not(x_jw) is a new clause
                            ++clause_number;
                            clauses += "0";
                            clauses += "\n";
                        }
                    }
                }
            }
        }

        // build the third section of clauses
        /*
        // not(x_iv) V not(x_jv), with 1 <= i,j <= k, i != j; 1 <= v <= n
        for (int v = 1; v <= n; ++v) {
            for (int i = 1; i <= k; ++i) {
                for (int j = 1; j <= k; ++j) {
                    if (i != j) {
                        // the variables are negated, so according to DIMACS CNF,
                        // they are negative
                        // add not(x_iv)
                        clauses += "-" + to_string(get_variable(i, v)) + " ";
                        // add not(x_jv)
                        clauses += "-" + to_string(get_variable(j, v)) + " ";
                        // each expression not(x_iv) V not(x_jv) is a new clause
                        ++clause_number;
                        clauses += "0";
                        clauses += "\n";
                    }
                }
            }
        }
        // not(x_iv) V not(x_iw), with 1 <= i <= k; 1 <= v,w <= n, v != w
        for (int v = 1; v <= n; ++v) {
            for (int w = 1; w <= n; ++w) {
                if (v != w) {
                    for (int i = 1; i <= k; ++i) {
                        // the variables are negated, so according to DIMACS CNF,
                        // they are negative
                        // add not(x_iv)
                        clauses += "-" + to_string(get_variable(i, v)) + " ";
                        // add not(x_iw)
                        clauses += "-" + to_string(get_variable(i, w)) + " ";
                        // each expression not(x_iv) V not(x_iw) is a new clause
                        ++clause_number;
                        clauses += "0";
                        clauses += "\n";
                    }
                }
            }
        }*/

        // not(x_iv) V not(x_jv), with 1 <= i < j <= k; 1 <= v <= n
        for (int v = 1; v <= n; ++v) {
            for (int i = 1; i <= k; ++i) {
                for (int j = i + 1; j <= k; ++j) {
                    // the variables are negated, so according to DIMACS CNF,
                    // they are negative
                    // add not(x_iv)
                    clauses += "-" + to_string(get_variable(i, v)) + " ";
                    // add not(x_jv)
                    clauses += "-" + to_string(get_variable(j, v)) + " ";
                    // each expression not(x_iv) V not(x_jv) is a new clause
                    ++clause_number;
                    clauses += "0";
                    clauses += "\n";
                }
            }
        }
        

        result += to_string(variable_number) + " ";
        result += to_string(clause_number) + "\n";
        result += clauses;

        // if (!(n == 7 && m == 10 && k == 3)) {
        //     return;
        // }

        ofstream out_file;
        //out_file.open("out.txt");
        out_file.open(oracle_in_filename);
        out_file << result;

        // out_file.close();
    }

    void solve() {
        return;
    }
    void decipher_oracle_answer() {
        
        ifstream in_file;
        in_file.open(oracle_out_filename);

        string boolean_value;
        in_file >> boolean_value;

        //out_test << boolean_value << "\n";

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
            // var represents an element x_iv, which is true if v is the i^th node in the clique
            // and false if it isn't
            // var is negative <=> x_iv = false, positive <=> x_iv = true
            in_file >> var;
            if (var < 0) {
                // if it is false, x_iv = false <=> it means the node associated to
                // this variable is not in the found clique
                continue;
            }
            // if it is true, we must determine the value of v (the node) based on the associated
            // variable number (number associated to each x_iv)
            int v = get_node(var);
            result += to_string(v) + " ";
        }

    }
    void write_answer() {
        cout << result;
    }


    int get_variable(int i, int v) {
        int number = (v - 1) * k;
        number += i;
        return number;
    }

    int get_node(int var) {
        if (var % k == 0) {
            return var / k;
        } else {
            return var / k + 1;
        }
    }

    void create_edge(int u, int v) {
        graph[u][v] = true;
        graph[v][u] = true;
    }

    bool is_edge(int u, int v) {
        return (graph[u][v] == true);
    }
};

int main() {
    return 0;
}