#include <iostream>

#include <vector>
#include <algorithm>
#include <set>
#include <random>
#include <map>
#include <sstream>
#include <optional>

using namespace std;

//: -1: *
//: -2: /
//: -3: -
//: -4: +


optional<long long> evaluateProd(const vector<int>& v, vector<int>::const_iterator & pos){
    long long value = *pos;
    pos++;
    while(pos != v.end() && (*pos == -1 || *pos == -2)){
        if(*pos == -1){
            pos++;
            value = value * (long long)(*(pos));
        }else if(*pos == -2){
            pos++;
            if(value % (*pos) != 0){
                return nullopt;
            }
            value = value / (long long)(*(pos));
        }
        pos++;
    }
    return value;

}
optional<long long> evaluate(const vector<int>& v){
    auto pos = v.begin();

    auto r = evaluateProd(v, pos);
    if(!r.has_value())
        return {};
    long long value = r.value();
    while(pos != v.end()) {
        if(*pos == -3){
            pos++;
            auto new_val = evaluateProd(v, pos);;
            if(!new_val.has_value())
                return {};
            value = value - new_val.value();
        }else if(*pos == -4) {
            pos++;
            auto new_val = evaluateProd(v, pos);;
            if(!new_val.has_value())
                return {};
            value = value + new_val.value();
        }
    }
    return value;
}

unordered_map<long long, vector<int>> solutions;
vector<int> numbers;

void recurse(vector<int>& val, int i){
    if(!evaluate(val).has_value())
        return;

    if(i > 14){
        auto r = evaluate(val);
        if(!r.has_value())
            return;
        long long value = r.value();
        if(value < 0)
            return;
        if(solutions.find(value) == solutions.end()){
            solutions[value] = val;
        }else{
            solutions[value] = {};
        }
        return;
    }
    for(auto op : {-1, -2, -3, -4}){
        val.push_back(op);
        val.push_back(numbers[i]);
        recurse(val, i+1);
        val.pop_back();
        val.pop_back();
    }
}
string to_string(const vector<int>& v){
    stringstream ss;
    for(auto x: v){
        if(x < 0){
            ss << " */-+"[-x];
        }else{
            ss << x;
        }
    }
    return ss.str();
}

int main() {

    mt19937 rng(0);
    uniform_int_distribution<int> dist(2, 9);
    for(int i = 0; i < 20; i++)
        numbers.push_back(dist(rng));


    vector<int> stack = {numbers[0]};
    recurse(stack, 1);

    int i = 0;
    for(const auto& x: solutions){
        if(!x.second.empty()){
            cout << to_string(x.second)<< " = " << x.first << endl;
            i++;
        }
    }
    cout << "Found " << i << " solutions.\n";
    return 0;
}
