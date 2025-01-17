#include "actions.h"
#include "state.h"

bool MV(pt_t src, pt_t dst, const state_t &s, vec<act_t> &moves) {
    act_t a={src,dst};
    if (s.mvok(a)) {
        moves.push_back(a);
        return true;
    }
    return false;
}

bool CA(pt_t src, pt_t dst, const state_t &s, vec<act_t> &moves) {
    act_t a={src,dst};
    if (s.capok(a)) {
        moves.push_back(a);
        return true;
    }
    return false;
}

// p: orig, d: dir
void sweep(pt_t p, pt_t d, const state_t &s, vec<act_t> &moves) {
    pt_t dst=p+d;
    while (MV(p,dst,s,moves)) {
        dst=dst+d;
    }
    CA(p,dst,s,moves);
}

void pawn_moves(const state_t &s, pt_t p, vec<act_t> &moves) {
    int rd=color(s.at(p))==0 ? -1 : 1; // row direction

    // 1 step forward
    MV(p, p+pt_t{rd,0}, s, moves);

    // 2 step forward
    if ((color(s.at(p))==0 && p.r==6) || (color(s.at(p))==1 && p.r==1)) {
        MV(p, p+pt_t{rd*2,0}, s, moves);
    }

    // capture
    CA(p, p+pt_t{rd,-1}, s, moves);
    CA(p, p+pt_t{rd,1}, s, moves);
}

void rook_moves(const state_t &s, pt_t p, vec<act_t> &moves) {
    int d4i[]={-1,0,1,0}, d4j[]={0,1,0,-1};
    for (int k=0; k<4; ++k) {
        sweep(p,{d4i[k],d4j[k]},s,moves);
    }
}

// actions for current state
vec<act_t> actions(const state_t &s, bool legal) {
    vec<act_t> moves;
    for (int i=0; i<8; ++i) {
        for (int j=0; j<8; ++j) {
            if (color(s.at({i,j}))==s.turn) {
                switch (tolower(s.at({i,j}))) {
                case 'p': pawn_moves(s, {i,j}, moves); break;
                case 'r': rook_moves(s, {i,j}, moves); break;
                }
            }
        }
    }

    return moves;
}
