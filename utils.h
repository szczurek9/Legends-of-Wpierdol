#pragma once

#define NOMINMAX
#include <iostream>
#include <conio.h>
#include <Windows.h>


static void set_color(int color)
{

	SetConsoleTextAttribute(
		GetStdHandle(STD_OUTPUT_HANDLE),
		color);
}

static void reset_color()
{

	set_color(COLOR_WHITE);
}

static void print_colored(std::string text, int color)
{

	set_color(color);
	std::cout << text;
	reset_color();
}

static void number_colored(int text, int color)
{

	set_color(color);
	std::cout << text;
	reset_color();
}

static void ui_colored(std::string text, int number, int color)
{

	set_color(color);
	std::cout << text << number;
	reset_color();
}

static void r_ui_colored(int number, std::string text, int color)
{

	set_color(color);
	std::cout << number << text;
	reset_color();
}

static void clear_screen()
{

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD count;
	DWORD cellCount;

	COORD homeCoords = { 0, 0 };

	if (hConsole == INVALID_HANDLE_VALUE)
		return;

	if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
		return;

	cellCount = csbi.dwSize.X * csbi.dwSize.Y;

	FillConsoleOutputCharacter(
		hConsole,
		' ',
		cellCount,
		homeCoords,
		&count);

	FillConsoleOutputAttribute(
		hConsole,
		csbi.wAttributes,
		cellCount,
		homeCoords,
		&count);

	SetConsoleCursorPosition(hConsole, homeCoords);
}

static void flush_input()
{
	FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
}

static void pause_game()
{
	flush_input();
	std::cout << "\n\tNaciśnij dowolny przycisk...";
	_getch();
}

static void draw_enemy_hp_bar(int current, int maximum, int width = 20)
{

	float ratio = (float)current / maximum;
	int filled = (int)(ratio * width);

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	print_colored(" ♥ ", COLOR_RED);

	number_colored(current, COLOR_RED);

	std::cout << "/";

	number_colored(maximum, COLOR_RED);

	std::cout << " [";

	for (int i = 0; i < width; i++)
	{

		if (i < filled)
		{

			if (ratio > 0.6f)
				SetConsoleTextAttribute(hConsole, 10);
			else if (ratio > 0.3f)
				SetConsoleTextAttribute(hConsole, 14);
			else
				SetConsoleTextAttribute(hConsole, 12);

			std::cout << "█";
		}
		else
		{

			SetConsoleTextAttribute(hConsole, 8);
			std::cout << "░";
		}
	}

	SetConsoleTextAttribute(hConsole, 7);

	std::cout << "] ";
}

static void draw_player_hp_bar(int current, int maximum, int width = 20)
{

	float ratio = (float)current / maximum;
	int filled = (int)(ratio * width);

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	print_colored(" ♥ [", COLOR_GREEN);

	for (int i = 0; i < width; i++)
	{

		if (i < filled)
		{

			if (ratio > 0.6f)
				SetConsoleTextAttribute(hConsole, 10);
			else if (ratio > 0.3f)
				SetConsoleTextAttribute(hConsole, 14);
			else
				SetConsoleTextAttribute(hConsole, 12);

			std::cout << "█";
		}
		else
		{

			SetConsoleTextAttribute(hConsole, 8);
			std::cout << "░";
		}
	}

	SetConsoleTextAttribute(hConsole, 7);

	print_colored("] ", COLOR_GREEN);
	number_colored(current, COLOR_GREEN);

	std::cout << "/";

	number_colored(maximum, COLOR_GREEN);
}

static void draw_player_mana_bar(int current, int maximum, int width = 20)
{

	float ratio = (float)current / maximum;
	int filled = (int)(ratio * width);

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	print_colored("◆ [", COLOR_BLUE);

	for (int i = 0; i < width; i++)
	{

		if (i < filled)
		{
			SetConsoleTextAttribute(hConsole, COLOR_BLUE);
			std::cout << "█";
		}
		else
		{
			SetConsoleTextAttribute(hConsole, 8);
			std::cout << "░";
		}
	}

	SetConsoleTextAttribute(hConsole, 7);

	print_colored("] ", COLOR_BLUE);

	number_colored(current, COLOR_BLUE);

	std::cout << "/";

	number_colored(maximum, COLOR_BLUE);
}