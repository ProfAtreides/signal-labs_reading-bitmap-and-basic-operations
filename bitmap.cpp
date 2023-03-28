#include "bitmap.h"
#include <cmath>
#include <iostream>
#include <fstream>
#include <algorithm>

template <typename T>
T max(T a, T b)
{
	return (a > b) ? a : b;
}
template <typename T>
T min(T a, T b)
{
	return (a > b) ? b : a;
}

color3f::color3f()
{
	this->r = 0;
	this->g = 0;
	this->b = 0;
}
color3f::color3f(double r, double g, double b)
{
	this->r = r;
	this->g = g;
	this->b = b;
}

color3f::~color3f()
{
}

bitmap::bitmap(int width, int height, const char* file_path)
{
	m_width = width;
	m_height = height;
	path = file_path;
	m_colors.resize(width * height);
}

bitmap::~bitmap()
{
}

color3f bitmap::get_color(int x, int y) const
{
	return m_colors[y * m_width + x];
}

void bitmap::set_color(const color3f& color, int x, int y)
{
	m_colors[y * m_width + x].r = color.r;
	m_colors[y * m_width + x].g = color.g;
	m_colors[y * m_width + x].b = color.b;
}

void bitmap::read_file()
{
	std::ifstream f;
	f.open(path, std::ios::in | std::ios::binary);

	if (!f.is_open())
	{
		std::cout << "File open not" << "\n";
		return;
	}

	const int fileHeaderSize = 14;
	const int informationHeaderSize = 40;

	unsigned char fileHeader[fileHeaderSize];
	f.read(reinterpret_cast<char*>(fileHeader), fileHeaderSize);

	if (fileHeader[0] != 'B' || fileHeader[1] != 'M')
	{
		std::cout << "Bitmap the file is not" << "\n";
		return;
	}

	unsigned char informationHeader[informationHeaderSize];
	f.read(reinterpret_cast<char*>(informationHeader), informationHeaderSize);

	int fileSize = fileHeader[2] + (fileHeader[3] << 8) + (fileHeader[4] << 16) + (fileHeader[5] << 24);
	m_width = informationHeader[4] + (informationHeader[5] << 8) + (informationHeader[6] << 16) + (informationHeader[7] << 24);
	m_height = informationHeader[8] + (informationHeader[9] << 8) + (informationHeader[10] << 16) + (informationHeader[11] << 24);

	m_colors.resize(m_width * m_height);

	const int paddingAmount = ((4 - (m_width * 3) % 4) % 4);

	for (int y = 0; y < m_height; y++)
	{
		for (int x = 0; x < m_width; x++)
		{
			unsigned char color[3];
			f.read(reinterpret_cast<char*>(color), 3);

			m_colors[y * m_width + x].r = static_cast <double>(color[2]) / 255.0f;
			m_colors[y * m_width + x].g = static_cast <double>(color[1]) / 255.0f;
			m_colors[y * m_width + x].b = static_cast <double>(color[0]) / 255.0f;
		}
		f.ignore(paddingAmount);
	}

	f.close();
}

void bitmap::export_file(const char* export_path) const
{
	std::ofstream f;
	f.open(export_path, std::ios::out | std::ios::binary);

	if (!f.is_open())
	{
		std::cout << "File open not" << "\n";
		return;
	}

	unsigned char bmpPad[] = { 0,0,0 };
	const int paddingAmount = ((4 - (m_width * 3) % 4) % 4);

	const int fileHeaderSize = 14;
	const int informationHeaderSize = 40;
	const int fileSize = fileHeaderSize + informationHeaderSize + m_width * m_width * 3 + paddingAmount * m_height;

	unsigned char fileHeader[fileHeaderSize];

	//File Type
	fileHeader[0] = 'B';
	fileHeader[1] = 'M';
	//File size
	fileHeader[2] = fileSize;
	fileHeader[3] = fileSize >> 8;
	fileHeader[4] = fileSize >> 16;
	fileHeader[5] = fileSize >> 24;
	//Reserved
	fileHeader[6] = 0;
	fileHeader[7] = 0;
	fileHeader[8] = 0;
	fileHeader[9] = 0;
	//Pixel data offset
	fileHeader[10] = fileHeaderSize + informationHeaderSize;
	fileHeader[11] = 0;
	fileHeader[12] = 0;
	fileHeader[13] = 0;

	unsigned char informationHeader[informationHeaderSize];

	//Header size
	informationHeader[0] = informationHeaderSize;
	informationHeader[1] = 0;
	informationHeader[2] = 0;
	informationHeader[3] = 0;
	//Image width
	informationHeader[4] = m_width;
	informationHeader[5] = m_width >> 8;
	informationHeader[6] = m_width >> 16;
	informationHeader[7] = m_width >> 24;
	//Image height
	informationHeader[8] = m_height;
	informationHeader[9] = m_height >> 8;
	informationHeader[10] = m_height >> 16;
	informationHeader[11] = m_height >> 24;
	//Planes
	informationHeader[12] = 1;
	informationHeader[13] = 0;
	//Bits per pixel (RGB)
	informationHeader[14] = 24;
	informationHeader[15] = 0;
	//Comperssion
	informationHeader[16] = 0;
	informationHeader[17] = 0;
	informationHeader[18] = 0;
	informationHeader[19] = 0;
	//Image size (No compression)
	informationHeader[20] = 0;
	informationHeader[21] = 0;
	informationHeader[22] = 0;
	informationHeader[23] = 0;
	//X pixels per meter (?)
	informationHeader[24] = 0;
	informationHeader[25] = 0;
	informationHeader[26] = 0;
	informationHeader[27] = 0;
	//Y pixels per meter (?)
	informationHeader[28] = 0;
	informationHeader[29] = 0;
	informationHeader[30] = 0;
	informationHeader[31] = 0;
	//Total colors (Color palette not used)
	informationHeader[32] = 0;
	informationHeader[33] = 0;
	informationHeader[34] = 0;
	informationHeader[35] = 0;
	//Important colors (Ignore idk why?)
	informationHeader[36] = 0;
	informationHeader[37] = 0;
	informationHeader[38] = 0;
	informationHeader[39] = 0;

	f.write(reinterpret_cast<char*>(fileHeader), fileHeaderSize);
	f.write(reinterpret_cast<char*>(informationHeader), informationHeaderSize);

	for (int y = 0; y < m_height; y++)
	{
		for (int x = 0; x < m_width; x++)
		{
			unsigned char r = static_cast<unsigned char> (get_color(x, y).r * 255.0f);
			unsigned char g = static_cast<unsigned char> (get_color(x, y).g * 255.0f);
			unsigned char b = static_cast<unsigned char> (get_color(x, y).b * 255.0f);

			unsigned char color[] = { b, g, r };

			f.write(reinterpret_cast<char*>(color), 3);
		}

		f.write(reinterpret_cast<char*>(bmpPad), paddingAmount);
	}

	f.close();

	std::cout << "Let there be file" << "\n";
}

