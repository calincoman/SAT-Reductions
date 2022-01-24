#ifndef TASK_H_
#define TASK_H_

#include <bits/stdc++.h>
#include <iostream>
#include <cstdio>
#include <fstream>
#include <array>
#include <utility>
#include <string>

#define NMAX 50

using namespace std;

class Task {
 public:
    virtual ~Task() = default;
    virtual void solve() = 0;
    virtual void read_problem_data() = 0;
    void formulate_oracle_question() {};
    void decipher_oracle_answer() {};
    virtual void write_answer() = 0;

    // the file containing the oracle input
    string oracle_in_filename = "sat.cnf";
    // the file containing the oracle output
    string oracle_out_filename = "sat.sol";

    // string used for writing in the sat.cnf file and in the output file
    string result = "p cnf ";
    // string used for building the clauses
    string clauses = "";

    // the verdict given by the oracle (SAT solver)  - true or false
    bool oracle_verdict;

    // matrix used for storing the graph
    // graph[i][j] = 1, if there is an edge between i an j
    //               0, otherwise
    bool graph[NMAX + 1][NMAX + 1] = {false};

    /*
     * Stores the files paths as class attributes.
     *
     * @param in_filename:         the file containing the problem input
     * @param oracle_in_filename:  the file containing the oracle input
     * @param oracle_out_filename: the file containing the oracle output
     * @param out_filename:        the file containing the problem output
     */
 
    /*
     * Asks the oracle for an answer to the formulated question.
     */
    void ask_oracle() {
        std::string solver_path = "sat_oracle.py";
        std::string command = "python3 " + solver_path + " sat.cnf sat.sol";
        std::array<char, 512> buffer{};
        std::string output;

        auto pipe = popen(command.c_str(), "r");

        while (!feof(pipe)) {
            if (fgets(buffer.data(), 512, pipe) != nullptr) {
                output += buffer.data();
            }
        }

        auto rc = pclose(pipe);

        if (rc != EXIT_SUCCESS) {
            std::cerr << "Error encountered while running oracle" << std::endl;
            std::cerr << output << std::endl;
            exit(-1);
        }
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

    void add_variable(int i, int v, int negated) {
        if (negated == -1) {
            clauses += "-";
        }
        clauses += to_string(get_variable(i, v)) + " ";
    }

    void new_clause(int &clause_number) {
        ++clause_number;
        clauses += "0";
        clauses += "\n";
    }

    int get_register(int var) {
        if (var % k == 0) {
            return k;
        } else {
            return var % k;
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



#endif  // TASK_H_
