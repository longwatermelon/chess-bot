#include "state.h"
#include "actions.h"
#include "bot.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <unordered_map>

inline void play_bot(state_t& s) {
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

    int winner;
    while (running) {
        // Bot 1 (White) plays
        act_t bot1_move = bot::best_move(s, 3); // Adjust depth as needed
        s.move(bot1_move);

        // Render board after Bot 1's move
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

        // Check for game over
        if (empty(actions(s,true))) {
            running = false;
            winner=0;
            break;
        }

        // Bot 2 (Black) plays
        act_t bot2_move = bot::best_move(s, 3); // Adjust depth as needed
        s.move(bot2_move);

        // Render board after Bot 2's move
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

        // Check for game over
        if (empty(actions(s,true))) {
            running = false;
            winner=1;
        }
    }

    printf("%s wins\n", vec<const char*>{"white","black"}[winner]);

    // Clean up
    for (auto& pair : piece_textures) {
        SDL_DestroyTexture(pair.second);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
}
