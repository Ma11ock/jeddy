#include <iostream>
#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <chrono>
#include <thread>
#include <array>
#include <functional>
#include "source.hpp"

namespace chron = std::chrono;
using namespace std::string_literals;
using namespace std::chrono_literals;

namespace
{
    inline void moveCursor(int x, int y)
    {
        std::printf("\033[%d;%dH", y, x);
        std::fflush(stdout);
    }

    inline void clearScreen()
    {
        std::printf("\e[1;1H\e[2J");
    }

    inline void printStr(const std::string &str)
    {
        std::printf("%s", str.c_str());
        std::fflush(stdout);
    }

    inline void moveCursorUp(int x)
    {
        std::printf("\033[%dA", x);
        std::fflush(stdout);
    }

    inline void moveCursorDown(int x)
    {
        std::printf("\033[%dB", x);
        std::fflush(stdout);
    }

    inline void moveCursorRight(int x)
    {
        std::printf("\033[%dC", x);
        std::fflush(stdout);
    }
    
    inline void moveCursorLeft(int x)
    {
        std::printf("\033[%dD", x);
        std::fflush(stdout);
    }

    inline void eraseAtCursor(int n = 0)
    {
        std::printf("\033[%dK", n);
    }

    inline void initCursor()
    {
        // Set cursor to block.
        std::printf("\033[?2c");
        // Set cursor color to dark green.
        std::printf("\e]P287AF5F");
        std::fflush(stdout);
    }


    struct textEffect
    {
        chron::milliseconds wait;
        std::string text;
        std::function<void()> preRunHook;
        std::function<void()> postRunHook;
        int x = 0;
        int y = 0;
    };

    void doTextEffect(const textEffect &effect)
    {
        if(effect.preRunHook)
            effect.preRunHook();
        std::this_thread::sleep_for(wait);
        if(effect.x >= 0 && effect.y >= 0)
            moveCursor(effect.x, effect.y);
        if(effect.text)
            printStr(effect.text);
        if(effect.postRunHook)
            effect.postRunHook();
    }
}

int main(int argc, const char * const argv[])
{
    constexpr std::array textEffects = {
        {}
    };
    clearScreen();

    moveCursor(20, 5);

    std::fwrite(main_cpp, main_cpp_len, 1, stdout);
    printStr("\nLol\n");
    return 0;
}
