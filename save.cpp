#include "save.h"
#include "globals.h"
#include "utils.h"
#include <fstream>
#include <iostream>

using namespace std;

bool save_exists() {

    ifstream file("save.LoW_save");
    return file.good();
}

void save_game() {

    ofstream file("save.LoW_save");

    if (!file)
        return;

    file << nickname << endl;

    file << player_level << endl;
    file << player_money << endl;

    file << player_health << endl;
    file << player_maxhealth << endl;

    file << player_weapon_name << endl;
    file << player_weapon_damage << endl;
    file << player_weapon_price << endl;

    file << player_lifesteal << endl;
    file << player_bonus_accuracy << endl;

    file << player_armor << endl;
    file << player_second_breath << endl;

    file << player_health_potion << endl;

    file << player_precision_potion << endl;
    file << player_vampire_potion << endl;

    file << precision_bonus << endl;
    file << precision_turns << endl;

    file << vampire_bonus << endl;
    file << vampire_turns << endl;

    file << player_crit_chance << endl;

    file << inventory_count << endl;

    for (int i = 0; i < inventory_count; i++) {

        file << inventory[i].name << endl;
        file << inventory[i].damage << endl;
        file << inventory[i].price << endl;
    }

    file.close();
}

bool load_game() {

    ifstream file("save.LoW_save");

    if (!file)
        return false;

    getline(file, nickname);

    file >> player_level;
    file >> player_money;

    file >> player_health;
    file >> player_maxhealth;

    file.ignore();

    getline(file, player_weapon_name);

    file >> player_weapon_damage;
    file >> player_weapon_price;

    file >> player_lifesteal;
    file >> player_bonus_accuracy;

    file >> player_armor;
    file >> player_second_breath;

    file >> player_health_potion;

    file >> player_precision_potion;
    file >> player_vampire_potion;

    file >> precision_bonus;
    file >> precision_turns;

    file >> player_crit_chance;

    file >> vampire_bonus;
    file >> vampire_turns;

    file >> inventory_count;

    file.ignore();

    for (int i = 0; i < inventory_count; i++) {

        getline(file, inventory[i].name);

        file >> inventory[i].damage;
        file >> inventory[i].price;

        file.ignore();
    }

    file.close();

    return true;
}

void manual_save() {

    clear_screen();

    save_game();

    cout << endl;
    cout << "	Gra została zapisana!\n";
    cout << endl;

    pause_game();
}