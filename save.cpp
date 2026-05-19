#include "save.h"
#include "globals.h"
#include "utils.h"
#include <fstream>
#include <iostream>
using namespace std;

bool save_exists() {
    ifstream file("save.sav");
    return file.good();
}

void save_game() {
    ofstream file("save.sav");
    if (!file) return;

    file << nickname << endl;
    file << player_level << endl;
    file << player_money << endl;
    file << player_health << endl;
    file << player_maxhealth << endl;
    file << player_weapon_name << endl;
    file << player_weapon_damage << endl;
    file << player_lifesteal << endl;
    file << player_bonus_accuracy << endl;

    file.close();
}

bool load_game() {
    ifstream file("save.sav");
    if (!file) return false;

    getline(file, nickname);
    file >> player_level;
    file >> player_money;
    file >> player_health;
    file >> player_maxhealth;
    file.ignore();
    getline(file, player_weapon_name);
    file >> player_weapon_damage;
    file >> player_lifesteal;
    file >> player_bonus_accuracy;

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