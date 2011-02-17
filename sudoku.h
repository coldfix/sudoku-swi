#ifndef __SUDOKU_H__INCLUDED__
#define __SUDOKU_H__INCLUDED__


namespace sudoku
{

    typedef int coord;
    typedef unsigned char value;
    
    enum {
        difficulty_easy,
        difficulty_medium,
        difficulty_hard
    };
    


/*
	class Size
		> represents the dimensions of a sudoku
		> dimensions can be set at runtime
*/

class Size
{
public:
	Size()					    { set(0,0); }
	Size(int bx, int by)		{ set(bx,by); }
	Size(const Size& s)		    { set(s.bx(),s.by()); }

    void set(int _nbx, int _nby)
    {
        nbx = _nbx;
        nby = _nby;
        row = nbx * nby;
        total = row * row;
    }

	/// size

	int bx() const					{ return nbx; }
	int by() const					{ return nby; }
	int fx() const					{ return nby; }
	int fy() const					{ return nbx; }
    
	int line() const				{ return row; }
	int area() const				{ return total; }

	bool initialized() const		{ return bx() && by(); }

	int max_value() const			{ return row; }
	int min_value() const			{ return 1; }
    int num_values() const          { return row; }

	bool operator  == (const Size& ds) const
    {
		return bx() == ds.bx() && by() == ds.by();
    }
    bool operator  != (const Size& ds) const
    {
        return !(*this == ds);
    }


	/// coord transformations

	int a_xy(int x,int y) const		{ return x * line() + y; }
	int a_bf(int b,int f) const		{ return a_xy(x_bf(b,f), y_bf(b,f)); }

	int x_a(int a) const			{ return a / line(); }
	int y_a(int a) const			{ return a % line(); }
	int b_a(int a) const			{ return b_hl(xb_a(a), yb_a(a)); }
	int f_a(int a) const			{ return f_hl(xf_a(a), yf_a(a)); }

	int x_bf(int b,int f) const		{ return x_hl(xb_b(b), xf_f(f)); }
	int y_bf(int b,int f) const		{ return y_hl(yb_b(b), yf_f(f)); }
	int b_xy(int x,int y) const		{ return b_hl(xb_x(x), yb_y(y)); }
	int f_xy(int x,int y) const		{ return f_hl(xf_x(x), yf_y(y)); }

	int x_hl(int h,int l) const		{ return h * fx() + l; }
	int y_hl(int h,int l) const		{ return h * fy() + l; }
	int b_hl(int h,int l) const		{ return h * by() + l; }
	int f_hl(int h,int l) const		{ return h * fy() + l; }

	int xb_x(int x) const			{ return x / fx(); }
	int xf_x(int x) const			{ return x % fx(); }
	int yb_y(int y) const			{ return y / fy(); }
	int yf_y(int y) const			{ return y % fy(); }

	int xb_b(int b) const			{ return b / by(); }
	int xf_f(int f) const			{ return f / fy(); }
	int yb_b(int b) const			{ return b % by(); }
	int yf_f(int f) const			{ return f % fy(); }

	int xb_a(int a) const			{ return xb_x(x_a(a)); }
	int xf_a(int a) const			{ return xf_x(x_a(a)); }
	int yb_a(int a) const			{ return yb_y(y_a(a)); }
	int yf_a(int a) const			{ return yf_y(y_a(a)); }

protected:

	/// members

	int nbx, nby, row, total;
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
			for (int i = 0; i < size().area(); i++)
				data_[i] = old[i];
		}
		free(old);
	}

	static value* alloc(int size)
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
            for (int a = 0; a < size().area(); a++)
                if ((data_[a] = _data[a]))
                    --unsolved_;
        }
        else {
            for (int a = 0; a < size().area(); a++)
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

	int unsolved() const { return unsolved_; }
	int solved() const { return size().area() - unsolved(); }
	bool empty() const { return solved() == 0; }
	bool filled() const { return unsolved() == 0; }
	bool is_solved() const { return filled() && valid(); }

	bool equals(const Sudoku& sudoku) const
    {
		if (size() != sudoku.size())
			return false;
		for (int a = 0; a < size().area(); a++)
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
		for (int a = 0; a < size().area(); a++)
		{
            int x = size().x_a(a), y = size().y_a(a),
                b = size().b_a(a), f = size().f_a(a),
				v = get(a);
			if (!v)
				continue;
            for (int xi = x + 1; xi < size().line(); xi++)
                if (get(size().a_xy(xi, y)) == v)
					return false;
            for (int yi = y + 1; yi < size().line(); yi++)
                if (get(size().a_xy(x, yi)) == v)
					return false;
            for (int fi = f + 1; fi < size().line(); fi++)
                if (get(size().a_bf(b, fi)) == v)
					return false;
		}
		return true;
	}


	/// coord-access

	value get(int a) const
    {
		return data_[a];
    }

	value get(int x, int y) const
    {
		return get(size().a_xy(x,y));
    }

	value operator [] (int a) const
    {
		return get(a);
    }


	void set(int a, value num)
	{
		if (shared())
            unshare();
		if (num && !data_[a])
			--unsolved_;
		else if (!num && data_[a])
			++unsolved_;
		data_[a] = num;
	}

	void set(int x, int y, value num)
    {
        set(size().a_xy(x,y), num);
    }


private:
	/// members
	value* data_;
	int unsolved_;
    Size size_;
};





}


#endif // __SUDOKU_H__INCLUDED__
