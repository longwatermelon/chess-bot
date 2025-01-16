#include "bot.h"
#include "util.h"
#include "actions.h"

double bot::static_eval(const state_t &s) {
    // checkmate?
    if (empty(actions(s,s.turn,true))) return vec<int>{-1,1}[s.turn]*1e9;

    // count pieces
    vec<char> pieces={'r','b','n','q','p'};
    map<char,int> wcnt, bcnt;
    for (char c:pieces) {
        wcnt[c]=count(all(s.board),toupper(c));
        bcnt[c]=count(all(s.board),c);
    }

    // accumulate values
    map<char,double> value = {
        {'q', 9},
        {'r', 5},
        {'b', 3}, {'n', 3},
        {'p', 1},
    };

    double res=0;
    for (char c:pieces) {
        res += value[c]*(wcnt[c] - bcnt[c]);
    }

    return res;
}

double bot::minimax(const state_t &s, int d) {
    vec<act_t> moves=actions(s, s.turn, true);

    // leaf
    if (d==0 || empty(moves)) return static_eval(s);

    if (s.turn==0) {
        // white
        double mx=-1e9;
        for (act_t &a : moves) {
            state_t sp=s;
            sp.move(a);
            ckmax(mx, minimax(sp, d-1));
        }

        return mx;
    } else {
        // black
        double mn=1e9;
        for (act_t &a : moves) {
            state_t sp=s;
            sp.move(a);
            ckmin(mn, minimax(sp, d-1));
        }

        return mn;
    }
}

act_t bot::best_move(const state_t &s, int depth) {
    vec<act_t> moves=actions(s, s.turn, true);

    double mn=1e9;
    act_t best;
    for (act_t a : moves) {
        state_t sp=s;
        sp.move(a);

        if (ckmin(mn, minimax(sp, depth))) {
            best=a;
        }
    }

    return best;
}
