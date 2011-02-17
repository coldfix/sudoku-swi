#ifndef __SUDOKU__SOLVER_H__INCLUDED__
#define __SUDOKU__SOLVER_H__INCLUDED__

#include "sudoku.h"

#include <vector>


namespace sudoku
{


typedef std::vector<value> field;



class Solver
    : protected Size
{
public:

	/// consts & types

	enum { state_error = 0 };

    
    enum{
        method_findany,
        method_findunique,
        method_test
    };


	///

	Solver();
	explicit Solver(const Size&);
	explicit Solver(const Sudoku&);
	~Solver();
    
	void set(const Sudoku&);
    void set(const Size&);
    Sudoku get() const;

    
	void solve(int max_depth, int method=method_findunique);			// solve until error / ambiguous
	void test(int max_depth);
	void fill();

	void deepthoughts(bool smart, bool clever);
	void thinksmart();
	void thinkclever();

    //
    void feed(int a, value v);
    void discard(int a, value v);
    
    
	///

	bool dirty() const { return dirty_; }

	int unsolved() const { return unsolved_; }
	bool solved() const { return unsolved() == 0; }

	bool error() const { return error_; }
	bool ok() const { return ! error(); }				//no error has been processed so far


	bool canfill() const { return hassolution_; }		//at least one solution has been found
	bool unique() const { return unique_; }				//the sudoku is solvable and there is only one solution that has been found.
	bool toohard() const { return toohard_; }			//Solver couldn't make sure there is no other way to solve the sudoku than the found solutions
	int depth() const { return depth_; }			    //

	int difficulty() const { return difficulty_; }
	bool smart() const { return smart_; }
	bool clever() const { return clever_; }
	bool multiple() const { return multiple_; }


	///

    bool ok(int a) const                { return cells_[a] != state_error && (!solved(a) || possible(a, solution(a))); }
	bool solved(int a) const            { return cells_[a] > 0; }
    
    bool* gourmets(value v)             { return values_ + (v-1) * area(); }
    const bool* gourmets(value v) const { return values_ + (v-1) * area(); }
    bool possible(int a, value v) const { return gourmets(v)[a]; }

	value solution(int a) const         { return cells_[a]; }


    int repertoire(int a) const         { return cells_[a] > 0 ? 1 : -cells_[a]; }
    void repertoire(int a, field& v) const;
    int repertoire(int a, int i) const;


    bool early() const                  { return error() && immediate_; }

	/// internal
protected:

	/// members

	signed char * cells_;
	bool * values_;
    
    bool error_, dirty_, smart_,clever_;
	int unsolved_;

	bool toohard_, unique_, hassolution_, multiple_, immediate_;
	int depth_, difficulty_;

    Size size_;
};


} // ns sudoku

#endif // include guard

