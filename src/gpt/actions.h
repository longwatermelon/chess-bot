#pragma once
#include "state.h"
#include "util.h"

inline void generate_pawn_moves(const state_t& state, int r, int c, vec<act_t>& actions) {
    int direction = (state.turn == 0) ? -1 : 1;
    int start_row = (state.turn == 0) ? 6 : 1;

    // Single step forward
    if (state.at(r + direction, c) == '.') {
        actions.push_back({{r, c}, {r + direction, c}});
    }

    // Double step forward from the starting position
    if (r == start_row && state.at(r + direction, c) == '.' && state.at(r + 2 * direction, c) == '.') {
        actions.push_back({{r, c}, {r + 2 * direction, c}});
    }

    // Captures
    for (int dc : {-1, 1}) {
        int nr = r + direction, nc = c + dc;
        if (nc >= 0 && nc < 8 && state.at(nr,nc)!='.' && color(state.at(nr, nc)) != state.turn) {
            actions.push_back({{r, c}, {nr, nc}});
        }
    }
}

inline void generate_rook_moves(const state_t& state, int r, int c, vec<act_t>& actions) {
    for (int dr : {-1, 1}) {
        for (int i = 1; i < 8; ++i) {
            int nr = r + i * dr, nc = c;
            if (nr < 0 || nr >= 8) break;
            if (state.at(nr, nc) == '.') {
                actions.push_back({{r, c}, {nr, nc}});
            } else {
                if (color(state.at(nr, nc)) != state.turn) {
                    actions.push_back({{r, c}, {nr, nc}});
                }
                break;
            }
        }
        for (int i = 1; i < 8; ++i) {
            int nr = r, nc = c + i * dr;
            if (nc < 0 || nc >= 8) break;
            if (state.at(nr, nc) == '.') {
                actions.push_back({{r, c}, {nr, nc}});
            } else {
                if (color(state.at(nr, nc)) != state.turn) {
                    actions.push_back({{r, c}, {nr, nc}});
                }
                break;
            }
        }
    }
}

inline void generate_knight_moves(const state_t& state, int r, int c, vec<act_t>& actions) {
    const int dr[8] = {-2, -1, 1, 2, 2, 1, -1, -2};
    const int dc[8] = {1, 2, 2, 1, -1, -2, -2, -1};

    for (int i = 0; i < 8; ++i) {
        int nr = r + dr[i], nc = c + dc[i];
        if (nr >= 0 && nr < 8 && nc >= 0 && nc < 8 && (state.at(nr, nc) == '.' || color(state.at(nr, nc)) != state.turn)) {
            actions.push_back({{r, c}, {nr, nc}});
        }
    }
}

inline void generate_bishop_moves(const state_t& state, int r, int c, vec<act_t>& actions) {
    for (int dr : {-1, 1}) {
        for (int dc : {-1, 1}) {
            for (int i = 1; i < 8; ++i) {
                int nr = r + i * dr, nc = c + i * dc;
                if (nr < 0 || nr >= 8 || nc < 0 || nc >= 8) break;
                if (state.at(nr, nc) == '.') {
                    actions.push_back({{r, c}, {nr, nc}});
                } else {
                    if (color(state.at(nr, nc)) != state.turn) {
                        actions.push_back({{r, c}, {nr, nc}});
                    }
                    break;
                }
            }
        }
    }
}

inline void generate_queen_moves(const state_t& state, int r, int c, vec<act_t>& actions) {
    generate_rook_moves(state, r, c, actions);
    generate_bishop_moves(state, r, c, actions);
}

inline void generate_king_moves(const state_t& state, int r, int c, vec<act_t>& actions) {
    const int dr[8] = {-1, -1, -1, 0, 1, 1, 1, 0};
    const int dc[8] = {-1, 0, 1, 1, 1, 0, -1, -1};

    for (int i = 0; i < 8; ++i) {
        int nr = r + dr[i], nc = c + dc[i];
        if (nr >= 0 && nr < 8 && nc >= 0 && nc < 8 && (state.at(nr, nc) == '.' || color(state.at(nr, nc)) != state.turn)) {
            actions.push_back({{r, c}, {nr, nc}});
        }
    }
}

// check if current player can take other player's king
inline bool is_king_in_check(const state_t& state) {
    int king_row = -1, king_col = -1;
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            char piece = state.at(r, c);
            if ((state.turn == 1 && piece == 'K') || (state.turn == 0 && piece == 'k')) {
                king_row = r;
                king_col = c;
            }
        }
    }

    if (king_row == -1 || king_col == -1) {
        std::cerr << "King not found on the board.\n";
        printf("%s\n", state.repr().c_str());
        assert(false);
    }

    // Check if any opponent piece can attack the king's position
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            char piece = state.at(r, c);
            if (piece != '.' && color(piece) == state.turn) {
                vec<act_t> opponent_moves;
                switch (tolower(piece)) {
                    case 'p':
                        generate_pawn_moves(state, r, c, opponent_moves);
                        break;
                    case 'r':
                        generate_rook_moves(state, r, c, opponent_moves);
                        break;
                    case 'n':
                        generate_knight_moves(state, r, c, opponent_moves);
                        break;
                    case 'b':
                        generate_bishop_moves(state, r, c, opponent_moves);
                        break;
                    case 'q':
                        generate_queen_moves(state, r, c, opponent_moves);
                        break;
                    case 'k':
                        generate_king_moves(state, r, c, opponent_moves);
                        break;
                }
                for (const auto& move : opponent_moves) {
                    if (move.dst.r == king_row && move.dst.c == king_col) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

inline vec<act_t> actions(state_t& state, bool legal) {
    vec<act_t> possible_actions;
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            char piece = state.at(r, c);
            if (piece != '.' && color(piece) == state.turn) {
                // Generate moves for each piece based on its type
                switch (tolower(piece)) {
                    case 'p': // Pawn
                        generate_pawn_moves(state, r, c, possible_actions);
                        break;
                    case 'r': // Rook
                        generate_rook_moves(state, r, c, possible_actions);
                        break;
                    case 'n': // Knight
                        generate_knight_moves(state, r, c, possible_actions);
                        break;
                    case 'b': // Bishop
                        generate_bishop_moves(state, r, c, possible_actions);
                        break;
                    case 'q': // Queen
                        generate_queen_moves(state, r, c, possible_actions);
                        break;
                    case 'k': // King
                        generate_king_moves(state, r, c, possible_actions);
                        break;
                }
            }
        }
    }

    // Filter moves for legality if required
    if (legal) {
        vec<act_t> legal_actions;
        for (const auto& move : possible_actions) {
            // Simulate the move and check if it puts the player in check
            char taken=state.temp_move(move);
            if (!is_king_in_check(state)) {
                legal_actions.push_back(move);
            }
            state.undo_move(move, taken);
        }
        return legal_actions;
    }

    return possible_actions;
}