matrix reversed_matrix_w(16, 16, { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
							0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
						   -3, 3, 0, 0,-2,-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
							2,-2, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
							0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
							0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
							0, 0, 0, 0, 0, 0, 0, 0,-3, 3, 0, 0,-2,-1, 0, 0,
							0, 0, 0, 0, 0, 0, 0, 0, 2,-2, 0, 0, 1, 1, 0, 0,
						   -3, 0, 3, 0, 0, 0, 0, 0,-2, 0,-1, 0, 0, 0, 0, 0,
							0, 0, 0, 0,-3, 0, 3, 0, 0, 0, 0, 0,-2, 0,-1, 0,
							9,-9,-9, 9, 6, 3,-6,-3, 6,-6, 3,-3, 4, 2, 2, 1,
						   -6, 6, 6,-6,-3,-3, 3, 3,-4, 4,-2, 2,-2,-2,-1,-1,
							2, 0,-2, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0,
							0, 0, 0, 0, 2, 0,-2, 0, 0, 0, 0, 0, 1, 0, 1, 0,
						   -6, 6, 6,-6,-4,-2, 4, 2,-3, 3,-3, 3,-2,-1,-2,-1,
							4,-4,-4, 4, 2, 2,-2,-2, 2,-2, 2,-2, 1, 1, 1, 1 });

double bitmap::bicubic_interpolate(matrix& a, double y, double x)//tbc
{
	a.resize(4, 4);
	float interpolation_value = 0;

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			interpolation_value += a(j, i) * pow(x, j) * pow(y, i);
		}
	}

	/*if (interpolation_value > 255.0)
	{
		interpolation_value = 255.0;
	}
	if (interpolation_value < 0.0)
	{
		interpolation_value = 0.0;
	}*/

	return interpolation_value;
}

