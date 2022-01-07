#include <iostream>
#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <string>

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
}

int main(int argc, const char * const argv[])
{
    clearScreen();
    moveCursor(20, 5);
    printStr("Lol");
    sleep(5);
    return 0;
}
