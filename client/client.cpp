#include <windows.h> 
#include <iostream> 
#include <string.h> 
#include <conio.h>
using namespace std;

// names of colors
char red[] = "red";
char blue[] = "blue";
char green[] = "green";
char white[] = "white";
char yellow[] = "yellow";
char cyan[] = "cyan";
char magenta[] = "magenta";

// Function returns true if elements of 2 char arrays are th same
bool areArraysEqual(char* a, char* b)
{
	int i = 0;
	while (a[i])
	{
		if (a[i] != b[i])
			return false;
		i++;
	}
	return true;
}
// Return bool as a True/False string
string boolToString(bool input)
{
	if (input) return "true";
	return "false";
}

// Class for drawing rectangles
class ConsoleRectangle
{
private:
	CHAR_INFO ci[80 * 25]; // прямоугольник, из которого будем выводить 
	COORD size; // размеры этого прямоугольника 
	COORD coord; // координаты левого угла прямоугольника, из которого выводим 
	
public:
	SMALL_RECT sr; // координаты левого угла прямоугольника, в который пишем 
	int hexColor;
	// Base constructor
	ConsoleRectangle()
	{
		// устанавливаем левый угол многоугольника, из которого пишем 
		coord.X = 0;
		coord.Y = 0;
		// устанавливаем размеры прямоугольника, который пишем 
		size.X = 80;
		size.Y = 25;

		sr.Left = 1; sr.Right = 1;
		sr.Bottom = 3; sr.Top = 3;

		for (int i = 0; i < 80 * 25; ++i)
		{
			ci[i].Char.AsciiChar = ' ';
			ci[i].Attributes = BACKGROUND_RED | BACKGROUND_INTENSITY;
		}
	}
	// Set all the coords, colors are red, blue or green only. 
	bool setRectangleData(int srLeft, int srTop,
		int srRight, int srBottom,
		char* color, HANDLE hStdOut, int elementNumber)
	{
		// устанавливаем левый угол многоугольника, из которого пишем 
		coord.X = 0;
		coord.Y = 0;
		// устанавливаем размеры прямоугольника, который пишем 
		size.X = 80;
		size.Y = 25;

		sr.Left = srLeft; sr.Right = srRight;
		sr.Bottom = srBottom; sr.Top = srTop;

		// This will go to ci Attributes
		
#pragma region colorPick
		if (areArraysEqual(color, red))
		{
			hexColor = BACKGROUND_RED;
		}
		else if (areArraysEqual(color, green))
		{
			hexColor = BACKGROUND_GREEN;
		}
		else if (areArraysEqual(color, blue))
		{
			hexColor = BACKGROUND_BLUE;
		}
		else if (areArraysEqual(color, white))
		{
			hexColor = BACKGROUND_BLUE | BACKGROUND_RED | BACKGROUND_GREEN;
		}
		else if (areArraysEqual(color, yellow))
		{
			hexColor = BACKGROUND_RED | BACKGROUND_GREEN;
		}
		else if (areArraysEqual(color, cyan))
		{
			hexColor = BACKGROUND_BLUE | BACKGROUND_GREEN;
		}
		else if (areArraysEqual(color, magenta))
		{
			hexColor = BACKGROUND_BLUE | BACKGROUND_RED;
		}
		else
		{
			return false;
		}
#pragma endregion



		for (int i = 0; i < 80 * 25; ++i)
		{
			ci[i].Char.UnicodeChar = elementNumber + 0x31;
			ci[i].Attributes = hexColor | 0x0080;
		}

		return drawRectangle(hStdOut);
	}

	// Set all the coords, colors are red, blue or green only. 
	bool setRectangleData1( HANDLE hStdOut, int elementNumber)
	{
		// устанавливаем левый угол многоугольника, из которого пишем 
		coord.X = 0;
		coord.Y = 0;
		// устанавливаем размеры прямоугольника, который пишем 
		size.X = 80;
		size.Y = 25;



		for (int i = 0; i < 80 * 25; ++i)
		{
			ci[i].Char.UnicodeChar = elementNumber + 0x31;
			ci[i].Attributes = hexColor | 0x0080;
		}

		return drawRectangle(hStdOut);
	}

	bool drawRectangle(HANDLE hStdOut)
	{
		// пишем прямоугольник в буфер экрана 
		if (!WriteConsoleOutput(
			hStdOut, // дескриптор буфера экрана 
			ci, // прямоугольник, из которого пишем 
			size, // размеры этого прямоугольника 
			coord, // и его левый угол 
			&sr)) // прямоугольник, в который пишем 
		{
			return false;
		}
		return true;
	}
	bool eraseRectangle(HANDLE hStdOut)
	{
		for (int i = 0; i < 80 * 25; ++i)
		{
			ci[i].Char.UnicodeChar = ' ';
			ci[i].Attributes = 0;
		}

		return drawRectangle(hStdOut);
	}

};


