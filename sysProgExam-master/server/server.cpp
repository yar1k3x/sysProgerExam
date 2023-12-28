#include <windows.h>
#include <iostream>
using namespace std;

int main()
{
    // Переменные для размера буфера и курсора
    int bufferSizeX, bufferSizeY, cursorSize;
    // Длина массива и вспомогательная переменная
    int arrLength = 0;
    int temp = 0;

    // Запрос размера буфера и курсора
    cout << "Input buffer size (X Y): ";
    cin >> bufferSizeX >> bufferSizeY;
    cout << "Input cursor size: ";
    cin >> cursorSize;

    // Ввод длины массива
    cout << "Input the length of the array: ";
    cin >> arrLength;

    // Создание массива
    int* arr = new int[arrLength];

    // Дескрипторы анонимных каналов
    HANDLE ReadPipe, WritePipe;
    SECURITY_ATTRIBUTES SecurityAttributes = { 0 };
    SecurityAttributes.nLength = sizeof(SecurityAttributes);
    SecurityAttributes.bInheritHandle = TRUE;

    // Создание анонимного канала
    if (CreatePipe(&ReadPipe, &WritePipe, &SecurityAttributes, 0))
    {
        STARTUPINFO StartupInfo = { 0 }; //для создания процесса
        StartupInfo.cb = sizeof(StartupInfo); // cb - count of bytes
        PROCESS_INFORMATION ProcessInfo = { 0 }; // о самом процессе

        // Получение пути к исполняемому файлу и формирование строки запуска
        wchar_t Path[MAX_PATH], CmdLine[MAX_PATH];
        GetModuleFileName(nullptr, Path, MAX_PATH);
        *wcsrchr(Path, '\\') = 0;
        swprintf_s(CmdLine, L"\"%s\\%s\" %jd %jd", Path, L"client.exe", (long long)ReadPipe, (long long)WritePipe);

        // Создание процесса-клиента
        if (CreateProcess(nullptr, CmdLine, nullptr, nullptr, true, CREATE_NEW_CONSOLE,
            nullptr, nullptr, &StartupInfo, &ProcessInfo))
        {
            //количество записанных и прочитанных байт
            DWORD BytesRead, BytesWritten;

            // Отправка типа сообщения для инициации коммуникации
            long messageType = 0;

            // Ожидание событий
            while (true)
            {
                // Если нажата правая кнопка мыши
                if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
                {
                    // Отправка типа сообщения и информации о буфере
                    messageType = 1;
                    WriteFile(WritePipe, &messageType, sizeof(messageType), &BytesWritten, nullptr);
                    WriteFile(WritePipe, &bufferSizeX, sizeof(bufferSizeX), &BytesWritten, nullptr);
                    WriteFile(WritePipe, &bufferSizeY, sizeof(bufferSizeY), &BytesWritten, nullptr);
                    WriteFile(WritePipe, &cursorSize, sizeof(cursorSize), &BytesWritten, nullptr);

                    cout << "Buffer size sent: " << bufferSizeX << " " << bufferSizeY << "\n" << endl;

                    break;
                }

                // Если нажата клавиша 'G'
                if (GetAsyncKeyState('G') & 0x8000)
                {
                    temp = 1;
                    // Отправка типа сообщения и длины массива
                    messageType = 2;
                    WriteFile(WritePipe, &messageType, sizeof(messageType), &BytesWritten, nullptr);
                    WriteFile(WritePipe, &arrLength, sizeof(arrLength), &BytesWritten, nullptr);

                    cout << "Array sent.\n" << endl;
                    break;
                }

                // Если нажата левая кнопка мыши
                if (GetAsyncKeyState('E') & 0x8000)
                {
                    cout << "Left mouse button pressed. Exiting..." << endl;
                    break;
                }

                // Засыпание на короткое время для предотвращения высокого использования процессора
                Sleep(100);
            }

            // Ожидание завершения процесса-клиента
            WaitForSingleObject(ProcessInfo.hProcess, INFINITE);

            // Если была отправлена информация о массиве
            if (temp == 1)
            {
                // Вывод полученных данных массива
                cout << "Received array: ";
                for (int i = 0; i < arrLength; i++)
                {
                    ReadFile(ReadPipe, &arr[i], sizeof(arr[i]), &BytesRead, nullptr);
                    cout << arr[i] << " ";
                }
                cout << endl;
            }

            // Закрытие дескрипторов процесса-клиента
            CloseHandle(ProcessInfo.hProcess);
            CloseHandle(ProcessInfo.hThread);
        }
        else
            cout << "Process creation failed";

        // Закрытие дескрипторов канала
        CloseHandle(ReadPipe);
        CloseHandle(WritePipe);
    }
    else
        cout << "Pipe creation failed";

    // Очистка памяти
    delete[] arr;

    // Завершение работы сервера
    cout << "Server's work is done. Press Enter to exit" << endl;
    system("pause");
    return 0;
}
