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

    // enum class Value {
    //     Error = 0
    // };

    enum { state_error = 0 };

    enum class Method {
        findany,
        findunique,
        test
    };


    ///

    Solver();
    explicit Solver(const Size&);
    explicit Solver(const Sudoku&);
    ~Solver();

    void set(const Sudoku&);
    void set(const Size&);
    Sudoku get() const;


    void solve(int max_depth,
            Method method=Method::findunique,       // solve until error / ambiguous
            int max_backtrack  = -1);
    void test(int max_depth);
    void fill(int max_backtrack = -1);

    void deepthoughts(bool smart, bool clever, bool test_alldiff = false);
    void thinksmart();
    void thinkclever();
    bool alldiff_constraints();

    //
    void feed(coord a, value v);
    void discard(coord a, value v);


    ///

    bool dirty() const { return dirty_; }

    coord unsolved() const { return unsolved_; }
    bool solved() const { return unsolved() == 0; }

    bool error() const { return error_; }
    bool ok() const { return ! error(); }               //no error has been processed so far


    bool canfill() const { return hassolution_; }       //at least one solution has been found
    bool unique() const { return unique_; }             //the sudoku is solvable and there is only one solution that has been found.
    bool toohard() const { return toohard_; }           //Solver couldn't make sure there is no other way to solve the sudoku than the found solutions
    int depth() const { return depth_; }                //

    Difficulty difficulty() const { return difficulty_; }
    bool smart() const { return smart_; }
    bool clever() const { return clever_; }
    bool multiple() const { return multiple_; }


    ///

    bool ok(coord a) const              { return cells_[a] != state_error && (!solved(a) || possible(a, solution(a))); }
    bool solved(coord a) const          { return cells_[a] >= min_value(); }

    bool* gourmets(value v)             { return values_ + (v - min_value()) * area(); }
    const bool* gourmets(value v) const { return values_ + (v - min_value()) * area(); }
    bool possible(coord a, value v) const { return gourmets(v)[a]; }

    value solution(coord a) const       { return cells_[a]; }


    coord repertoire(coord a) const     { return cells_[a] > 0 ? 1 : -cells_[a]; }
    void repertoire(coord a, field& v) const;
    value repertoire(coord a, coord i) const;


    bool early() const                  { return error() && immediate_; }

    /// internal
protected:

    /// members

    value* cells_;
    bool * values_;

    bool error_, dirty_, smart_,clever_;
    coord unsolved_;

    bool toohard_, unique_, hassolution_, multiple_, immediate_;
    int depth_;
    Difficulty difficulty_;

    Size size_;
};


} // ns sudoku

#endif // include guard

