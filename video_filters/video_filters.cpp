#include <iostream>
#include "rgb_image.h"
#include "read_ppm.h"
#include "stegano.h"

static const int test_width = 16;
static const int test_height = 9;

void test_1();
void test_2();

int main()
{
	test_1();
	test_2();
	int i;
	std::cin >> i;
	return 0;
}

//test basic pixel access and out-of-bounds operations
void test_1()
{
	rgb_image test(test_width, test_height);
	for (int i = 0; i < test_height + 5; ++i)
	{
		for (int j = 0; j < test_width + 3; j++)
		{
			test[i][j].r = i;
			test[i][j].g = j;
			test[i][j].b = i + j;
		}
	}
	for (int i = 0; i < test_height; ++i)
	{
		for (int j = 0; j < test_width; j++)
		{
			std::cout << test[i][j].r << "," << test[i][j].g << "," << test[i][j].b << ";";
		}
		std::cout << std::endl;
	}
}

//test read ppm, pgm, pbm
void test_2()
{
	rgb_image ppmtest;
	read_ppm("..\\..\\image_samples\\marbles.pbm", ppmtest);
	rgb_image tgt;
	steganography_encode(ppmtest, "hello world", tgt);
	for (int i = 0; i < ppmtest.height; ++i)
	{
		for (int j = 0; j < ppmtest.width; j++)
		{
			std::cout << ppmtest[i][j].r << "," << ppmtest[i][j].g << "," << ppmtest[i][j].b << ";";
		}
		std::cout << std::endl;
	}
}