matrix bitmap::bicub_scaled_matrix(matrix& pixels, int y, int x)
{
	if (y + 1 < pixels.columns() && x + 1 < pixels.rows())
	{
		//std::cout << pixels(x + 1, y + 1);
		bool stop = true;
		matrix coefficients(16, 1, {
				pixels(x,y), //1
				pixels(x + 1,y),	//2
				pixels(x,y + 1),//3
				pixels(x + 1,y + 1),	//4
				y > 0 ? (pixels(x,y) - pixels(x,y - 1)) : 0,	//5
				y > 0 ? (pixels(x + 1,y) - pixels(x + 1,y - 1)) : 0,	//6
				pixels(x,y + 1) - pixels(x,y),	//7
				pixels(x + 1,y + 1) - pixels(x + 1,y),//8
				x > 0 ? (pixels(x,y) - pixels(x - 1,y)) : 0,	//9
				pixels(x + 1,y) - pixels(x,y),	//10
				x > 0 ? (pixels(x ,y + 1) - pixels(x - 1,y + 1)) : 0,	//11
				pixels(x + 1,y + 1) - pixels(x,y + 1),	//12
				y > 0 && x > 0 ? (pixels(x,y) - pixels(x - 1, y - 1)) : 0,	//13
				y > 0 ? (pixels(x + 1,y) - pixels(x , y - 1)) : 0,	//14
				x > 0 ? (pixels(x	,y + 1) - pixels(x - 1, y)) : 0,//15
				pixels(x + 1,y + 1) - pixels(x, y) //16
			});
		bool stop2 = true;
		matrix interpolate = reversed_matrix_w * coefficients;
		return interpolate;
	}
	else if (y + 1 < pixels.columns())
	{
		matrix coefficients(16, 1, {
				pixels(x - 1,y), //1
				pixels(x ,y),	//2
				pixels(x,y + 1),//3
				pixels(x ,y + 1),	//4
				y > 0 ? (pixels(x - 1,y) - pixels(x - 1,y - 1)) : 0,	//5
				y > 0 ? (pixels(x ,y) - pixels(x ,y - 1)) : 0,	//6
				pixels(x - 1,y + 1) - pixels(x - 1,y),	//7
				pixels(x ,y + 1) - pixels(x ,y),//8
				x > 0 ? (pixels(x - 1,y) - pixels(x - 2 ,y)) : 0,	//9
				pixels(x ,y) - pixels(x - 1,y),	//10
				x > 0 ? (pixels(x - 1 ,y + 1) - pixels(x - 2,y + 1)) : 0,	//11
				pixels(x ,y + 1) - pixels(x - 1,y + 1),	//12
				y > 0 && x > 0 ? (pixels(x - 1,y) - pixels(x - 2, y - 1)) : 0,	//13
				y > 0 ? (pixels(x ,y) - pixels(x - 1 , y - 1)) : 0,	//14
				x > 0 ? (pixels(x - 1	,y + 1) - pixels(x - 2, y)) : 0,//15
				pixels(x ,y + 1) - pixels(x - 1, y) //16
			});

		matrix interpolate = reversed_matrix_w * coefficients;
		return interpolate;
	}
	else
	{
		matrix coefficients(16, 1, {
				pixels(x - 1,y - 1), //1
				pixels(x ,y - 1),	//2
				pixels(x,y),//3
				pixels(x ,y),	//4
				y > 0 && x > 0 ? (pixels(x - 1,y - 1) - pixels(x - 1,y - 2)) : 0,	//5
				y > 0 ? (pixels(x ,y - 1) - pixels(x ,y - 2)) : 0,	//6
				pixels(x - 1,y) - pixels(x - 1,y - 1),	//7
				pixels(x ,y) - pixels(x ,y - 1),//8
				x > 0 ? (pixels(x - 1,y - 1) - pixels(x - 2 ,y - 1)) : 0,	//9
				pixels(x ,y - 1) - pixels(x - 1,y - 1),	//10
				x > 0 ? (pixels(x - 1 ,y) - pixels(x - 2,y)) : 0,	//11
				pixels(x ,y) - pixels(x - 1,y),	//12
				y > 0 && x > 0 ? (pixels(x - 1,y - 1) - pixels(x - 2, y - 2)) : 0,	//13
				y > 0 ? (pixels(x ,y - 1) - pixels(x - 1 , y - 2)) : 0,	//14
				x > 0 ? (pixels(x - 1	,y) - pixels(x - 2, y - 1)) : 0,//15
				pixels(x ,y) - pixels(x - 1, y - 1) //16
			});

		matrix interpolate = reversed_matrix_w * coefficients;
		return interpolate;
	}
}

