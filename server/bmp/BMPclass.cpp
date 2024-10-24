#include "BMPclass.h"
/* ���������, ��� ��� ������� �������� �� ������ ��������, ��� � ���� �� ��������� 
 * �� �������, ��� ���� ��� ������ ����� � ���� ������. */
/* �� ��� ����� �� ������� ������, ������� ����� ��������� �� ���������*/
int convertation_numder(char* string, int start_byte, int length) // ��������� ������, ������ �����, � �������� �������� ������, � ������ 
{
    unsigned int number = 0;
    for (int i = start_byte + length - 1; i >= start_byte; i--) 
        number = (number << 8) + (string[i] & 255);
    return number;
}




int ImageBMP::ReadBMPfile(std::string path)
{
    std::fstream imageFile;
    /* ����� ���� ������� ��� � ������� ������������, � �� ��������� open */
    imageFile.open(path, std::ios::in | std::ios::binary); //��������� ���� � ���������

    if (!imageFile.is_open()) // �������� �� ��������
        return -1;

    int x = 0;
    int i = 0, j = 0;

    imageFile.read(HeaderAndInformation, 54); // ������ ��������� � ���������� �� ����� 

    if (HeaderAndInformation[0] != 'B' || HeaderAndInformation[1] != 'M') // ���������, �������� �� ������ ���� ��� ���������
        return -2;
    if(int(HeaderAndInformation[28]) != 24) // ��������� ������� ����� ( ���� ��� ������ ��������� �������� ������ � ���������� �������� 24 ���/�������
        return -3;
    /* ��, � �����, ����� ���� ��� ���� �����. ������ �������� ������� ��������� ���������, � �� �� � �������� �� �����������
     * �����! */
    ImageSize = convertation_numder(HeaderAndInformation, 2, 4); // ��������� ������ ��������
    ImageWidth = convertation_numder(HeaderAndInformation, 18, 4); // ��������� ������ ��������
    ImageHeight = convertation_numder(HeaderAndInformation, 22, 4); // ��������� ������ ��������
    State = ImageState::DEFAULT;

    int SpaceRest = ImageWidth & 3;
    Image = new ColorPixel*[ImageHeight];

    int R = 0, G = 0, B = 0;
    for (i = 0; i < ImageHeight; i++)
    {
        Image[i] = new ColorPixel[ImageWidth];
        for (j = 0; j < ImageWidth; j++)
        {
            imageFile.read((char*)&Image[i][j].B, 1);
            imageFile.read((char*)&Image[i][j].G, 1);
            imageFile.read((char*)&Image[i][j].R, 1);
        } 
        imageFile.seekg(int(imageFile.tellg()) + SpaceRest, std::ios::beg);
    }

    imageFile.close(); // ����� ��� �������� �����
    return 0;
}


void ImageBMP::Rotation(ImageState turn) // ���������, � ����� ������� �������� ��������, � ������ � ���������� ���������� 
{
    State = ImageState((int(State) + int(turn)) & 3); // ������� �� ������� �� 4
}


