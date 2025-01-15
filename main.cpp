#include "template.h"

// 2d point
struct pt_t {
    int r,c;
};

// action src->dst
struct act_t {
    pt_t src, dst;
};

struct game_t {
    string board;
    int turn; // 0=white, 1=black

    game_t() {
        board = "rnbqkbnr"
                "pppppppp"
                "........"
                "........"
                "........"
                "........"
                "PPPPPPPP"
                "RNBQKBNR";
        turn=0;
    }

    // possible actions for current player
#include "gpt/game-actions"

    // don't pass .
    int color(char c) {
        if (c=='.') return -1;
        if (c<='Z') return 0;
        if (c>='a') return 1;
        assertm(false, "invalid character passed to game color() function");
    }

    char &atref(int r, int c) {
        return board[r*8+c];
    }

    char at(int r, int c) {
        return board[r*8+c];
    }

    string repr() {
        string s;
        for (int r=0; r<8; ++r) {
            for (int c=0; c<8; ++c) {
                s.push_back(at(r,c));
            }
            s.push_back('\n');
        }

        s.pop_back();
        return s;
    }
};

// play interactive chess game
#include "gpt/play"

int main() {
    game_t game;
    game.board = "........"
                 "........"
                 "....k..."
                 "....Q..."
                 "....K..."
                 "........"
                 "........"
                 "........";

    play(game);
}
