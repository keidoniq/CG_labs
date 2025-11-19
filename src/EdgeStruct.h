#pragma once
#include <utility>

class EdgeStruct {
private:
    int first, second;

public:
    EdgeStruct(int v1, int v2) {
        if (v1 <= v2) {
            first = v1;
            second = v2;
        } else {
            first = v2;
            second = v1;
        }
    }

    int getFirst() const { return first; }
    int getSecond() const { return second; }
    
    bool operator<(const EdgeStruct& other) const {
        if (first != other.first) return first < other.first;
        return second < other.second;
    }
    
    bool operator==(const EdgeStruct& other) const {
        return first == other.first && second == other.second;
    }
    bool operator!=(const EdgeStruct& other) const {
        return first != other.first || second != other.second;
    }
};