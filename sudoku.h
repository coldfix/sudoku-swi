#ifndef __SUDOKU_H__INCLUDED__
#define __SUDOKU_H__INCLUDED__

#include <boost/cstdint.hpp>


namespace sudoku
{
    typedef boost::int16_t coord;
    typedef boost::int8_t value;

    enum class Difficulty { Easy, Medium, Hard };


/*
    class Size
        > represents the dimensions of a sudoku
        > dimensions can be set at runtime
*/

class Size
{
public:
    Size() : nbx(), nby(), row(), total() { }
    Size(coord bx, coord by)        { set(bx,by); }
    Size(const Size& s) = default;

    void set(coord _nbx, coord _nby)
    {
        nbx = _nbx;
        nby = _nby;
        row = nbx * nby;
        total = row * row;
    }

    /// size
    coord bx() const                { return nbx; }
    coord by() const                { return nby; }
    coord fx() const                { return nby; }
    coord fy() const                { return nbx; }

    coord line() const              { return row; }
    coord area() const              { return total; }

    bool initialized() const        { return bx() && by(); }

    value max_value() const         { return row; }
    value min_value() const         { return 1; }
    coord num_values() const        { return row; }

    bool operator  == (const Size& ds) const
    {
        return bx() == ds.bx() && by() == ds.by();
    }

    bool operator  != (const Size& ds) const
    {
        return !(*this == ds);
    }

    /// coord transformations
    coord a_xy(coord x,coord y) const   { return x * line() + y; }
    coord a_bf(coord b,coord f) const   { return a_xy(x_bf(b,f), y_bf(b,f)); }

    coord x_a(coord a) const            { return a / line(); }
    coord y_a(coord a) const            { return a % line(); }
    coord b_a(coord a) const            { return b_hl(xb_a(a), yb_a(a)); }
    coord f_a(coord a) const            { return f_hl(xf_a(a), yf_a(a)); }

    coord x_bf(coord b,coord f) const   { return x_hl(xb_b(b), xf_f(f)); }
    coord y_bf(coord b,coord f) const   { return y_hl(yb_b(b), yf_f(f)); }
    coord b_xy(coord x,coord y) const   { return b_hl(xb_x(x), yb_y(y)); }
    coord f_xy(coord x,coord y) const   { return f_hl(xf_x(x), yf_y(y)); }

    coord x_hl(coord h,coord l) const   { return h * fx() + l; }
    coord y_hl(coord h,coord l) const   { return h * fy() + l; }
    coord b_hl(coord h,coord l) const   { return h * by() + l; }
    coord f_hl(coord h,coord l) const   { return h * fy() + l; }

    coord xb_x(coord x) const           { return x / fx(); }
    coord xf_x(coord x) const           { return x % fx(); }
    coord yb_y(coord y) const           { return y / fy(); }
    coord yf_y(coord y) const           { return y % fy(); }

    coord xb_b(coord b) const           { return b / by(); }
    coord xf_f(coord f) const           { return f / fy(); }
    coord yb_b(coord b) const           { return b % by(); }
    coord yf_f(coord f) const           { return f % fy(); }

    coord xb_a(coord a) const           { return xb_x(x_a(a)); }
    coord xf_a(coord a) const           { return xf_x(x_a(a)); }
    coord yb_a(coord a) const           { return yb_y(y_a(a)); }
    coord yf_a(coord a) const           { return yf_y(y_a(a)); }

protected:
    /// members
    coord nbx, nby, row, total;
};




/*
    class Sudoku
        > represents a sudoku with n_box_x * n_box_y boxes with each n_box_y*n_box_x fields
        > size can be set at runtime

    we dont provide a statSudoku class because the sudoku class isn't used in performance-sensitive context
*/

class Sudoku
{
public:

    /// construction & destruction

    Sudoku()
    {
        data_ = 0;
        unsolved_ = 0;
    }

