#include "state.h"
// #include "playcli.h"
#include "gpt/play.h"
#include "gpt/play-bot.h"

int main(int argc, char **argv) {
    state_t s;

    int bot_color = argc==1 ? 1 : stoi(argv[1]);
    play(s,bot_color,4);
}