void ImageBMP::GaussFilter(int R)
{
    int D = 2 * R + 1; // �����
    ColorPixel* Win = new ColorPixel[D]; // ����
    double* Core = new double[D]; // ���� (����)

    const double PI = 3.141592653589793;
    double valueR, valueG, valueB; // �������� �������

    int i, j, k;

    for (i = 0; i < R + 1; i++) // ���������� �����������, ����� ��������� ������ ������. ��������
    {
        Core[i] = (1 / (exp(((R - i) * (R - i)) / (2 * R * R)))); 
        Core[D - 1 - i] = Core[i];
    }
    /* ��� ������ �������� ����������� ����������. ����� �����. ����� ���-�� �������,
     * ��������, ��������� ��������������� ������� */
    // ��������� ������ � �������
    for (i = 0; i < ImageHeight; i++)
    {
        for (k = 0; k < D; k++) // ��������� ���� ( ��-�� ����� �� ������ ����� 0�� ������� (����� ��������� �������� �������� �������� ������))
        {
            if (k < R)
            {
                Win[k].B = Image[i][0].B;
                Win[k].G = Image[i][0].G;
                Win[k].R = Image[i][0].R;
            }
            else 
            {
                Win[k].B = Image[i][k - R].B;
                Win[k].G = Image[i][k - R].G;
                Win[k].R = Image[i][k - R].R;
            }
        }
        // ��������� ������ � �������
        for (j = 0; j < ImageWidth; j++)
        {
            valueR = 0;
            valueG = 0;
            valueB = 0;
            for (k = 0; k < D; k++)
            {
                // ���������� �������� � �������
                valueR += Win[k].R * Core[k];
                valueG += Win[k].G * Core[k];
                valueB += Win[k].B * Core[k];

                // ����� ����
                if(k > 0) // �������� ������ ��-� ����
                {
                    Win[k - 1].B = Win[k].B;
                    Win[k - 1].G = Win[k].G;
                    Win[k - 1].R = Win[k].R;
                }
                if (k == D - 1)
                {
                    if (j + R >= ImageWidth) // ���� ������� ������ ������� �� ������
                    {
                        Win[k].B = Image[i][ImageWidth - 1].B;
                        Win[k].G = Image[i][ImageWidth - 1].G;
                        Win[k].R = Image[i][ImageWidth - 1].R;
                        
                    }
                    else
                    {
                        Win[k].B = Image[i][j + R].B;
                        Win[k].G = Image[i][j + R].G;
                        Win[k].R = Image[i][j + R].R;
                    }
                }
            }
            // ���������� ��������
            Image[i][j].R = (unsigned char)(valueR / (sqrt(2 * PI) * R));
            Image[i][j].G = (unsigned char)(valueG / (sqrt(2 * PI) * R));
            Image[i][j].B = (unsigned char)(valueB / (sqrt(2 * PI) * R));
        } 
    }


    /* ��� ��� ��������� ���������. ����� ����� �������, ���� �� ���-�� ���������� */
    // ��������� ������ � ��������
    for (j = 0; j < ImageWidth; j++)
    {
        for (k = 0; k < D; k++) // ��������� ���� ( ��-�� ����� �� ������ ����� 0�� �������)
        {
            if (k < R)
            {
                Win[k].B = Image[0][j].B;
                Win[k].G = Image[0][j].G;
                Win[k].R = Image[0][j].R;
            }
            else
            {
                Win[k].B = Image[k - R][j].B;
                Win[k].G = Image[k - R][j].G;
                Win[k].R = Image[k - R][j].R;
            }
        }


        for (i = 0; i < ImageHeight; i++)
        {
            valueR = 0;
            valueG = 0;
            valueB = 0;
            for (k = 0; k < D; k++)
            {
                // ���������� �������� � �������
                valueR += Win[k].R * Core[k];
                valueG += Win[k].G * Core[k];
                valueB += Win[k].B * Core[k];

                // ����� ����
                if (k > 0) // �������� ������ ��-� ����
                {
                    Win[k - 1].B = Win[k].B;
                    Win[k - 1].G = Win[k].G;
                    Win[k - 1].R = Win[k].R;
                }
                if (k == D - 1)
                {
                    if (i + R >= ImageHeight) // ���� ������� ������ ������� �� ������
                    {
                        Win[k].B = Image[ImageHeight - 1][j].B;
                        Win[k].G = Image[ImageHeight - 1][j].G;
                        Win[k].R = Image[ImageHeight - 1][j].R;
                    }
                    else
                    {
                        Win[k].B = Image[i + R][j].B;
                        Win[k].G = Image[i + R][j].G;
                        Win[k].R = Image[i + R][j].R;
                    }
                }
            }
            // ���������� ��������
            Image[i][j].R = (unsigned char)(valueR / (sqrt(2 * PI) * R));
            Image[i][j].G = (unsigned char)(valueG / (sqrt(2 * PI) * R));
            Image[i][j].B = (unsigned char)(valueB / (sqrt(2 * PI) * R));

        }
    }

    delete[] Win;
    delete[] Core;
}




