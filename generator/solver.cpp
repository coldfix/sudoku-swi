// #include <cstring>       // memcpy, memset

#include <algorithm>    // fill_n
#include <vector>
#include <functional>
#include "solver.h"


// to do:
// > update queue [ring buffer] (to avoid multiple updating)
// > conflict domain (to improve backtracking)


namespace std
{
    //--------------------------------------------------
    // template <class T>
    //     bool equal_to(const T& a, const T& b) {
    //         return a == b;
    //     }
    //--------------------------------------------------
    //--------------------------------------------------
    // template <class I, class P>
    //     bool any_of(I begin, I end, P pred) {
    //         return find_if(begin, end, pred) != end;
    //     }
    //--------------------------------------------------
    template <class C, class P>
        bool any_of(C container, P pred) {
            return any_of(container.begin(), container.end(), pred);
        }
}


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
            cells_ = new value[area()];
            values_ = new bool[line() * area()];
        }
        //std::memset(values_, (char) 1, line() * area());
        //std::memset(cells_, (char) -line(), area());
        std::fill_n(values_, line() * area(), true);
        std::fill_n(cells_, area(), (char) -line());
    }

    error_ = !initialized();
    immediate_ = false;
    unsolved_ = area();
    dirty_ = true;
}


void Solver::set(const Sudoku& sudoku)
{
    set(sudoku.size());

    for (coord a = 0; a < area(); a++)
        if (sudoku.get(a))
            feed(a, sudoku.get(a));
}



/*
*/


Sudoku Solver::get() const
{
    Sudoku sudoku((const Size&)*this);
    for (coord a = 0; a < area(); a++)
        if (solved(a))
            sudoku.set(a, solution(a));
    return sudoku;
}

void Solver::repertoire(coord a, field& va) const
{
    va.clear();
    va.reserve(line());
    for (value v = min_value(); v <= max_value(); v++)
        if (possible(a, v))
            va.push_back(v);
}

value Solver::repertoire(coord a, coord i) const
{
    for (value v = min_value(); v <= max_value(); v++)
        if (possible(a, v) && ! i--)
            return v;
    return 0;
}




/*
 * Set a cell to a specified value
*/

void Solver::feed(coord a, value val)
{
    if (solved(a))
        return;
    --unsolved_;
    dirty_ = true;

    cells_[a] = val;                                // update field info
    for (value v = min_value(); v <= max_value(); v++)
        if (v != val)
            gourmets(v)[a] = false;

    bool * vpossible = gourmets(val);
    if (!vpossible[a])
        error_ = true;
    if (early())
        return;

    coord y = y_a(a),
          x = x_a(a),
          b = b_xy(x,y),
          f = f_xy(x,y);

    // constraint forwarding..
    for (coord u = 0; u < line(); ++u) {
        if (vpossible[a_xy(u, y)] && u != x) discard(a_xy(u, y), val);
        if (vpossible[a_xy(x, u)] && u != y) discard(a_xy(x, u), val);
        if (vpossible[a_bf(b, u)] && u != f) discard(a_bf(b, u), val);
        if (early())
            return;
    }
}


void Solver::discard(coord a, value val)
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

    if (++cells_[a] == -1) {    // just solved the cell? so lets go and publish the new value!
        for (value v = min_value(); v <= max_value(); v++) {
            if (possible(a, v)) {
                feed(a, v);
                break;
            }
        }
    }
}


