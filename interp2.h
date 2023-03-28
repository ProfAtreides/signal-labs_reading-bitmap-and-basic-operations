/*!
  \file interp2.h 2D Interpolation Functions

  (c) Mircea Neacsu 2021. All rights reserved.

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#pragma once

#include <memory>
#include <cmath>
#include <assert.h>
#include <initializer_list>

//#define DO_NOT_USE_EIGEN

#if __has_include (<eigen/dense>) && !defined(DO_NOT_USE_EIGEN)
#include <eigen/dense>
template <typename T>
using  Matrix = Eigen::Matrix<T, -1, -1>;
#else
//uncomment the next line if you don't want to use std::unique_ptr
//#define USE_NEW

template <typename T>
class Matrix
{
public:
  Matrix ();
  Matrix (int rows, int cols);
  Matrix (std::initializer_list <std::initializer_list<T> > v);
  Matrix (const Matrix& m);
  Matrix (Matrix&& m);
#ifdef USE_NEW
  ~Matrix ();
#endif

  int cols () const;
  int rows () const;
  T& operator () (int i, int j);
  const T& operator () (int i, int j) const;
  void resize (int r, int c);

private:
  int nc;
  int nr;

  //data points - cols*rows points in column order
#ifdef USE_NEW
  T* vals;
#else
  std::unique_ptr<T[]> vals;
#endif

  // matrix multiplication
  friend Matrix operator *(const Matrix& a, const Matrix& b)
  {
    assert (a.vals && b.vals && a.cols () == b.rows ());
    Matrix<T> r (a.rows (), b.cols ());
    for (int i = 0; i < a.rows (); i++)
      for (int j = 0; j < b.cols(); j++)
      {
        T v = 0;
        for (int k = 0; k < a.cols (); k++)
          v += a (i, k) * b (k, j);
        r (i, j) = v;
      }
    return r;
  }
};

// default constructor
template <typename T>
Matrix<T>::Matrix ()
  : nc{ 0 }
  , nr{ 0 }
#ifdef USE_NEW
  , vals {0}
#endif
{
}

// constructor
template <typename T>
Matrix<T>::Matrix (int rows, int cols)
  : nc{ cols }
  , nr{ rows }
#ifdef USE_NEW
  ,vals {new T[nc*nr]}
#else
  , vals { std::make_unique<T[]> (nc * nr) }
#endif
{
}

// constructor from initializer list
template<typename T>
Matrix<T>::Matrix (std::initializer_list<std::initializer_list<T>> v)
{
  nr = v.size ();
  nc = v.begin ()->size ();
#ifdef USE_NEW
  vals = new T[nr * nc];
#else
  vals = std::make_unique<T[]>(nc * nr);
#endif
  int i = 0;
  for (auto r = v.begin (); r != v.end (); r++, i++)
  {
    assert (r->size () == nc);
    int j = 0;
    for (auto c = r->begin (); c != r->end (); c++, j++)
      vals[i + nr * j] = *c;
  }
}

// copy constructor
template<typename T>
inline Matrix<T>::Matrix (const Matrix& m)
{
  nc = m.nc;
  nr = m.nr;
#ifdef USE_NEW
  vals = new T[nr*nc];
  memcpy (vals, m.vals, nc * nr * sizeof (T));
#else
  vals = std::make_unique<T[]>(nc * nr);
  memcpy (vals.get (), m.vals.get (), nc * nr * sizeof (T));
#endif
}

// move constructor
template<typename T>
inline Matrix<T>::Matrix (Matrix&& m)
  :nc{m.nc}
  ,nr{m.nr}
#ifndef USE_NEW
  ,vals {m.vals.release()}
#endif
{
#ifdef USE_NEW
  vals = m.vals;
  m.vals = 0;
#endif
}

// destructor
#ifdef USE_NEW
template<typename T>
inline Matrix<T>::~Matrix ()
{
  delete vals;
}
#endif

// return number of columns
template <typename T>
int Matrix<T>::cols () const
{
  return nc;
}

// return number of rows
template <typename T>
int Matrix<T>::rows () const
{
  return nr;
}

// access a matrix element
template <typename T>
T& Matrix<T>::operator () (int i, int j)
{
  assert (vals && j < nc && i < nr);
  return vals[i + nr * j];
}

// access a matrix element (const variant)
template<typename T>
inline const T& Matrix<T>::operator () (int i, int j) const
{
  assert (vals && j < nc && i < nr);
  return vals[i + nr * j];
}

//reshape a matrix
template<typename T>
inline void Matrix<T>::resize (int r, int c)
{
  assert (r * c == nr * nc);
  nr = r;
  nc = c;
}
#endif

// Nearest neighbor interpolation
template<typename T>
void nni (const Matrix<T>& in, Matrix<T>& out)
{
  //scaling factors
  double xr = (double)out.cols () / (in.cols ()-1);
  double yr = (double)out.rows () / (in.rows ()-1);

  assert (xr >= 1 && yr >= 1);

  for (int i = 0; i < out.rows (); i++)
    for (int j = 0; j < out.cols (); j++)
      out (i, j) = in (lround(i / yr), lround(j / xr));
}

// Bilinear interpolation
template<typename T>
void bilin (const Matrix<T>& in, Matrix<T>& out)
{
  //scaling factors
  double xr = (double)out.cols () / (in.cols ()-1);
  double yr = (double)out.rows () / (in.rows ()-1);

  assert (xr >= 1 && yr >= 1);

  for (int i = 0; i < out.rows (); i++)
  {
    int ii = (int)floor (i / xr);
    for (int j = 0; j < out.cols (); j++)
    {
      int jj = (int)floor (j / yr);
      T v00 = in (ii, jj), v01 = in (ii, jj + 1),
        v10 = in (ii + 1, jj), v11 = in (ii + 1, jj + 1);
      double fi = i / xr - ii, fj = j / yr - jj;
      out (i, j) = (1 - fi) * (1 - fj) * v00 + (1 - fi) * fj * v01 +
        fi * (1 - fj) * v10 + fi * fj * v11;
    }
  }
}

// Biquadratic interpolation
template<typename T>
void biquad (const Matrix<T>& in, Matrix<T>& out)
{
  //scaling factors
  double xr = (double)out.cols () / (in.cols () - 1);
  double yr = (double)out.rows () / (in.rows () - 1);

  assert (xr >= 1 && yr >= 1);
  auto qterp = [](double t, T a0, T a1, T a2)->T {
    return a0 + t * (a1 - a0) + 0.5 * t * (t - 1) * (a2 - 2 * a1 + a0); };
  for (int i = 0; i < out.rows (); i++)
  {
    int ii = (int)floor (i / xr);
    if (ii == in.rows () - 2)
      ii--;
    double fi = i / xr - ii;
    for (int j = 0; j < out.cols (); j++)
    {
      int jj = (int)floor (j / yr);
      if (jj == in.cols () - 2)
        jj--;
      double fj = j / yr - jj;
      T cols[3] = { qterp (fi, in (ii,jj), in (ii + 1, jj), in (ii + 2, jj)),
                 qterp (fi, in (ii, jj + 1), in (ii + 1, jj + 1), in (ii + 2, jj + 1)),
                 qterp (fi, in (ii, jj + 2), in (ii + 1, jj + 2), in (ii + 2, jj + 2)) };
      out (i, j) = qterp (fj, cols[0], cols[1], cols[2]);
    }
  }
}

// Bicubic interpolation
template<typename T>
void bicube (const Matrix<T>& in, Matrix<T>& out)
{
  //scaling factors
  double xr = (double)out.cols () / (in.cols () - 1);
  double yr = (double)out.rows () / (in.rows () - 1);

  //interpolation function
  auto bic = [](double x, double y, const Matrix<T>& a) ->T {
    double v = 0;
    for (int i = 0; i < 4; i++)
      for (int j = 0; j < 4; j++)
        v += a (i, j) * pow (x, i) * pow (y, j);
    return v;
  };
  //coefficients
  static Matrix<double> w{ { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                           { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                           {-3, 3, 0, 0,-2,-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                           { 2,-2, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                           { 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
                           { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
                           { 0, 0, 0, 0, 0, 0, 0, 0,-3, 3, 0, 0,-2,-1, 0, 0},
                           { 0, 0, 0, 0, 0, 0, 0, 0, 2,-2, 0, 0, 1, 1, 0, 0},
                           {-3, 0, 3, 0, 0, 0, 0, 0,-2, 0,-1, 0, 0, 0, 0, 0},
                           { 0, 0, 0, 0,-3, 0, 3, 0, 0, 0, 0, 0,-2, 0,-1, 0},
                           { 9,-9,-9, 9, 6, 3,-6,-3, 6,-6, 3,-3, 4, 2, 2, 1},
                           {-6, 6, 6,-6,-3,-3, 3, 3,-4, 4,-2, 2,-2,-2,-1,-1},
                           { 2, 0,-2, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0},
                           { 0, 0, 0, 0, 2, 0,-2, 0, 0, 0, 0, 0, 1, 0, 1, 0},
                           {-6, 6, 6,-6,-4,-2, 4, 2,-3, 3,-3, 3,-2,-1,-2,-1},
                           { 4,-4,-4, 4, 2, 2,-2,-2, 2,-2, 2,-2, 1, 1, 1, 1} };
  assert (xr >= 1 && yr >= 1);
  for (int i = 0; i < out.rows (); i++)
  {
    for (int j = 0; j < out.cols (); j++)
    {
      int ii = (int)floor (i / xr), jj = (int)floor (j / yr);
      // values, derivatives and cross derivatives
      Matrix<T> b  {
        {in (ii,jj)}, 
        {in (ii + 1, jj)},
        {in (ii, jj + 1)},
        {in (ii + 1, jj + 1)},
        {jj > 0 ? (in (ii,jj) - in (ii,jj - 1)) : 0},
        {jj > 0 ? (in (ii + 1, jj) - in (ii + 1, jj - 1)) : 0},
        {in (ii, jj + 1) - in (ii,jj)},
        {in (ii + 1,jj + 1) - in (ii + 1, jj)},
        {ii > 0 ? (in (ii, jj) - in (ii - 1, jj)) : 0},
        {in (ii + 1, jj) - in (ii, jj)},
        {ii > 0 ? (in (ii, jj + 1) - in (ii - 1, jj + 1)) : 0},
        {in (ii + 1, jj + 1) - in (ii,jj + 1)},
        {ii > 0 && jj > 0 ? in (ii, jj) - in (ii - 1, jj - 1) : 0},
        {jj > 0 ? in (ii + 1, jj) - in (ii, jj - 1) : 0},
        {ii > 0 ? in (ii, jj + 1) - in (ii - 1, jj) : 0},
        {in (ii + 1, jj + 1) - in (ii, jj)}
      };
      Matrix<T> a = w * b;
      a.resize (4, 4);
      out (i, j) = bic (i / yr - ii, j / xr - jj, a);
    }
  }
}

// Constrained bicubic interpolation
template<typename T>
void cbi (const Matrix<T>& in, Matrix<T>& out)
{
  //scaling factors
  double xr = (double)out.cols () / (in.cols () - 1);
  double yr = (double)out.rows () / (in.rows () - 1);

  assert (xr >= 1 && yr >= 1);

  //weighting function
  auto w = [](double x, double y) -> double {
    return x * x * y * y * (9 - 6 * x - 6 * y + 4 * x * y);
  };
  
  for (int i = 0; i < out.rows (); i++)
  {
    int ii = (int)floor (i / xr);
    for (int j = 0; j < out.cols (); j++)
    {
      int jj = (int)floor (j / yr);
      T v00 = in (ii, jj), v01 = in (ii, jj + 1),
        v10 = in (ii + 1, jj), v11 = in (ii + 1, jj + 1);
      double fi = i / xr - ii, fj = j / yr - jj;
      out (i, j) = w(1 - fi, 1 - fj) * v00 + w(1 - fi, fj) * v01 +
        w(fi, 1 - fj) * v10 + w(fi, fj) * v11;
    }
  }
}
