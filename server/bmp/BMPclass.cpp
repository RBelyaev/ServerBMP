#include "BMPclass.h"
/* Правильно, что эта функция отдельна от класса картинки, она к нему не относится 
 * но странно, что тебе это вообще нужно в этой задаче. */
/* Ты тут вроде не меняешь строку, следует через указатель на константу*/
int convertation_numder(char* string, int start_byte, int length) // принимаем строку, индекс байта, с которого начинаем читать, и длинну 
{
    unsigned int number = 0;
    for (int i = start_byte + length - 1; i >= start_byte; i--) 
        number = (number << 8) + (string[i] & 255);
    return number;
}




int ImageBMP::ReadBMPfile(std::string path)
{
    std::fstream imageFile;
    /* Можно было открыть его с помощью конструктора, а не отдельным open */
    imageFile.open(path, std::ios::in | std::ios::binary); //открывает файл в бинарнике

    if (!imageFile.is_open()) // проверка на открытие
        return -1;

    int x = 0;
    int i = 0, j = 0;

    imageFile.read(HeaderAndInformation, 54); // читаем заголовок и информацию об файле 

    if (HeaderAndInformation[0] != 'B' || HeaderAndInformation[1] != 'M') // проверяем, является ли данный файл бмп картинкой
        return -2;
    if(int(HeaderAndInformation[28]) != 24) // проверяем глубину цвета ( пока что данная программа работает только с картинками глубиной 24 бит/пиксель
        return -3;
    /* Всё, я понял, зачем тебе это было нужно. Другие товарищи стырили структуру заголовка, а ты ее в принципе не используешь
     * Годно! */
    ImageSize = convertation_numder(HeaderAndInformation, 2, 4); // вычисляем размер картинки
    ImageWidth = convertation_numder(HeaderAndInformation, 18, 4); // вычисляем ширину картинки
    ImageHeight = convertation_numder(HeaderAndInformation, 22, 4); // вычисляем высоту картинки
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

    imageFile.close(); // метод для закрытия файла
    return 0;
}


void ImageBMP::Rotation(ImageState turn) // принимает, в какую сторону повернут картинку, и сложит с остальными поворотами 
{
    State = ImageState((int(State) + int(turn)) & 3); // остаток от деления на 4
}


void ImageBMP::GaussFilter(int R)
{
    int D = 2 * R + 1; // центр
    ColorPixel* Win = new ColorPixel[D]; // окно
    double* Core = new double[D]; // ядро (коэф)

    const double PI = 3.141592653589793;
    double valueR, valueG, valueB; // значиние пикселя

    int i, j, k;

    for (i = 0; i < R + 1; i++) // выписываем коэфициенты, чтобы соверщать меньше арфмет. операций
    {
        Core[i] = (1 / (exp(((R - i) * (R - i)) / (2 * R * R)))); 
        Core[D - 1 - i] = Core[i];
    }
    /* Тут просто лютейшая вложенность получается. Глаза текут. Стоит как-то разбить,
     * вероятно, введением вспомогательной функции */
    // применяем фильтр к строкам
    for (i = 0; i < ImageHeight; i++)
    {
        for (k = 0; k < D; k++) // заполняем окно ( эл-ты слева от центра равны 0му пикселю (чтобы корректно посчтать значение крайнего пкселя))
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
        // применяем фильтр к строкам
        for (j = 0; j < ImageWidth; j++)
        {
            valueR = 0;
            valueG = 0;
            valueB = 0;
            for (k = 0; k < D; k++)
            {
                // вычисление значения в пикселе
                valueR += Win[k].R * Core[k];
                valueG += Win[k].G * Core[k];
                valueB += Win[k].B * Core[k];

                // сдвиг окна
                if(k > 0) // значения других эл-в окна
                {
                    Win[k - 1].B = Win[k].B;
                    Win[k - 1].G = Win[k].G;
                    Win[k - 1].R = Win[k].R;
                }
                if (k == D - 1)
                {
                    if (j + R >= ImageWidth) // если крайний правый выходит за массив
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
            // присваивем значение
            Image[i][j].R = (unsigned char)(valueR / (sqrt(2 * PI) * R));
            Image[i][j].G = (unsigned char)(valueG / (sqrt(2 * PI) * R));
            Image[i][j].B = (unsigned char)(valueB / (sqrt(2 * PI) * R));
        } 
    }


    /* Тут код практческ однаковый. Точно нужна функция, либо же как-то объединить */
    // применяем фильтр к столбцам
    for (j = 0; j < ImageWidth; j++)
    {
        for (k = 0; k < D; k++) // заполняем окно ( эл-ты слева от центра равны 0му пикселю)
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
                // вычисление значения в пикселе
                valueR += Win[k].R * Core[k];
                valueG += Win[k].G * Core[k];
                valueB += Win[k].B * Core[k];

                // сдвиг окна
                if (k > 0) // значения других эл-в окна
                {
                    Win[k - 1].B = Win[k].B;
                    Win[k - 1].G = Win[k].G;
                    Win[k - 1].R = Win[k].R;
                }
                if (k == D - 1)
                {
                    if (i + R >= ImageHeight) // если крайний правый выходит за массив
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
            // присваивем значение
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
        открываем файл по пути Path
        записываем туда HeaderAndInformation
            меняем в HeaderAndInformation местами байты высоты и ширины, если картинка была повернута на 90 градусов
        записываем двумерный массив (с учетом поворота), при этом не забывая добавлять доп. байт в конец строки, 
            если ширина (или высота, если повернули картинку на 90гр)
        закрываем файл 
    
    */
    std::fstream imageFile;

    imageFile.open(path, std::ios::out | std::ios::binary); //открывает файл
    if (!imageFile.is_open()) // проверка на открытие
        return -1;

    int i, j;
    std::string Space;
    /* Точно нет. Повторяющегося кода быть не должно. Два метода поворота смотрелось бы лучше,
     * Круто, конечно, что никаких действий над данными не происходит. Если мы все-таки хотим сделать,
     * то нужно придумать метод, в котором будет объедняться функциональность каждого варианта (вероятно,
     * одними из параметров будут ограничения для координат в форах) */

    if (State == ImageState::RIGHT || State == ImageState::LEFT) // если изображение повернуто, то меняем высоту и ширину местами при записи в файл
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


    if (State == ImageState::DEFAULT) // поворот не совершен
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
    else if (State == ImageState::RIGHT) // поворот направо
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
    else if (State == ImageState::FLIP) // перевернуть картинку
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
    else if (State == ImageState::LEFT) // поворот налево
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

/* А удалять пиксели кто будет? */
ImageBMP::~ImageBMP()
{
    for (int i = 0; i < ImageHeight; i++)
    {
        delete[] Image[i];
    }
    
    delete[] Image;
}
