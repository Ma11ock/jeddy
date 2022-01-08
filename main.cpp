#include <cstdint>
#include <string>
#include <chrono>
#include <thread>
#include <array>
#include <functional>
#include "source.hpp"

extern "C" {
#include <ncurses.h>
}

namespace chron = std::chrono;
using namespace std::string_literals;
using namespace std::chrono_literals;

namespace
{
    // Maximum x and y values.
    int maxX = 0;
    int maxY = 0;

    struct textEffect
    {
        chron::milliseconds wait;
        // If x or y are 0, ignore them.
        int x = 0;
        int y = 0;
        // If dx is true then the x value is relative to the current
        // position. Same for y.
        bool dx = false;
        bool dy = false;
        std::string text;
        std::function<void()> postRunHook;

        void doTextEffect() const
        {
            // Get the current x and y position.
            int curX = 0;
            int curY = 0;
            getyx(stdscr, curY, curX);
            // Sleep the thread.
            std::this_thread::sleep_for(wait);
            // Move the cursor.
            move(y + (dy ? curY : 0), x + (dx ? curX : 0));
            refresh();
            // Print the string if there is one and run the post run
            // hook.
            if(!text.empty())
                printw("%s", text.c_str());
            if(postRunHook)
                postRunHook();
        }
    };
}

int main(int argc, const char * const argv[])
{
    const static std::array textEffects = {
        textEffect{ 1000ms, 1, 0, true, true },
        textEffect{ 1000ms, 1, 0, true, true },
        textEffect{ 1000ms, 1, 0, true, true },
    };
    // Init ncurses.
    initscr();
    raw();
    noecho();
    // Get the maximum x and y and decrease the maximum y and x values
    // to 0-count them.
    getmaxyx(stdscr, maxY, maxX);
    maxY--;
    maxX--;

    for(const auto &effect : textEffects)
        effect.doTextEffect();

    endwin();
    return 0;
}
