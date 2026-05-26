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
    cout << "money <liczba> - dodaj pieniądze\n";
    cout << "health <liczba> - ustaw ilosc zdrowia\n";
    cout << "level <liczba> - ustaw level\n";
    cout << "weapon <id> - ustaw broń\n";
    cout << "Aby wyjść z konsoli rekomenduje wpisanie np. money 0\n\n";
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

    string choices[choice_amount] = {
        "Graj",
        "Zbrojownia",
        "Ekwipunek",
        "Opcje"
    };

    while (true) {
        int choice = 0;
        int keyboard_button;

        while (true) {
            clear_screen();
            cout << endl;

            cout << "\tNick: ";
            print_colored(nickname, player_nickname_color);

            cout << " | ";
            ui_colored("LVL: ", player_level, COLOR_BLUE);

            cout << endl;

            cout << "\t";
            ui_colored("HP: ", player_health, COLOR_GREEN);
            ui_colored("/", player_maxhealth, COLOR_GREEN);

            cout << " | ";
            ui_colored("Pancerz: ", player_armor, COLOR_YELLOW);

            cout << endl;

            cout << "\t";
            ui_colored("Hajs: $", player_money, COLOR_DARK_GREEN);

            cout << endl;

            cout << "\tBroń: ";
            print_colored(player_weapon_name, COLOR_CYAN);

            cout << " - ";
            r_ui_colored(player_weapon_damage, " DMG", COLOR_RED);

            cout << " | Kryt: ";
            r_ui_colored(player_crit_chance, "%", COLOR_PURPLE);

            cout << endl << endl;
            for (int i = 0; i < choice_amount; i++) {
                if (i == choice) {
                    cout << "	►  "; print_colored(choices[i], COLOR_BRIGHT_WHITE); cout << endl;
                }
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
        case 1: game_armory(); break;
        case 2: game_inventory(); break;
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
        cout << "	Zbrojownia\n";
        cout << "		1. W Legends of Wpierdol sklep działa w taki sposób że zakup broni wrzuca\n";
        cout << "		   aktualną broń z twojego ekwipunku.\n\n";
        cout << "		2. Wzmocnienia dają dodatkowe statystyki do gracza.\n";
        cout << "		3. Umiejętności dają dodatkowe, unikalne statystyki.\n";
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
                if (i == choice) {
                    cout << "	►  "; print_colored(choices[i], COLOR_BRIGHT_WHITE); cout << endl;
                }
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
        cout << "	Balancing:\n	na pewno nie szczurek9\n\n";
        cout << "	Game Testing:\n	Owcacejk\n\n";
        cout << "	Bug Fixer:\n	Mispolarny1\n\n";
        cout << "	Specjalne podziękowania:\n";
        cout << "	Maximum412\n";
        cout << "	Galaxy S22\n	Akali mains\n	Valve za przycisk `\n\n";
        cout << "	Version: 1.4\n\n";
        keyboard_button = _getch();
        if (keyboard_button == 27) return;
    }
}

void game_options() {
    const int choice_amount = 5;
    string choices[choice_amount] = { "Zapisz grę", "Zmień kolor nicku" ,"Pomoc", "Twórcy", "Wyjście z gry"};

    while (true) {
        int choice = 0;
        int keyboard_button;

        while (true) {
            clear_screen();
            cout << "ESC - Powrót\n\n";
            cout << "	Opcje:\n\n";

            for (int i = 0; i < choice_amount; i++) {
                if (i == choice) {
                    cout << "	►  "; print_colored(choices[i], COLOR_BRIGHT_WHITE); cout << endl;
                }
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
        case 0: manual_save(); break;
        case 1: game_nickname_color(); break;
        case 2: game_help(); break;
        case 3: game_credits(); break;
        case 4: exit(0); break;
        }
    }
}

void game_inventory() {

    while (true) {

        clear_screen();

        cout << "\n	Ekwipunek\n\n";

        if (inventory_count <= 0) {
            cout << "	Brak przedmiotów.\n\n";
            pause_game();
            return;
        }

        for (int i = 0; i < inventory_count; i++) {

            cout << "\t" << i + 1 << ". ";

            print_colored(inventory[i].name, COLOR_CYAN);

            cout << " | ";

            number_colored(inventory[i].damage, COLOR_RED);

            cout << " DMG | ";

            r_ui_colored(inventory[i].price * 40 / 100, "$", COLOR_DARK_GREEN);

            cout << endl;
        }

        cout << "\n	0. Powrót";
        cout << "\n\n	Wybierz: ";

        int choice;
        cin >> choice;

        if (choice == 0)
            return;

        choice--;

        if (choice < 0 || choice >= inventory_count)
            continue;

        clear_screen();

        cout << "\n\t";
        print_colored(inventory[choice].name, COLOR_CYAN);
        cout << endl;

        cout << "\t1. ";
        print_colored("Załóż", COLOR_GREEN);
        cout << endl;

        cout << "\t2. ";
        print_colored("Sprzedaj", COLOR_RED);
        cout << endl;

        cout << "\t0. Powrót\n\n";

        int action;
        cin >> action;

        if (action == 1) {

            string oldName = player_weapon_name;
            int oldDamage = player_weapon_damage;
            int oldPrice = player_weapon_price;

            player_weapon_name = inventory[choice].name;
            player_weapon_damage = inventory[choice].damage;
            player_weapon_price = inventory[choice].price;

            inventory[choice].name = oldName;
            inventory[choice].damage = oldDamage;
            inventory[choice].price = oldPrice;

            cout << "\n	Założono broń!\n";
        }
        else if (action == 2) {

            int money = inventory[choice].price * 40 / 100;

            player_money += money;

            for (int i = choice; i < inventory_count - 1; i++) {

                inventory[i] = inventory[i + 1];
            }

            inventory_count--;

            cout << "\n\tSprzedano za ";
            r_ui_colored(money, "$", COLOR_DARK_GREEN);
            cout << "!\n";
        }

        pause_game();
    }
}

void game_armory() {

    const int choice_amount = 4;

    string choices[choice_amount] = {
        "Rynek Broni",
        "Wzmocnienia",
        "Umiejętności",
        "Alchemik"
    };

    while (true) {

        int choice = 0;
        int keyboard_button;

        while (true) {

            clear_screen();
            cout << "ESC - Powrót\n\n";
            cout << "	Zbrojownia\n\n";

            for (int i = 0; i < choice_amount; i++) {
                if (i == choice) {
                    cout << "	►  "; print_colored(choices[i], COLOR_BRIGHT_WHITE); cout << endl;
                }
                else
                    cout << "	  " << choices[i] << endl;
            }

            keyboard_button = _getch();

            if (keyboard_button == 224) {

                keyboard_button = _getch();

                if (keyboard_button == 72) {

                    choice--;

                    if (choice < 0)
                        choice = choice_amount - 1;
                }
                else if (keyboard_button == 80) {

                    choice++;

                    if (choice >= choice_amount)
                        choice = 0;
                }
            }
            else if (keyboard_button == 13) {
                break;
            }
            else if (keyboard_button == 27) {
                return;
            }
        }

        switch (choice) {

        case 0:
            game_shop();
            break;

        case 1:
            game_upgrades();
            break;

        case 2:
            game_abilities();
            break;
        case 3:
            game_consumables();
            break;
        }
    }
}

void game_nickname_color() {

    const int color_amount = 15;

    string color_names[color_amount] = {
        "Biały",
        "Jasny Biały",

        "Niebieski",
        "Ciemny Niebieski",

        "Zielony",
        "Ciemny Zielony",

        "Cyan",
        "Ciemny Cyan",

        "Czerwony",
        "Ciemny Czerwony",

        "Fioletowy",
        "Ciemny Fioletowy",

        "Żółty",
        "Ciemny Żółty",

        "Szary"
    };

    int colors[color_amount] = {
        COLOR_WHITE,
        COLOR_BRIGHT_WHITE,

        COLOR_BLUE,
        COLOR_DARK_BLUE,

        COLOR_GREEN,
        COLOR_DARK_GREEN,

        COLOR_CYAN,
        COLOR_DARK_CYAN,

        COLOR_RED,
        COLOR_DARK_RED,

        COLOR_PURPLE,
        COLOR_DARK_PURPLE,

        COLOR_YELLOW,
        COLOR_DARK_YELLOW,

        COLOR_GRAY
    };

    int choice = 0;

    while (true) {

        clear_screen();

        cout << "ESC - Powrót\n\n";
        cout << "\tKolor Nicku\n\n";

        for (int i = 0; i < color_amount; i++) {

            if (i == choice)
                cout << "\t► ";
            else
                cout << "\t  ";

            set_color(colors[i]);
            cout << color_names[i];
            reset_color();

            cout << endl;
        }

        int key = _getch();

        if (key == 224) {

            key = _getch();

            if (key == 72) {

                choice--;

                if (choice < 0)
                    choice = color_amount - 1;
            }
            else if (key == 80) {

                choice++;

                if (choice >= color_amount)
                    choice = 0;
            }
        }
        else if (key == 13) {

            player_nickname_color = colors[choice];

            clear_screen();

            cout << "\n\tZmieniono kolor nicku!\n\n";

            pause_game();

            return;
        }
        else if (key == 27) {

            return;
        }
    }
}