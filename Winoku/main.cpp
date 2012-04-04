#include <assert.h>
#include <stdlib.h>
#include <typeinfo>
#include "stdafx.h"
#include "resource.h"
#include "Board.h"
#include "Player.h"
#include "ExampleAI.h"
#include "Player1.h"
#include "PlayerDaybreak.h"
#include "Ares.h"
#include "Platypus.h"


/**** SET WHICH AI PLAY HERE ****/
#define PLAYER1AI Ares
#define PLAYER2AI Ares
/********************************/

using namespace std;

#define MAX_LOADSTRING 100

// Each player gets 3 minutes
#define PLAYER_TIME 3*60*1000	// in milliseconds

#define USE_THREADS false

// Display configuration options
#define CLOCK1_Y 10
#define CLOCK1_X 30
#define CLOCK2_Y 30
#define CLOCK2_X 30
#define DIRECTIONS_X 30
#define DIRECTIONS_Y 60
#define GRID_X 30
#define GRID_Y 80
#define GRID_CELL_SIZE 25 // width of each cell
#define GRID_BORDER_SIZE 2 // width of the border between cells
#define GRID_CELL_PADDING 1 // amount of white space inside each cell

#define GRID_LENGTH ((GRID_CELL_SIZE * BoardSize) + (GRID_BORDER_SIZE * (BoardSize+1)))

// Global Variables:
HINSTANCE hInst; // current instance
HWND hWnd; // current window
TCHAR szTitle[MAX_LOADSTRING]; // The title bar text
TCHAR szWindowClass[MAX_LOADSTRING]; // the main window class name

Board gameBoard;
Player *player1; // ai for player 1
Player *player2; // ai for player 2
Player *currentPlayer; // Who's current turn
Winner winner; // Has a player won or has there been a draw
bool gameRunning; // Is the game running until completion
bool thinking; // Are we waiting for a response from AI
int timeRemainingPlayer1; // in milliseconds
int timeRemainingPlayer2;

// Function forward declarations
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);
int GetTime();
void SubtractTime(Player* player, int startTime);
int SecondsRemainingForPlayer(Player *player);

void Reset() {
	gameBoard.Clear();
	timeRemainingPlayer1 = PLAYER_TIME;
	timeRemainingPlayer2 = PLAYER_TIME;
	winner = WinnerNone;
	gameRunning = false;
	thinking = false;
	player1 = new PLAYER1AI();
	player2 = new PLAYER2AI();
	currentPlayer = player1;
}

bool IsPlayer1(Player *player) {
	return player == player1;
}
bool IsPlayer1Turn() {
	return IsPlayer1(currentPlayer);
}

Piece PieceForPlayer(Player *player) {
	return (IsPlayer1(player)) ? PiecePlayer1 : PiecePlayer2;
}
Player* OtherPlayer() {
	return (IsPlayer1Turn()) ? player2 : player1;
}
void WindowNeedsDisplay() {
	InvalidateRect(hWnd, NULL, 1);
}
bool GameOver() {
	return winner != WinnerNone || timeRemainingPlayer1 < 0 || timeRemainingPlayer2 < 0;
}
Winner WinnerForPlayer(Player *player) {
	return IsPlayer1(player) ? WinnerPlayer1 : WinnerPlayer2;
}

// Play a single round of Gomoku
void PlayRound() {
	int row, col;
	thinking = true;
	WindowNeedsDisplay();

	int start = GetTime();
	currentPlayer->GetMove(row, col, SecondsRemainingForPlayer(currentPlayer));
	SubtractTime(currentPlayer, start);
	
	thinking = false;

	// Make sure this is a valid move
	assert(gameBoard.GetPiece(row, col) == PieceNone);
	gameBoard.SetPiece(row, col, PieceForPlayer(currentPlayer));

	(OtherPlayer())->OpponentDidMove(row, col);

	if (!gameBoard.IsSolved(row, col, winner)) {
		currentPlayer = OtherPlayer(); // Swap players
	}

	WindowNeedsDisplay();
}

// Play game until completion
void PlayGame() {
	gameRunning = true;
	while (gameRunning && !GameOver()) {
		PlayRound();
	}
	gameRunning = false;
}

// PLAYER TIME FUNCTIONS

int GetTime() {
	SYSTEMTIME clock;
	GetSystemTime(&clock);
	// to milliseconds
	int ms = ((clock.wHour*60 + clock.wMinute)*60 + clock.wSecond)*1000 + clock.wMilliseconds;
	return ms;
}

int SecondsRemainingForPlayer(Player *player) {
	int time = IsPlayer1(player) ? timeRemainingPlayer1 : timeRemainingPlayer2;
	return time / 1000;
}

void SubtractTime(Player* player, int start) {
	int *time = IsPlayer1(player) ? &timeRemainingPlayer1 : &timeRemainingPlayer2;
	int end = GetTime();
	assert(end >= start);
	(*time) -= (end - start);
}

// DRAWING FUNCTIONS

void DrawGridLines(HDC hdc) {
	for (int i=0; i <= BoardSize; i++) {
		// Draw vertical lines
		int x = (i * GRID_CELL_SIZE) + (GRID_BORDER_SIZE * i) + GRID_X;
		Rectangle(hdc, x, GRID_Y, x+GRID_BORDER_SIZE, GRID_Y+GRID_LENGTH);

		// Draw horizontal lines
		int y = x + (GRID_Y - GRID_X);
		Rectangle(hdc, GRID_X, y, GRID_X+GRID_LENGTH, y+GRID_BORDER_SIZE);
	}
}

