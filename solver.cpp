#include "solver.h"
#include <cstring>		// memcpy, memset

namespace sudoku
{



/*
*/

Solver::Solver()
{
}

Solver::Solver(const Size& size)
{
	set(size);
}

Solver::Solver(const Sudoku& sudoku)
{
	set(sudoku);
}

Solver::~Solver()
{
	if (initialized()) {
		delete [] cells_;
		delete [] values_;
    }
}


/*
*/

void Solver::set(const Size& size)
{
	bool realloc = size.area() != area();

	if (initialized() && realloc){
		delete [] cells_;
		delete [] values_;
    }

	Size::set(size.bx(), size.by());

	if (initialized()){
		if (realloc) {
			cells_ = new signed char[area()];
			values_ = new bool[line() * area()];
        }
        std::memset(values_, 1, line() * area());
        std::memset(cells_, (char) -line(), area());
	}

	error_ = !initialized();
	immediate_ = false;
	unsolved_ = area();
	dirty_ = true;
}


void Solver::set(const Sudoku& sudoku)
{
	set(sudoku.size());

	for (int a = 0; a < area(); a++)
		if (sudoku.get(a))
			feed(a, sudoku.get(a));
}



/*
*/


Sudoku Solver::get() const
{
	Sudoku sudoku((const Size&)*this);
	for (int a = 0; a < area(); a++)
		if (solved(a))
			sudoku.set(a, solution(a));
	return sudoku;
}

void Solver::repertoire(int a, field& va) const
{
	va.clear();
	va.reserve(line());
	for (value v = 1; v <= line(); v++)
		if (possible(a, v))
			va.push_back(v);
}

int Solver::repertoire(int a, int i) const
{
	for (value v = 1; v <= line(); v++)
		if (possible(a, v) && ! i--)
			return v;
	return 0;
}




/*
*/


void Solver::feed(int a, value val)
{
	if (solved(a))
		return;
	--unsolved_;
	dirty_ = true;

	cells_[a] = val;								// update field info
	for (value v = 1; v <= line(); v++)
		if (v != val)
			gourmets(v)[a] = false;

    bool * vpossible = gourmets(val);
	if (!vpossible[a])
		error_ = true;

	if (early())
		return;


	int y = y_a(a),
		x = x_a(a);

	for (int xi = 0; xi < line(); xi++) {				// walk row
		if (vpossible[a_xy(xi, y)] && xi != x){
			discard(a_xy(xi, y), val);
			if (early())
				return;
        }
    }

	for (int yi = 0; yi < line(); yi++) {				// walk col
		if (vpossible[a_xy(x, yi)] && yi != y){
			discard(a_xy(x,yi), val);
			if (early())
				return;
        }
    }

	int b = b_xy(x,y),
		f = f_xy(x,y);

	for (int fi = 0; fi < line(); fi++) {				// walk box
		if (vpossible[a_bf(b, fi)] && fi != f){
			discard(a_bf(b, fi), val);
			if (early())
				return;
        }
    }
}


void Solver::discard(int a, value val)
{
	if (!possible(a,val))
		return;
    gourmets(val)[a] = false;
	dirty_ = true;

	if (solution(a) == val) {
		error_ = true;
		return;
    }
	if (solved(a))              // return if cell had already been solved..
		return;

	++ cells_[a];
    if (repertoire(a) == 1) {   // just solved the cell? so lets go and publish the new value!
		for (value v = 1; v <= line(); v++) {
			if (possible(a, v)) {
				feed(a,v);
				break;
            }
        }
	}
}


void Solver::deepthoughts(bool smart, bool clever)
{
	while (dirty() && !solved() && !error()) {
		dirty_ = false;
		if (smart)
			thinksmart();
		if (dirty())
			continue;
		if (clever)
			thinkclever();
	}
}

/*

	thinksmart algorithm: 

	walk through unit U
	if C is the only cell of U that can possibly be solved by the value V
	then solve cell C with the value V

*/

void Solver::thinksmart()
{
    smart_ = true;
	for (int a = 0; a < area() && !dirty(); a++) {			// search for unique values
		if (solved(a))
			continue;
		int x = x_a(a), y = y_a(a),
			b = b_a(a), f = f_a(a);

		for (value v = 1; v <= line(); v++) {
            bool* possible = gourmets(v);
			if (!possible[a])
				continue;

			bool solve = true;
			for (int xi = 0; xi < line(); xi++)				// in one row
				if (possible[a_xy(xi, y)] && xi != x) {
					solve = false;
					break; }
			if (solve) {
				feed(a, v);
				break; }

			solve = true;
			for (int yi = 0; yi < line(); yi++)				// in one column
				if (possible[a_xy(x, yi)] && yi != y) {
					solve = false;
					break; }
			if (solve) {
				feed(a, v);
				break; }

			solve = true;
			for (int fi = 0; fi < line(); fi++)				// in one box
				if (possible[a_bf(b, fi)] && fi != f) {
					solve = false;
					break; }
			if (solve) {
				feed(a, v);
				break; }
		}
	}
	if (dirty()){
		// .. debug count.. ?
	}
}


/*

	thinkclever algorithm

	walk through unit U.
	if there is a subunit SU which can hold a value V as the only subunit of U,
	then discard V in the accurate "cross-unit" CU of SU

	CU is (besides U) the only unit that completely includes SU
*/

void Solver::thinkclever()
{
    clever_ = true;
	for (value v = 1; v <= line(); v++) {		//search vals that are possible in only
        bool *possible = gourmets(v);

		for (int x = 0; x < line(); x++) {			// one box's col within a col
			bool operate = false;
			int yb;
			for (int y = 0; y < line(); y++)
				if (possible[a_xy(x,y)]) {
					operate = !solved(a_xy(x,y));
					yb = yb_y(y);
					break; }
			if (!operate)
				continue;
			for (int y = y_hl(yb+1, 0); y < line(); y++)
				if (possible[a_xy(x, y)]) {
					operate = false;
					break; }
			if (!operate)
				continue;
			int b = b_hl(xb_x(x), yb),				// and remove this possibility from the rest of the box
				xf = xf_x(x);
			for (int f = 0; f < line(); f++)
				if (xf_f(f) != xf && possible[a_bf(b,f)]) {
					discard(a_bf(b,f), v);
					dirty_ = true; }
			if (dirty())
				return;
		}

		for (int y = 0; y < line(); y++) {			// in one box's row of a row
			bool operate = false;
			int xb;
			for (int x = 0; x < line(); x++)
				if (possible[a_xy(x,y)]) {
					operate = !solved(a_xy(x,y));
					xb = xb_x(x);
					break; }
			if (!operate)
				continue;
			for (int x = x_hl(xb+1, 0); x < line(); x++)
				if (possible[a_xy(x,y)]) {
					operate = false;
					break; }
			if (!operate)
				continue;
			int b = b_hl(xb, yb_y(y)),				// and remove this possibility from the rest of the box
				yf = yf_y(y);
			for (int f = 0; f < line(); f++)
				if (yf_f(f) != yf && possible[a_bf(b,f)]) {
					discard(a_bf(b,f), v);
					dirty_ = true; }
			if (dirty())
				return;
		}

		for (int b = 0; b < line(); b++){			// in one col of a box
			bool operate = false;
			int xf;
			for (int f = 0; f < line(); f++)
				if (possible[a_bf(b, f)]) {
					operate = !solved(a_bf(b, f));
					xf = xf_f(f);
					break; }
			if (!operate)
				continue;
			for (int f = f_hl(xf+1, 0); f < line(); f++)
				if (possible[a_bf(b, f)]) {
					operate = false;
					break; }
			if (!operate)
				continue;
			int x=x_hl(xb_b(b),xf),					// and remove these vals in the rest of the col
				yb=yb_b(b);
			for (int y = 0; y < line(); y++)
				if (yb_y(y) != yb && possible[a_xy(x,y)]) {
					discard(a_xy(x,y), v);
					dirty_ = true; }
			if (dirty())
				return;
		}

		for (int b = 0; b < line(); b++) {			// in one row of a box
			bool operate = false;
			int yf;
			for (int f = 0; f < line(); f++)
				if (possible[a_bf(b, f)]) {
					operate = !solved(a_bf(b, f));
					yf = yf_f(f);
					break; }
			if (!operate)
				continue;
			for (int f = f_hl(0, yf+1); f < line(); f++)
				if (possible[a_bf(b,f)] && yf_f(f) != yf) {
					operate = false;
					break; }
			if (!operate)
				continue;
			int y = y_hl(yb_b(b), yf),				// and remove these vals in the rest of the row
				xb = xb_b(b);
			for (int x = 0; x < line(); x++)
				if (xb_x(x) != xb && possible[a_xy(x,y)]) {
					discard(a_xy(x,y), v);
					dirty_ = true; }
			if (dirty())
				return;
		}
	}
}



/*

*/




/// solve


void Solver::fill()
{
	solve(-1, method_findany);
}

void Solver::test(int max_depth)
{
	solve(max_depth, method_test);
}


struct L {
    L (int _a) : a(_a)  {}
    int a;
    field values;
};
    
void Solver::solve(int max_depth, int method)
{
	bool smart = true, clever = false;

    depth_ = 0;
	smart_ = false;
	clever_ = false;
	immediate_ = method == method_findany || method == method_test;
	deepthoughts(smart, clever);

	Sudoku solution,
            state = get();


	std::vector<L> track;
	int currentlevel=0;
	bool found = solved() && !error(),
		toohard = false, multiple = false, usesolution = false,
		pushlevel = !solved() && !error();

	while (true) {
		if (pushlevel) {
			if (currentlevel == max_depth){
				toohard = true;
				break;
            }
			int a = 0,
                cardinality = line() + 1;
			for (int ai = 0; ai < area(); ai++)
                if (!solved(ai) && repertoire(ai) < cardinality) {  // currently : select cell with smallest repertoire ..(?)
					a = ai;
                    cardinality = repertoire(a); }
			track.push_back(L(a));
            repertoire(a, track.back().values);
			if (++currentlevel > depth_)
				depth_ = currentlevel;
		}

		if (track.empty())
			break;

		L& l = track.back();
		if (l.values.empty()) {					// pop level
			track.pop_back();
			--currentlevel;
			if (!track.empty())
				state.set(track.back().a, 0);
			set(state);
			pushlevel = false;
		} else {
			int iv = l.values.size()-1;
			int v = l.values[iv];
			l.values.erase(l.values.begin() + iv);
			feed(l.a, v);
			deepthoughts(smart, clever);

			if (solved() && !error()) {
                if (found)
                    multiple = true;
                
				if (multiple && (method == method_findunique || method == method_test)) {
					multiple = true;
					break;
				} else {
					found = true;
					if (method == method_findany)
						break;
					solution = get();
					usesolution = true;
				}
			}

			if (! error() && !solved()){
				state.set(l.a, v);
				pushlevel = true;
			}else{
				if (!l.values.empty())
					set(state);
				pushlevel = false; }
		}
	}

	if (multiple || toohard)
	{
		if (depth_) {
			for (int i = 0; i < track.size(); i++) {
				L& l = track[i];
				state.set(l.a, 0);
            }
			set(state);
			thinksmart();
        }
	}
	else if (found)
	{
		if (usesolution)
			set(solution);
	}
	else		// ... error
	{
		if (depth_) {
			for (int i = 0; i < track.size(); i++){
				L& l = track[i];
				state.set(l.a, 0);
            }
			set(state);
			thinksmart();
        }
		error_ = true;
	}

	if (!smart_)
		difficulty_ = difficulty_easy;
	else if (depth_ == 0 && !clever_)
		difficulty_ = difficulty_medium;
	else
		difficulty_ = difficulty_hard;

	multiple_ = multiple;
	unique_ = solved() && !error();
	toohard_ = toohard;
	hassolution_ = found;

	immediate_ = false;
}

}
