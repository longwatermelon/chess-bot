#pragma once
#include "state.h"

namespace bot {
    double static_eval(const state_t &s);
    double minimax(state_t &s, int d, double alpha, double beta, int &visited);
    act_t best_move(const state_t &s, int depth);
}