void Solver::deepthoughts(bool smart, bool clever, bool test_alldiff)
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
    if (test_alldiff && !solved() && !error() && !alldiff_constraints())
        error_ = true;
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
    for (coord a = 0; a < area() && !dirty(); a++) {            // search for unique values
        if (solved(a))
            continue;
        coord x = x_a(a), y = y_a(a),
              b = b_a(a), f = f_a(a);

        for (value v = min_value(); v <= max_value(); v++) {
            bool* possible = gourmets(v);
            if (!possible[a])
                continue;

            bool solve = true;
            for (coord xi = 0; xi < line(); xi++)               // in one row
                if (possible[a_xy(xi, y)] && xi != x) {
                    solve = false;
                    break; }
            if (solve) {
                feed(a, v);
                break; }

            solve = true;
            for (coord yi = 0; yi < line(); yi++)               // in one column
                if (possible[a_xy(x, yi)] && yi != y) {
                    solve = false;
                    break; }
            if (solve) {
                feed(a, v);
                break; }

            solve = true;
            for (coord fi = 0; fi < line(); fi++)               // in one box
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
    then discard V in the complement unit CU of SU (CU [+] SU = U)

    CU is (besides U) the only unit that completely includes SU
*/

void Solver::thinkclever()
{
    clever_ = true;
    for (value v = min_value(); v <= max_value(); v++) {        //search vals that are possible in only
        bool *possible = gourmets(v);

        for (coord x = 0; x < line(); x++) {            // one box's col within a col
            bool operate = false;
            coord yb;
            for (coord y = 0; y < line(); y++)
                if (possible[a_xy(x,y)]) {
                    operate = !solved(a_xy(x,y));
                    yb = yb_y(y);
                    break; }
            if (!operate)
                continue;
            for (coord y = y_hl(yb+1, 0); y < line(); y++)
                if (possible[a_xy(x, y)]) {
                    operate = false;
                    break; }
            if (!operate)
                continue;
            coord b = b_hl(xb_x(x), yb),                // and remove this possibility from the rest of the box
                xf = xf_x(x);
            for (coord f = 0; f < line(); f++)
                if (xf_f(f) != xf && possible[a_bf(b,f)]) {
                    discard(a_bf(b,f), v);
                    dirty_ = true; }
            if (dirty())
                return;
        }

        for (coord y = 0; y < line(); y++) {            // in one box's row of a row
            bool operate = false;
            coord xb;
            for (coord x = 0; x < line(); x++)
                if (possible[a_xy(x,y)]) {
                    operate = !solved(a_xy(x,y));
                    xb = xb_x(x);
                    break; }
            if (!operate)
                continue;
            for (coord x = x_hl(xb+1, 0); x < line(); x++)
                if (possible[a_xy(x,y)]) {
                    operate = false;
                    break; }
            if (!operate)
                continue;
            coord b = b_hl(xb, yb_y(y)),                // and remove this possibility from the rest of the box
                yf = yf_y(y);
            for (coord f = 0; f < line(); f++)
                if (yf_f(f) != yf && possible[a_bf(b,f)]) {
                    discard(a_bf(b,f), v);
                    dirty_ = true; }
            if (dirty())
                return;
        }

        for (coord b = 0; b < line(); b++){         // in one col of a box
            bool operate = false;
            coord xf;
            for (coord f = 0; f < line(); f++)
                if (possible[a_bf(b, f)]) {
                    operate = !solved(a_bf(b, f));
                    xf = xf_f(f);
                    break; }
            if (!operate)
                continue;
            for (coord f = f_hl(xf+1, 0); f < line(); f++)
                if (possible[a_bf(b, f)]) {
                    operate = false;
                    break; }
            if (!operate)
                continue;
            coord x=x_hl(xb_b(b),xf),                   // and remove these vals in the rest of the col
                yb=yb_b(b);
            for (coord y = 0; y < line(); y++)
                if (yb_y(y) != yb && possible[a_xy(x,y)]) {
                    discard(a_xy(x,y), v);
                    dirty_ = true; }
            if (dirty())
                return;
        }

        for (coord b = 0; b < line(); b++) {            // in one row of a box
            bool operate = false;
            coord yf;
            for (coord f = 0; f < line(); f++)
                if (possible[a_bf(b, f)]) {
                    operate = !solved(a_bf(b, f));
                    yf = yf_f(f);
                    break; }
            if (!operate)
                continue;
            for (coord f = f_hl(0, yf+1); f < line(); f++)
                if (possible[a_bf(b,f)] && yf_f(f) != yf) {
                    operate = false;
                    break; }
            if (!operate)
                continue;
            coord y = y_hl(yb_b(b), yf),                // and remove these vals in the rest of the row
                xb = xb_b(b);
            for (coord x = 0; x < line(); x++)
                if (xb_x(x) != xb && possible[a_xy(x,y)]) {
                    discard(a_xy(x,y), v);
                    dirty_ = true; }
            if (dirty())
                return;
        }
    }
}

/*
 * Walk through every unit U and check if there are more than
 * sizeof(U) possible values over all.
 */

bool Solver::alldiff_constraints()
{
    std::vector<bool>
        row_values(num_values(), false),
        col_values(num_values(), false),
        block_values(num_values(), false);
    for (coord u = 0; u < line(); ++u) {
        // to do: speed this up by looking at cells first which allow all values (?)
        for (coord w = 0; w < line(); ++w) {
            for (value v = min_value(); v <= max_value(); ++v) {
                if (possible(a_xy(w, u), v)) row_values[v - 1] = true;
                if (possible(a_xy(u, w), v)) col_values[v - 1] = true;
                if (possible(a_bf(u, w), v)) block_values[v - 1] = true;
            }
        }
        if (std::any_of(row_values, std::bind2nd(std::equal_to<bool>(), false))
                || std::any_of(col_values, std::bind2nd(std::equal_to<bool>(), false))
                || std::any_of(block_values, std::bind2nd(std::equal_to<bool>(), false)))
            return false;
    }
    return true;
}


/*

*/




/// solve


void Solver::fill(int max_backtrack)
{
    solve(-1, Method::findany, max_backtrack);
}

void Solver::test(int max_depth)
{
    solve(max_depth, Method::test);
}


struct L {
    L (coord _a) : a(_a)  {}
    coord a;
    field values;
};

void Solver::solve(int max_depth, Method method, int max_backtrack)
{
    bool smart = true, clever = false;

    depth_ = 0;
    smart_ = false;
    clever_ = false;
    immediate_ = method == Method::findany || method == Method::test;
    // deepthoughts(smart, clever, method == Method::findany);
    deepthoughts(smart, clever);

    Sudoku solution,
            state = get();


    std::vector<L> track;
    int currentlevel = 0,
        backtracks = 0;
    bool found = solved() && !error(),
        toohard = false, multiple = false, usesolution = false,
        pushlevel = !solved() && !error();

    while (true) {
        if (pushlevel) {
            if (currentlevel == max_depth){
                toohard = true;
                break;
            }
            // apply MRV heuristic:
            // (to do: gradheuristic [waehle zelle,wert mit groesstem verzweigungsgrad])
            coord a = 0,
                cardinality = line() + 1;
            for (coord ai = 0; ai < area(); ai++)
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
        if (l.values.empty()) {                 // pop level
            track.pop_back();
            --currentlevel;
            if (!track.empty())
                state.set(track.back().a, 0);
            set(state);
            pushlevel = false;
            if (++backtracks == max_backtrack) {
                toohard_ = true;
                error_ = false;
                break;
            }
        } else {
            // to do: heuristik des geringst einschraenkenden wertes
            int iv = l.values.size()-1;
            value v = l.values[iv];
            l.values.erase(l.values.begin() + iv);
            feed(l.a, v);
            // deepthoughts(smart, clever, method == Method::findany);
            deepthoughts(smart, clever);

            if (solved() && !error()) {
                if (found)
                    multiple = true;

                if (multiple && (method == Method::findunique || method == Method::test)) {
                    multiple = true;
                    break;
                } else {
                    found = true;
                    if (method == Method::findany)
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
            for (std::size_t i = 0; i < track.size(); i++) {
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
    else        // ... error
    {
        if (depth_) {
            for (std::size_t i = 0; i < track.size(); i++){
                L& l = track[i];
                state.set(l.a, 0);
            }
            set(state);
            thinksmart();
        }
        error_ = true;
    }

    if (!smart_)
        difficulty_ = Difficulty::Easy;
    else if (depth_ == 0 && !clever_)
        difficulty_ = Difficulty::Medium;
    else
        difficulty_ = Difficulty::Hard;

    multiple_ = multiple;
    unique_ = solved() && !error();
    toohard_ = toohard;
    hassolution_ = found;

    immediate_ = false;
}

}

