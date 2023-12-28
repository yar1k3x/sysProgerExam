#include <windows.h> 
#include <iostream> 
using namespace std;
int wmain(int argc, wchar_t* argv[])
{
	/*

	ПОЛУЧЕНИЕ РАЗМЕРА МАССИВА

	*/
	// Получаем handlы каналов
	HANDLE ReadPipe = (HANDLE)_wtoi64(argv[1]);
	HANDLE WritePipe = (HANDLE)_wtoi64(argv[2]);

	DWORD BytesWritten, BytesRead;

	Sleep(500);

	long n = 0;
	WriteFile(WritePipe, &n, sizeof(n), &BytesWritten, nullptr);

	int arrLength = 0;


	// Ожидаем записи в канал
	WaitForSingleObject(ReadPipe, INFINITE);

	// Читаем значение
	ReadFile(ReadPipe, &arrLength, sizeof(arrLength), &BytesRead, nullptr);

	// Выводим его в консоль
	cout << "The array length is " << arrLength << endl;

	float* arr = new float[arrLength];

	cout << endl << endl;
	/*

	ПОЛУЧЕНИЕ МАССИВА

	*/
	{
		cout << "Receive started" << endl;
		for (int i = 0; i < arrLength; i++)
		{
			ReadFile(ReadPipe, &arr[i], sizeof(arr[i]), &BytesRead, nullptr);
			cout << "Received " << arr[i] << "; ";
		}
		cout << endl << "Receive done." << endl;
	}

	cout << endl << endl;
	/*

	ЗАПРОС ВЕЩЕСТВЕННЫХ ЧИСЕЛ

	*/
	float startRangeN = 0;
	float endRangeM = 0;
	{
		cout << "Input the first number of range: ";
		cin >> startRangeN;

		cout << "Input the last number of range: ";
		cin >> endRangeM;
	}


	float* arrCorrect = new float[arrLength];

	cout << endl << endl;

	/*

	ОПРЕДЕЛЕНИЕ ПРИНАДЛЕЖНОСТИ ОТРЕЗКУ

	*/
	int corrSize = 0;
	{
		cout << "The numbers of array that belong in [" << startRangeN << ", " << endRangeM << "]: ";
		
		for (int i = 0; i < arrLength; i++)
		{
			if (arr[i] >= startRangeN && arr[i] <= endRangeM)
			{
				arrCorrect[corrSize] = arr[i];
				cout << arrCorrect[corrSize++] << " ";
			}
		}
		cout << endl;
	}
	cout << endl << endl;
	/*

	ОТПРАВКА

	*/
	cout << "Left-click to send the array";
	while (true)
		if (GetKeyState(VK_LBUTTON) & 0x80)
		{
			WriteFile(WritePipe, &corrSize, sizeof(corrSize), &BytesWritten, nullptr);
			cout << endl << endl;
			cout << endl << "Send started" << endl;
			for (int i = 0; i < corrSize; i++)
			{
				WriteFile(WritePipe, &arrCorrect[i], sizeof(arrCorrect[i]), &BytesWritten, nullptr);
				cout << "Sent " << arrCorrect[i] << "; ";
			}
			cout << endl << "Send done." << endl;

			break;
		}



	cout << endl << endl;
	system("pause");
	return 0;
}