    Sudoku(Size _size, const value* _data = 0)
    {
        data_ = 0;
        size(_size, _data);
    }

    Sudoku(const Sudoku& sudoku)
        : size_(sudoku.size())
    {
        ref(sudoku.data_);
        unsolved_ = sudoku.unsolved_;
    }

    ~Sudoku(void)
    {
        free(data_);
    }


protected:

    bool shared() const
    {
        return data_ && data_[-1] != 0;
    }

    void unshare(bool copy = true)
    {
        value* old = data_;
        data_ = alloc(size().area());

        if (copy && data_ && old) {
            for (coord i = 0; i < size().area(); i++)
                data_[i] = old[i];
        }
        free(old);
    }

    static value* alloc(coord size)
    {
        if (size == 0)
            return 0;
        value *data = 1 + new value[size + 1];
        data [-1] = 0;
        return data;
    }
    static void free(value * data)
    {
        if (data && data[-1]-- == 0)
            delete [] (data - 1);
        data = 0;
    }
    void ref(value * data)
    {
        if ((data_ = data))
            ++data_[-1];
    }
public:

    void size(const Size& _size, const value* _data = 0)
    {
        Size oldsize = size();
        size_ = _size;
        if (shared() || size() != oldsize)
            unshare(false);
        unsolved_ = _size.area();

        if (_data) {
            for (coord a = 0; a < size().area(); a++)
                if ((data_[a] = _data[a]))
                    --unsolved_;
        }
        else {
            for (coord a = 0; a < size().area(); a++)
                data_[a] = 0;
        }
    }

    Size size() const
    {
        return size_;
    }

    Sudoku& operator = (const Sudoku& sudoku)
    {
        if (sudoku.data_ != data_) {
            free(data_);
            ref(sudoku.data_);
        }
        size_ = sudoku.size();
        unsolved_ = sudoku.unsolved_;
        return *this;
    }


    /// test

    coord unsolved() const { return unsolved_; }
    coord solved() const { return size().area() - unsolved(); }
    bool empty() const { return solved() == 0; }
    bool filled() const { return unsolved() == 0; }
    bool is_solved() const { return filled() && valid(); }

    bool equals(const Sudoku& sudoku) const
    {
        if (size() != sudoku.size())
            return false;
        for (coord a = 0; a < size().area(); a++)
            if (get(a) != sudoku.get(a))
                return false;
        return true;
    }

    bool operator == (const Sudoku& sudoku) const
    {
        return equals(sudoku);
    }

    bool operator != (const Sudoku& sudoku) const
    {
        return ! (*this == sudoku);
    }

    bool valid() const
    {
        for (coord a = 0; a < size().area(); a++)
        {
            coord x = size().x_a(a), y = size().y_a(a),
                b = size().b_a(a), f = size().f_a(a),
                v = get(a);
            if (!v)
                continue;
            for (coord xi = x + 1; xi < size().line(); xi++)
                if (get(size().a_xy(xi, y)) == v)
                    return false;
            for (coord yi = y + 1; yi < size().line(); yi++)
                if (get(size().a_xy(x, yi)) == v)
                    return false;
            for (coord fi = f + 1; fi < size().line(); fi++)
                if (get(size().a_bf(b, fi)) == v)
                    return false;
        }
        return true;
    }


    /// coord-access

    value get(coord a) const
    {
        return data_[a];
    }

    value get(coord x, coord y) const
    {
        return get(size().a_xy(x,y));
    }

    value operator [] (coord a) const
    {
        return get(a);
    }


    void set(coord a, value num)
    {
        if (shared())
            unshare();
        if (num && !data_[a])
            --unsolved_;
        else if (!num && data_[a])
            ++unsolved_;
        data_[a] = num;
    }

    void set(coord x, coord y, value num)
    {
        set(size().a_xy(x,y), num);
    }


private:
    /// members
    value* data_;
    coord unsolved_;
    Size size_;
};





}


#endif // __SUDOKU_H__INCLUDED__
