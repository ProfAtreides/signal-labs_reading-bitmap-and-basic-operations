#include "matrix.h"
matrix::matrix()
{
	nc = 0;
	nr = 0;
	values.resize(0);
}

matrix::matrix(int rows, int columns)
{
	nc = columns;
	nr = rows;
	values.resize(columns * rows);
}

matrix::matrix(int rows, int columns, std::vector <double> values)
{
	nc = columns;
	nr = rows;
	values.resize(rows * columns);
	this->values = values;
}

matrix::matrix(const matrix&& m)
{
	nc = m.nc;
	nr = m.nr;
	values.resize(m.values.size());
	values = m.values;
}

matrix::~matrix()
{
}

int matrix::columns() const
{
	return nc;
}

int matrix::rows() const
{
	return nr;
}

double& matrix::operator()(int i, int j)
{
	return values[i + nr * j];
}

const double& matrix::operator()(int i, int j) const
{
	return values[i + nr * j];
}

void matrix::resize(int r, int c)
{
	nc = c;
	nr = r;
	if (r * c != values.size())
	{
		values.resize(r * c);
	}
}

void matrix::set_values(std::vector<double> new_values)
{
	values.resize(new_values.size());
	values = new_values;
}