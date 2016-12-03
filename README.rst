sudoku-cli
----------

Simple command line interface for a sudoku generator.

Coded years ago in horrible C++11 ;)


Build
~~~~~

If you're lucky, have *gcc* on linux, just type::

    make


Usage
~~~~~

Generate "normal" sudoku::

    $ ./sudoku
    9 6 0  0 2 0  7 5 0
    0 0 2  0 3 0  0 0 4
    0 0 0  9 0 0  0 0 0

    0 3 0  4 0 0  0 0 0
    5 2 0  3 0 0  4 0 0
    0 1 7  6 8 2  0 3 0

    0 0 0  0 7 0  0 0 0
    0 0 0  1 0 9  6 0 0
    0 0 4  0 0 0  2 0 8

With solutions blended in::

    $ ./sudoku --solution
    ~9 ~6  3   8  2 ~4  ~7 ~5  1
    ~8 ~5 ~2   7 ~3 ~1  ~9  6 ~4
    ~7  4  1  ~9 ~5 ~6  ~3 ~8 ~2

    ~6 ~3 ~9   4 ~1  5   8  2 ~7
    ~5 ~2  8  ~3  9 ~7   4  1 ~6
    ~4 ~1  7  ~6  8 ~2  ~5  3 ~9

    ~3  9  6  ~2  7 ~8  ~1 ~4  5
    ~2  8  5   1 ~4 ~9  ~6 ~7  3
    ~1 ~7  4  ~5 ~6  3  ~2 ~9 ~8

Custom size::

    $ ./sudoku 3x4
     0  0  6  3   0  4  0  0   0  5  2  0
     0  8  0  2  10  0  0  9   0  0  0  4
    10  0  0  1  12  6  0  0   0  0  0  0

     9  3  0  6   5  0  8  0   0  7  0  0
     0  0 11  5   0  0  4  0   0  1  9 12
     0  1  0  0   0  0  0  0   5  3  0  2

     0  0  0  0   0  0 10  0   4  0  0  0
     0 11  0  0   4  1  0  0   3  0 10  7
     0  0  1  7   8  5  3  0   0  2  0  0

     3  0  9  0   0  0  2  0   8 10  0 11
     0  0  0  0   0  0  0  0   0  0  0  6
     1  0  7  0   6  8  0  0   2  9  0  0

**NOTE:** you shouldn't goo higher than 5v5 – the algorithm is really bad and
will take forever.
