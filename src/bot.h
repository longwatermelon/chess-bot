#pragma once
#include "state.h"

namespace bot {
    double static_eval(const state_t &s);
    double minimax(const state_t &s, int d);
    act_t best_move(const state_t &s, int depth);
}