void DrawPieces(HDC hdc) {
	HPEN pen = CreatePen(PS_NULL, 0, 0);
	SelectObject(hdc, pen);
	HBRUSH red = CreateSolidBrush(RGB(255,0,0));
	HBRUSH blue = CreateSolidBrush(RGB(0,0,255));
	for (int row=0; row < BoardSize; row++) {
		for (int col=0; col < BoardSize; col++) {
			int x = (col * GRID_CELL_SIZE) + (GRID_BORDER_SIZE * (col+1)) + GRID_X;
			int y = (row * GRID_CELL_SIZE) + (GRID_BORDER_SIZE * (row+1)) + GRID_Y;
			int left = x + GRID_CELL_PADDING;
			int top = y + GRID_CELL_PADDING;
			int right = x + GRID_CELL_SIZE - GRID_CELL_PADDING + 1; // Why +1? Dunno. Windows!
			int bottom = y + GRID_CELL_SIZE - GRID_CELL_PADDING + 1;
			Piece piece = gameBoard.GetPiece(row, col);
			if (piece == PiecePlayer1) {
				SelectObject(hdc, red);
				Ellipse(hdc, left, top, right, bottom);
			} else if (piece == PiecePlayer2) {
				SelectObject(hdc, blue);
				Ellipse(hdc, left, top, right, bottom);
			}
		}
	}
	DeleteObject(red);
	DeleteObject(blue);
	DeleteObject(pen);
}

string FormatTimeRemaining(Player *player) {
	int totalSeconds = SecondsRemainingForPlayer(player);
	int minutes = totalSeconds / 60;
	int seconds = totalSeconds % 60;
	if (minutes > 9) {
		minutes = 9;
		seconds = 59;
	}
	char buffer[5];
	buffer[0] = '0' + minutes;
	buffer[1] = ':';
	buffer[2] = '0' + (seconds / 10);
	buffer[3] = '0' + seconds % 10;
	buffer[4] = 0;
	return string(buffer);
}

string AIName(Player *player) {
	string name;
	if (IsPlayer1(player)) name = typeid(PLAYER1AI).name();
	else name = typeid(PLAYER2AI).name();
	return name;
}

wstring ToWString(string s) {
	wstring w(s.length(), L'');
	copy(s.begin(), s.end(), w.begin());
	return w;
}

void DrawClocks(HDC hdc) {
	string clock1 = "Player1 (RED " + AIName(player1) + ") - " + FormatTimeRemaining(player1);
	string clock2 = "Player2 (BLU " + AIName(player2) + ") - " + FormatTimeRemaining(player2);
	string suffix = (thinking) ? "  ..." : "  <";
	if (IsPlayer1Turn()) clock1 += suffix;
	else clock2 += suffix;

	wstring w1 = ToWString(clock1);
	wstring w2 = ToWString(clock2);
	TextOut(hdc, CLOCK1_X, CLOCK1_Y, w1.c_str(), w1.size());
	TextOut(hdc, CLOCK2_X, CLOCK2_Y, w2.c_str(), w2.size());
}

void DrawDirections(HDC hdc) {
	wstring m;
	if (GameOver()) {
		m = L"Game over!";
		if (winner == WinnerPlayer1) m.append(L" Player1 Wins!");
		if (winner == WinnerPlayer2) m.append(L" Player2 Wins!");
		if (winner == WinnerDraw) m.append(L" Draw!");
		if (timeRemainingPlayer1 < 0) m.append(L" Player1 out of time.");
		if (timeRemainingPlayer2 < 0) m.append(L" Player2 out of time.");
	} else if (gameRunning) {
		m = L"Game is in progress...";
	} else {
		m = L"Click to advance once. Space to play out rest of game.";
	}
	TextOut(hdc, DIRECTIONS_X, DIRECTIONS_Y, m.c_str(), m.size());
}

// THREADING FUNCTIONS

DWORD WINAPI RoundThread(LPVOID lpParam) {
	// rand needs to be seeded on every thread creation
	srand(GetTime());
	PlayRound();
	return 0;
}
void StartRoundThread() {
	DWORD threadid;
	HANDLE handle;
	if (USE_THREADS) handle = CreateThread(NULL, 0, RoundThread, 0, 0, &threadid);
	else PlayRound();
}

DWORD WINAPI GameThread(LPVOID lpParam) {
	srand(GetTime());
	PlayGame();
	return 0;
}
void StartGameThread() {
	DWORD threadid;
	HANDLE handle;
	if (USE_THREADS) handle = CreateThread(NULL, 0, GameThread, 0, 0, &threadid);
	else PlayGame();
}

// WINDOWS API FUNCTIONS

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	srand(GetTime());
	Reset();

	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_WINOKU, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINOKU));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINOKU));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_WINOKU);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{

   hInst = hInstance; // Store instance handle in our global variable

   const int window_title = 30;
   const int window_border = 8;
   int width = (GRID_X * 2) + GRID_LENGTH + window_border*2;
   int height = (GRID_Y + GRID_X) + GRID_LENGTH + window_title + window_border;

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, width, height, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	RECT ClientRect;
	GetClientRect(hWnd, &ClientRect);

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		return DefWindowProc(hWnd, message, wParam, lParam);
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		DrawGridLines(hdc);
		DrawPieces(hdc);
		DrawClocks(hdc);
		DrawDirections(hdc);

		EndPaint(hWnd, &ps);
		break;
	case WM_LBUTTONDOWN:
		if (GameOver()) Reset();
		if (!gameRunning) StartRoundThread();
		break;
	case WM_KEYUP:
		if (wParam == VK_SPACE) {
			if (GameOver()) Reset();
			if (!gameRunning) StartGameThread();
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}