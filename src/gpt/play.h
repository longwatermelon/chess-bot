#include "state.h"
#include "actions.h"
#include "bot.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <unordered_map>

inline void play(state_t& s) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "Failed to initialize SDL_image: " << IMG_GetError() << std::endl;
        SDL_Quit();
        return;
    }

    SDL_Window* window = SDL_CreateWindow("Chess Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 800, 0);
    if (!window) {
        std::cerr << "Failed to create SDL window: " << SDL_GetError() << std::endl;
        IMG_Quit();
        SDL_Quit();
        return;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Failed to create SDL renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return;
    }

    std::unordered_map<std::string, SDL_Texture*> piece_textures;
    const std::string colors[2] = {"w", "b"};
    const std::string pieces[6] = {"bishop", "king", "knight", "pawn", "queen", "rook"};

    // Load textures for all pieces
    for (const auto& color : colors) {
        for (const auto& piece : pieces) {
            std::string path = "res/" + color + "-" + piece + ".png";
            SDL_Surface* surface = IMG_Load(path.c_str());
            if (!surface) {
                std::cerr << "Failed to load image: " << path << " Error: " << IMG_GetError() << std::endl;
                continue;
            }
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_FreeSurface(surface);
            if (!texture) {
                std::cerr << "Failed to create texture from surface: " << SDL_GetError() << std::endl;
                continue;
            }
            piece_textures[color + "-" + piece] = texture;
        }
    }

    bool running = true;
    SDL_Event event;
    pt_t selected = {-1, -1};
    std::vector<pt_t> highlighted_squares;

    // act_t bot_move = bot::best_move(s, 4); // Depth 2 for bot
    // s.move(bot_move);

    while (running) {
        // Handle events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                int x = event.button.x;
                int y = event.button.y;
                pt_t clicked = {y / 100, x / 100};

                if (!highlighted_squares.empty()) {
                    // Check if the click is on a highlighted square
                    auto it = std::find(highlighted_squares.begin(), highlighted_squares.end(), clicked);
                    if (it != highlighted_squares.end()) {
                        // Move the piece
                        act_t move = {selected, clicked};
                        s.move(move);

                        // Render board after player's move
                        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                        SDL_RenderClear(renderer);

                        // Draw the board squares
                        for (int row = 0; row < 8; ++row) {
                            for (int col = 0; col < 8; ++col) {
                                SDL_Rect square = {col * 100, row * 100, 100, 100};
                                SDL_SetRenderDrawColor(renderer, (row + col) % 2 == 0 ? 240 : 80, (row + col) % 2 == 0 ? 217 : 50, (row + col) % 2 == 0 ? 181 : 30, 255);
                                SDL_RenderFillRect(renderer, &square);
                            }
                        }

                        // Draw pieces
                        for (int row = 0; row < 8; ++row) {
                            for (int col = 0; col < 8; ++col) {
                                pt_t pos = {row, col};
                                char piece = s.at(pos);
                                if (piece != '.') {
                                    std::string key;
                                    if (isupper(piece)) {
                                        key = "w-";
                                    } else {
                                        key = "b-";
                                        piece = toupper(piece);
                                    }
                                    switch (piece) {
                                        case 'K': key += "king"; break;
                                        case 'Q': key += "queen"; break;
                                        case 'R': key += "rook"; break;
                                        case 'B': key += "bishop"; break;
                                        case 'N': key += "knight"; break;
                                        case 'P': key += "pawn"; break;
                                        default: continue;
                                    }
                                    SDL_Rect dest = {col * 100, row * 100, 100, 100};
                                    SDL_RenderCopy(renderer, piece_textures[key], nullptr, &dest);
                                }
                            }
                        }

                        SDL_RenderPresent(renderer);

                        // Reset selection
                        selected = {-1, -1};
                        highlighted_squares.clear();

                        // Bot plays
                        act_t bot_move = bot::best_move(s, 3); // Depth 2 for bot
                        s.move(bot_move);
                    } else {
                        // Deselect if clicking elsewhere
                        selected = {-1, -1};
                        highlighted_squares.clear();
                    }
                } else {
                    // Select a piece
                    selected = clicked;
                    highlighted_squares.clear();

                    char piece = s.at(selected);
                    if (piece != '.') {
                        vec<act_t> possible_moves = actions(s, true);
                        for (const auto& move : possible_moves) {
                            if (move.src.r == selected.r && move.src.c == selected.c) {
                                highlighted_squares.push_back(move.dst);
                            }
                        }
                    }
                }
            }
        }

        // Render chessboard
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        // Draw the board squares
        for (int row = 0; row < 8; ++row) {
            for (int col = 0; col < 8; ++col) {
                SDL_Rect square = {col * 100, row * 100, 100, 100};
                SDL_SetRenderDrawColor(renderer, (row + col) % 2 == 0 ? 240 : 80, (row + col) % 2 == 0 ? 217 : 50, (row + col) % 2 == 0 ? 181 : 30, 255);
                SDL_RenderFillRect(renderer, &square);
            }
        }

        // Highlight possible moves
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 128);
        for (const auto& square : highlighted_squares) {
            SDL_Rect highlight = {square.c * 100, square.r * 100, 100, 100};
            SDL_RenderFillRect(renderer, &highlight);
        }

        // Draw pieces
        for (int row = 0; row < 8; ++row) {
            for (int col = 0; col < 8; ++col) {
                pt_t pos = {row, col};
                char piece = s.at(pos);
                if (piece != '.') {
                    std::string key;
                    if (isupper(piece)) {
                        key = "w-";
                    } else {
                        key = "b-";
                        piece = toupper(piece);
                    }
                    switch (piece) {
                        case 'K': key += "king"; break;
                        case 'Q': key += "queen"; break;
                        case 'R': key += "rook"; break;
                        case 'B': key += "bishop"; break;
                        case 'N': key += "knight"; break;
                        case 'P': key += "pawn"; break;
                        default: continue;
                    }
                    SDL_Rect dest = {col * 100, row * 100, 100, 100};
                    SDL_RenderCopy(renderer, piece_textures[key], nullptr, &dest);
                }
            }
        }

        SDL_RenderPresent(renderer);
    }

    // Clean up
    for (auto& pair : piece_textures) {
        SDL_DestroyTexture(pair.second);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
}