int ImageBMP::SaveImage(std::string path)
{
    /*
        ��������� ���� �� ���� Path
        ���������� ���� HeaderAndInformation
            ������ � HeaderAndInformation ������� ����� ������ � ������, ���� �������� ���� ��������� �� 90 ��������
        ���������� ��������� ������ (� ������ ��������), ��� ���� �� ������� ��������� ���. ���� � ����� ������, 
            ���� ������ (��� ������, ���� ��������� �������� �� 90��)
        ��������� ���� 
    
    */
    std::fstream imageFile;

    imageFile.open(path, std::ios::out | std::ios::binary); //��������� ����
    if (!imageFile.is_open()) // �������� �� ��������
        return -1;

    int i, j;
    std::string Space;
    /* ����� ���. �������������� ���� ���� �� ������. ��� ������ �������� ���������� �� �����,
     * �����, �������, ��� ������� �������� ��� ������� �� ����������. ���� �� ���-���� ����� �������,
     * �� ����� ��������� �����, � ������� ����� ����������� ���������������� ������� �������� (��������,
     * ������ �� ���������� ����� ����������� ��� ��������� � �����) */

    if (State == ImageState::RIGHT || State == ImageState::LEFT) // ���� ����������� ���������, �� ������ ������ � ������ ������� ��� ������ � ����
    {
        imageFile.write(reinterpret_cast<char*>(HeaderAndInformation), 18);
        imageFile.write(reinterpret_cast<char*>(&HeaderAndInformation[22]), 4);
        imageFile.write(reinterpret_cast<char*>(&HeaderAndInformation[18]), 4);
        imageFile.write(reinterpret_cast<char*>(&HeaderAndInformation[26]), 28);
    }
    else
    {
        imageFile.write(reinterpret_cast<char*>(HeaderAndInformation), 54);
    }


    if (State == ImageState::DEFAULT) // ������� �� ��������
    {
        Space = std::string(ImageWidth & 3, '\0');
        for (i = 0; i < ImageHeight; i++)
        {
            for (j = 0; j < ImageWidth; j++)
            {
                imageFile.write(reinterpret_cast<char*>(&Image[i][j].B), 1);
                imageFile.write(reinterpret_cast<char*>(&Image[i][j].G), 1);
                imageFile.write(reinterpret_cast<char*>(&Image[i][j].R), 1);
            }
            imageFile << Space;
        }
    }
    else if (State == ImageState::RIGHT) // ������� �������
    {
        Space = std::string(ImageHeight & 3, '\0');
        for (j = ImageWidth - 1; j >= 0; j--)
        {
            for (i = 0; i < ImageHeight; i++)
            {
                imageFile.write(reinterpret_cast<char*>(&Image[i][j].B), 1);
                imageFile.write(reinterpret_cast<char*>(&Image[i][j].G), 1);
                imageFile.write(reinterpret_cast<char*>(&Image[i][j].R), 1);
            }
            imageFile << Space;
        }
    }
    else if (State == ImageState::FLIP) // ����������� ��������
    {
        Space = std::string(ImageWidth & 3, '\0');
        for (i = ImageHeight - 1; i >= 0; i--)
        {
            for (j = ImageWidth - 1; j >= 0; j--)
            {
                imageFile.write(reinterpret_cast<char*>(&Image[i][j].B), 1);
                imageFile.write(reinterpret_cast<char*>(&Image[i][j].G), 1);
                imageFile.write(reinterpret_cast<char*>(&Image[i][j].R), 1);
            }
            imageFile << Space;
        }
    }
    else if (State == ImageState::LEFT) // ������� ������
    {
        Space = std::string(ImageHeight & 3, '\0');

        for (j = 0; j < ImageWidth; j++)
        {
            for (i = ImageHeight - 1; i >= 0; i--)
            {
                imageFile.write(reinterpret_cast<char*>(&Image[i][j].B), 1);
                imageFile.write(reinterpret_cast<char*>(&Image[i][j].G), 1);
                imageFile.write(reinterpret_cast<char*>(&Image[i][j].R), 1);
            }
            imageFile << Space;
        }
    }
    imageFile.close();
    return 0;
}

/* � ������� ������� ��� �����? */
ImageBMP::~ImageBMP()
{
    for (int i = 0; i < ImageHeight; i++)
    {
        delete[] Image[i];
    }
    
    delete[] Image;
}