void bitmap::bayer_lens(std::vector <color3f>& pixels)
{
	std::vector <double> red_pixels;
	std::vector <double> green_pixels;
	std::vector <double> blue_pixels;

	for (int y = 0; y < m_height; y++)
	{
		for (int x = 0; x < m_width; x++)
		{
			if (y % 2 == 0)
			{
				green_pixels.push_back(get_color(x, y).g * 255.0);
				if (x + 1 < m_width)
				{
					x++;
					red_pixels.push_back(get_color(x, y).r * 255.0);
				}
			}
			else
			{
				blue_pixels.push_back(get_color(x, y).b * 255.0);
				if (x + 1 < m_width)
				{
					x++;
					green_pixels.push_back(get_color(x, y).g * 255.0);
				}
				else
				{
					if (m_width % 2 == 1)
					{
						//std::cout << "WLAZLO";
						green_pixels.push_back(0.0);
						break;
					}
				}
			}
		}
	}

	matrix red_filter(m_width / 2, m_height / 2, red_pixels);
	matrix green_filter;
	matrix blue_filter;
	double red_scale_height = (double)m_height / (m_height / 2 - 1), red_scale_width = (double)m_width / (m_width / 2 - 1);
	double green_scale_height_1, green_scale_width_1, green_scale_height_2, green_scale_width_2;
	double blue_scale_height, blue_scale_width;

	if (m_width % 2 == 1 && m_height % 2 == 1)
	{
		green_filter.set_values(green_pixels);
		green_filter.resize(m_width / 2 + 1, m_height);
		green_scale_height_1 = (double)m_height / (m_height - 1), green_scale_width_1 = (double)m_width / (m_width / 2 - 1);
		green_scale_height_2 = (double)m_height / (m_height - 1), green_scale_width_2 = (double)m_width / (m_width / 2 - 2);
		blue_filter.set_values(blue_pixels);
		blue_filter.resize(m_width / 2 + 1, m_height / 2);
		blue_scale_height = (double)m_height / (m_height / 2 - 1), blue_scale_width = (double)m_width / (m_width / 2 - 1);
	}
	else if (m_width % 2 == 0 && m_height % 2 == 1)
	{
		green_filter.resize(m_width / 2, m_height);
		green_filter.set_values(green_pixels);
		green_scale_height_1 = m_height, green_scale_width_1 = m_width / (m_width / 2 - 1);
		blue_filter.resize(m_width / 2, m_height / 2);
		blue_filter.set_values(blue_pixels);
		blue_scale_height = m_height / (m_height / 2 - 1), blue_scale_width = m_width / (m_width / 2 - 1);
	}
	else if (m_width % 2 == 1 && m_height % 2 == 0)
	{
		green_filter.resize(m_width / 2 + 1, m_height);
		green_filter.set_values(green_pixels);
		green_scale_height_1 = m_height, green_scale_width_1 = m_width / (m_width / 2 - 2);
		blue_filter.resize(m_width / 2 + 1, m_height / 2);
		blue_filter.set_values(blue_pixels);
		blue_scale_height = m_height / (m_height / 2 - 1), blue_scale_width = m_width / (m_width / 2);
	}
	else
	{
		green_filter.resize(m_width / 2, m_height);
		green_filter.set_values(green_pixels);
		green_scale_height_1 = m_height / (m_height - 1), green_scale_width_1 = m_width / (m_width / 2 - 1);
		blue_filter.resize(m_width / 2, m_height / 2);
		blue_filter.set_values(blue_pixels);
		blue_scale_height = m_height / (m_height / 2), blue_scale_width = m_width / (m_width / 2);
	}

	for (int i = 0; i < m_height; i++)
	{
		for (int j = 0; j < m_width; j++)
		{
			//std::cout << j << " " << i << "\n";
			int i_red = (int)floor(i / red_scale_height), j_red = (int)floor(j / red_scale_width);
			int i_green = (int)floor(i / green_scale_height_1), j_green = (int)floor(j / green_scale_width_1);
			int i_blue = (int)floor(i / blue_scale_height), j_blue = (int)floor(j / blue_scale_width);

			if (i % 2 == 0)
			{
				matrix temp_red = bicub_scaled_matrix(red_filter, i_red, j_red);
				matrix temp_blue = bicub_scaled_matrix(blue_filter, i_blue, j_blue);
				pixels[j + i * m_width].r = bicubic_interpolate(temp_red, (double)i / red_scale_height - i_red, (double)j / red_scale_width - j_red);
				pixels[j + i * m_width].g = green_filter(j_green, i_green);
				pixels[j + i * m_width].b = bicubic_interpolate(temp_blue, (double)i / blue_scale_height - i_blue, (double)j / blue_scale_width - j_blue);

				if (j + 1 < m_width)
				{
					j++;
					j_red = (int)floor(j / red_scale_width);
					j_green = (int)floor(j / green_scale_width_1);
					j_blue = (int)floor(j / blue_scale_width);
					matrix temp_green = bicub_scaled_matrix(green_filter, i_green, j_green);
					matrix temp_blue = bicub_scaled_matrix(blue_filter, i_blue, j_blue);
					pixels[j + i * m_width].r = red_filter(j_red, i_red);
					pixels[j + i * m_width].g = bicubic_interpolate(temp_red, (double)i / green_scale_height_1 - i_green, (double)j / green_scale_width_1 - j_green);
					pixels[j + i * m_width].b = bicubic_interpolate(temp_blue, (double)i / blue_scale_height - i_blue, (double)j / blue_scale_width - j_blue);
				}
			}
			else
			{
				j_green = (int)floor(j / green_scale_width_1);
				matrix temp_red = bicub_scaled_matrix(red_filter, i_red, j_red);
				matrix temp_green = bicub_scaled_matrix(green_filter, i_green, j_green);
				pixels[j + i * m_width].r = bicubic_interpolate(temp_red, (double)i / red_scale_height - i_red, (double)j / red_scale_width - j_red);
				pixels[j + i * m_width].g = bicubic_interpolate(temp_red, (double)i / green_scale_height_1 - i_green, (double)j / green_scale_width_1 - j_green);
				pixels[j + i * m_width].b = blue_filter(j_blue, i_blue);
				if (j + 1 < m_width)
				{
					j++;
					j_green = (int)floor(j / green_scale_width_1);
					j_red = (int)floor(j / red_scale_width);
					j_blue = (int)floor(j / blue_scale_width);
					matrix temp_red = bicub_scaled_matrix(red_filter, i_red, j_red);
					matrix temp_blue = bicub_scaled_matrix(blue_filter, i_blue, j_blue);
					pixels[j + i * m_width].r = bicubic_interpolate(temp_red, (double)i / red_scale_height - i_red, (double)j / red_scale_width - j_red);
					pixels[j + i * m_width].g = green_filter(j_green, i_green);
					pixels[j + i * m_width].b = bicubic_interpolate(temp_blue, (double)i / blue_scale_height - i_blue, (double)j / blue_scale_width - j_blue);
				}
			}
		}
	}
}

