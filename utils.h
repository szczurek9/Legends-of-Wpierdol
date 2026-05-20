#pragma once

#include <iostream>
#include <conio.h>
#include <windows.h>

using namespace std;

static void clear_screen() {

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
        &count
    );

    FillConsoleOutputAttribute(
        hConsole,
        csbi.wAttributes,
        cellCount,
        homeCoords,
        &count
    );

    SetConsoleCursorPosition(hConsole, homeCoords);
}

static void pause_game() {

    cout << "\n\tNaciśnij dowolny przycisk...";
    _getch();
}

static void draw_enemy_hp_bar(int current, int maximum, int width = 20) {

    float ratio = (float)current / maximum;
    int filled = (int)(ratio * width);

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    cout << " ♥ ";
    cout << current << "/" << maximum;
    cout << " [";

    for (int i = 0; i < width; i++) {

        if (i < filled) {

            if (ratio > 0.6f)
                SetConsoleTextAttribute(hConsole, 10);
            else if (ratio > 0.3f)
                SetConsoleTextAttribute(hConsole, 14);
            else
                SetConsoleTextAttribute(hConsole, 12);

            cout << "█";
        }
        else {

            SetConsoleTextAttribute(hConsole, 8);
            cout << "░";
        }
    }

    SetConsoleTextAttribute(hConsole, 7);

    cout << "] ";
}

static void draw_player_hp_bar(int current, int maximum, int width = 20) {

    float ratio = (float)current / maximum;
    int filled = (int)(ratio * width);

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    cout << " ♥ [";

    for (int i = 0; i < width; i++) {

        if (i < filled) {

            if (ratio > 0.6f)
                SetConsoleTextAttribute(hConsole, 10);
            else if (ratio > 0.3f)
                SetConsoleTextAttribute(hConsole, 14);
            else
                SetConsoleTextAttribute(hConsole, 12);

            cout << "█";
        }
        else {

            SetConsoleTextAttribute(hConsole, 8);
            cout << "░";
        }
    }

    SetConsoleTextAttribute(hConsole, 7);

    cout << "] ";
    cout << current << "/" << maximum;
}