#include <windows.h> 
#include <iostream> 
#include <conio.h>
#define stop (void)_getch()
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
	bool setRectangleData( int srLeft, int srTop,
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
			ci[i].Attributes = hexColor | 0x0080 ;
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

// This function returns clicked rectangle index; -1 if none
static int whatRectangleWasClicked(int mouseX, int mouseY, ConsoleRectangle* rectArray, int rectCount)
{
	for (int i = rectCount-1 ; i >= 0; i--)
	{
		if (mouseX >= rectArray[i].sr.Left && mouseX <= rectArray[i].sr.Right)
			if (mouseY >= rectArray[i].sr.Top && mouseY <= rectArray[i].sr.Bottom)
				return i+1;
	}
	return -1;
}

int main()
{

#pragma region getReadyToSend
	HANDLE hNamedPipe;
	DWORD dwBytesRead; // ��� ���������� ����������� ������ 
	DWORD dwBytesWrite; // ��� ���������� ���������� ������ 
	char pchMessage[80]; // ��� ��������� 
	int nMessageLength; // ����� ��������� 

	wchar_t pipeName[] = L"\\\\.\\pipe\\demo_pipe";

	// ������� ����������� ����� ��� ������ � ������ 
	hNamedPipe = CreateNamedPipe(
		pipeName, // ��� ������ 
		PIPE_ACCESS_DUPLEX, // ������ �� ������ � ����� � ����� 
		PIPE_TYPE_MESSAGE | PIPE_WAIT, // ���������� �������� ��������� 
		1, // ������������ ���������� ����������� ������ 
		0, // ������ ��������� ������ �� ��������� 
		0, // ������ �������� ������ �� ��������� 
		INFINITE, // ������ ���� ����� ���������� ����� 
		NULL // ������������ �� ��������� 
	);
	// ��������� �� �������� �������� 
	if (hNamedPipe == INVALID_HANDLE_VALUE)
	{
		cerr << "Create named pipe failed." << endl
			<< "The last error code: " << GetLastError() << endl;
		cout << "Press any key to exit.";
		stop;
		return 0;
	}

	// ����, ���� ������ �������� � ������� 
	cout << "The server is waiting for connection with a client." << endl;
	if (!ConnectNamedPipe(
		hNamedPipe, // ���������� ������ 
		NULL // ����� ���������� 
	))
	{
		cerr << "Connect named pipe failed." << endl
			<< "The last error code: " << GetLastError() << endl;
		CloseHandle(hNamedPipe);
		cout << "Press any key to exit.";
		stop;
		return 0;
	}
#pragma endregion

#pragma region rectanglesDrawing


	cout << "Rectangles width <= 80, height <= 25. Sizes are input manually." << endl;

	HANDLE hStdOut; // ���������� ������������ ������ 
	// ������ ����������� ���������� ������ 
	hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

	int rectangleCount = 0;
	cout << "Enter number of rectangles (small number): ";
	cin >> rectangleCount;


#pragma region sendRectangleCount

	// �������� ������� 
	if (!WriteFile(
		hNamedPipe, // ���������� ������ 
		&rectangleCount, // ���������� ���������������
		sizeof(rectangleCount), // ���������� ������������ ������ 
		&dwBytesWrite, // ���������� ���������� ������ 
		NULL // �������� ������ ���������� 
	))
	{
		cerr << "Write file failed." << endl
			<< "The last error code: " << GetLastError() << endl;
		CloseHandle(hNamedPipe);
		cout << "Press any key to exit.";
		stop;
		return 0;
	}
	// ������� ��������� ������� ��������� �� ������� 
	cout << "The server sent the rectangle count to a client: "
		<< endl << '\t' << rectangleCount << endl;
#pragma endregion


	ConsoleRectangle* rectangles = new ConsoleRectangle[rectangleCount];

	for (int i = 0; i < rectangleCount; i++)
	{
		int srLeft, srTop, srRight, srBottom;
		char color[] = "123456789123456789";
		cout << endl<< "	RECTANGLE " << i+1 << endl << endl;
		
		cout << "Input left coordinate to write: ";
		cin >> srLeft;
		cout << "Input top coordinate to write: ";
		cin >> srTop;
		cout << "Input right coordinate to write: ";
		cin >> srRight;
		cout << "Input down coordinate to write: ";
		cin >> srBottom;

		cout << "Input color (red, green, blue, white, yellow, cyan, magenta): ";
		cin >> color;

		if (!rectangles[i].setRectangleData(srLeft, srTop,
			srRight, srBottom, color, hStdOut, i))
		{

			cerr << "Write console output failed." << endl;
			cout << "Press any key to exit. ";
			(void)_getch();
			return 1;
		}
	
	
	}
#pragma endregion

#pragma region sendRectangles
	cout << endl << endl;

	int rectanglesLeft = rectangleCount;
	bool stillGoing = true;

#pragma region mouseClickVars
	HANDLE In = GetStdHandle(STD_INPUT_HANDLE);
	DWORD OldMode;
	GetConsoleMode(In, &OldMode);
	SetConsoleMode(In, ENABLE_MOUSE_INPUT);
	INPUT_RECORD InBuf[512];
#pragma endregion
	cout << "Click on rectangles to send them to client" << endl;
	while (stillGoing)
	{
		int rectangleIndex = 0;

#pragma region mouseClick
		
		bool Flag = true;
		while (Flag)
		{
			DWORD NumRec;
			ReadConsoleInput(In, InBuf, 16, &NumRec);

			for (DWORD i = 0; i < NumRec; i++)
			{
				if (InBuf[i].EventType == MOUSE_EVENT)
				{
					switch (InBuf[i].Event.MouseEvent.dwEventFlags)
					{
					case 0: // CLICK

						if (InBuf[i].Event.MouseEvent.dwButtonState = FROM_LEFT_1ST_BUTTON_PRESSED &&
							(GetAsyncKeyState(VK_LBUTTON) & 0x8000))
						{
							COORD coord = InBuf[i].Event.MouseEvent.dwMousePosition;
							rectangleIndex = whatRectangleWasClicked(coord.X, coord.Y, rectangles, rectangleCount);
							if (rectangleIndex != -1)
								Flag = false;

						}
						break;
					}
				}
			}
		}

#pragma endregion

		
#pragma region afterGettingIndex
		// send rectangle index
		if (!WriteFile(
			hNamedPipe, // ���������� ������ 
			&rectangleIndex,
			sizeof(rectangleIndex), // ���������� ������������ ������ 
			&dwBytesWrite, // ���������� ���������� ������ 
			NULL // �������� ������ ���������� 
		))
		{
			cerr << "Write file failed." << endl
				<< "The last error code: " << GetLastError() << endl;
			CloseHandle(hNamedPipe);
			cout << "Press any key to exit.";
			stop;
			return 0;
		}
		// ������� ��������� ������� ��������� �� ������� 
		cout << "The server sent the rectangle index to a client:" << "\t" << rectangleIndex << endl;

		
		// send sr
		if (!WriteFile(
			hNamedPipe, // ���������� ������ 
			&rectangles[rectangleIndex - 1].sr,
			sizeof(rectangles[rectangleIndex - 1].sr), // ���������� ������������ ������ 
			&dwBytesWrite, // ���������� ���������� ������ 
			NULL // �������� ������ ���������� 
		))
		{
			cerr << "Write file failed." << endl
				<< "The last error code: " << GetLastError() << endl;
			CloseHandle(hNamedPipe);
			cout << "Press any key to exit.";
			stop;
			return 0;
		}

		// send color
		if (!WriteFile(
			hNamedPipe, // ���������� ������ 
			&rectangles[rectangleIndex - 1].hexColor,
			sizeof(rectangles[rectangleIndex - 1].hexColor), // ���������� ������������ ������ 
			&dwBytesWrite, // ���������� ���������� ������ 
			NULL // �������� ������ ���������� 
		))
		{
			cerr << "Write file failed." << endl
				<< "The last error code: " << GetLastError() << endl;
			CloseHandle(hNamedPipe);
			cout << "Press any key to exit.";
			stop;
			return 0;
		}



		rectangles[rectangleIndex - 1].eraseRectangle(hStdOut);

		rectanglesLeft--;
		if (rectanglesLeft == 0)
		{
			stillGoing = 0;
		}


		// send if to continue
		if (!WriteFile(
			hNamedPipe, // ���������� ������ 
			&stillGoing,
			sizeof(stillGoing), // ���������� ������������ ������ 
			&dwBytesWrite, // ���������� ���������� ������ 
			NULL // �������� ������ ���������� 
		))
		{
			cerr << "Write file failed." << endl
				<< "The last error code: " << GetLastError() << endl;
			CloseHandle(hNamedPipe);
			cout << "Press any key to exit.";
			stop;
			return 0;
		}
		// ������� ��������� ������� ��������� �� ������� 
#pragma endregion

		
	}
#pragma endregion

	
	// ��������� ���������� ������ 
	CloseHandle(hNamedPipe);


	cout << "The program is finished.";
	stop;
	return 0;
}