bitmap bitmap::rescale(int new_width, int new_height)
{
	double ratio_y = (double)new_height / (m_height - 1);
	double ratio_x = (double)new_width / (m_width - 1);

	bitmap rescaled(new_width, new_height, "rescaled.bmp");

	std::vector <double> red_pixels;
	std::vector <double> green_pixels;
	std::vector <double> blue_pixels;

	for (int i = 0; i < m_height; i++)
	{
		for (int j = 0; j < m_width; j++)
		{
			red_pixels.push_back(get_color(j, i).r);
			green_pixels.push_back(get_color(j, i).g);
			blue_pixels.push_back(get_color(j, i).b);
		}
	}

	matrix red_filter(m_width, m_height, red_pixels);
	matrix green_filter(m_width, m_height, green_pixels);
	matrix blue_filter(m_width, m_height, blue_pixels);

	color3f pixel;

	for (int i = 0; i < new_height; i++)
	{
		for (int j = 0; j < new_width; j++)
		{
			if (i == 569 && j == 835)
			{
				std::cout << i << " " << j << "\n";
				bool stop = true;
			}

			int orginal_i = (int)floor(i / ratio_y), orginal_j = (int)floor(j / ratio_x);
			matrix temp_red = bicub_scaled_matrix(red_filter, orginal_i, orginal_j);
			matrix temp_green = bicub_scaled_matrix(green_filter, orginal_i, orginal_j);
			matrix temp_blue = bicub_scaled_matrix(blue_filter, orginal_i, orginal_j);
			pixel.r = bicubic_interpolate(temp_red, (double)i / ratio_y - orginal_i, (double)j / ratio_x - orginal_j);
			pixel.g = bicubic_interpolate(temp_green, (double)i / ratio_y - orginal_i, (double)j / ratio_x - orginal_j);
			pixel.b = bicubic_interpolate(temp_blue, (double)i / ratio_y - orginal_i, (double)j / ratio_x - orginal_j);

			rescaled.set_color(pixel, j, i);
		}
	}
	return rescaled;
}

void bitmap::resize(int new_width, int new_height)
{
	m_width = new_width;
	m_height = new_height;

	m_colors.clear();
	m_colors.resize(m_width * m_height);
}

void bitmap::rotate(double degree)
{
	std::ofstream logs("logs.txt");

	degree *= 0.0174532925;
	double sinx = sin(degree);
	double cosx = cos(degree);

	int x1 = -m_height * sinx;
	int x2 = m_width * cosx - m_height * sinx;
	int x3 = m_width * cosx;
	int y1 = m_height * cosx;
	int y2 = m_height * cosx + m_width * sinx;
	int y3 = m_width * sinx;

	int minx = min(0, min(x1, min(x2, x3)));
	int miny = min(0, min(y1, min(y2, y3)));
	int maxx = max(x1, max(x2, x3));
	int maxy = max(y1, max(y2, y3));

	int new_width = maxx - minx;
	int new_height = maxy - miny;

	// TBD fix issue with corners being black idk if possible
	std::vector <color3f> rotated_image;
	rotated_image.resize(new_height * new_width);

	for (int y = 0; y < new_height; y++)
	{
		for (int x = 0; x < new_width; x++)
		{
			int original_x = ((x + minx) * cosx + (y + miny) * sinx);
			int original_y = ((y + miny) * cosx - (x + minx) * sinx);

			logs << x << " " << y << " |" << original_x << " " << original_y << "\n";

			if (original_x >= 0 && original_x < m_width &&
				original_y >= 0 && original_y < m_height//&&
				//rotated_x + rotated_y * new_height < rotated_image.size()
				)
			{
				rotated_image[x + y * new_width] = get_color(original_x, original_y);
			}
		}
	}

	resize(new_width, new_height);

	m_colors = rotated_image;
}

