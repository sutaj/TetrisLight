/***
 *     ▄▄▄       ██ ▄█▀ ██▓ ██▓
 *    ▒████▄     ██▄█▒ ▓██▒▓██▒
 *    ▒██  ▀█▄  ▓███▄░ ▒██▒▒██░
 *    ░██▄▄▄▄██ ▓██ █▄ ░██░▒██░
 *     ▓█   ▓██▒▒██▒ █▄░██░░██████▒
 *     ▒▒   ▓▒█░▒ ▒▒ ▓▒░▓  ░ ▒░▓  ░
 *      ▒   ▒▒ ░░ ░▒ ▒░ ▒ ░░ ░ ▒  ░
 *      ░   ▒   ░ ░░ ░  ▒ ░  ░ ░
 *          ░  ░░  ░    ░      ░  ░
 *
 *     For code updates visit repository on https://github.com/sutaj
 */

// code is NOT refactored... for your ease of use.
// to compile on Linux and other machines, just deal with Windows.h include and those few lines we take from it. It's just mendling with console window...
// I will do this later...

#include <stdio.h>
#include <Windows.h>
#include <thread>
#include <vector>
#include <iostream>
using namespace std;

// canvas - our space to work with - for standard console window 80x120 chars
int nCanvasHeight = 80;
int nCanvasWidth = 120;
// board
int nBoardOffset = 2;					// position whole game to side and down - by 2 chars in this case
int nBoardWidth = 16;					// game board width
int nBoardHeight = 22;					// game board height
unsigned char* pBoard = nullptr;		// our board pointer
// our tetris blocks (tetramino or something like that) array
wstring tetrisBlock[7];
// game is running flag
bool bIsRunning = true;
// game vars
int nCurrentBlock = rand() % 7;			// draw 1st block
int nCurrentRotation = rand() % 4;		// rotate it... at random too
int nCurrentX = (nBoardWidth / 2) - 2;	// current tetris block position - we start on top-middle
int nCurrentY = 0;						// same but on y pos
int nNextBlock = rand() % 7;			// what is next block - so we can show this on right side
bool bKey[4];							// keys array to control game
bool bIsRotating = false;				// is tetris block rotating right now flag (so it won't spin like crazy - it's in console after all)
int nGameSpeed = 30;					// current game speed 30 - slow 1 - super fast
int nSCounter = 0;						// speed counter used for speeding up game 
bool bPushDw = false;					// push down block flag
int nBlockCount = 0;					// block counter helping us count speed inceremting
vector<int> vBoardLines;				// lines array 
int nGameScore = 0;						// score... duh
int nCombo = 1;							// combo multiplyer... remember about divided by 0 :]

// rotate tetris block
int Rotation(int x, int y, int a) {
	switch (a % 4)
	{
	case 0: return y * 4 + x;
	case 1: return 12 + y - (x * 4);
	case 2: return 15 - (y * 4) - x;
	case 3: return 3 - y + (x * 4);
	}
	return 0;
}

// check if block can fit in place
bool isFit(int block, int rotation, int blockx, int blocky) {
	for (int x = 0; x < 4; x++)
	{
		for (int y = 0; y < 4; y++)
		{
			int indx = Rotation(x, y, rotation);
			int findx = (blocky + y) * nBoardWidth + (blockx + x);

			if (blockx + x >= 0 && blockx + x < nBoardWidth) {
				if (blocky + y >= 0 && blocky + y < nBoardHeight) {
					if (tetrisBlock[block][indx] == L'X' && pBoard[findx] != 0) {
						return false;
					}
				}
			}
		}
	}

	return true;
}

