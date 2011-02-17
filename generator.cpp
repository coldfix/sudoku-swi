
#include <cstdlib>     /* rand functions */
#include <ctime>       /*  */
#include <algorithm>    /* std::generate */
#include <vector>


#include "generator.h"
#include "solver.h"



namespace sudoku
{


/*
*/


int getrand(int range, int min = 0)
{
    return std::rand()/2%range + min;
}


template<class T>
T remove_random(std::vector<T>& v)
{
	int i = getrand(v.size());
	int elem = v[i];
	std::swap(v[i], v[v.size()-1]);
	v.pop_back();
	return elem;
}

class IntSequence {
	int counter;
public:
	IntSequence (int start = 0) : counter(start) { }

	int operator() () {
		return counter++; }
};

/*
*/

 // difficulty settings

int minimum_fill(int difficulty, Size size)
{
    if (size.line() == 9) {
        switch (difficulty) {
            case difficulty_hard:   return 15;
            case difficulty_medium: return 27;
            case difficulty_easy:   return 35;
        }
    }
    else if (size.line() == 6) {
        switch (difficulty) {
            case difficulty_hard:   return 4;
            case difficulty_medium: return 10;
            case difficulty_easy:   return 14;
        }
    }
    else {
        return (int) (size.area() / (2.1 + difficulty * 0.5));
    }
	return 0;
}

int maximum_fill(int difficulty, Size size)
{
    if (size.line() == 9) {
        switch (difficulty) {
            case difficulty_hard:   return 26;
            case difficulty_medium: return 34;
            case difficulty_easy:   return 44;
        }
    }
    else if (size.line() == 6) {
        switch (difficulty) {
            case difficulty_hard:   return 9;
            case difficulty_medium: return 13;
            case difficulty_easy:   return 18;
        }
    }
    else {
        return (int) (size.area() / (1.2 + difficulty * 0.5));
    }
	return size.area();
}

int maximum_depth(int difficulty, Size size)
{
    if (difficulty < difficulty_hard)
        return 0;
    else if (size.line() <= 9)
        return 6;
    else
        return 1;
}




/*
*/

bool Generator::s_initialized = 0;


void Generator::initRand()
{
	if (!s_initialized) {
        std::srand(std::time(0));
		s_initialized = true;
	}
}


Generator::Generator()
{
    initRand();
}

Generator::Generator(const Sudoku& sudoku)
	: m_sudoku(sudoku)
{
    initRand();
}

Generator::Generator(const Size& size)
	: m_sudoku(size)
{
    initRand();
}


void Generator::set(const Sudoku& sudoku)
{
    m_sudoku = sudoku;
}

void Generator::set(const Size& size)
{
    m_sudoku = size;
}


bool Generator::obfuscate(int difficulty, int min_fill, int max_fill, int max_depth)
{
	const Sudoku& sudoku = m_sudoku;

    Size size = m_sudoku.size();

    if (min_fill < 0)   min_fill = minimum_fill(difficulty, size);
    if (max_fill < 0)   max_fill = maximum_fill(difficulty, size);
    if (max_depth < 0)  max_depth = maximum_depth(difficulty, size);


	Solver solve(size);
	Sudoku riddle(size);

	std::vector<int> as(size.area());
	std::generate(as.begin(), as.end(), IntSequence(0));

	while (!solve.solved() || riddle.solved() < min_fill)	// offene felder auswuerfeln
	{
		if (!as.size())
			break;

		int a = remove_random(as);
		if (solve.solved(a) || !sudoku[a])
			continue;

		riddle.set(a, sudoku[a]);
		solve.feed(a, sudoku[a]);

		if (solve.error()){
			return false; }
	}

	if (riddle.solved() >= min_fill)		// obfuscate riddle according to difficulty level
	{
		as.clear();
		as.reserve(riddle.solved());
        for (int a = 0; a < size.area(); a++)
			if (riddle[a])
				as.push_back(a);

		while (as.size() && riddle.solved() > min_fill)
		{
			int a = remove_random(as);
			riddle.set(a, 0);
			solve.set(riddle);
			solve.test(max_depth);

			if (!solve.solved() || solve.difficulty() > difficulty)
			{
				riddle.set(a, sudoku[a]);
				continue;
			}

			if (solve.difficulty() == difficulty
				&& riddle.solved() <= max_fill
				&& getrand(1000) < 350)
			{
				break;
			}
		}
	}

	m_sudoku = riddle;
	return true;
}


bool Generator::fill(int max_backtrack)
{
	Solver solve;
    Size size = m_sudoku.size();
    bool empty = solve.unsolved() == size.area();

    do {
        solve.set(m_sudoku);
        if (empty) {
            for (int f = 0; f < size.line(); f++) {				// fill 1st block
                if (solve.error())                                  // this should be impossible
                    return false;

                int a = size.a_bf(0, f);
                int iv = getrand(solve.repertoire(a));
                solve.feed(a, solve.repertoire(a, iv));
            }

        //	for (int y=solve.fy(); y < solve.line(); y++)				// fill 1st column
        //	{
        //		int a = solve.a_xy(0,y);
        //		int iv = getrand(solve.possibilities(a));
        //		solve.setsolution(a, solve.possibility(a, iv));
        //	}

        }

        solve.fill(max_backtrack);
        if (solve.error())
            return false;
    } while (solve.toohard() && empty);

	m_sudoku = solve.get();
	return true;
}

bool Generator::generate(int difficulty, int min_fill, int max_fill, int max_depth, int max_backtrack)
{
    return fill(max_backtrack) &&
            obfuscate(difficulty, min_fill, max_fill, max_depth);
}



}