void bitmap::fuji_lens(std::vector <color3f>& pixels)
{
	std::vector <double> red_pixels;
	std::vector <double> green_pixels;
	std::vector <double> blue_pixels;

	pixels.clear();
	pixels.resize(m_width * m_height);

	char filter_sample[6][6] =
	{
		{'G','B','R','G','R','B'},
		{'R','G','G','B','G','G'},
		{'B','G','G','R','G','G'},
		{'G','R','B','G','B','R'},
		{'B','G','G','R','G','G'},
		{'R','G','G','B','G','G'}
	};

	std::vector <char> filter_image;

	for (int y = 0; y < m_height; y++)
	{
		for (int x = 0; x < m_width; x++)
		{
			filter_image.push_back(filter_sample[y % 6][x % 6]);
		}
	}

	std::vector <std::vector <double>> green_map;
	std::vector <std::vector <double>> red_map;
	std::vector <std::vector <double>> blue_map;

	for (int y = 0; y < m_height; y++)
	{
		for (int x = 0; x < m_width; x++)
		{
			if (filter_image[x + y * m_width] == 'G')
			{
				green_pixels.push_back(get_color(x, y).g * 255.0);
				if ((y % 6 == 1 || y % 6 == 5) && x % 6 == 4)
				{
					red_pixels.push_back(0.0);
					blue_pixels.push_back(0.0);
				}
				if ((y % 6 == 2 || y % 6 == 4) && x % 6 == 4)
				{
					blue_pixels.push_back(0.0);
					red_pixels.push_back(0.0);
				}
			}
			else if (filter_image[x + y * m_width] == 'R')
			{
				red_pixels.push_back(get_color(x, y).r * 255.0);
				if (y % 6 == 0 && x % 6 == 2)
				{
					green_pixels.push_back(0.0);
				}
				if (y % 6 == 3 && x % 6 == 5)
				{
					green_pixels.push_back(0.0);
				}
			}
			else
			{
				blue_pixels.push_back(get_color(x, y).b * 255.0);
				if (y % 6 == 3 && x % 6 == 2)
				{
					green_pixels.push_back(0.0);
				}
				if (y % 6 == 0 && x % 6 == 5)
				{
					green_pixels.push_back(0.0);
				}
			}
		}

		red_map.push_back(red_pixels);
		red_pixels.clear();
		green_map.push_back(green_pixels);
		green_pixels.clear();
		blue_map.push_back(blue_pixels);
		blue_pixels.clear();
	}

	int green_width = ceil(m_width * 0.66666666);
	int blue_width = m_width / 3 + (m_width % 6 >= 4 ? (1) : 0);
	int red_width = m_width / 3 + (m_width % 6 >= 4 ? (1) : 0);

	for (int i = 0; i < m_height; i++)
	{
		if (red_map[i].size() == red_width)
		{
			for (int j = 0; j < red_width; j++)
			{
				red_pixels.push_back(red_map[i][j]);
			}
		}
		else
		{
			red_map[i].push_back(0.0);
			for (int j = 0; j < red_width; j++)
			{
				red_pixels.push_back(red_map[i][j]);
			}
		}
		if (green_map[i].size() == green_width)
		{
			for (int j = 0; j < green_width; j++)
			{
				green_pixels.push_back(green_map[i][j]);
			}
		}
		else
		{
			green_map[i].push_back(0.0);
			for (int j = 0; j < green_width; j++)
			{
				green_pixels.push_back(green_map[i][j]);
			}
		}
		if (blue_map[i].size() == blue_width)
		{
			for (int j = 0; j < blue_width; j++)
			{
				blue_pixels.push_back(blue_map[i][j]);
			}
		}
		else
		{
			blue_map[i].push_back(0.0);
			for (int j = 0; j < blue_width; j++)
			{
				blue_pixels.push_back(blue_map[i][j]);
			}
		}
	}

	matrix red_broken_filter(red_width, m_height, red_pixels);
	matrix green_broken_filter(green_width, m_height, green_pixels);
	matrix blue_broken_filter(blue_width, m_height, blue_pixels);

	for (int i = 0; i < m_height; i++)
	{
		for (int j = 0; j < green_width; j++)
		{
			if (green_broken_filter(j, i) == 0.0)
			{
				matrix temp_green = bicub_scaled_matrix(green_broken_filter, i, j);
				green_pixels[j + i * green_width] = bicubic_interpolate(temp_green, 1, 1);
			}
		}
	}
	for (int i = 0; i < m_height; i++)
	{
		for (int j = 0; j < red_width; j++)
		{
			if (red_broken_filter(j, i) == 0.0)
			{
				matrix temp_red = bicub_scaled_matrix(red_broken_filter, i, j);
				red_pixels[j + i * red_width] = bicubic_interpolate(temp_red, 1, 1);
			}
		}
	}

	for (int i = 0; i < m_height; i++)
	{
		for (int j = 0; j < blue_width; j++)
		{
			if (blue_broken_filter(j, i) == 0.0)
			{
				matrix temp_blue = bicub_scaled_matrix(blue_broken_filter, i, j);
				blue_pixels[j + i * blue_width] = bicubic_interpolate(temp_blue, 1, 1);
			}
		}
	}

	matrix red_filter(red_width, m_height, red_pixels);
	matrix green_filter(green_width, m_height, green_pixels);
	matrix blue_filter(blue_width, m_height, blue_pixels);

	double red_scale_height = m_height / (m_height - 1), red_scale_width = (double)m_width / (red_width - 1);
	double green_scale_height = m_height / (m_height - 1), green_scale_width = (double)m_width / (green_width - 1);
	double blue_scale_height = m_height / (m_height - 1), blue_scale_width = (double)m_width / (blue_width - 1);

	for (int i = 0; i < m_height; i++)
	{
		for (int j = 0; j < m_width; j++)
		{
			int j_red = (int)floor(j / red_scale_width);
			int j_green = (int)floor(j / green_scale_width);
			int j_blue = (int)floor(j / blue_scale_width);

			if (filter_image[j + i * m_width] == 'G')
			{
				matrix temp_red = bicub_scaled_matrix(red_filter, i, j_red);
				matrix temp_blue = bicub_scaled_matrix(blue_filter, i, j_blue);
				pixels[j + i * m_width].r = bicubic_interpolate(temp_red, 1, (double)j / red_scale_width - j_red);
				pixels[j + i * m_width].g = green_filter(j_green, i);
				pixels[j + i * m_width].b = bicubic_interpolate(temp_blue, 1, (double)j / blue_scale_width - j_blue);
			}
			else if (filter_image[j + i * m_width] == 'R')
			{
				matrix temp_green = bicub_scaled_matrix(green_filter, i, j_green);
				matrix temp_blue = bicub_scaled_matrix(blue_filter, i, j_blue);
				pixels[j + i * m_width].r = red_filter(j_red, i);
				pixels[j + i * m_width].g = bicubic_interpolate(temp_green, 1, (double)j / green_scale_width - j_green);
				pixels[j + i * m_width].b = bicubic_interpolate(temp_blue, 1, (double)j / blue_scale_width - j_blue);
			}
			else
			{
				matrix temp_red = bicub_scaled_matrix(red_filter, i, j_red);
				matrix temp_green = bicub_scaled_matrix(green_filter, i, j_green);
				pixels[j + i * m_width].r = bicubic_interpolate(temp_red, 1, (double)j / red_scale_width - j_red);
				pixels[j + i * m_width].g = bicubic_interpolate(temp_green, 1, (double)j / green_scale_width - j_green);
				pixels[j + i * m_width].b = blue_filter(j_blue, i);
			}
		}
	}
}

