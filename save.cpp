#include "save.h"
#include "globals.h"
#include "utils.h"
#include <fstream>
#include <iostream>


bool save_exists() {

    std::ifstream file("save.LoW_save");
    return file.good();
}

void save_game() {

    std::ofstream file("save.LoW_save");

    if (!file)
        return;

    file << nickname << std::endl;
    file << player_nickname_color << std::endl;

    file << player_level << std::endl;
    file << player_money << std::endl;

    file << player_health << std::endl;
    file << player_maxhealth << std::endl;

    file << player_weapon_name << std::endl;
    file << player_weapon_damage << std::endl;
    file << player_weapon_price << std::endl;

    file << player_lifesteal << std::endl;
    file << player_bonus_accuracy << std::endl;

    file << player_armor << std::endl;
    file << player_second_breath << std::endl;

    file << player_health_potion << std::endl;

    file << player_precision_potion << std::endl;
    file << player_vampire_potion << std::endl;

    file << precision_bonus << std::endl;
    file << precision_turns << std::endl;

    file << vampire_bonus << std::endl;
    file << vampire_turns << std::endl;

    file << player_crit_chance << std::endl;

    // Nowe zmienne: klasa, mana, magic stats
    file << (int)player_class << std::endl;
    file << player_class_bonus_ad << std::endl;
    file << player_mana << std::endl;
    file << player_maxmana << std::endl;
    file << player_spell_power << std::endl;
    file << player_magic_resist_pen << std::endl;
    file << player_magic_resist << std::endl;
    file << player_armor_pen << std::endl;
    // enemy_armor_pen jest teraz polem w battle_enemy, nie trzeba zapisywać
    file << enemy_stun_turns << std::endl;

    // Sloty na itemy magiczne
    file << magic_item_count << std::endl;
    for (int i = 0; i < MAX_MAGIC_ITEMS; i++) {
        file << magic_item_slots[i] << std::endl;
    }

    // Nowe zmienne v1.6: Overkill, Spell Vamp, cooldowny, aktywne efekty
    file << overkill_stored << std::endl;
    file << overkill_armor_ignore << std::endl;
    file << player_spell_vamp << std::endl;
    file << cd_primal_strike << std::endl;
    file << cd_undodgable_speed << std::endl;
    file << cd_slayer_of_slowest << std::endl;
    file << cd_star_strike << std::endl;
    file << cd_deadly_vines << std::endl;
    file << cd_mirror_of_death << std::endl;
    file << primal_strike_active << std::endl;
    file << undodgable_turns << std::endl;
    file << mirror_active << std::endl;

    file << inventory_count << std::endl;

    for (int i = 0; i < inventory_count; i++) {

        file << inventory[i].name << std::endl;
        file << inventory[i].damage << std::endl;
        file << inventory[i].price << std::endl;
    }

    file.close();
}

bool load_game() {

    std::ifstream file("save.LoW_save");

    if (!file)
        return false;

    std::getline(file, nickname);

    file >> player_nickname_color;
    file.ignore();

    file >> player_level;
    file >> player_money;

    file >> player_health;
    file >> player_maxhealth;

    file.ignore();

    std::getline(file, player_weapon_name);

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

    file >> vampire_bonus;
    file >> vampire_turns;

    file >> player_crit_chance;

    // Nowe zmienne (kompatybilność: jeśli nie ma - zostają domyślne wartości)
    int class_int = 0;
    if (file >> class_int) {
        player_class = (PlayerClass)class_int;
        file >> player_class_bonus_ad;
        file >> player_mana;
        file >> player_maxmana;
        file >> player_spell_power;
        file >> player_magic_resist_pen;
        file >> player_magic_resist;
        file >> player_armor_pen;
        // enemy_armor_pen jest teraz polem w battle_enemy, nie trzeba wczytywać
        file >> enemy_stun_turns;

        file >> magic_item_count;
        for (int i = 0; i < MAX_MAGIC_ITEMS; i++) {
            file >> magic_item_slots[i];
        }

        // Nowe zmienne v1.6: Overkill, Spell Vamp, cooldowny, aktywne efekty
        int ov_stored, ov_ignore, sv, cps, cus, css, css2, cdv, cmd, psa, ut, ma;
        if (file >> ov_stored) {
            overkill_stored = ov_stored;
            file >> ov_ignore; overkill_armor_ignore = (bool)ov_ignore;
            file >> sv;        player_spell_vamp = sv;
            file >> cps;       cd_primal_strike = cps;
            file >> cus;       cd_undodgable_speed = cus;
            file >> css;       cd_slayer_of_slowest = css;
            file >> css2;      cd_star_strike = css2;
            file >> cdv;       cd_deadly_vines = cdv;
            file >> cmd;       cd_mirror_of_death = cmd;
            file >> psa;       primal_strike_active = (bool)psa;
            file >> ut;        undodgable_turns = ut;
            file >> ma;        mirror_active = (bool)ma;
        }
    }

    file >> inventory_count;

    file.ignore();

    for (int i = 0; i < inventory_count; i++) {

        std::getline(file, inventory[i].name);

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

    std::cout << std::endl;
    std::cout << "\tGra została zapisana!\n";
    std::cout << std::endl;

    pause_game();
}
