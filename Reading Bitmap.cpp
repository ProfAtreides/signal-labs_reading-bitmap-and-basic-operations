#include "bitmap.h"
#include <iostream>
#include <fstream>
const char* file_path1 = "birdmap.bmp";
const char* file_path2 = "rotate.bmp";

int main()
{
	const int width = 32;
	const int height = 32;

	std::ios_base::sync_with_stdio(false);
	std::cin.tie(NULL);

	bitmap bitmap1(width, height, file_path1);

	//std::cout << "DEBUG";
	bitmap1.read_file();

	/*double scale;
	std::cin >> scale;

	int new_width = bitmap1.m_width * scale;
	int new_height = bitmap1.m_height * scale;
	//std::cout << new_height << new_width;*/

	//bitmap rescaled_bitmap = bitmap1.rescale(1000, 1000);
	//rescaled_bitmap.rotate(180.0);
	//rescaled_bitmap.export_file("haha.bmp");

	//bitmap1.rotate(310);
	//bitmap1.export_file("rotated.bmp");

	bitmap1.grayscale();
}