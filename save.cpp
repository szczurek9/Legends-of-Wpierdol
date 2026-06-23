#include "save.h"
#include "globals.h"
#include "utils.h"
#include "battle.h"
#include <fstream>
#include <iostream>

bool save_exists()
{
	std::ifstream file("save.LoW_save");
	return file.good();
}

void save_game()
{
	std::ofstream file("save.LoW_save");
	if (!file)
		return;

	file << nickname << "\n"
		<< player_nickname_color << "\n";
	file << player_level << "\n"
		<< player_money << "\n";
	file << player_health << "\n"
		<< player_maxhealth << "\n";
	file << player_weapon_name << "\n";
	file << player_weapon_damage << "\n"
		<< player_weapon_price << "\n";
	file << player_lifesteal << "\n"
		<< player_bonus_accuracy << "\n";
	file << player_bonus_health << "\n";
	file << player_armor << "\n"
		<< player_second_breath << "\n"
		<< player_escape_count << "\n";
	file << player_health_potion << "\n";
	file << player_accuracy_potion << "\n"
		<< player_vampirism_potion << "\n";
	file << accuracy_potion_bonus << "\n"
		<< accuracy_potion_turns << "\n";
	file << vampirism_potion_bonus << "\n"
		<< vampirism_potion_turns << "\n";
	file << player_crit_chance << "\n";
	file << (int)player_class << "\n"
		<< player_class_bonus_ad << "\n";
	file << player_mana << "\n"
		<< player_maxmana << "\n";
	file << player_spell_power << "\n"
		<< player_magic_resist_pen << "\n"
		<< player_magic_resist << "\n";
	file << player_armor_pen << "\n"
		<< enemy_stun_turns << "\n";

	int max_slots = get_max_magic_items();
	file << magic_item_count << "\n"
		<< max_slots << "\n";
	for (int i = 0; i < max_slots; i++)
	{
		file << magic_item_slots[i] << "\n";
		file << adept_book_stacks[i] << "\n";
		file << adept_book_upgraded[i] << "\n";
	}

	file << overkill_stored << "\n"
		<< overkill_armor_ignore << "\n";
	file << player_spell_vamp << "\n";
	file << cd_primal_strike << "\n"
		<< cd_undodgeable_speed << "\n"
		<< cd_slayer_of_slowest << "\n";
	file << cd_star_strike << "\n"
		<< cd_deadly_vines << "\n"
		<< cd_mirror_of_death << "\n";
	file << cd_stone_bastion << "\n"
		<< cd_accumulated_wrath << "\n"
		<< cd_iron_taunt << "\n";
	file << cd_kogeki_strike << "\n"
		<< cd_mushin_ability << "\n";
	file << primal_strike_active << "\n"
		<< undodgeable_turns << "\n"
		<< mirror_active << "\n";
	file << tank_bastion_armor_turns << "\n"
		<< tank_bastion_armor_bonus << "\n";
	file << tank_wrath_stored << "\n"
		<< tank_taunt_turns << "\n";
	file << samurai_attack_counter << "\n"
		<< sen_no_kata_active << "\n"
		<< mushin_shield_active << "\n";

	file << inventory_count << "\n";
	for (int i = 0; i < inventory_count; i++)
	{
		file << inventory[i].name << "\n"
			<< inventory[i].damage << "\n"
			<< inventory[i].price << "\n";
	}

	file.close();
}

bool load_game()
{
	std::ifstream file("save.LoW_save");
	if (!file)
		return false;

	std::getline(file, nickname);
	file >> player_nickname_color;
	file.ignore();
	file >> player_level >> player_money;
	file >> player_health >> player_maxhealth;
	file.ignore();
	std::getline(file, player_weapon_name);
	file >> player_weapon_damage >> player_weapon_price;
	file >> player_lifesteal >> player_bonus_accuracy;
	file >> player_bonus_health;
	file >> player_armor >> player_second_breath >> player_escape_count;
	file >> player_health_potion;
	file >> player_accuracy_potion >> player_vampirism_potion;
	file >> accuracy_potion_bonus >> accuracy_potion_turns;
	file >> vampirism_potion_bonus >> vampirism_potion_turns;
	file >> player_crit_chance;

	int class_int = 0;
	if (!(file >> class_int))
	{
		file.close();
		return true;
	}
	player_class = (PlayerClass)class_int;
	file >> player_class_bonus_ad;
	file >> player_mana >> player_maxmana;
	file >> player_spell_power >> player_magic_resist_pen >> player_magic_resist;
	file >> player_armor_pen >> enemy_stun_turns;

	int saved_count = 0, saved_slots = 0;
	if (!(file >> saved_count >> saved_slots))
	{
		file.close();
		return true;
	}
	magic_item_count = saved_count;
	// Inicjalizuj wszystkie sloty na -1
	for (int i = 0; i < 8; i++)
	{
		magic_item_slots[i] = -1;
		adept_book_stacks[i] = 0;
		adept_book_upgraded[i] = false;
	}
	for (int i = 0; i < saved_slots && i < 8; i++)
	{
		file >> magic_item_slots[i] >> adept_book_stacks[i] >> adept_book_upgraded[i];
	}

	int tmp;
	file >> overkill_stored >> tmp;
	overkill_armor_ignore = (bool)tmp;
	file >> player_spell_vamp;
	file >> cd_primal_strike >> cd_undodgeable_speed >> cd_slayer_of_slowest;
	file >> cd_star_strike >> cd_deadly_vines >> cd_mirror_of_death;
	file >> cd_stone_bastion >> cd_accumulated_wrath >> cd_iron_taunt;
	file >> cd_kogeki_strike >> cd_mushin_ability;
	file >> tmp;
	primal_strike_active = (bool)tmp;
	file >> undodgeable_turns;
	file >> tmp;
	mirror_active = (bool)tmp;
	file >> tank_bastion_armor_turns >> tank_bastion_armor_bonus;
	file >> tank_wrath_stored >> tank_taunt_turns;
	file >> samurai_attack_counter;
	file >> tmp;
	sen_no_kata_active = (bool)tmp;
	file >> tmp;
	mushin_shield_active = (bool)tmp;

	file >> inventory_count;
	file.ignore();
	for (int i = 0; i < inventory_count; i++)
	{
		std::getline(file, inventory[i].name);
		file >> inventory[i].damage >> inventory[i].price;
		file.ignore();
	}

	file.close();
	return true;
}

void manual_save()
{
	clear_screen();
	save_game();
	std::cout << "\n\tGra została zapisana!\n\n";
	pause_game();
}
