#include <iostream>
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

    inline int getX()
    {
        int x = 0;
        int y = 0;
        getyx(stdscr, y, x);
        return x;
    }

    inline int getY()
    {
        int x = 0;
        int y = 0;
        getyx(stdscr, y, x);
        return y;
    }

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
        // Transformation functions. If xTransformFunc is defined it
        // will be ran for each character in the string --- it is used
        // to change the x value of the cursor. Same goes for y.
        // They return the new x or y value, and take in the current
        // index of the character in `text', and the current x or y
        // coordinate.
        std::function<int(std::size_t, int)> xTransformFunc;
        std::function<int(std::size_t, int)> yTransformFunc;
        // If defined, this function will run after everything else has
        // been done.
        std::function<void()> postRunHook;
        // Wait between printing characters.
        chron::milliseconds outputWait = 75ms;

        void doTextEffect() const
        {
            // Get the new x and y position.
            int newCursorX = 0;
            int newCursorY = 0;
            getyx(stdscr, newCursorY, newCursorX);
            newCursorX = dx ? (newCursorX + x) : x;
            newCursorY = dy ? (newCursorY + y) : y;
            // Sleep the thread.
            std::this_thread::sleep_for(wait);
            // Move the newCursorsor.
            move(newCursorY, newCursorX);
            // Print the string if it is defined and run the transform
            // functions.
            if(!text.empty())
            {
                for(std::size_t i = 0; i < text.size(); i++)
                {
                    int curX = newCursorX;
                    int curY = newCursorY;
                    std::this_thread::sleep_for(outputWait);
                    if(xTransformFunc)
                        curX = xTransformFunc(i, newCursorX);
                    if(yTransformFunc)
                        curY = yTransformFunc(i, newCursorY);
                    // Might sometimes cause text to go off screen if
                    // curX + i is greater than the width.
                    mvprintw(curY, curX + i, "%c", text[i]);
                    refresh();
                }
            }
            if(postRunHook)
                postRunHook();
            refresh();
        }
    };
}

int main(int argc, const char * const argv[])
{
    std::size_t startTextEffect = 0;
    const static std::array textEffects = {
        textEffect{ 2000ms, 1, 0, true, true },
        textEffect{ 2000ms, 2, 0, true, true },
        textEffect{ 2000ms, 2, 0, true, true },
        textEffect{ 2000ms, 2, 0, true, true },
        textEffect{ 2000ms, 1, 1, true, true },
        textEffect{ 100ms, 0, 0, false, true, "You'd said I'd wake up\\"},
        textEffect{ 100ms, 0, 1, false, true, "Dead drunk\\"},
        textEffect{ 100ms, 0, 1, false, true, "Alone in the park"},
        textEffect{ 2000ms, 1, 0, true, true },
        textEffect{ 2000ms, 1, 0, true, true },
        textEffect{ 100ms, 1, 1, false, true, "I called you a LIAR",
            [](std::size_t index, int curX) -> int {
                return curX + static_cast<int>(index);
            },
            [](std::size_t index, int curY) -> int {
                return curY + static_cast<int>(index);
            }
        },

    };
    // Init ncurses.
    initscr();
    raw();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    scrollok(stdscr, TRUE);
    // Init the cursor (set to green and blinking).
    //std::cout << "\e]PFFFFFFFF " << std::flush;
    // Get the maximum x and y and decrease the maximum y and x values
    // to 0-count them.
    getmaxyx(stdscr, maxY, maxX);
    maxY--;
    maxX--;

    refresh();
    for(auto it = textEffects.begin() + startTextEffect;
        it < textEffects.end(); it++)
    {
        it->doTextEffect();
        // If the keyboard was hit, pause playing until it is hit again.
        if(getch() != ERR)
            while(getch() == ERR) std::this_thread::sleep_for(50ms);
    }

    endwin();
    return 0;
}
