#pragma once
#include <iostream>
#include <fstream>
#include <cmath>

int convertation_numder(char* string, int start_byte, int length);

struct ColorPixel {
	unsigned char R;
	unsigned char G;
	unsigned char B;
};


class ImageBMP {
public:
	enum class ImageState
	{
		DEFAULT = 0,
		RIGHT = 1,
		FLIP = 2,
		LEFT = 3
	};

	~ImageBMP();
    int ReadBMPfile(std::string path);
    int SaveImage(std::string path);
    void Rotation(ImageState turn);
    void GaussFilter(int r);

private:
	/* Между заголовком бмп и пикселями на некоторых системах есть еще данные. 
	 * на моей системе твоя программа навернка бы не работала */
	char HeaderAndInformation[52];
	ColorPixel** Image;

    int ImageSize;
	int ImageWidth;
	int ImageHeight;
	/* Тут надо было сделать enum class, потому что чар можно здать произвольный, а енам при
	 * бессмысленных данных выведет ошибку */
    ImageState State; // defald - 0, right - 1, left - 3, flip - 2
};



/*
	План работы:
	1 разбить массив на заголовок и информацию
		сделать позицию через енум +
	2 сделать две фукции для того, чтобы указыть способ чтения массива пикселей при записи в файл
	3 вынести запись заголовка и информации из условия (собственно, от условий избавиться)
	4 прописать функцию для применения фильтра гаусса к конкретному пикселю 
	5 прописать функцию для применения фильтра к строкам и столбцам 
*/
