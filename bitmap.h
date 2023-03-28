#pragma once

#include <vector>
#include "matrix.h"

struct color3f {
	double r, g, b;
	color3f();
	color3f(double r, double g, double b);
	~color3f();
};

class bitmap
{
public:

	const char* path;

	bitmap(int width, int height, const char* path);

	~bitmap();

	color3f get_color(int x, int y) const;

	void set_color(const color3f& color, int x, int y);

	void read_file();
	void export_file(const char* export_path) const;

	void mosaicking(char interpolation_type);

	void fuji_lens(std::vector <color3f>& pixels);
	void bayer_lens(std::vector <color3f>& pixels);

	bitmap rescale(int new_width, int new_height);

	void rotate(double degree);

	void resize(int new_width, int new_height); // used while rotating

	void grayscale();

	int m_width;
	int m_height;
private:

	std::vector <color3f> m_colors;
	double bicubic_interpolate(matrix& a, double y, double x);
	matrix bicub_scaled_matrix(matrix& pixels, int y, int x);
};