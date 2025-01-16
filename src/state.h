#pragma once
#include "util.h"

// action src->dst
struct act_t {
    pt_t src, dst;
};

struct state_t {
    string board;
    int turn; // 0=white, 1=black

    state_t() {
        board = "rnbqkbnr"
                "pppppppp"
                "........"
                "........"
                "........"
                "........"
                "PPPPPPPP"
                "RNBQKBNR";
        turn=0;
    }

    void move(act_t a) {
        atref(a.dst.r, a.dst.c) = at(a.src.r, a.src.c);
        atref(a.src.r, a.src.c) = '.';
        turn=1-turn;
    }

    char &atref(int r, int c) {
        return board[r*8+c];
    }

    char at(int r, int c) const {
        return board[r*8+c];
    }

    string repr() const {
        string s;
        for (int r=0; r<8; ++r) {
            for (int c=0; c<8; ++c) {
                s.push_back(at(r,c));
            }
            s.push_back('\n');
        }

        s.pop_back();
        return s;
    }
};
