#ifndef __SUDOKU__GENERATOR_H__INCLUDED__
#define __SUDOKU__GENERATOR_H__INCLUDED__

#include "sudoku.h"

namespace sudoku
{

class Generator
{
public:
    Generator();
    Generator(const Sudoku&);
    Generator(const Size&);

    void set(const Sudoku&);
    void set(const Size&);

	const Sudoku& get() const { return m_sudoku; }


    bool fill();
    bool obfuscate(int difficulty, int min_fill=-1, int max_fill=-1, int max_depth=-1);
    bool generate(int difficulty, int min_fill=-1, int max_fill=-1, int max_depth=-1);


    void initRand();

protected:
    static bool s_initialized;
	Sudoku m_sudoku;

};


} // ns sudoku

#endif // include guard
