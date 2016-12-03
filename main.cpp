#include <string>
#include <iostream>
using namespace std;


#include "solver.h"
#include "generator.h"


bool isdigit(char c)
{
    return c <= '9' && c >= '0';
}

bool parseInt(char*& iter, int& val)
{
    if (!isdigit(*iter))
        return false;
    for (val = 0; isdigit(*iter); ++iter)
        val = val * 10 + (*iter - '0');
    return true;
}

std::string formatInt(unsigned number)
{
    std::string fmt;
    do {
        fmt = (char)(number % 10 + '0') + fmt;
    } while (number /= 10);
    return fmt;
}

std::string fill(std::string fmt, unsigned width, char space = ' ')
{
    while (width > fmt.size())
        fmt = space + fmt;
    return fmt;
}

int getWidth(int number)
{
    int width = 1;
    while (number /= 10)
        ++width;
    return width;
}

int main(int argc, char* argv[])
{
    int dim_x = 3, dim_y = 3;
    bool printsolution = false;

    for (int carg = 1; carg < argc; ++carg) {
        char* c = argv[carg];
        if (c == std::string("--solution")) {
            printsolution = true;
        } else if (parseInt(c, dim_x) && *(c++) == 'x' && parseInt(c, dim_y)) {
        }
    }


    sudoku::Size size = sudoku::Size(dim_x, dim_y);
    sudoku::Generator generator(size);

    generator.fill();
    sudoku::Sudoku full = generator.get();

    generator.obfuscate(sudoku::Difficulty::Medium);
    sudoku::Sudoku riddle = generator.get();

    sudoku::Sudoku& print = printsolution ? full : riddle;

    int width = getWidth(size.max_value()) + printsolution * 1;

    //cout << size.bx() << ' ' << size.by() << '\n';

    for (int y = 0; y < size.line(); y++) {
        if (y && y % size.fy() == 0)
            cout << '\n';

        for (int x = 0; x < size.line(); x++) {
            if (x)
                cout << (x % size.fx() ? " " : "  ");

            std::string number;
            if (printsolution && riddle.get(x, y) == 0)
                number += '~';
            number += formatInt((int) print.get(x, y));

            cout << fill(number, width);
        }

        cout << '\n';
    }
    cout << flush;

    return 0;
}

