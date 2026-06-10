#include <iostream>
#include <clocale>
#include <cstdlib>
#include <ctime>

#define NOMINMAX
#include <windows.h>
#include <conio.h>

#include "globals.h"
#include "utils.h"
#include "menu.h"
#include "save.h"
#include "shop.h"


void game_intro() {
    clear_screen();

    if (save_exists()) {
        const int choice_amount = 2;
        std::string choices[choice_amount] = { "Wczytaj grę", "Nowa gra" };
        int choice = 0;
        int keyboard_button;

        while (true) {
            clear_screen();
            std::cout << std::endl << "\tWykryto zapis gry!\n\n";

            for (int i = 0; i < choice_amount; i++) {
                if (i == choice) {
                    std::cout << "\t►  "; print_colored(choices[i], COLOR_BRIGHT_WHITE); std::cout << std::endl;
                }
                else
                    std::cout << "\t  " << choices[i] << std::endl;
            }

            keyboard_button = _getch();

            if (keyboard_button == 224) {
                keyboard_button = _getch();
                if (keyboard_button == 72) {
                    choice--;
                    if (choice < 0) choice = choice_amount - 1;
                }
                else if (keyboard_button == 80) {
                    choice++;
                    if (choice >= choice_amount) choice = 0;
                }
            }
            else if (keyboard_button == 13) {
                if (choice == 0) {
                    if (load_game()) {
                        // Przelicz statystyki magiczne po wczytaniu
                        recalculate_magic_stats();
                        clear_screen();
                        std::cout << std::endl << "\tGra wczytana!\n\n";
                        pause_game();
                        game_menu();
                        return;
                    }
                }
                else if (choice == 1) {
                    remove("save.LoW_save");
                    break;
                }
            }
        }
    }

    remove("save.sav");
    clear_screen();
    std::cout << std::endl << "\tPodaj swój nick: " << std::endl;
    std::cin >> nickname;

    clear_screen();
    std::cout << std::endl;
    std::cout << "\tWitaj " << nickname << "! Witaj w grze Legends of Wpierdol!" << std::endl;
    std::cout << "\tW skrócie:\n";
    std::cout << "\t\t- napierdalasz się z różnymi przeciwnikami\n";
    std::cout << "\t\t- za zdobyte pieniądze kupujesz różne bronie\n";
    std::cout << "\t\t- resztę ogarniesz sam :)\n\n";
    pause_game();

    // ===== WYBÓR KLASY POSTACI =====
    choose_player_class();
    game_menu();
}

int main() {

    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    setlocale(LC_ALL, ".UTF8");

    SetConsoleTitleA("Legends of Wpierdol | by szczurek9");

    srand(time(NULL));

    const int choice_amount = 2;
    std::string choices[choice_amount] = { "Graj", "Wyjdź" };
    int choice = 0;
    int keyboard_button;

    while (true) {
        clear_screen();
        std::cout << std::endl << "\tLegends of Wpierdol\n\n";

        for (int i = 0; i < choice_amount; i++) {
            if (i == choice) {
                std::cout << "\t►  "; print_colored(choices[i], COLOR_BRIGHT_WHITE); std::cout << std::endl;
            }
            else
                std::cout << "\t  " << choices[i] << std::endl;
        }

        keyboard_button = _getch();

        if (keyboard_button == 224) {
            keyboard_button = _getch();
            if (keyboard_button == 72) {
                choice--;
                if (choice < 0) choice = choice_amount - 1;
            }
            else if (keyboard_button == 80) {
                choice++;
                if (choice >= choice_amount) choice = 0;
            }
        }
        else if (keyboard_button == 13) break;
    }

    switch (choice) {
    case 0: game_intro(); break;
    case 1: exit(0); break;
    }

    return 0;
}
