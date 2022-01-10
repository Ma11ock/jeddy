#include <iostream>
#include <cstdint>
#include <string>
#include <chrono>
#include <thread>
#include <array>
#include <string_view>
#include <charconv>
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

    // TODO maybe system where there are default transformations,
    // instead of the draw loop incrementing X manually.

    // Text transformation functions that will be used multiple times.

    // 
    int transformVerticalX(std::size_t index, int curX)
    {
        return curX - static_cast<int>(index);
    }

    int transformVerticalY(std::size_t index, int curY)
    {
        return curY + static_cast<int>(index);
    }

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
        std::string_view text;
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
            // Scroll the cursor if necessary.
            if(newCursorY > maxY)
            {
                scrl(newCursorY - maxY);
                newCursorY = maxY; 
            }
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
                    mvaddch(curY, curX + i, text[i]);
                    refresh();
                    if(text[i] == '\n')
                    {
                        newCursorY = std::min(maxY, newCursorY + 1);
                        newCursorX = -i - 1;
                    }
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
    // Starting text effects (for debugging).
    std::size_t startTextEffect = 0;

    // Parse cmd args.
    std::vector<std::string_view> args(argv + 1, argv + argc);
    auto checkIt = [&](const std::vector<std::string_view>::iterator &it) -> bool
    {
        return it < args.end();
    };
    for(auto it = args.begin(); it < args.end(); it++)
    {
        std::string_view str = *it;
        if(str == "-n")
        {
            if(!checkIt(++it))
            {
                std::cerr << "Fatal error: no flag for -n.\n";
                return 1;
            }
            str = *it;
            if(auto result = std::from_chars(str.begin(),
                                             str.begin() + str.size(),
                                             startTextEffect);
               result.ec != std::errc())
            {
                std::cerr << "Fatal error for flag -n: invalid argument.\n";
                return 1;
            }
        }
    }

    // Text effect array.
    const static std::array textEffects = {
        textEffect{ 2000ms, 1, 0, true, true },
        textEffect{ 2000ms, 2, 0, true, true },
        textEffect{ 2000ms, 2, 0, true, true },
        textEffect{ 2000ms, 2, 0, true, true },
        textEffect{ 2000ms, 1, 1, true, true },
        textEffect{ 100ms, 0, 0, false, true, "You'd said I'd wake up\\" },
        textEffect{ 100ms, 0, 1, false, true, "Dead drunk\\" },
        textEffect{ 100ms, 0, 1, false, true, "Alone in the park" },
        textEffect{ 2000ms, 1, 0, true, true },
        textEffect{ 2000ms, 1, 0, true, true },
        textEffect{ 100ms, 1, 1, true, true, "I called you a LIAR",
            [](std::size_t index, int curX) -> int {
                return curX + static_cast<int>(index);
            },
            [](std::size_t index, int curY) -> int {
                return curY + static_cast<int>(index);
            }
        },
        textEffect{ 2000ms, 0, 2, false, true, "But how right you were" },
        textEffect{ 2000ms, 1, 0, true, true },
        textEffect{ 2000ms, 1, 1, true, true },
        textEffect{ 2000ms, 1, 1, true, true },
        textEffect{ 100ms, 0, 0, true, true },
        textEffect{ 2000ms, 1, 1, true, true, "", nullptr, nullptr,
            []()
            {
                clear();
                refresh();
                move(0, 0);
            }
        },
        textEffect{ 2000ms, 1, 1, true, true, "Air Conditioned" },
        textEffect{ 0ms, 0, 0, true, true,    "dnaL VT         ",
            [](std::size_t index, int curX) -> int {
                return curX - (static_cast<int>(index) * 2);
            }
        },
        textEffect{ 0ms, 0, 0, true, true, "Twenty Grand" },
        textEffect{ 0ms, 1, 1, true, true, "W a l k", transformVerticalX,
            transformVerticalY },
        textEffect{ 0ms, 1, 1, true, true, "T o" },
        textEffect{ 0ms, 1, 1, true, true, "T h e" , transformVerticalX,
            transformVerticalY },
        textEffect{ 0ms, 0, 1, true, true, "$$$$$$$$$$$$$$$$$$$$",
            [](std::size_t index, int curX) ->int {
                index++;
                if(index <= 5)
                    return curX - static_cast<int>(index - 1);
                
            },
            [](std::size_t index, int curY) -> int {
                index++;
                if(index <= 5 || index >= (22 - 5))
                {
                    if(index % 5 == 0)
                        return curY + 4;
                    else if(index % 4 == 0)
                        return curY + 3;
                    else if(index % 3 == 0)
                        return curY + 2;
                    else if(index % 2 == 0)
                        return curY + 1;
                    return curY;
                }
                if(index % 2 == 0)
                    return curY;
                return curY + 4;
            },
            nullptr, 16ms
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

    // Draw the source code.
    char *sourceTxt = reinterpret_cast<char*>(main_cpp);
    (textEffect{ 50ms, 0, 0, true, true,
         std::string_view(sourceTxt, main_cpp_len),
         nullptr, nullptr, nullptr, 25ms } ).doTextEffect();

    endwin();
    return 0;
}
