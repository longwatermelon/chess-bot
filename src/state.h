#pragma once
#include "util.h"
#include "actions.h"

// action src->dst
struct act_t {
    pt_t src, dst;
};

struct state_t {
    string board;
    array<bool,64> attacked; // opponent attacking square
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
        // castle
        if (at(a.src)=='K' && a.src==pt_t{7,4} && !oppatk({7,4}) && a.dst==pt_t{7,6} && at({7,5})=='.' && !oppatk({7,5}) && at({7,6})=='.' && !oppatk({7,6}) && at({7,7})=='R' && !king_moved[0] && !rook_moved[0][1]) {
            atref({7,5}) = 'R';
            atref({7,7}) = '.';
        }
        if (at(a.src)=='K' && a.src==pt_t{7,4} && !oppatk({7,4}) && a.dst==pt_t{7,2} && at({7,3})=='.' && !oppatk({7,3}) && at({7,2})=='.' && !oppatk({7,2}) && at({7,1})=='.' && !oppatk({7,1}) && at({7,0})=='R' && !king_moved[0] && !rook_moved[0][0]) {
            atref({7,3}) = 'R';
            atref({7,0}) = '.';
        }
        if (at(a.src)=='k' && a.src==pt_t{0,4} && !oppatk({0,4}) && a.dst==pt_t{0,6} && at({0,5})=='.' && !oppatk({0,5}) && at({0,6})=='.' && !oppatk({0,6}) && at({0,7})=='r' && !king_moved[1] && !rook_moved[1][1]) {
            atref({0,5}) = 'r';
            atref({0,7}) = '.';
        }
        if (at(a.src)=='k' && a.src==pt_t{0,4} && !oppatk({0,4}) && a.dst==pt_t{0,2} && at({0,3})=='.' && !oppatk({0,3}) && at({0,2})=='.' && !oppatk({0,2}) && at({0,1})=='.' && !oppatk({0,1}) && at({0,0})=='r' && !king_moved[1] && !rook_moved[1][0]) {
            atref({0,3}) = 'r';
            atref({0,0}) = '.';
        }

        // if (at(a.src)=='k' && a.src==pt_t{0,4} && a.dst==pt_t{0,6} && at({0,5})=='.' && at({0,6})=='.' && at({0,7})=='r' && !king_moved[1] && !rook_moved[1][1]) {
        //     atref({0,5}) = 'r';
        //     atref({0,7}) = '.';
        // }
        // if (at(a.src)=='k' && a.src==pt_t{0,4} && a.dst==pt_t{0,2} && at({0,3})=='.' && at({0,2})=='.' && at({0,1})=='.' && at({0,0})=='r' && !king_moved[1] && !rook_moved[1][0]) {
        //     atref({0,3}) = 'r';
        //     atref({0,0}) = '.';
        // }

        if (a.src==pt_t{7,4}) {
            king_moved[0]=true;
        }
        if (a.src==pt_t{0,4}) {
            king_moved[1]=true;
        }
        if (a.src==pt_t{7,0}) {
            rook_moved[0][0]=true;
        }
        if (a.src==pt_t{7,7}) {
            rook_moved[0][1]=true;
        }
        if (a.src==pt_t{0,0}) {
            rook_moved[1][0]=true;
        }
        if (a.src==pt_t{0,7}) {
            rook_moved[1][1]=true;
        }

        // move
        atref(a.dst) = at(a.src);
        atref(a.src) = '.';

        // pawn promotion
        if (at(a.dst)=='P' && a.dst.r==0) {
            atref(a.dst)='Q';
        }
        if (at(a.dst)=='p' && a.dst.r==7) {
            atref(a.dst)='q';
        }

        vec<act_t> moves=actions(*this, false);
        fill(all(attacked),false);
        for (act_t a:moves) {
            oppatkref(a.dst)=true;
        }

        turn=1-turn;
    }

    char &atref(pt_t p) {
        assert(inside(p));
        return board[p.r*8+p.c];
    }

    char at(pt_t p) const {
        if (!inside(p)) {
            printf("failed: %d %d\n", p.r, p.c);
        }
        assert(inside(p));
        return board[p.r*8+p.c];
    }

    bool oppatk(pt_t p) const {
        return attacked[p.r*8+p.c];
    }

    bool &oppatkref(pt_t p) {
        return attacked[p.r*8+p.c];
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
