//
// Created by niels on 06.03.22.
//

#include <algorithm>
#include <iostream>
#include <random>
#include <unordered_set>
#include "Solver.h"
#include "State.h"

using namespace std;
std::string Solver::solve(std::string string, int c) {

    State s = State::fromString(string);
    cout << ".toString(): " << s.toString() << "\n";
    cout << ".visualize(): \n";
    cout << s.visualize();
    unordered_set<State, StateHash> values;
    values.insert(s);
    long long max_v = -1;
    for(int i = 0; i < c; i++){
        unordered_set<State, StateHash> next_states;
        for(const auto& old: values){
            for(State next_state : getNextStates(old)){
                next_states.insert(next_state);
                if(next_state.getValue() > max_v){
                    max_v = next_state.getValue();
                    cout << "Value: " << next_state.getValue() << "\n";
                    cout << next_state.visualize();
                }
            }
        }
        values = next_states;
    }

    return string;
}

std::unordered_set<State, StateHash> Solver::getNextStates(const State& state) {
    std::unordered_set<State, StateHash> result;
    //iterate over all used position
    State s = state;
    for(int digit1 = 0; digit1 < s.positions.size(); digit1++){
        for(int segment1 = 0; segment1 < 7; segment1++){
            if(!s.positions[digit1][segment1])
                continue;

            //iterate over all unused positions
            for(int digit2 = 0; digit2 < s.positions.size(); digit2++){
                for(int segment2 = 0; segment2 < 7; segment2++){
                    if(s.positions[digit2][segment2])
                        continue;

                    //add this state
                    auto new_state = s;
                    new_state.positions[digit1][segment1].flip();
                    new_state.positions[digit2][segment2].flip();
                    result.insert(new_state);

                }
            }
        }
    }
    return result;
}



