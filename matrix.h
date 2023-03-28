#pragma once

#include <vector>

class matrix
{
public:
	matrix();
	matrix(int rows, int columns); //create empty matrix with columns and rows
	matrix(int rows, int columns, std::vector <double> values); // create matrix with values from vector
	matrix(const matrix&& m); // copy
	~matrix();
	int columns() const;
	int rows() const;
	double& operator () (int i, int j);
	const double& operator () (int i, int j) const;
	void resize(int rows, int columns);
	void set_values(std::vector <double> new_values);
private:
	int nc;//number of columns
	int nr;//number of rows
	std::vector <double> values;
	//matrix multiplication
	friend matrix operator *(const matrix& a, const matrix& b)
	{
		double v = 0;
		matrix r(a.rows(), b.columns());
		for (int i = 0; i < a.rows(); i++)
			for (int j = 0; j < b.columns(); j++)
			{
				for (int k = 0; k < a.columns(); k++)
				{
					v += a(k, i) * b(k, j);
				}
				r(i, j) = v;
				v = 0;
			}
		return r;
	}
};