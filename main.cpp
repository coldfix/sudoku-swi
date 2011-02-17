#include <iostream>
#include <sstream>
using namespace std;


#include "generator.h"


int main(int argc, char* argv[])
{
	int dim_x = 3, dim_y = 3;
	if (argc >= 2) {
		istringstream parse(argv[1]);
		char c;
		if (parse >> dim_x >> c >> dim_y && c == 'x') {
		}
		else {
			dim_x = dim_y = 3;
		}
	}

    sudoku::Generator generator(sudoku::Size(dim_x, dim_y));
    generator.generate(sudoku::difficulty_medium);

    sudoku::Sudoku sk = generator.get();
    sudoku::Size size = sk.size();


    //cout << size.bx() << ' ' << size.by() << '\n';
    
    for (int y = 0; y < size.line(); y++) {
        if (y && y % size.fy() == 0)
            cout << '\n';
        
        for (int x = 0; x < size.line(); x++) {
            if (x)
                cout << (x % size.fx() ? " " : "  ");

            cout << (int) sk.get(x, y);
        }

        cout << '\n';
    }
    cout << flush;
    
    return 0;
}

