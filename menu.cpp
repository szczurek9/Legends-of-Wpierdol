#include "menu.h"
#include "globals.h"
#include "utils.h"
#include "battle.h"
#include "shop.h"
#include "save.h"
#include <iostream>
#include <limits>
using namespace std;

void debug_console() {
    clear_screen();
    string command;
    int value;

    cout << "=== DEBUG CONSOLE ===\n\n";
    cout << "Komendy:\n";
    cout << "money <liczba>\n";
    cout << "health <liczba>\n";
    cout << "level <liczba>\n";
    cout << "weapon <id>\n\n";
    cout << "> ";

    cin >> command >> value;

    if (command == "money") {
        if (value < 0) value = 0;
        player_money += value;
        cout << "\nDodano $" << value << endl;
    }
    else if (command == "health") {
        if (value < 1) value = 1;
        player_health = value;
        if (player_health > player_maxhealth) player_health = player_maxhealth;
        cout << "\nUstawiono HP na " << player_health << endl;
    }
    else if (command == "level") {
        if (value < 1) value = 1;
        if (value > enemy_amount) value = enemy_amount;
        player_level = value;
        cout << "\nUstawiono level na " << player_level << endl;
    }
    else if (command == "weapon") {
        if (value >= 0 && value < shop_weapon_amount) {
            player_weapon_name = shop[value].name;
            player_weapon_damage = shop[value].damage;
            cout << "\nOtrzymano bron: " << player_weapon_name << endl;
        }
        else {
            cout << "\nNiepoprawne ID broni!\n";
        }
    }
    else {
        cout << "\nNieznana komenda!\n";
    }

    cout << endl;
    cin.clear();
    pause_game();
}

void game_menu() {
    const int choice_amount = 4;
    string choices[choice_amount] = { "Graj", "Sklep", "Wzmocnienia i Umiejętności", "Opcje" };

    while (true) {
        int choice = 0;
        int keyboard_button;

        while (true) {
            clear_screen();
            cout << endl;
            cout << "	Nick: " << nickname << endl;
            cout << "	LVL: " << player_level << endl;
            cout << "	HP: " << player_health << "/" << player_maxhealth << endl;
            cout << "	Hajs: $" << player_money << endl;
            cout << "	Broń: " << player_weapon_name << " - " << player_weapon_damage << " DMG" << endl << endl;

            for (int i = 0; i < choice_amount; i++) {
                if (i == choice)
                    cout << "	> " << choices[i] << endl;
                else
                    cout << "	  " << choices[i] << endl;
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
                break;
            }
            else if (keyboard_button == '`') {
                debug_console();
            }
        }

        switch (choice) {
        case 0: game_battle(); break;
        case 1: game_shop(); break;
        case 2: game_skills(); break;
        case 3: game_options(); break;
        }
    }
}

void game_help_battle() {
    int keyboard_button;
    while (true) {
        clear_screen();
        cout << "ESC - Powrót\n\n";
        cout << "	Walka\n";
        cout << "		1. Walka w Legends of Wpierdol działa podobnie do systemu walki w Pokemonach.\n";
        cout << "		   Gracz atakuje swoją bronią a gra wylicza prawdopodobieństwo trafienia ataku:\n";
        cout << "		   Jeżeli trafi - zadaje obrażenia przeciwnikowi, jeśli nie - przeciwnikowi\n";
        cout << "		   nic się nie stanie. Potem przeciwnik atakuje w taki sam sposób.\n";
        keyboard_button = _getch();
        if (keyboard_button == 27) return;
    }
}

void game_help_shop() {
    int keyboard_button;
    while (true) {
        clear_screen();
        cout << "ESC - Powrót\n\n";
        cout << "	Sklep, Wzmocnienia i Umiejętności\n";
        cout << "		1. W Legends of Wpierdol sklep działa w taki sposób że zakup broni usuwa\n";
        cout << "		   aktualną broń z twojego ekwipunku.\n\n";
        cout << "		2. Wzmocnienia dają dodatkowe statystyki do gracza.\n";
        keyboard_button = _getch();
        if (keyboard_button == 27) return;
    }
}

void game_help() {
    const int choice_amount = 2;
    string choices[choice_amount] = { "Walka", "Sklep, Wzmocnienia i Umiejętności" };

    while (true) {
        int choice = 0;
        int keyboard_button;

        while (true) {
            clear_screen();
            cout << "ESC - Powrót\n\n";
            cout << "	Pomoc\n\n	Wybierz temat:\n\n";

            for (int i = 0; i < choice_amount; i++) {
                if (i == choice)
                    cout << "	> " << choices[i] << endl;
                else
                    cout << "	  " << choices[i] << endl;
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
            else if (keyboard_button == 27) return;
        }

        switch (choice) {
        case 0: game_help_battle(); break;
        case 1: game_help_shop(); break;
        }
    }
}

void game_credits() {
    int keyboard_button;
    while (true) {
        clear_screen();
        cout << "ESC - Powrot\n\n";
        cout << "	Twórcy\n\n";
        cout << "	Legends of Wpierdol\n\n";
        cout << "	Programowanie:\n	szczurek9\n\n";
        cout << "	Game Design:\n	szczurek9\n\n";
        cout << "	Balancing:\n	chyba szczurek9\n\n";
        cout << "	Writing:\n	też szczurek9\n\n";
        cout << "	Specjalne podziękowania:\n";
        cout << "	Owcacejk, Mispolarny1, maximum412, Toster57\n";
        cout << "	Galaxy S22\n	Akali mains\n	Valve za przycisk `\n\n";
        cout << "	Version: 1.0\n\n";
        keyboard_button = _getch();
        if (keyboard_button == 27) return;
    }
}

void game_options() {
    const int choice_amount = 4;
    string choices[choice_amount] = { "Pomoc", "Zapisz grę", "Twórcy", "Wyjście z gry" };

    while (true) {
        int choice = 0;
        int keyboard_button;

        while (true) {
            clear_screen();
            cout << "ESC - Powrót\n\n";
            cout << "	Opcje:\n\n";

            for (int i = 0; i < choice_amount; i++) {
                if (i == choice)
                    cout << "	> " << choices[i] << endl;
                else
                    cout << "	  " << choices[i] << endl;
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
            else if (keyboard_button == 27) return;
        }

        switch (choice) {
        case 0: game_help(); break;
        case 1: manual_save(); break;
        case 2: game_credits(); break;
        case 3: exit(0); break;
        }
    }
}