#pragma once
#include "util.h"

// action src->dst
struct act_t {
    pt_t src, dst;
};

struct state_t {
    string board;
    int turn; // 0=white, 1=black

    bool king_moved[2] = {false,false};
    bool rook_moved[2][2] = {{false,false},{false,false}};

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
        atref(a.dst) = at(a.src);
        atref(a.src) = '.';
        turn=1-turn;
    }

    char &atref(pt_t p) {
        return board[p.r*8+p.c];
    }

    char at(pt_t p) const {
        return board[p.r*8+p.c];
    }

    bool mvok(act_t a) const {
        return inside(a.src) && inside(a.dst) && at(a.dst)=='.';
    }

    bool capok(act_t a) const {
        return inside(a.src) && inside(a.dst) && color(at(a.dst))==1-color(at(a.src));
    }

    bool inside(pt_t p) const {
        return 0<=p.r && p.r<8 && 0<=p.c && p.c<8;
    }

    string repr() const {
        string s;
        for (int r=0; r<8; ++r) {
            for (int c=0; c<8; ++c) {
                s.push_back(at({r,c}));
            }
            s.push_back('\n');
        }

        s.pop_back();
        return s;
    }

    bool has_king_moved(int turn) const {
        return king_moved[turn];
    }

    bool has_rook_moved(int turn, bool king_side) const {
        return rook_moved[turn][king_side];
    }
};
