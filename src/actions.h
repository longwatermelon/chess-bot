#pragma once
#include "util.h"

struct state_t;
struct act_t;
vec<act_t> actions(const state_t &s, bool legal);
