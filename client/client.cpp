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
	CHAR_INFO ci[80 * 25]; // �������������, �� �������� ����� �������� 
	COORD size; // ������� ����� �������������� 
	COORD coord; // ���������� ������ ���� ��������������, �� �������� ������� 
	
public:
	SMALL_RECT sr; // ���������� ������ ���� ��������������, � ������� ����� 
	int hexColor;
	// Base constructor
	ConsoleRectangle()
	{
		// ������������� ����� ���� ��������������, �� �������� ����� 
		coord.X = 0;
		coord.Y = 0;
		// ������������� ������� ��������������, ������� ����� 
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
		// ������������� ����� ���� ��������������, �� �������� ����� 
		coord.X = 0;
		coord.Y = 0;
		// ������������� ������� ��������������, ������� ����� 
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
		// ������������� ����� ���� ��������������, �� �������� ����� 
		coord.X = 0;
		coord.Y = 0;
		// ������������� ������� ��������������, ������� ����� 
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
		// ����� ������������� � ����� ������ 
		if (!WriteConsoleOutput(
			hStdOut, // ���������� ������ ������ 
			ci, // �������������, �� �������� ����� 
			size, // ������� ����� �������������� 
			coord, // � ��� ����� ���� 
			&sr)) // �������������, � ������� ����� 
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
	DWORD dwBytesRead; // ��� ���������� ����������� ������ 

	// ������ ��� ������ � ����, �� ������� �������� ������ 
	cout << "Enter a name of the server machine: ";
	wcin >> machineName;
	// ����������� ��� ������ � ��� ������ 
	wsprintf(pipeName, L"\\\\%s\\pipe\\demo_pipe", machineName);
	// ����������� � ����������� ������� 
	hNamedPipe = CreateFile(
		pipeName, // ��� ������ 
		GENERIC_READ | GENERIC_WRITE, // ������ � ���������� � ����� 
		FILE_SHARE_READ | FILE_SHARE_WRITE, // ��������� ������ � ������ 
		NULL, // ������������ �� ��������� 
		OPEN_EXISTING, // ��������� ������������ ����� 
		FILE_ATTRIBUTE_NORMAL, // �������� �� ��������� 
		NULL); // �������������� ��������� ��� 

	// ��������� ����� � ������� 
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

	// ������ �� ������������ ������ 
	if (!ReadFile(
		hNamedPipe, // ���������� ������ 
		&rectangleCount, // ������ 
		sizeof(rectangleCount), // ������ ������ 
		&dwBytesRead, // ���������� ���������� ������ 
		NULL)) // ���������� ������ 
	{
		// ������ ������ 
		cerr << "Read file failed: " << endl
			<< "The last error code: " << GetLastError() << endl;
		CloseHandle(hNamedPipe);
		cout << "Press any key to exit.";
		(void)_getch();
		return 0;
	}
	// ������� ���������� ��������� �� ������� 
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

	HANDLE hStdOut; // ���������� ������������ ������ 
	// ������ ����������� ���������� ������ 
	hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

	bool stillGoing = true;
	while (stillGoing)
	{
		int rectangleIndex = 0;
		// Receive rectangle index
		if (!ReadFile(
			hNamedPipe, // ���������� ������ 
			&rectangleIndex, // ������ 
			sizeof(rectangleIndex), // ������ ������ 
			&dwBytesRead, // ���������� ���������� ������ 
			NULL)) // ���������� ������ 
		{
			// ������ ������ 
			cerr << "Read file failed: " << endl
				<< "The last error code: " << GetLastError() << endl;
			CloseHandle(hNamedPipe);
			cout << "Press any key to exit.";
			(void)_getch();
			return 0;
		}
		

		// Receive sr
		if (!ReadFile(
			hNamedPipe, // ���������� ������ 
			&rectangles[rectangleIndex - 1].sr, // ������ 
			sizeof(rectangles[rectangleIndex - 1].sr), // ������ ������ 
			&dwBytesRead, // ���������� ���������� ������ 
			NULL)) // ���������� ������ 
		{
			// ������ ������ 
			cerr << "Read file failed: " << endl
				<< "The last error code: " << GetLastError() << endl;
			CloseHandle(hNamedPipe);
			cout << "Press any key to exit.";
			(void)_getch();
			return 0;
		}

		// Receive color
		if (!ReadFile(
			hNamedPipe, // ���������� ������ 
			&rectangles[rectangleIndex - 1].hexColor, // ������ 
			sizeof(rectangles[rectangleIndex - 1].hexColor), // ������ ������ 
			&dwBytesRead, // ���������� ���������� ������ 
			NULL)) // ���������� ������ 
		{
			// ������ ������ 
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
			hNamedPipe, // ���������� ������ 
			&stillGoing, // ������ 
			sizeof(stillGoing), // ������ ������ 
			&dwBytesRead, // ���������� ���������� ������ 
			NULL)) // ���������� ������ 
		{
			// ������ ������ 
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


	// ��������� ���������� ������ 
	CloseHandle(hNamedPipe);
	// ��������� ������� 
	cout << "Press any key to exit.";
	(void)_getch();
	return 0;
}

