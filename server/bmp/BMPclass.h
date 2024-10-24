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
	/* ����� ���������� ��� � ��������� �� ��������� �������� ���� ��� ������. 
	 * �� ���� ������� ���� ��������� �������� �� �� �������� */
	char HeaderAndInformation[52];
	ColorPixel** Image;

    int ImageSize;
	int ImageWidth;
	int ImageHeight;
	/* ��� ���� ���� ������� enum class, ������ ��� ��� ����� ����� ������������, � ���� ���
	 * ������������� ������ ������� ������ */
    ImageState State; // defald - 0, right - 1, left - 3, flip - 2
};



/*
	���� ������:
	1 ������� ������ �� ��������� � ����������
		������� ������� ����� ���� +
	2 ������� ��� ������ ��� ����, ����� ������� ������ ������ ������� �������� ��� ������ � ����
	3 ������� ������ ��������� � ���������� �� ������� (����������, �� ������� ����������)
	4 ��������� ������� ��� ���������� ������� ������ � ����������� ������� 
	5 ��������� ������� ��� ���������� ������� � ������� � �������� 
*/