void bitmap::mosaicking(char interpolation_type)
{
	std::vector <color3f> pixels;

	std::ofstream bayer_map("bayer.bmp");
	std::ofstream red_bayer_map("red_bayer_map.bmp");
	std::ofstream green_bayer_map("green_bayer_map.bmp");
	std::ofstream blue_bayer_map("blue_bayer_map.bmp");

	std::ofstream fuji_map("fuji.bmp");
	std::ofstream red_fuji_map("red_fuji_map.bmp");
	std::ofstream green_fuji_map("green_fuji_map.bmp");
	std::ofstream blue_fuji_map("blue_fuji_map.bmp");

	if (!fuji_map.is_open())
	{
		std::cout << "File open not" << "\n";
		return;
	}

	unsigned char bmpPad[3] = { 0,0,0 };
	const int paddingAmount = ((4 - (m_width * 3) % 4) % 4);

	const int fileHeaderSize = 14;
	const int informationHeaderSize = 40;
	const int fileSize = fileHeaderSize + informationHeaderSize + m_width * m_width * 3 + paddingAmount * m_height;

	unsigned char fileHeader[fileHeaderSize];

	//File Type
	fileHeader[0] = 'B';
	fileHeader[1] = 'M';
	//File size
	fileHeader[2] = fileSize;
	fileHeader[3] = fileSize >> 8;
	fileHeader[4] = fileSize >> 16;
	fileHeader[5] = fileSize >> 24;
	//Reserved
	fileHeader[6] = 0;
	fileHeader[7] = 0;
	fileHeader[8] = 0;
	fileHeader[9] = 0;
	//Pixel data offset
	fileHeader[10] = fileHeaderSize + informationHeaderSize;
	fileHeader[11] = 0;
	fileHeader[12] = 0;
	fileHeader[13] = 0;

	unsigned char informationHeader[informationHeaderSize];

	//Header size
	informationHeader[0] = informationHeaderSize;
	informationHeader[1] = 0;
	informationHeader[2] = 0;
	informationHeader[3] = 0;
	//Image width
	informationHeader[4] = m_width;
	informationHeader[5] = m_width >> 8;
	informationHeader[6] = m_width >> 16;
	informationHeader[7] = m_width >> 24;
	//Image height
	informationHeader[8] = m_height;
	informationHeader[9] = m_height >> 8;
	informationHeader[10] = m_height >> 16;
	informationHeader[11] = m_height >> 24;
	//Planes
	informationHeader[12] = 1;
	informationHeader[13] = 0;
	//Bits per pixel (RGB)
	informationHeader[14] = 24;
	informationHeader[15] = 0;
	//Comperssion
	informationHeader[16] = 0;
	informationHeader[17] = 0;
	informationHeader[18] = 0;
	informationHeader[19] = 0;
	//Image size (No compression)
	informationHeader[20] = 0;
	informationHeader[21] = 0;
	informationHeader[22] = 0;
	informationHeader[23] = 0;
	//X pixels per meter (?)
	informationHeader[24] = 0;
	informationHeader[25] = 0;
	informationHeader[26] = 0;
	informationHeader[27] = 0;
	//Y pixels per meter (?)
	informationHeader[28] = 0;
	informationHeader[29] = 0;
	informationHeader[30] = 0;
	informationHeader[31] = 0;
	//Total colors (Color palette not used)
	informationHeader[32] = 0;
	informationHeader[33] = 0;
	informationHeader[34] = 0;
	informationHeader[35] = 0;
	//Important colors (Ignore idk why?)
	informationHeader[36] = 0;
	informationHeader[37] = 0;
	informationHeader[38] = 0;
	informationHeader[39] = 0;

	bayer_map.write(reinterpret_cast<char*>(fileHeader), fileHeaderSize);
	bayer_map.write(reinterpret_cast<char*>(informationHeader), informationHeaderSize);
	red_bayer_map.write(reinterpret_cast<char*>(fileHeader), fileHeaderSize);
	red_bayer_map.write(reinterpret_cast<char*>(informationHeader), informationHeaderSize);
	green_bayer_map.write(reinterpret_cast<char*>(fileHeader), fileHeaderSize);
	green_bayer_map.write(reinterpret_cast<char*>(informationHeader), informationHeaderSize);
	blue_bayer_map.write(reinterpret_cast<char*>(fileHeader), fileHeaderSize);
	blue_bayer_map.write(reinterpret_cast<char*>(informationHeader), informationHeaderSize);

	std::ofstream diffrence("diffrence.bmp");

	diffrence.write(reinterpret_cast<char*>(fileHeader), fileHeaderSize);
	diffrence.write(reinterpret_cast<char*>(informationHeader), informationHeaderSize);
	fuji_map.write(reinterpret_cast<char*>(fileHeader), fileHeaderSize);
	fuji_map.write(reinterpret_cast<char*>(informationHeader), informationHeaderSize);
	red_fuji_map.write(reinterpret_cast<char*>(fileHeader), fileHeaderSize);
	red_fuji_map.write(reinterpret_cast<char*>(informationHeader), informationHeaderSize);
	green_fuji_map.write(reinterpret_cast<char*>(fileHeader), fileHeaderSize);
	green_fuji_map.write(reinterpret_cast<char*>(informationHeader), informationHeaderSize);
	blue_fuji_map.write(reinterpret_cast<char*>(fileHeader), fileHeaderSize);
	blue_fuji_map.write(reinterpret_cast<char*>(informationHeader), informationHeaderSize);

	pixels.resize(m_width * m_height);

	bayer_lens(pixels);

	std::ofstream blue_logs("test_logs.txt");

	for (int i = 0; i < pixels.size(); i++)
	{
		blue_logs << pixels[i].b << " " << pixels[i].g << " " << pixels[i].b << "\n";
	}

	blue_logs.close();

	for (int y = 0; y < m_height; y++)
	{
		for (int x = 0; x < m_width; x++)
		{
			unsigned char r = static_cast<unsigned char> (pixels[y * m_width + x].r);
			unsigned char g = static_cast<unsigned char> (pixels[y * m_width + x].g);
			unsigned char b = static_cast<unsigned char> (pixels[y * m_width + x].b);

			unsigned char color[] = { b, g, r };
			unsigned char color_red[] = { 0, 0, r };
			unsigned char color_green[] = { 0, g, 0 };
			unsigned char color_blue[] = { b, 0, 0 };

			bayer_map.write(reinterpret_cast<char*>(color), 3);
			red_bayer_map.write(reinterpret_cast<char*>(color_red), 3);
			green_bayer_map.write(reinterpret_cast<char*>(color_green), 3);
			blue_bayer_map.write(reinterpret_cast<char*>(color_blue), 3);
		}

		bayer_map.write(reinterpret_cast<char*>(bmpPad), paddingAmount);
		red_bayer_map.write(reinterpret_cast<char*>(bmpPad), paddingAmount);
		green_bayer_map.write(reinterpret_cast<char*>(bmpPad), paddingAmount);
		blue_bayer_map.write(reinterpret_cast<char*>(bmpPad), paddingAmount);
	}

	bayer_map.close();
	red_bayer_map.close();
	green_bayer_map.close();
	blue_bayer_map.close();

	pixels.clear();

	pixels.resize(m_height * m_width);

	fuji_lens(pixels);

	for (int y = 0; y < m_height; y++)
	{
		for (int x = 0; x < m_width; x++)
		{
			unsigned char r = static_cast<unsigned char> (pixels[y * m_width + x].r);
			unsigned char g = static_cast<unsigned char> (pixels[y * m_width + x].g);
			unsigned char b = static_cast<unsigned char> (pixels[y * m_width + x].b);

			unsigned char color[] = { b, g, r };
			unsigned char color_red[] = { 0, 0, r };
			unsigned char color_green[] = { 0, g, 0 };
			unsigned char color_blue[] = { b, 0, 0 };

			fuji_map.write(reinterpret_cast<char*>(color), 3);
			red_fuji_map.write(reinterpret_cast<char*>(color_red), 3);
			blue_fuji_map.write(reinterpret_cast<char*>(color_blue), 3);
			green_fuji_map.write(reinterpret_cast<char*>(color_green), 3);
		}

		fuji_map.write(reinterpret_cast<char*>(bmpPad), paddingAmount);
		red_fuji_map.write(reinterpret_cast<char*>(bmpPad), paddingAmount);
		green_fuji_map.write(reinterpret_cast<char*>(bmpPad), paddingAmount);
		blue_fuji_map.write(reinterpret_cast<char*>(bmpPad), paddingAmount);
	}

	fuji_map.close();
	red_fuji_map.close();
	green_fuji_map.close();
	blue_fuji_map.close();

	std::cout << "JESUS WEPT AS THERE WERE NO MORE WORLDS TO CONQUER" << "\n";
}