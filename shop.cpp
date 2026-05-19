#include "shop.h"
#include "globals.h"
#include "utils.h"
#include <iostream>
using namespace std;

void shop_buy_weapon(int id) {
    clear_screen();
    cout << endl;
    cout << "	Wybrałeś: " << shop[id].name << endl;
    cout << "	Cena: $" << shop[id].price << endl;

    if (player_money >= shop[id].price) {
        player_money -= shop[id].price;
        player_weapon_name = shop[id].name;
        player_weapon_damage = shop[id].damage;
        cout << endl << "	Kupiono!" << endl << endl;
        pause_game();
    }
    else {
        cout << endl << "Za mało hajsu!" << endl << endl;
        pause_game();
    }
}

void game_shop() {
    int choice = 0;
    int keyboard_button;

    while (true) {
        clear_screen();
        cout << "ESC - Powrót\n\n";
        cout << "	Sklep:\n\n";
        cout << "	Hajs: $" << player_money << endl;
        cout << "	Aktualna Broń: " << player_weapon_name << " - " << player_weapon_damage << " DMG" << endl << endl;

        for (int i = 0; i < shop_weapon_amount; i++) {
            if (i == choice)
                cout << "	> " << shop[i].damage << " DMG | $" << shop[i].price << " - " << shop[i].name << endl;
            else
                cout << "	  " << shop[i].damage << " DMG | $" << shop[i].price << " - " << shop[i].name << endl;
        }

        keyboard_button = _getch();

        if (keyboard_button == 224) {
            keyboard_button = _getch();
            if (keyboard_button == 72) {
                choice--;
                if (choice < 0) choice = shop_weapon_amount - 1;
            }
            else if (keyboard_button == 80) {
                choice++;
                if (choice >= shop_weapon_amount) choice = 0;
            }
        }
        else if (keyboard_button == 13) {
            shop_buy_weapon(choice);
        }
        else if (keyboard_button == 27) {
            return;
        }
    }
}

void game_skills() {
    int choice = 0;
    int keyboard_button;

    while (true) {
        clear_screen();
        cout << "ESC - Powrot\n\n";
        cout << "	Wzmocnienia i Umiejetnosci\n\n";
        cout << "	Hajs: $" << player_money << endl << endl;

        for (int i = 0; i < skills_amount; i++) {
            if (i == choice)
                cout << "	> " << skills[i].name << " | $" << skills[i].price << endl;
            else
                cout << "	  " << skills[i].name << " | $" << skills[i].price << endl;
            cout << "	   " << skills[i].description << endl;
        }

        keyboard_button = _getch();

        if (keyboard_button == 224) {
            keyboard_button = _getch();
            if (keyboard_button == 72) {
                choice--;
                if (choice < 0) choice = skills_amount - 1;
            }
            else if (keyboard_button == 80) {
                choice++;
                if (choice >= skills_amount) choice = 0;
            }
        }
        else if (keyboard_button == 13) {
            if (player_money >= skills[choice].price) {
                switch (choice) {
                case 0:
                    if (player_lifesteal >= MAX_LIFESTEAL) {
                        clear_screen();
                        cout << endl << "	Osiagnieto maksymalny poziom Wampirycznego Ostrza!\n" << endl;
                        pause_game();
                        continue;
                    }
                    player_lifesteal += 10;
                    if (player_lifesteal > MAX_LIFESTEAL) player_lifesteal = MAX_LIFESTEAL;
                    break;
                case 1:
                    player_bonus_health += 50;
                    player_maxhealth += 50;
                    player_health += 50;
                    break;
                case 2:
                    player_bonus_health += 210;
                    player_maxhealth += 210;
                    player_health += 210;
                    break;
                case 3:
                    if (player_bonus_accuracy >= MAX_ACCURACY) {
                        clear_screen();
                        cout << endl << "	Osiagnieto maksymalną ilość Kryształów Skupienia!\n" << endl;
                        pause_game();
                        continue;
                    }
                    player_bonus_accuracy += 15;
                    if (player_bonus_accuracy > MAX_ACCURACY) player_bonus_accuracy = MAX_ACCURACY;
                    break;
                }

                clear_screen();
                cout << endl << "	Kupiono: " << skills[choice].name << endl << endl;
                player_money -= skills[choice].price;
                pause_game();
            }
            else {
                clear_screen();
                cout << endl << "	Za mało hajsu!\n" << endl;
                pause_game();
            }
        }
        else if (keyboard_button == 27) {
            return;
        }
    }
}