#include <windows.h>
#include <iostream>
using namespace std;

int wmain(int argc, wchar_t* argv[])
{
    // Получаем дескрипторы анонимных каналов из аргументов командной строки
    HANDLE ReadPipe = (HANDLE)_wtoi64(argv[1]);
    HANDLE WritePipe = (HANDLE)_wtoi64(argv[2]);

    DWORD BytesWritten, BytesRead;

    // Ждем некоторое время для обеспечения синхронизации процессов
    Sleep(500);

    // Получаем тип сообщения для определения его структуры
    long messageType;
    ReadFile(ReadPipe, &messageType, sizeof(messageType), &BytesRead, nullptr);

    if (messageType == 1) // Сообщение о размере буфера и размере курсора
    {
        // Получаем размеры буфера и курсора от сервера
        int bufferSizeX, bufferSizeY, cursorSize;
        ReadFile(ReadPipe, &bufferSizeX, sizeof(bufferSizeX), &BytesRead, nullptr);
        ReadFile(ReadPipe, &bufferSizeY, sizeof(bufferSizeY), &BytesRead, nullptr);
        ReadFile(ReadPipe, &cursorSize, sizeof(cursorSize), &BytesRead, nullptr);

        // Устанавливаем размер курсора
        HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_CURSOR_INFO cci;
        cci.dwSize = cursorSize;
        SetConsoleCursorInfo(hStdOut, &cci);

        // Устанавливаем размер буфера экрана
        COORD coord;
        coord.X = bufferSizeX;
        coord.Y = bufferSizeY;
        SetConsoleScreenBufferSize(hStdOut, coord);

        cout << "Received buffer size: " << bufferSizeX << " " << bufferSizeY << endl;
    }

    else if (messageType == 2) // Сообщение о массиве
    {
        // Получаем длину массива от сервера
        int arrLength;
        ReadFile(ReadPipe, &arrLength, sizeof(arrLength), &BytesRead, nullptr);

        // Создаем и генерируем массив
        int* arr = new int[arrLength];

        cout << "Received array: ";
        for (int i = 0; i < arrLength; i++)
        {
            arr[i] = i + 1;

            // Отправляем сгенерированный массив поэлементно
            WriteFile(WritePipe, &arr[i], sizeof(arr[i]), &BytesWritten, nullptr);
            cout << arr[i] << " ";
        }
        cout << endl;

        // Освобождаем выделенную память для массива
        delete[] arr;
    }

    // Завершаем работу клиентского процесса
    cout << "Client's work is done. Press Enter to exit" << endl;
    system("pause");
    return 0;
}