// main entry point
int main()
{
	// prep up
	HWND console = GetConsoleWindow();
	RECT ConsoleRect;
	GetWindowRect(console, &ConsoleRect);
	SetWindowText(console, L"TetrisLight :: https://github.com/sutaj/");
	pBoard = new unsigned char[nBoardHeight * nBoardWidth];

	// blocks - or our game "assets"
	// i'm leaving in this form, so you can see what this is :)
	tetrisBlock[0].append(L"..X.");
	tetrisBlock[0].append(L"..X.");
	tetrisBlock[0].append(L"..X.");
	tetrisBlock[0].append(L"..X.");

	tetrisBlock[1].append(L"..X.");
	tetrisBlock[1].append(L".XX.");
	tetrisBlock[1].append(L".X..");
	tetrisBlock[1].append(L"....");

	tetrisBlock[2].append(L".X..");
	tetrisBlock[2].append(L".XX.");
	tetrisBlock[2].append(L"..X.");
	tetrisBlock[2].append(L"....");

	tetrisBlock[3].append(L".XX.");
	tetrisBlock[3].append(L".XX.");
	tetrisBlock[3].append(L"....");
	tetrisBlock[3].append(L"....");

	tetrisBlock[4].append(L"..X.");
	tetrisBlock[4].append(L".XX.");
	tetrisBlock[4].append(L"..X.");
	tetrisBlock[4].append(L"....");

	tetrisBlock[5].append(L".XX.");
	tetrisBlock[5].append(L"..X.");
	tetrisBlock[5].append(L"..X.");
	tetrisBlock[5].append(L"....");

	tetrisBlock[6].append(L".XX.");
	tetrisBlock[6].append(L".X..");
	tetrisBlock[6].append(L".X..");
	tetrisBlock[6].append(L"....");


	for (int x = 0; x < nBoardWidth; x++)
	{
		for (int y = 0; y < nBoardHeight; y++)
		{
			if (x == 0 || x == nBoardWidth - 1 || y == nBoardHeight - 1) {
				pBoard[y * nBoardWidth + x] = 9;
			}else
			{
				pBoard[y * nBoardWidth + x] = 0;
			}
		}
	}

	wchar_t* screen = new wchar_t[nCanvasHeight * nCanvasWidth];
	for (int i = 0; i < nCanvasHeight * nCanvasWidth; i++)
	{
		screen[i] = L' ';
	}

	HANDLE hWindow = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hWindow);
	DWORD dwBW = 0;


	// main game loop
	while (bIsRunning)
	{

		// timing
		this_thread::sleep_for(45ms);
		nSCounter++;
		bPushDw = (nSCounter == nGameSpeed);


		// next
		swprintf_s(&screen[(nBoardOffset + 6) * nCanvasWidth + nBoardWidth + 6], 12, L"next piece:");
		for (int x = 0; x < 4; x++)
		{
			for (int y = 0; y < 4; y++) {
				if (tetrisBlock[nNextBlock][Rotation(x, y, 0)] == L'X') {
					screen[(nBoardOffset + 8 + y) * nCanvasWidth + nBoardWidth + 9 + x] = L'▒';
				}
				else
				{
					screen[(nBoardOffset + 8 + y) * nCanvasWidth + nBoardWidth + 9 + x] = L' ';
				}
			}
		}

		// deal with user input
		for (int key = 0; key < 4; key++)
		{
			// bKey[key] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28\x10"[key]))) != 0;  // <-- switch with SHIFT
			bKey[key] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28\x20"[key]))) != 0;  // <-- switch witch space
		}

		nCurrentX += (bKey[0] && isFit(nCurrentBlock, nCurrentRotation, nCurrentX + 1, nCurrentY)) ? 1 : 0;
		nCurrentX -= (bKey[1] && isFit(nCurrentBlock, nCurrentRotation, nCurrentX - 1, nCurrentY)) ? 1 : 0;
		nCurrentY += (bKey[2] && isFit(nCurrentBlock, nCurrentRotation, nCurrentX, nCurrentY + 1)) ? 1 : 0;
		if (bKey[3])
		{
			nCurrentRotation += (bIsRotating && isFit(nCurrentBlock, nCurrentRotation + 1, nCurrentX, nCurrentY)) ? 1 : 0;
			bIsRotating = false;
		}
		else {
			bIsRotating = true;
		}

		if (bPushDw) {
			nSCounter = 0;

			if (isFit(nCurrentBlock, nCurrentRotation, nCurrentX, nCurrentY + 1)) {
				nCurrentY++;
			}
			else
			{
				for (int x = 0; x < 4; x++)
				{
					for (int y = 0; y < 4; y++)
					{
						if (tetrisBlock[nCurrentBlock][Rotation(x, y, nCurrentRotation)] == L'X') {
							pBoard[(nCurrentY + y) * nBoardWidth + (nCurrentX + x)] = nCurrentBlock + 1;
						}

					}
				}

				nBlockCount++;
				if (nBlockCount % 20 == 0)
				{
					if (nGameSpeed > 1)
					{
						nGameSpeed--;
					}
					else
					{
						nGameSpeed = 1;
					}
				}

				for (int y = 0; y < 4; y++)
				{
					if (nCurrentY + y < nBoardHeight - 1) {
						bool bLine = true;
						for (int x = 1; x < nBoardWidth - 1; x++) {
							bLine &= (pBoard[(nCurrentY + y) * nBoardWidth + x]) != 0;
						}

						if (bLine)
						{
							for (int x = 1; x < nBoardWidth - 1; x++)
							{
								pBoard[(nCurrentY + y) * nBoardWidth + x] = 8;
							}

							vBoardLines.push_back(nCurrentY + y);
						}

					}
				}

				// score up player
				nGameScore += 5 * nCombo;
				if (!vBoardLines.empty())
				{
					nGameScore += (1 << vBoardLines.size()) * 100;
				}
				else
				{
					nGameScore += 5 * nCombo;
					nCombo = 1;
				}


				nCurrentX = (nBoardWidth / 2) - 2;
				nCurrentY = 0;
				nCurrentRotation = rand() % 4; // randomise rotation for more fun
				nCurrentBlock = nNextBlock;

				bIsRunning = isFit(nCurrentBlock, nCurrentRotation, nCurrentX, nCurrentY);

				nNextBlock = rand() % 7;
			}
		}


		for (int x = 0; x < nBoardWidth; x++)
		{
			for (int y = 0; y < nBoardHeight; y++)
			{
				screen[(y + nBoardOffset) * nCanvasWidth + (x + nBoardOffset)] = L" ▓▓▓▓▓▓▓░█"[pBoard[y * nBoardWidth + x]];
			}
		}

		// update ui text
		swprintf_s(&screen[nBoardOffset * nCanvasWidth + nBoardWidth + 6], 16, L"SCORE: %8d", nGameScore);
		swprintf_s(&screen[(nBoardOffset + 1) * nCanvasWidth + nBoardWidth + 5], 18, L"─────────────────");
		if (nGameSpeed == 1) {
			swprintf_s(&screen[(nBoardOffset + 2) * nCanvasWidth + nBoardWidth + 6], 16, L"Speed:      MAX");
		}
		else
		{
			swprintf_s(&screen[(nBoardOffset + 2) * nCanvasWidth + nBoardWidth + 6], 16, L"Speed: %8d", 31 - nGameSpeed);
		}
		swprintf_s(&screen[(nBoardOffset + 3) * nCanvasWidth + nBoardWidth + 6], 17, L"Combo: %8dX", nCombo);


		// draw a block
		for (int x = 0; x < 4; x++)
		{
			for (int y = 0; y < 4; y++) {
				if (tetrisBlock[nCurrentBlock][Rotation(x, y, nCurrentRotation)] == L'X') {
					screen[(nCurrentY + y + nBoardOffset) * nCanvasWidth + (nCurrentX + x + nBoardOffset)] = L'▓';
				}
			}
		}

		// woooosh....
		if (!vBoardLines.empty())
		{
			WriteConsoleOutputCharacter(hWindow, screen, nCanvasHeight * nCanvasWidth, { 0,0 }, & dwBW);
			this_thread::sleep_for(400ms);
			for (auto &v : vBoardLines)
			{
				for (int x = 1; x < nBoardWidth - 1; x++)
				{
					for (int y = v; y > 0; y--) {
						pBoard[y * nBoardWidth + x] = pBoard[(y - 1) * nBoardWidth + x];
					}
					pBoard[x] = 0;
				}
			}
			vBoardLines.clear();
			nCombo++;
		}
		
		WriteConsoleOutputCharacter(hWindow, screen, nCanvasHeight * nCanvasWidth, { 0, 0 }, &dwBW);
	}

	// GAME OVER!
	CloseHandle(hWindow);

	cout << "\t   :'######::::::'###::::'##::::'##:'########:\r\n";
	cout << "\t   '##... ##::::'## ##::: ###::'###: ##.....::\r\n";
	cout << "\t    ##:::..::::'##:. ##:: ####'####: ##:::::::\r\n";
	cout << "\t    ##::'####:'##:::. ##: ## ### ##: ######:::\r\n";
	cout << "\t    ##::: ##:: #########: ##. #: ##: ##...::::\r\n";
	cout << "\t    ##::: ##:: ##.... ##: ##:.:: ##: ##:::::::\r\n";
	cout << "\t   . ######::: ##:::: ##: ##:::: ##: ########:\r\n";
	cout << "\t   :......::::..:::::..::..:::::..::........::\r\n";
	cout << "\t   :'#######::'##::::'##:'########:'########::\r\n";
	cout << "\t   '##.... ##: ##:::: ##: ##.....:: ##.... ##:\r\n";
	cout << "\t    ##:::: ##: ##:::: ##: ##::::::: ##:::: ##:\r\n";
	cout << "\t    ##:::: ##: ##:::: ##: ######::: ########::\r\n";
	cout << "\t    ##:::: ##:. ##:: ##:: ##...:::: ##.. ##:::\r\n";
	cout << "\t    ##:::: ##::. ## ##::: ##::::::: ##::. ##::\r\n";
	cout << "\t   . #######::::. ###:::: ########: ##:::. ##:\r\n";
	cout << "\t   :.......::::::...:::::........::..:::::..::\r\n";


	printf("\r\n\t\t\tYour score %d8.\r\n\r\n\t\t", nGameScore);
	this_thread::sleep_for(750ms);
	system("pause");
	return 0;
}



