#pragma once
#include "state.h"
#include "actions.h"
#include "bot.h"

inline void play(state_t& game) {
    while (true) {
        // Display the board
        for (int r = 0; r < 8; ++r) {
            for (int c = 0; c < 8; ++c) {
                std::cout << game.at(r, c) << ' ';
            }
            std::cout << '\n';
        }

        printf("eval: %.2f\n", bot::static_eval(game));

        // Show current turn
        std::cout << (game.turn == 0 ? "White's turn" : "Black's turn") << "\n";

        // Check if current player is in check
        if (is_king_in_check(game, game.turn)) {
            std::cout << (game.turn == 0 ? "White is in check!" : "Black is in check!") << "\n";
        }

        // Get possible actions
        vec<act_t> possible_moves = actions(game, game.turn, true);

        if (possible_moves.empty()) {
            std::cout << (game.turn == 0 ? "Black wins!" : "White wins!") << "\n";
            break;
        }

        if (game.turn == 1) { // Black's turn
            act_t move = bot::best_move(game, 2); // Depth set to 3 for example
            std::cout << "Black plays: " << static_cast<char>('a' + move.src.c) << (8 - move.src.r) 
                      << " " << static_cast<char>('a' + move.dst.c) << (8 - move.dst.r) << "\n";

            // Execute the move
            game.atref(move.dst.r, move.dst.c) = game.at(move.src.r, move.src.c);
            game.atref(move.src.r, move.src.c) = '.';

            // Switch turn
            game.turn = 1 - game.turn;
        } else { // White's turn
            while (true) {
                std::string src, dst;
                std::cout << "Enter move (e.g., e2 e4): ";
                std::cin >> src >> dst;

                if (src.length() != 2 || dst.length() != 2) {
                    std::cout << "Invalid format. Use 'e2 e4'.\n";
                    continue;
                }

                int src_col = src[0] - 'a';
                int src_row = 8 - (src[1] - '0');
                int dst_col = dst[0] - 'a';
                int dst_row = 8 - (dst[1] - '0');

                if (src_row < 0 || src_row >= 8 || src_col < 0 || src_col >= 8 ||
                    dst_row < 0 || dst_row >= 8 || dst_col < 0 || dst_col >= 8) {
                    std::cout << "Invalid move coordinates. Try again.\n";
                    continue;
                }

                act_t attempted_move = {{src_row, src_col}, {dst_row, dst_col}};

                // Check if the move is valid
                bool valid_move = false;
                for (const auto& move : possible_moves) {
                    if (move.src.r == attempted_move.src.r && move.src.c == attempted_move.src.c &&
                        move.dst.r == attempted_move.dst.r && move.dst.c == attempted_move.dst.c) {
                        valid_move = true;
                        break;
                    }
                }

                if (valid_move) {
                    // Execute the move
                    game.atref(dst_row, dst_col) = game.at(src_row, src_col);
                    game.atref(src_row, src_col) = '.';

                    // Switch turn
                    game.turn = 1 - game.turn;
                    break;
                } else {
                    std::cout << "Invalid move. Try again.\n";
                }
            }
        }
    }
}