int main()
{
	
#pragma region getReadyToReceive
	wchar_t machineName[80];

	wchar_t pipeName[80];
	HANDLE hNamedPipe;
	DWORD dwBytesRead; // для количества прочитанных байтов 

	// вводим имя машины в сети, на которой работает сервер 
	cout << "Enter a name of the server machine: ";
	wcin >> machineName;
	// подставляем имя машины в имя канала 
	wsprintf(pipeName, L"\\\\%s\\pipe\\demo_pipe", machineName);
	// связываемся с именованным каналом 
	hNamedPipe = CreateFile(
		pipeName, // имя канала 
		GENERIC_READ | GENERIC_WRITE, // читаем и записываем в канал 
		FILE_SHARE_READ | FILE_SHARE_WRITE, // разрешаем чтение и запись 
		NULL, // безопасность по умолчанию 
		OPEN_EXISTING, // открываем существующий канал 
		FILE_ATTRIBUTE_NORMAL, // атрибуты по умолчанию 
		NULL); // дополнительных атрибутов нет 

	// проверяем связь с каналом 
	if (hNamedPipe == INVALID_HANDLE_VALUE)
	{
		cerr << "Connection with the named pipe failed." << endl
			<< "The last error code: " << GetLastError() << endl;
		cout << "Press any key to exit.";
		(void)_getch();
		return 0;
	}
	cout << "Waiting for server response" << endl;
#pragma endregion
	
#pragma region getRectangleCount
	int rectangleCount = 0;

	// читаем из именованного канала 
	if (!ReadFile(
		hNamedPipe, // дескриптор канала 
		&rectangleCount, // данные 
		sizeof(rectangleCount), // размер данных 
		&dwBytesRead, // количество записанных байтов 
		NULL)) // синхронное чтение 
	{
		// ошибка чтения 
		cerr << "Read file failed: " << endl
			<< "The last error code: " << GetLastError() << endl;
		CloseHandle(hNamedPipe);
		cout << "Press any key to exit.";
		(void)_getch();
		return 0;
	}
	// выводим полученное сообщение на консоль 
	cout << "The client received the rectangle count from a server: "
		<< endl << '\t' << rectangleCount << endl;

	// Create rectangle array
	ConsoleRectangle* rectangles = new ConsoleRectangle[rectangleCount];

	cout << "Waiting for rectangles..."<<endl;

#pragma endregion


	// push end lines
	for (int i = 0; i < 30; i++)
		cout << endl;

#pragma region getRenctangles

	HANDLE hStdOut; // дескриптор стандартного вывода 
	// читаем стандартный дескриптор вывода 
	hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

	bool stillGoing = true;
	while (stillGoing)
	{
		int rectangleIndex = 0;
		// Receive rectangle index
		if (!ReadFile(
			hNamedPipe, // дескриптор канала 
			&rectangleIndex, // данные 
			sizeof(rectangleIndex), // размер данных 
			&dwBytesRead, // количество записанных байтов 
			NULL)) // синхронное чтение 
		{
			// ошибка чтения 
			cerr << "Read file failed: " << endl
				<< "The last error code: " << GetLastError() << endl;
			CloseHandle(hNamedPipe);
			cout << "Press any key to exit.";
			(void)_getch();
			return 0;
		}
		

		// Receive sr
		if (!ReadFile(
			hNamedPipe, // дескриптор канала 
			&rectangles[rectangleIndex - 1].sr, // данные 
			sizeof(rectangles[rectangleIndex - 1].sr), // размер данных 
			&dwBytesRead, // количество записанных байтов 
			NULL)) // синхронное чтение 
		{
			// ошибка чтения 
			cerr << "Read file failed: " << endl
				<< "The last error code: " << GetLastError() << endl;
			CloseHandle(hNamedPipe);
			cout << "Press any key to exit.";
			(void)_getch();
			return 0;
		}

		// Receive color
		if (!ReadFile(
			hNamedPipe, // дескриптор канала 
			&rectangles[rectangleIndex - 1].hexColor, // данные 
			sizeof(rectangles[rectangleIndex - 1].hexColor), // размер данных 
			&dwBytesRead, // количество записанных байтов 
			NULL)) // синхронное чтение 
		{
			// ошибка чтения 
			cerr << "Read file failed: " << endl
				<< "The last error code: " << GetLastError() << endl;
			CloseHandle(hNamedPipe);
			cout << "Press any key to exit.";
			(void)_getch();
			return 0;
		}

		// draw
		rectangles[rectangleIndex - 1].setRectangleData1( hStdOut, rectangleIndex-1);

#pragma region continue

		// Read if to continue
		if (!ReadFile(
			hNamedPipe, // дескриптор канала 
			&stillGoing, // данные 
			sizeof(stillGoing), // размер данных 
			&dwBytesRead, // количество записанных байтов 
			NULL)) // синхронное чтение 
		{
			// ошибка чтения 
			cerr << "Read file failed: " << endl
				<< "The last error code: " << GetLastError() << endl;
			CloseHandle(hNamedPipe);
			cout << "Press any key to exit.";
			(void)_getch();
			return 0;
		}
		if (!stillGoing)
			cout << "Received termination request from a server." << endl;
#pragma endregion

	}

#pragma endregion


	// закрываем дескриптор канала 
	CloseHandle(hNamedPipe);
	// завершаем процесс 
	cout << "Press any key to exit.";
	(void)_getch();
	return 0;
}

