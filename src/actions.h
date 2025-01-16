#pragma once
#include "state.h"
#include "util.h"

inline void generate_pawn_moves(const state_t& state, int r, int c, vec<act_t>& actions, int player_color) {
    int direction = (player_color == 0) ? -1 : 1;
    int start_row = (player_color == 0) ? 6 : 1;

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
        if (nc >= 0 && nc < 8 && color(state.at(nr, nc)) != player_color) {
            actions.push_back({{r, c}, {nr, nc}});
        }
    }
}

inline void generate_rook_moves(const state_t& state, int r, int c, vec<act_t>& actions, int player_color) {
    for (int dr : {-1, 1}) {
        for (int i = 1; i < 8; ++i) {
            int nr = r + i * dr, nc = c;
            if (nr < 0 || nr >= 8) break;
            if (state.at(nr, nc) == '.') {
                actions.push_back({{r, c}, {nr, nc}});
            } else {
                if (color(state.at(nr, nc)) != player_color) {
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
                if (color(state.at(nr, nc)) != player_color) {
                    actions.push_back({{r, c}, {nr, nc}});
                }
                break;
            }
        }
    }
}

inline void generate_knight_moves(const state_t& state, int r, int c, vec<act_t>& actions, int player_color) {
    const int dr[8] = {-2, -1, 1, 2, 2, 1, -1, -2};
    const int dc[8] = {1, 2, 2, 1, -1, -2, -2, -1};

    for (int i = 0; i < 8; ++i) {
        int nr = r + dr[i], nc = c + dc[i];
        if (nr >= 0 && nr < 8 && nc >= 0 && nc < 8 && (state.at(nr, nc) == '.' || color(state.at(nr, nc)) != player_color)) {
            actions.push_back({{r, c}, {nr, nc}});
        }
    }
}

inline void generate_bishop_moves(const state_t& state, int r, int c, vec<act_t>& actions, int player_color) {
    for (int dr : {-1, 1}) {
        for (int dc : {-1, 1}) {
            for (int i = 1; i < 8; ++i) {
                int nr = r + i * dr, nc = c + i * dc;
                if (nr < 0 || nr >= 8 || nc < 0 || nc >= 8) break;
                if (state.at(nr, nc) == '.') {
                    actions.push_back({{r, c}, {nr, nc}});
                } else {
                    if (color(state.at(nr, nc)) != player_color) {
                        actions.push_back({{r, c}, {nr, nc}});
                    }
                    break;
                }
            }
        }
    }
}

inline void generate_queen_moves(const state_t& state, int r, int c, vec<act_t>& actions, int player_color) {
    generate_rook_moves(state, r, c, actions, player_color);
    generate_bishop_moves(state, r, c, actions, player_color);
}

inline void generate_king_moves(const state_t& state, int r, int c, vec<act_t>& actions, int player_color) {
    const int dr[8] = {-1, -1, -1, 0, 1, 1, 1, 0};
    const int dc[8] = {-1, 0, 1, 1, 1, 0, -1, -1};

    for (int i = 0; i < 8; ++i) {
        int nr = r + dr[i], nc = c + dc[i];
        if (nr >= 0 && nr < 8 && nc >= 0 && nc < 8 && (state.at(nr, nc) == '.' || color(state.at(nr, nc)) != player_color)) {
            actions.push_back({{r, c}, {nr, nc}});
        }
    }
}

inline bool is_king_in_check(const state_t& state, int player_color) {
    int king_row = -1, king_col = -1;
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            char piece = state.at(r, c);
            if ((player_color == 0 && piece == 'K') || (player_color == 1 && piece == 'k')) {
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
            if (piece != '.' && color(piece) != player_color) {
                vec<act_t> opponent_moves;
                switch (tolower(piece)) {
                    case 'p':
                        generate_pawn_moves(state, r, c, opponent_moves, color(piece));
                        break;
                    case 'r':
                        generate_rook_moves(state, r, c, opponent_moves, color(piece));
                        break;
                    case 'n':
                        generate_knight_moves(state, r, c, opponent_moves, color(piece));
                        break;
                    case 'b':
                        generate_bishop_moves(state, r, c, opponent_moves, color(piece));
                        break;
                    case 'q':
                        generate_queen_moves(state, r, c, opponent_moves, color(piece));
                        break;
                    case 'k':
                        generate_king_moves(state, r, c, opponent_moves, color(piece));
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

inline vec<act_t> actions(const state_t& state, int player_color, bool legal) {
    vec<act_t> possible_actions;
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            char piece = state.at(r, c);
            if (piece != '.' && color(piece) == player_color) {
                // Generate moves for each piece based on its type
                switch (tolower(piece)) {
                    case 'p': // Pawn
                        generate_pawn_moves(state, r, c, possible_actions, player_color);
                        break;
                    case 'r': // Rook
                        generate_rook_moves(state, r, c, possible_actions, player_color);
                        break;
                    case 'n': // Knight
                        generate_knight_moves(state, r, c, possible_actions, player_color);
                        break;
                    case 'b': // Bishop
                        generate_bishop_moves(state, r, c, possible_actions, player_color);
                        break;
                    case 'q': // Queen
                        generate_queen_moves(state, r, c, possible_actions, player_color);
                        break;
                    case 'k': // King
                        generate_king_moves(state, r, c, possible_actions, player_color);
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
            char captured_piece = state.at(move.dst.r, move.dst.c);
            char temp = state.at(move.src.r, move.src.c);
            state_t temp_state = state;
            temp_state.atref(move.dst.r, move.dst.c) = temp_state.at(move.src.r, move.src.c);
            temp_state.atref(move.src.r, move.src.c) = '.';

            if (!is_king_in_check(temp_state, player_color)) {
                legal_actions.push_back(move);
            }
        }
        return legal_actions;
    }

    return possible_actions;
}
