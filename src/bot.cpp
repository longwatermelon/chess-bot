#include "bot.h"
#include "util.h"
#include "actions.h"

double bot::static_eval(state_t &s) {
    // checkmate?
    if (empty(actions(s,true))) return vec<int>{-1,1}[s.turn]*1e9;

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

double bot::minimax(state_t &s, int d, double alpha, double beta, int &visited) {
    visited++;
    vec<act_t> moves=actions(s, true);
    sort(all(moves),[s](act_t a, act_t b){return (s.at(a.dst)!='.') > (s.at(b.dst)!='.');});

    // leaf
    if (d==0 || empty(moves)) return static_eval(s);

    if (s.turn==0) {
        // white
        double mx=-1e9;
        for (act_t &a : moves) {
            state_t sp=s;
            sp.move(a);
            double eval=minimax(sp, d-1, alpha, beta, visited);

            ckmax(mx, eval);

            ckmax(alpha, eval);
            if (beta<=alpha) break;
        }

        return mx;
    } else {
        // black
        double mn=1e9;
        for (act_t &a : moves) {
            state_t sp=s;
            sp.move(a);
            double eval=minimax(sp, d-1, alpha, beta, visited);

            ckmin(mn, eval);

            ckmin(beta, eval);
            if (beta<=alpha) break;
        }

        return mn;
    }
}

act_t bot::best_move(state_t &s, int depth) {
    vec<act_t> moves=actions(s, true);

    auto st=chrono::steady_clock::now();

    act_t best;
    double best_ev;
    int vis=0;

    if (s.turn==0) {
        double mx=-1e9;
        for (act_t a : moves) {
            state_t sp=s;
            sp.move(a);
            double ev=minimax(sp, depth, -1e9, 1e9, vis);
            if (ckmax(mx, ev)) {
                best=a;
                best_ev=mx;
            } else if (mx==ev && rand()%100<=50) {
                best=a;
            }
        }
    } else {
        double mn=1e9;
        for (act_t a : moves) {
            state_t sp=s;
            sp.move(a);
            double ev=minimax(sp, depth, -1e9, 1e9, vis);
            if (ckmin(mn, ev)) {
                best=a;
                best_ev=mn;
            } else if (mn==ev && rand()%100<=50) {
                best=a;
            }
        }
    }

    int dur=chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now()-st).count();

    printf("visited %d states, took %.2fs\n", vis, (double)dur/1000);
    printf("%s going for best eval of %.2f\n", s.turn==0?"white":"black", best_ev);
    return best;
}
