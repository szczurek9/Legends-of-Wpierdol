#include "battle.h"
#include "globals.h"
#include "utils.h"
#include "save.h"
#include <iostream>
#include <cstdlib>

// ===== POMOCNICZE: regeneracja many na koniec tury =====
void regen_mana() {
	// Baza: 3% std::max many na turę + bonus z itemów (base_mana_regen_bonus %)
	int base_regen_bonus_pct = 0;
	for (int i = 0; i < MAX_MAGIC_ITEMS; i++) {
		if (magic_item_slots[i] >= 0) {
			base_regen_bonus_pct += magic_items[magic_item_slots[i]].base_mana_regen_bonus;
		}
	}
	// Mag: +50% bazowej regeneracji many
	if (player_class == CLASS_MAGE) base_regen_bonus_pct += 50;

	// Bazowa regen = 3% std::max many
	int base_regen = (int)(player_maxmana * 0.03f);
	// Bonus z itemów/klasy (100% bazy = +base_regen)
	int bonus_regen = (base_regen * base_regen_bonus_pct) / 100;
	int total_regen = base_regen + bonus_regen;
	player_mana += total_regen;
	if (player_mana > player_maxmana)
		player_mana = player_maxmana;
}

// ===== POMOCNICZE: limit armor pen gracza =====
void clamp_player_armor_pen() {
	if (player_armor_pen > 70)
		player_armor_pen = 70;
}

// ===== POMOCNICZE: zegarek cooldownów — odejmuje 1 od każdego CD (wywołuj raz na turę gracza) =====
void tick_cooldowns() {
	if (cd_primal_strike > 0)    cd_primal_strike--;
	if (cd_undodgable_speed > 0) cd_undodgable_speed--;
	if (cd_slayer_of_slowest > 0) cd_slayer_of_slowest--;
	if (cd_star_strike > 0)      cd_star_strike--;
	if (cd_deadly_vines > 0)     cd_deadly_vines--;
	if (cd_mirror_of_death > 0)  cd_mirror_of_death--;

	if (undodgable_turns > 0) {
		undodgable_turns--;
	}
}

// ===== POMOCNICZE: leczenie ze Spell Vamp =====
void apply_spell_vamp(int raw_magic_damage) {
	if (player_spell_vamp <= 0) return;
	int heal = (raw_magic_damage * player_spell_vamp) / 100;
	if (heal < 1) return;
	player_health += heal;
	if (player_health > player_maxhealth)
		player_health = player_maxhealth;
	std::cout << "\t";
	print_colored("Dłonie Wampira uzdrowiły: ", COLOR_PURPLE);
	number_colored(heal, COLOR_GREEN);
	print_colored(" HP!", COLOR_PURPLE);
	std::cout << "\n";
}

// ===== POMOCNICZE: wspólna logika zadania obrażeń magicznych =====
// Zwraca rzeczywiste obrażenia po redukcji MR. Jeśli enemy_health=NULL nie obcina do HP.
int deal_magic_damage(int raw_damage, int enemyIndex) {
	int enemy_mr = enemy[enemyIndex].magic_resist;
	int effective_mr = enemy_mr - player_magic_resist_pen;
	if (effective_mr < 0) effective_mr = 0;
	float mr_reduction = effective_mr / (effective_mr + 100.0f);
	int dealt = (int)(raw_damage * (1.0f - mr_reduction));
	if (dealt < 1) dealt = 1;
	if (dealt > current_enemy_health) dealt = current_enemy_health;
	return dealt;
}

// ===== POMOCNICZE: blok po pokonaniu fali — nagroda + drugie tchnienie + reset =====
void handle_wave_kill(int enemyIndex, float battle_seconds) {
	float multiplier = 1.0f;
	if (battle_seconds < 8)  multiplier = 1.5f;
	else if (battle_seconds < 15) multiplier = 1.2f;

	int base_reward = enemy[enemyIndex].kill_reward;
	int final_reward = (int)(base_reward * multiplier);
	int bonus_money_amount = final_reward - base_reward;
	player_money += final_reward;

	// Zabójca: +10% std::max many przywracane przy zabójstwie
	if (player_class == CLASS_ASSASSIN) {
		int mana_restore = (int)(player_maxmana * 0.10f);
		player_mana += mana_restore;
		if (player_mana > player_maxmana) player_mana = player_maxmana;
	}

	clear_screen();
	std::cout << "\n\t";
	print_colored("Przeciwnik pokonany!", COLOR_GREEN);
	std::cout << "\n\n\t";
	print_colored("Otrzymujesz: $", COLOR_DARK_GREEN);
	number_colored(base_reward, COLOR_DARK_GREEN);
	print_colored(" + $", COLOR_YELLOW);
	number_colored(bonus_money_amount, COLOR_YELLOW);
	print_colored(" za czas walki!", COLOR_BRIGHT_WHITE);
	std::cout << "\n\t";
	print_colored("Suma: $", COLOR_GREEN);
	number_colored(final_reward, COLOR_GREEN);
	std::cout << std::endl;

	// Reset stunu po pokonaniu fali
	enemy_stun_turns = 0;

	if (player_second_breath) {
		int healAmount = (int)(player_maxhealth * 0.25f);
		int manaAmount = (int)(player_maxmana * 0.25f);
		player_health += healAmount;
		player_mana += manaAmount;
		if (player_health > player_maxhealth) player_health = player_maxhealth;
		if (player_mana > player_maxmana) player_mana = player_maxmana;
		std::cout << "\n\t";
		ui_colored("Drugie Tchnienie przywróciło: ", healAmount, COLOR_GREEN); print_colored(" HP!", COLOR_GREEN);
		std::cout << "\n\t";
		ui_colored("Drugie Tchnienie przywróciło: ", manaAmount, COLOR_BLUE); print_colored(" many!", COLOR_BLUE); std::cout << "\n";
	}
}

// ===== CZARY MAGA =====

// Powiew Burzy: mały dmg magiczny, brak CD
void cast_storm_breeze(int enemyIndex) {
	int mana_cost = 30;
	if (player_mana < mana_cost) {
		clear_screen();
		std::cout << "\n\t";
		print_colored("Za mało many! (potrzebujesz 30)", COLOR_RED);
		std::cout << "\n\n";
		pause_game();
		return;
	}
	player_mana -= mana_cost;
	int raw_damage = (int)(5 + player_spell_power * 0.10f);
	int dealt = deal_magic_damage(raw_damage, enemyIndex);
	current_enemy_health -= dealt;

	clear_screen();
	std::cout << "\n\t";
	print_colored("Rzucono: Powiew Burzy!", COLOR_CYAN);
	std::cout << "\n\t";
	print_colored("Zadano ", COLOR_GREEN);
	number_colored(dealt, COLOR_RED);
	print_colored(" obrażeń magicznych!", COLOR_GREEN);
	std::cout << "\n";
	apply_spell_vamp(dealt);
	std::cout << "\t";
	print_colored("Pozostałe HP przeciwnika: ", COLOR_BRIGHT_WHITE);
	number_colored(current_enemy_health, COLOR_RED);
	std::cout << "\n\n";
	pause_game();
}

// Gwiezdne Uderzenie: wysoki burst damage, 1 tura CD
void cast_star_strike(int enemyIndex) {
	int mana_cost = 120;
	if (player_mana < mana_cost) {
		clear_screen();
		std::cout << "\n\t";
		print_colored("Za mało many! (potrzebujesz 120)", COLOR_RED);
		std::cout << "\n\n";
		pause_game();
		return;
	}
	if (cd_star_strike > 0) {
		clear_screen();
		std::cout << "\n\t";
		print_colored("Gwiezdne Uderzenie jest na cooldownie! (", COLOR_RED);
		number_colored(cd_star_strike, COLOR_YELLOW);
		print_colored(" tur)", COLOR_RED);
		std::cout << "\n\n";
		pause_game();
		return;
	}

	player_mana -= mana_cost;
	cd_star_strike = 1;

	int raw_damage = (int)(25 + player_spell_power * 0.80f);
	int dealt = deal_magic_damage(raw_damage, enemyIndex);
	current_enemy_health -= dealt;

	clear_screen();
	std::cout << "\n\t";
	print_colored("Rzucono: Gwiezdne Uderzenie!", COLOR_YELLOW);
	std::cout << "\n\t";
	print_colored("POTĘŻNY cios magiczny! Zadano ", COLOR_YELLOW);
	number_colored(dealt, COLOR_RED);
	print_colored(" obrażeń!", COLOR_YELLOW);
	std::cout << "\n";
	apply_spell_vamp(dealt);
	std::cout << "\t";
	print_colored("Pozostałe HP przeciwnika: ", COLOR_BRIGHT_WHITE);
	number_colored(current_enemy_health, COLOR_RED);
	std::cout << "\n\n";
	pause_game();
}

// Zabójcze Pnącza: stun 3 tury + 5+(3%AP) dmg/turę ogłuszenia — 4 tur CD
void cast_deadly_vines(int enemyIndex) {
	int mana_cost = 80;
	if (player_mana < mana_cost) {
		clear_screen();
		std::cout << "\n\t";
		print_colored("Za mało many! (potrzebujesz 80)", COLOR_RED);
		std::cout << "\n\n";
		pause_game();
		return;
	}
	if (cd_deadly_vines > 0) {
		clear_screen();
		std::cout << "\n\t";
		print_colored("Zabójcze Pnącza są na cooldownie! (", COLOR_RED);
		number_colored(cd_deadly_vines, COLOR_YELLOW);
		print_colored(" tur)", COLOR_RED);
		std::cout << "\n\n";
		pause_game();
		return;
	}
	if (enemy_stun_turns > 0) {
		clear_screen();
		std::cout << "\n\t";
		print_colored("Przeciwnik jest już ogłuszony!", COLOR_YELLOW);
		std::cout << "\n\n";
		pause_game();
		return;
	}

	player_mana -= mana_cost;
	cd_deadly_vines = 4;

	// Zadaj dmg za pierwszą turę ogłuszenia
	int raw_damage = (int)(5 + player_spell_power * 0.03f);
	int dealt = deal_magic_damage(raw_damage, enemyIndex);
	current_enemy_health -= dealt;
	enemy_stun_turns = 3;  // 3 tury stunu

	clear_screen();
	std::cout << "\n\t";
	print_colored("Rzucono: Zabójcze Pnącza!", COLOR_DARK_GREEN);
	std::cout << "\n\t";
	print_colored("Zadano ", COLOR_GREEN);
	number_colored(dealt, COLOR_RED);
	print_colored(" obrażeń magicznych!", COLOR_GREEN);
	std::cout << "\n";
	apply_spell_vamp(dealt);
	std::cout << "\t";
	print_colored("Przeciwnik zostaje ogłuszony na 3 tury!", COLOR_YELLOW);
	std::cout << "\n\t";
	print_colored("Pozostałe HP przeciwnika: ", COLOR_BRIGHT_WHITE);
	number_colored(current_enemy_health, COLOR_RED);
	std::cout << "\n\n";
	pause_game();
}

// Śmiertelne Lustro: blokuje śmiertelny atak — 5 tur CD
void cast_mirror_of_death(int enemyIndex) {
	int mana_cost = 50;
	if (player_mana < mana_cost) {
		clear_screen();
		std::cout << "\n\t";
		print_colored("Za mało many! (potrzebujesz 50)", COLOR_RED);
		std::cout << "\n\n";
		pause_game();
		return;
	}
	if (cd_mirror_of_death > 0) {
		clear_screen();
		std::cout << "\n\t";
		print_colored("Śmiertelne Lustro jest na cooldownie! (", COLOR_RED);
		number_colored(cd_mirror_of_death, COLOR_YELLOW);
		print_colored(" tur)", COLOR_RED);
		std::cout << "\n\n";
		pause_game();
		return;
	}

	player_mana -= mana_cost;
	cd_mirror_of_death = 5;
	mirror_active = true;

	clear_screen();
	std::cout << "\n\t";
	print_colored("Śmiertelne Lustro aktywowane!", COLOR_CYAN);
	std::cout << "\n\t";
	print_colored("Następny śmiertelny atak wroga zostanie odparty!", COLOR_CYAN);
	std::cout << "\n\n";
	pause_game();
}

// ===== CZARY ZABÓJCY =====

// Primal Strike: następny atak zawsze krytuje (lub 2.25x jeśli już miał kryta)
void cast_primal_strike() {
	int mana_cost = 200;
	if (player_mana < mana_cost) {
		clear_screen();
		std::cout << "\n\t";
		print_colored("Za mało many! (potrzebujesz 200)", COLOR_RED);
		std::cout << "\n\n";
		pause_game();
		return;
	}
	// Brak CD — ale aktywna flaga blokuje ponowne użycie
	if (primal_strike_active) {
		clear_screen();
		std::cout << "\n\t";
		print_colored("Pierwotne Uderzenie jest już aktywne!", COLOR_YELLOW);
		std::cout << "\n\n";
		pause_game();
		return;
	}

	player_mana -= mana_cost;
	primal_strike_active = true;

	clear_screen();
	std::cout << "\n\t";
	print_colored("Rzucono: Pierwotne Uderzenie!", COLOR_RED);
	std::cout << "\n\t";
	print_colored("Następny atak zawsze trafi krytycznie!", COLOR_PURPLE);
	std::cout << "\n\n";
	pause_game();
}

// Un-Dodgable Speed: +25% accuracy gracza, -30% accuracy wroga na 3 tury — 4 tur CD
void cast_undodgable_speed() {
	int mana_cost = 80;
	if (player_mana < mana_cost) {
		clear_screen();
		std::cout << "\n\t";
		print_colored("Za mało many! (potrzebujesz 80)", COLOR_RED);
		std::cout << "\n\n";
		pause_game();
		return;
	}
	if (cd_undodgable_speed > 0) {
		clear_screen();
		std::cout << "\n\t";
		print_colored("Nieomijalna Prędkość jest na cooldownie! (", COLOR_RED);
		number_colored(cd_undodgable_speed, COLOR_YELLOW);
		print_colored(" tur)", COLOR_RED);
		std::cout << "\n\n";
		pause_game();
		return;
	}

	player_mana -= mana_cost;
	cd_undodgable_speed = 4;
	undodgable_turns = 3;

	clear_screen();
	std::cout << "\n\t";
	print_colored("Rzucono: Nieomijalna Prędkość!", COLOR_CYAN);
	std::cout << "\n\t";
	print_colored("+25% accuracy gracza, -30% accuracy wroga na 3 tury!", COLOR_CYAN);
	std::cout << "\n\n";
	pause_game();
}

// Slayer of the Slowest: jeśli dodge wroga < dodge gracza, następny atak zadaje 1%(+1% za 20AP) dmg bazowego
void cast_slayer_of_slowest(int enemyIndex) {
	int mana_cost = 20;
	if (player_mana < mana_cost) {
		clear_screen();
		std::cout << "\n\t";
		print_colored("Za mało many! (potrzebujesz 20)", COLOR_RED);
		std::cout << "\n\n";
		pause_game();
		return;
	}
	if (cd_slayer_of_slowest > 0) {
		clear_screen();
		std::cout << "\n\t";
		print_colored("Pogromca Powolnych jest na cooldownie! (", COLOR_RED);
		number_colored(cd_slayer_of_slowest, COLOR_YELLOW);
		print_colored(" tury)", COLOR_RED);
		std::cout << "\n\n";
		pause_game();
		return;
	}

	// Efektywny dodge gracza = bonus_accuracy + efekty
	int player_effective_dodge = player_bonus_accuracy;
	if (undodgable_turns > 0) player_effective_dodge += 25;

	int enemy_dodge = enemy[enemyIndex].dodge_chance;
	if (undodgable_turns > 0) enemy_dodge = std::max(0, enemy_dodge - 30);

	if (enemy_dodge >= player_effective_dodge) {
		clear_screen();
		std::cout << "\n\t";
		print_colored("Przeciwnik jest za szybki! (dodge wroga >= twój dodge)", COLOR_YELLOW);
		std::cout << "\n\n";
		pause_game();
		return;
	}

	player_mana -= mana_cost;
	cd_slayer_of_slowest = 1;

	// 1% dmg bazowego broni (+1% za każde 20 AP)
	int base_dmg = player_weapon_damage + player_class_bonus_ad;
	int pct = 1 + (player_spell_power / 20);
	int raw_damage = (base_dmg * pct) / 100;
	if (raw_damage < 1) raw_damage = 1;
	int dealt = deal_magic_damage(raw_damage, enemyIndex);
	current_enemy_health -= dealt;

	clear_screen();
	std::cout << "\n\t";
	print_colored("Rzucono: Pogromca Powolnych!", COLOR_PURPLE);
	std::cout << "\n\t";
	print_colored("Zadano ", COLOR_GREEN);
	number_colored(dealt, COLOR_RED);
	print_colored(" obrażeń magicznych!", COLOR_GREEN);
	std::cout << "\n";
	apply_spell_vamp(dealt);
	std::cout << "\t";
	print_colored("Pozostałe HP przeciwnika: ", COLOR_BRIGHT_WHITE);
	number_colored(current_enemy_health, COLOR_RED);
	std::cout << "\n\n";
	pause_game();
}

// ===== MENU CZARÓW MAGA =====
void battle_spells_mage(int enemyIndex, bool& spell_used) {
	int choice = 0;
	int keyboard_button;

	const int spell_amount = 5;

	while (true) {
		clear_screen();
		print_colored("\n\tCzary Maga\n\n", COLOR_BRIGHT_WHITE);

		std::cout << "\t";
		print_colored("Mana: ", COLOR_BLUE);
		number_colored(player_mana, COLOR_BLUE);
		std::cout << " / ";
		number_colored(player_maxmana, COLOR_BLUE);
		std::cout << "  |  ";
		print_colored("AP: ", COLOR_PURPLE);
		number_colored(player_spell_power, COLOR_PURPLE);
		std::cout << "\n\n";

		// Powiew Burzy
		std::string s0 = "Powiew Burzy [Magiczne]\n";
		s0 += "\t     Zadaje 5 (+10% AP) obrazen. Koszt: 30 Mana";
		if (player_mana < 30) s0 += "  [ZA MALO MANY]";

		// Gwiezdne Uderzenie
		std::string s1 = "Gwiezdne Uderzenie [Magiczne]\n";
		s1 += "\t     Zadaje 25 (+80% AP) obrazen. Koszt: 120 Mana  CD: 1 tura";
		if (cd_star_strike > 0) s1 += "  [CD: " + std::to_string(cd_star_strike) + " tur]";
		else if (player_mana < 120) s1 += "  [ZA MALO MANY]";

		// Zabójcze Pnącza
		std::string s2 = "Zabojcze Pnacza [Magiczne]\n";
		s2 += "\t     Oglusza na 3 tury. Podczas ogluszenia zadaje 5 (+3% AP) co ture. Koszt: 80 Mana  CD: 4 tury";
		if (cd_deadly_vines > 0) s2 += "  [CD: " + std::to_string(cd_deadly_vines) + " tur]";
		else if (enemy_stun_turns > 0) s2 += "  [PRZECIWNIK JUZ OGLUSZONY]";
		else if (player_mana < 80) s2 += "  [ZA MALO MANY]";

		// Śmiertelne Lustro
		std::string s3 = "Smiertelne Lustro [Fizyczne]\n";
		s3 += "\t     Jesli nast. atak wroga bylby smiertelny: blokuje 99% dmg, odbija 5%,\n";
		s3 += "\t     leczy 15% zablok. (50% zablok. jesli zabicie tą umiej.). Jesli nie smiertelny:\n";
		s3 += "\t     blokuje 10%, odbija 20% zablok. Koszt: 50 Mana  CD: 5 tur";
		if (mirror_active) s3 += "  [AKTYWNE]";
		else if (cd_mirror_of_death > 0) s3 += "  [CD: " + std::to_string(cd_mirror_of_death) + " tur]";
		else if (player_mana < 50) s3 += "  [ZA MALO MANY]";

		std::string spell_lines[spell_amount] = { s0, s1, s2, s3, "Powrot" };

		for (int i = 0; i < spell_amount; i++) {
			if (i == choice) {
				std::cout << "\t►  "; print_colored(spell_lines[i], COLOR_BRIGHT_WHITE); std::cout << "\n";
			}
			else {
				std::cout << "\t   " << spell_lines[i] << "\n";
			}
		}

		std::cout << "\n";

		keyboard_button = _getch();

		if (keyboard_button == 224) {
			keyboard_button = _getch();
			if (keyboard_button == 72) { choice--; if (choice < 0) choice = spell_amount - 1; }
			else if (keyboard_button == 80) { choice++; if (choice >= spell_amount) choice = 0; }
		}
		else if (keyboard_button == 13) {
			if (choice == 0) { cast_storm_breeze(enemyIndex);   spell_used = true; return; }
			else if (choice == 1) { cast_star_strike(enemyIndex);    spell_used = true; return; }
			else if (choice == 2) { cast_deadly_vines(enemyIndex);   spell_used = true; return; }
			else if (choice == 3) { cast_mirror_of_death(enemyIndex); spell_used = true; return; }
			else if (choice == 4) { return; }  // Powrót — spell_used = false, brak tury wroga
		}
		else if (keyboard_button == 27) { return; }
	}
}

// ===== MENU CZARÓW ZABÓJCY =====
void battle_spells_assassin(int enemyIndex, bool& spell_used) {
	int choice = 0;
	int keyboard_button;

	const int spell_amount = 4;

	while (true) {
		clear_screen();
		print_colored("\n\tCzary Zabójcy\n\n", COLOR_BRIGHT_WHITE);

		std::cout << "\t";
		print_colored("Mana: ", COLOR_BLUE);
		number_colored(player_mana, COLOR_BLUE);
		std::cout << " / ";
		number_colored(player_maxmana, COLOR_BLUE);
		std::cout << "  |  ";
		print_colored("AP: ", COLOR_PURPLE);
		number_colored(player_spell_power, COLOR_PURPLE);
		std::cout << "\n\n";

		// Primal Strike
		std::string s0 = "Primal Strike [Fizyczne]\n";
		s0 += "\t     Nastepny atak krytuje. Jesli mial byc krytem, zadaje 2.25x dmg.\n";
		s0 += "\t     Nadmiar obrazen trafia do Overkill. Koszt: 200 Mana";
		if (primal_strike_active) s0 += "  [AKTYWNE]";
		else if (player_mana < 200) s0 += "  [ZA MALO MANY]";

		// Un-Dodgable Speed
		std::string s1 = "Un-Dodgable Speed\n";
		s1 += "\t     Przez 3 tury: +25% Accuracy, przeciwnik -30% Accuracy. Koszt: 80 Mana  CD: 4 tury";
		if (cd_undodgable_speed > 0) s1 += "  [CD: " + std::to_string(cd_undodgable_speed) + " tur]";
		else if (player_mana < 80) s1 += "  [ZA MALO MANY]";
		if (undodgable_turns > 0) s1 += "  [AKTYWNE: " + std::to_string(undodgable_turns) + " tur]";

		// Slayer of the Slowest
		std::string s2 = "Slayer of the Slowest [Magiczne]\n";
		s2 += "\t     Jesli masz wiecej Dodge niz przeciwnik, nast. atak zadaje\n";
		s2 += "\t     1% (+1% za kazde 20 AP) std::max HP celu. Koszt: 20 Mana  CD: 1 tura";
		if (cd_slayer_of_slowest > 0) s2 += "  [CD: " + std::to_string(cd_slayer_of_slowest) + " tur]";
		else if (player_mana < 20) s2 += "  [ZA MALO MANY]";

		std::string spell_lines[spell_amount] = { s0, s1, s2, "Powrot" };

		for (int i = 0; i < spell_amount; i++) {
			if (i == choice) {
				std::cout << "\t►  "; print_colored(spell_lines[i], COLOR_BRIGHT_WHITE); std::cout << "\n";
			}
			else {
				std::cout << "\t   " << spell_lines[i] << "\n";
			}
		}

		std::cout << "\n";

		keyboard_button = _getch();

		if (keyboard_button == 224) {
			keyboard_button = _getch();
			if (keyboard_button == 72) { choice--; if (choice < 0) choice = spell_amount - 1; }
			else if (keyboard_button == 80) { choice++; if (choice >= spell_amount) choice = 0; }
		}
		else if (keyboard_button == 13) {
			if (choice == 0) { cast_primal_strike();             spell_used = true; return; }
			else if (choice == 1) { cast_undodgable_speed();          spell_used = true; return; }
			else if (choice == 2) { cast_slayer_of_slowest(enemyIndex); spell_used = true; return; }
			else if (choice == 3) { return; }  // Powrót — brak tury wroga
		}
		else if (keyboard_button == 27) { return; }
	}
}

// ===== MENU CZARÓW — DISPATCHER =====
void battle_spells_menu(int enemyIndex, bool& spell_used) {
	if (player_class == CLASS_ASSASSIN)
		battle_spells_assassin(enemyIndex, spell_used);
	else
		battle_spells_mage(enemyIndex, spell_used);
}

// ===== ATAK GRACZA — blok wykonania z Overkill, Primal Strike =====
// Zwraca: true = wróg pokonany, false = jeszcze żyje
bool do_player_attack(int enemyIndex) {
	int miss = rand() % 100;
	int bonusAccuracy = player_bonus_accuracy;

	if (precision_turns > 0)
		bonusAccuracy += precision_bonus;

	// Nieomijalna Prędkość: +25% accuracy gracza
	if (undodgable_turns > 0)
		bonusAccuracy += 25;

	int enemy_dodge_chance = enemy[enemyIndex].dodge_chance;
	// Nieomijalna Prędkość: -30% accuracy wroga
	if (undodgable_turns > 0)
		enemy_dodge_chance = std::max(0, enemy_dodge_chance - 30);

	int final_dodge = enemy_dodge_chance - bonusAccuracy;
	if (final_dodge < 0) final_dodge = 0;

	if (miss < final_dodge) {
		// ATAK NIETRAFIONY
		clear_screen();
		std::cout << "\n\t";
		print_colored("Twoja tura!", COLOR_BRIGHT_WHITE);
		std::cout << "\n\n\t";
		print_colored("Przeciwnik uniknął twojego ataku!", COLOR_RED);
		std::cout << std::endl << std::endl;
		pause_game();
		return false;
	}

	// TRAFIENIE
	bool critical = false;
	bool super_crit = false;  // Primal Strike + już krytyczny

	int critRoll = rand() % 100;
	bool would_crit = (critRoll < player_crit_chance);

	if (primal_strike_active) {
		if (would_crit) {
			super_crit = true;
		}
		critical = true;
		primal_strike_active = false;  // zużyte
	}
	else {
		critical = would_crit;
	}

	// Całkowity DMG gracza: broń + bonus AD klasy
	int total_player_dmg = player_weapon_damage + player_class_bonus_ad;
	// Zabójca: +10% obrażeń dla każdej broni (stały bonus klas)
	if (player_class == CLASS_ASSASSIN) {
		total_player_dmg = (int)(total_player_dmg * 1.10f);
	}
	// Tank: -25% obrażeń dla broni powyżej 500 DMG
	if (player_class == CLASS_TANK && player_weapon_damage > 500) {
		total_player_dmg = (int)(total_player_dmg * 0.75f);
	}

	int dealt_damage = total_player_dmg;

	if (super_crit) {
		dealt_damage = (int)(dealt_damage * 2.25f);
	}
	else if (critical) {
		dealt_damage = (int)(dealt_damage * 1.5f);
	}

	// Redukcja przez pancerz przeciwnika (uwzględnia armor pen gracza, std::max 70)
	clamp_player_armor_pen();
	int enemy_armor_val = enemy[enemyIndex].armor;
	int effective_pen = player_armor_pen;
	bool overkill_ignore = overkill_armor_ignore;
	overkill_armor_ignore = false;  // reset — jednokrotne użycie

	int effective_enemy_armor = enemy_armor_val - effective_pen;
	if (overkill_ignore) {
		// Ignoruje 80% pancerza (po standardowym pen)
		effective_enemy_armor = (int)(effective_enemy_armor * 0.20f);
	}
	if (effective_enemy_armor < 0) effective_enemy_armor = 0;

	float armor_reduction = effective_enemy_armor / (effective_enemy_armor + 100.0f);
	dealt_damage = (int)(dealt_damage * (1.0f - armor_reduction));
	if (dealt_damage < 1) dealt_damage = 1;

	// Oblicz Overkill (nadmiarowe obrażenia)
	int overkill_excess = 0;
	if (dealt_damage > current_enemy_health) {
		overkill_excess = dealt_damage - current_enemy_health;
		dealt_damage = current_enemy_health;
	}

	current_enemy_health -= dealt_damage;

	// Lifesteal
	int currentLifesteal = player_lifesteal;
	// Zabójca: 5% lifesteal startowy
	if (player_class == CLASS_ASSASSIN) currentLifesteal += 5;
	if (vampire_turns > 0) currentLifesteal += vampire_bonus;

	int heal = (dealt_damage * currentLifesteal) / 100;
	player_health += heal;
	if (player_health > player_maxhealth)
		player_health = player_maxhealth;

	// ATAK TRAFIONY — wyświetlenie
	clear_screen();
	std::cout << "\n\t";
	print_colored("Twoja tura!", COLOR_BRIGHT_WHITE);
	std::cout << "\n\n\t";

	if (super_crit) {
		print_colored("PRIMAL STRIKE SUPER KRYTYK (2.25x)!", COLOR_PURPLE);
		std::cout << "\n\t";
	}
	else if (critical) {
		print_colored("TRAFIENIE KRYTYCZNE!", COLOR_PURPLE);
		std::cout << "\n\t";
	}

	if (overkill_ignore) {
		print_colored("[Overkill: ignorujesz 80% pancerza!] ", COLOR_YELLOW);
		std::cout << "\n\t";
	}

	print_colored("Zadałeś ", COLOR_GREEN);
	number_colored(dealt_damage, COLOR_RED);
	print_colored(" obrażeń!", COLOR_GREEN);

	if (effective_enemy_armor > 0 && !overkill_ignore) {
		std::cout << "\n\t";
		print_colored("(Pancerz wroga zredukował część obrażeń)", COLOR_YELLOW);
	}

	std::cout << std::endl << "\n\t";
	print_colored("Przeciwnik ma teraz ", COLOR_BRIGHT_WHITE);
	number_colored(current_enemy_health, COLOR_RED);
	print_colored(" HP!", COLOR_BRIGHT_WHITE);

	if (heal > 0) {
		std::cout << std::endl << "\n\t";
		print_colored("Odnowiono ", COLOR_GREEN);
		number_colored(heal, COLOR_GREEN);
		print_colored(" HP dzięki lifestealowi!", COLOR_GREEN);
	}

	// Overkill: przechowaj nadmiar, sprawdź czy aktywuje ignore armor
	if (overkill_excess > 0) {
		overkill_stored += overkill_excess;

		// Leczenie z Overkill: 5% + 70% armor_pen + 80% armor
		float overkill_heal_pct = 0.05f + (player_armor_pen * 0.70f / 100.0f) + (player_armor * 0.80f / 100.0f);
		int overkill_heal = (int)(overkill_stored * overkill_heal_pct);
		if (overkill_heal > 0) {
			player_health += overkill_heal;
			if (player_health > player_maxhealth) player_health = player_maxhealth;
			std::cout << "\n\t";
			print_colored("OVERKILL! Uleczono ", COLOR_CYAN);
			number_colored(overkill_heal, COLOR_GREEN);
			print_colored(" HP z nadmiaru obrażeń!", COLOR_CYAN);
		}

		// Sprawdź próg: overkill_stored > 50% std::max HP wroga
		if (overkill_stored > (enemy[enemyIndex].health / 2)) {
			overkill_armor_ignore = true;
			std::cout << "\n\t";
			print_colored("OVERKILL PRÓG! Następny atak ignoruje 80% pancerza wroga!", COLOR_YELLOW);
		}
	}

	std::cout << std::endl << std::endl;
	pause_game();

	return (current_enemy_health <= 0);
}

// ===== ATAK PRZECIWNIKA — zwraca true jeśli gracz pokonany =====
bool do_enemy_attack(int enemyIndex) {
	// Sprawdź Mirror of Death PRZED atakiem
	// Oblicz potencjalny dmg wroga
	int effective_player_armor = player_armor - enemy[enemyIndex].armor_pen;
	if (effective_player_armor < 0) effective_player_armor = 0;
	float reductionPercent = effective_player_armor / (effective_player_armor + 100.0f);
	int enemyDamage = current_enemy_damage;
	bool enemyCritical = false;
	int enemyCritRoll = rand() % 100;
	if (enemyCritRoll < enemy[enemyIndex].crit_chance) enemyCritical = true;
	if (enemyCritical) enemyDamage = (int)(enemyDamage * 1.5f);
	int finalDamage = (int)(enemyDamage * (1.0f - reductionPercent));
	if (finalDamage < 1) finalDamage = 1;
	int originalDamage = current_enemy_damage;
	int amountOfDamageReducted = originalDamage - finalDamage;

	// Mirror of Death: sprawdź czy atak byłby śmiertelny
	if (mirror_active) {
		mirror_active = false;  // zużyte niezależnie od wyniku
		bool is_lethal = (finalDamage >= player_health);

		clear_screen();
		std::cout << "\n\t";
		print_colored("Tura Przeciwnika!", COLOR_BRIGHT_WHITE);
		std::cout << "\n\n\t";
		print_colored("Śmiertelne Lustro reaguje!", COLOR_CYAN);
		std::cout << "\n\t";

		if (is_lethal) {
			// Blokuj 99% obrażeń
			int blocked = (int)(finalDamage * 0.99f);
			int taken = finalDamage - blocked;
			if (taken < 1) taken = 1;

			player_health -= taken;

			// 5% z zablokowanych zadaj wrogowi
			int counter_dmg = (int)(blocked * 0.05f);
			if (counter_dmg < 1) counter_dmg = 1;
			if (counter_dmg > current_enemy_health) counter_dmg = current_enemy_health;
			current_enemy_health -= counter_dmg;

			// Sprawdź czy zabiliśmy wroga tym counter-dmg
			bool killed_by_mirror = (current_enemy_health <= 0);

			// Leczenie: 15% z zablokowanych (lub 50% jeśli zabito)
			int mirror_heal;
			if (killed_by_mirror) {
				mirror_heal = (int)(blocked * 0.50f);
			}
			else {
				mirror_heal = (int)(blocked * 0.15f);
			}
			player_health += mirror_heal;
			if (player_health > player_maxhealth) player_health = player_maxhealth;

			print_colored("ATAK ŚMIERTELNY ODPARTY! Zablokowano 99% obrażeń!", COLOR_CYAN);
			std::cout << "\n\t";
			print_colored("Otrzymano tylko ", COLOR_YELLOW);
			number_colored(taken, COLOR_RED);
			print_colored(" obrażeń!", COLOR_YELLOW);
			std::cout << "\n\t";
			print_colored("Kontratak: ", COLOR_GREEN);
			number_colored(counter_dmg, COLOR_RED);
			print_colored(" obrażeń zadanych wrogowi!", COLOR_GREEN);
			std::cout << "\n\t";
			print_colored("Uleczono ", COLOR_GREEN);
			number_colored(mirror_heal, COLOR_GREEN);
			print_colored(killed_by_mirror ? " HP (zabójstwo Lustrem)!" : " HP!", COLOR_GREEN);
			std::cout << "\n\n";
			pause_game();

			if (player_health <= 0) {
				clear_screen();
				std::cout << std::endl << "\tGAME OVER!\n" << std::endl;
				pause_game();
				exit(0);
			}
			return (current_enemy_health <= 0);
		}
		else {
			// Nie śmiertelny: blokuj 10%, zadaj 20% zablokowanych
			int blocked = (int)(finalDamage * 0.10f);
			int taken = finalDamage - blocked;
			if (taken < 1) taken = 1;
			player_health -= taken;

			int counter_dmg = (int)(blocked * 0.20f);
			if (counter_dmg < 1) counter_dmg = 1;
			if (counter_dmg > current_enemy_health) counter_dmg = current_enemy_health;
			current_enemy_health -= counter_dmg;

			print_colored("Atak nie był śmiertelny — Lustro blokuje 10% obrażeń.", COLOR_YELLOW);
			std::cout << "\n\t";
			print_colored("Otrzymano ", COLOR_YELLOW);
			number_colored(taken, COLOR_RED);
			print_colored(" obrażeń, kontratak: ", COLOR_YELLOW);
			number_colored(counter_dmg, COLOR_RED);
			print_colored(" wrogowi!", COLOR_YELLOW);
			std::cout << "\n\n";
			pause_game();

			if (player_health <= 0) {
				clear_screen();
				std::cout << std::endl << "\tGAME OVER!\n" << std::endl;
				pause_game();
				exit(0);
			}
			return (current_enemy_health <= 0);
		}
	}

	// Normalny atak przeciwnika
	int enemyAttack = rand() % 100;
	if (enemyAttack < enemy[enemyIndex].atk_chance) {

		player_health -= finalDamage;

		// ATAK PRZECIWNIKA TRAFIONY
		clear_screen();
		std::cout << "\n\t";
		print_colored("Tura Przeciwnika!", COLOR_BRIGHT_WHITE);
		std::cout << "\n\n\t";

		if (enemyCritical) {
			print_colored("PRZECIWNIK TRAFIŁ KRYTYCZNIE!", COLOR_PURPLE);
			std::cout << "\n\t";
		}
		if (amountOfDamageReducted > 0) {
			ui_colored("Obrażenia przeciwnika zredukowane o: ", amountOfDamageReducted, COLOR_YELLOW);
			std::cout << "\n\t";
		}
		print_colored("Przeciwnik zadał Ci ", COLOR_RED);
		number_colored((int)finalDamage, COLOR_RED);
		print_colored(" obrażeń!", COLOR_RED);
		std::cout << std::endl << std::endl;
		pause_game();
	}
	else {
		// ATAK PRZECIWNIKA NIETRAFIONY
		clear_screen();
		std::cout << "\n\t";
		print_colored("Tura Przeciwnika!", COLOR_BRIGHT_WHITE);
		std::cout << "\n\n\t";
		print_colored("Przeciwnik nie trafił!", COLOR_GREEN);
		std::cout << "\n\t";
		print_colored("Nie otrzymujesz obrażeń!", COLOR_GREEN);
		std::cout << std::endl << std::endl;
		pause_game();
	}

	if (player_health <= 0) {
		clear_screen();
		std::cout << std::endl << "\tGAME OVER!\n" << std::endl;
		pause_game();
		exit(0);
	}
	return false;
}

// ===== OGŁUSZONY — tick i dmg z Pnączy =====
void do_enemy_stun_tick(int enemyIndex) {
	// Dmg z Zabójczych Pnączy podczas ogłuszenia
	int dot_dealt = 0;
	if (cd_deadly_vines > 0) {
		// Pnącza aktywne — zadaj dmg per tura
		int raw_dot = (int)(5 + player_spell_power * 0.03f);
		dot_dealt = deal_magic_damage(raw_dot, enemyIndex);
		if (dot_dealt > current_enemy_health) dot_dealt = current_enemy_health;
		current_enemy_health -= dot_dealt;
	}

	enemy_stun_turns--;

	clear_screen();
	std::cout << "\n\t";
	print_colored("Tura Przeciwnika!", COLOR_BRIGHT_WHITE);
	std::cout << "\n\n\t";
	print_colored("Przeciwnik jest ogłuszony i nie może zaatakować!", COLOR_YELLOW);
	if (dot_dealt > 0) {
		std::cout << "\n\t";
		print_colored("Zabójcze Pnącza zadają ", COLOR_DARK_GREEN);
		number_colored(dot_dealt, COLOR_RED);
		print_colored(" obrażeń!", COLOR_DARK_GREEN);
		apply_spell_vamp(dot_dealt);
	}
	if (enemy_stun_turns > 0) {
		std::cout << "\n\t";
		print_colored("Pozostałe tury ogłuszenia: ", COLOR_YELLOW);
		number_colored(enemy_stun_turns, COLOR_YELLOW);
	}
	std::cout << "\n\n";
	pause_game();
}

void game_battle() {

	clock_t battle_start = clock();

	// Rozszerzone menu walki
	const int choice_amount = 4;
	std::string choices[] = {
		"Walcz",
		"Czary",
		"Przedmioty",
		"Uciekaj"
	};

	int enemyIndex = player_level - 1;

	if (enemyIndex >= enemy_amount) {
		clear_screen();
		std::cout << std::endl;
		std::cout << "\tUkończyłeś wszystkie dostępne poziomy gry!\n";
		std::cout << std::endl;
		std::cout << "\tDzięki za grę :)\n";
		std::cout << std::endl;
		pause_game();
		return;
	}

	total_waves = enemy[enemyIndex].wave;
	current_wave = 1;
	if (enemy[enemyIndex].boss) {
		float hpMultiplier = 1.0f + ((current_wave - 1) * 0.25f);
		current_enemy_max_health = (int)(enemy[enemyIndex].health * hpMultiplier);
		current_enemy_health = current_enemy_max_health;
	}
	else {
		current_enemy_max_health = enemy[enemyIndex].health;
		current_enemy_health = enemy[enemyIndex].health;
	}
	current_enemy_damage = enemy[enemyIndex].damage;

	int choice = 0;
	int keyboard_button;

	while (true) {

		clear_screen();
		std::cout << std::endl;
		print_colored("\tLvl", COLOR_BRIGHT_WHITE);
		std::cout << ": " << player_level << " | ";

		print_colored("Runda", COLOR_BRIGHT_WHITE);
		std::cout << ": " << current_wave << "/" << total_waves << "\n\n";

		std::cout << "\t                    ";
		print_colored(enemy[enemyIndex].name, COLOR_RED);

		// Pokaż ogłuszenie
		if (enemy_stun_turns > 0) {
			print_colored(" [OGŁUSZONY: ", COLOR_YELLOW);
			number_colored(enemy_stun_turns, COLOR_YELLOW);
			print_colored(" tur]", COLOR_YELLOW);
		}

		std::cout << "\n\t                    ";
		draw_enemy_hp_bar(current_enemy_health, current_enemy_max_health);

		std::cout << "\n\n\t                    ";

		print_colored("DMG przeciwnika", COLOR_RED);
		std::cout << ": " << current_enemy_damage;

		// Pancerz i MR przeciwnika
		std::cout << "  |  ";
		print_colored("Pancerz", COLOR_YELLOW);
		std::cout << ": " << enemy[enemyIndex].armor;
		std::cout << "  |  ";
		print_colored("MR", COLOR_CYAN);
		std::cout << ": " << enemy[enemyIndex].magic_resist;

		std::cout << std::endl << std::endl;

		std::cout << "\t";
		print_colored(nickname, player_nickname_color);
		std::cout << ":\n       ";

		draw_player_hp_bar(player_health, player_maxhealth);

		std::cout << "\n\t";

		// Mana
		draw_player_mana_bar(player_mana, player_maxmana);

		std::cout << "\n\n\t";

		print_colored("Broń", COLOR_BRIGHT_WHITE);
		std::cout << ": ";

		print_colored(player_weapon_name, COLOR_CYAN);

		std::cout << " - ";

		// Całkowity DMG = broń + bonus AD z klasy
		int total_dmg = player_weapon_damage + player_class_bonus_ad;
		// Zabójca: +10%
		if (player_class == CLASS_ASSASSIN) total_dmg = (int)(total_dmg * 1.10f);
		number_colored(total_dmg, COLOR_RED);
		print_colored(" DMG", COLOR_RED);

		if (player_class_bonus_ad > 0) {
			print_colored(" (+", COLOR_YELLOW);
			number_colored(player_class_bonus_ad, COLOR_YELLOW);
			print_colored(" AD klasy)", COLOR_YELLOW);
		}

		std::cout << "\n\t";

		print_colored("Mikstury Zdrowia", COLOR_GREEN);

		std::cout << ": " << player_health_potion;

		// Wyświetl aktywne efekty
		if (primal_strike_active) {
			std::cout << "\n\t";
			print_colored("[Pierwotne Uderzenie aktywne!]", COLOR_PURPLE);
		}
		if (mirror_active) {
			std::cout << "\n\t";
			print_colored("[Śmiertelne Lustro aktywne!]", COLOR_CYAN);
		}
		if (undodgable_turns > 0) {
			std::cout << "\n\t";
			print_colored("[Nieomijalna Prędkość: ", COLOR_CYAN);
			number_colored(undodgable_turns, COLOR_CYAN);
			print_colored(" tur]", COLOR_CYAN);
		}
		if (overkill_stored > 0) {
			std::cout << "\n\t";
			print_colored("[Overkill przechowany: ", COLOR_YELLOW);
			number_colored(overkill_stored, COLOR_YELLOW);
			print_colored("]", COLOR_YELLOW);
			if (overkill_armor_ignore) {
				print_colored(" [Ignoruj 80% pancerza!]", COLOR_RED);
			}
		}

		std::cout << "\n\n";

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
				// ===== WALKA FIZYCZNA =====

				bool enemy_killed = do_player_attack(enemyIndex);

				if (enemy_killed) {
					clock_t battle_end = clock();
					float seconds = (float)(battle_end - battle_start) / CLOCKS_PER_SEC;
					handle_wave_kill(enemyIndex, seconds);

					current_wave++;
					battle_start = clock();

					if (current_wave > total_waves) {
						std::cout << "\tGratulacje! Przeszłeś poziom " << player_level << "!\n" << std::endl;
						player_level++;
						player_escape_count = 0;
						overkill_stored = 0;  // reset Overkill na nowy poziom
						save_game();
						pause_game();
						return;
					}
					if (enemy[enemyIndex].boss) {
						float hpMultiplier = 1.0f + ((current_wave - 1) * 0.25f);
						float damageMultiplier = 1.0f + ((current_wave - 1) * 0.15f);
						current_enemy_max_health = (int)(enemy[enemyIndex].health * hpMultiplier);
						current_enemy_health = current_enemy_max_health;
						current_enemy_damage = (int)(enemy[enemyIndex].damage * damageMultiplier);
					}
					else {
						current_enemy_max_health = enemy[enemyIndex].health;
						current_enemy_health = current_enemy_max_health;
						current_enemy_damage = enemy[enemyIndex].damage;
					}
					pause_game();
					continue;
				}

				// ===== ATAK PRZECIWNIKA (tura liczona od kolejnego ataku gracza) =====
				if (enemy_stun_turns > 0) {
					do_enemy_stun_tick(enemyIndex);
				}
				else {
					do_enemy_attack(enemyIndex);
				}

				// Sprawdź czy wróg pokonany przez Mirror/Pnącza
				if (current_enemy_health <= 0) {
					clock_t battle_end = clock();
					float seconds = (float)(battle_end - battle_start) / CLOCKS_PER_SEC;
					handle_wave_kill(enemyIndex, seconds);

					current_wave++;
					battle_start = clock();

					if (current_wave > total_waves) {
						std::cout << "\tGratulacje! Przeszłeś poziom " << player_level << "!\n" << std::endl;
						player_level++;
						player_escape_count = 0;
						overkill_stored = 0;
						save_game();
						pause_game();
						return;
					}
					if (enemy[enemyIndex].boss) {
						float hpMultiplier = 1.0f + ((current_wave - 1) * 0.25f);
						float damageMultiplier = 1.0f + ((current_wave - 1) * 0.15f);
						current_enemy_max_health = (int)(enemy[enemyIndex].health * hpMultiplier);
						current_enemy_health = current_enemy_max_health;
						current_enemy_damage = (int)(enemy[enemyIndex].damage * damageMultiplier);
					}
					else {
						current_enemy_max_health = enemy[enemyIndex].health;
						current_enemy_health = current_enemy_max_health;
						current_enemy_damage = enemy[enemyIndex].damage;
					}
					pause_game();
					continue;
				}

				if (precision_turns > 0) precision_turns--;
				if (vampire_turns > 0)   vampire_turns--;

				// Cooldowny tickują po ataku gracza (od następnego ataku)
				tick_cooldowns();

				// Regeneracja many na koniec tury
				regen_mana();
			}

			else if (choice == 1) {
				// ===== CZARY =====

				// Sprawdź czy gracz ma manę na jakikolwiek dostępny czar (z uwzględnieniem CD)
				bool any_available = false;
				if (player_class == CLASS_ASSASSIN) {
					if (!primal_strike_active && player_mana >= 200)                           any_available = true;
					if (cd_undodgable_speed == 0 && player_mana >= 80)                        any_available = true;
					if (cd_slayer_of_slowest == 0 && player_mana >= 20)                       any_available = true;
				}
				else {
					if (player_mana >= 30)                                                     any_available = true;
					if (cd_star_strike == 0 && player_mana >= 120)                            any_available = true;
					if (cd_deadly_vines == 0 && enemy_stun_turns == 0 && player_mana >= 80)   any_available = true;
					if (!mirror_active && cd_mirror_of_death == 0 && player_mana >= 50)       any_available = true;
				}

				if (!any_available) {
					clear_screen();
					std::cout << "\n\t";
					print_colored("Za mało many lub wszystkie czary na cooldownie!", COLOR_RED);
					std::cout << "\n\n";
					pause_game();
					continue;
				}

				bool spell_used = false;
				battle_spells_menu(enemyIndex, spell_used);

				// Jeśli Powrót / nie użyto czaru — wróć do menu walki bez tury wroga
				if (!spell_used) {
					continue;
				}

				// Po użyciu czaru: sprawdź czy przeciwnik pokonany
				if (current_enemy_health <= 0) {
					clock_t battle_end = clock();
					float seconds = (float)(battle_end - battle_start) / CLOCKS_PER_SEC;
					handle_wave_kill(enemyIndex, seconds);

					current_wave++;
					battle_start = clock();
					enemy_stun_turns = 0;

					if (current_wave > total_waves) {
						std::cout << "\tGratulacje! Przeszłeś poziom " << player_level << "!\n" << std::endl;
						player_level++;
						player_escape_count = 0;
						overkill_stored = 0;
						save_game();
						pause_game();
						return;
					}
					if (enemy[enemyIndex].boss) {
						float hpMultiplier = 1.0f + ((current_wave - 1) * 0.25f);
						float damageMultiplier = 1.0f + ((current_wave - 1) * 0.15f);
						current_enemy_max_health = (int)(enemy[enemyIndex].health * hpMultiplier);
						current_enemy_health = current_enemy_max_health;
						current_enemy_damage = (int)(enemy[enemyIndex].damage * damageMultiplier);
					}
					else {
						current_enemy_max_health = enemy[enemyIndex].health;
						current_enemy_health = current_enemy_max_health;
						current_enemy_damage = enemy[enemyIndex].damage;
					}
					pause_game();
					continue;
				}

				// Po czarze: tura przeciwnika
				if (enemy_stun_turns > 0) {
					do_enemy_stun_tick(enemyIndex);
				}
				else {
					do_enemy_attack(enemyIndex);
				}

				// Sprawdź czy wróg pokonany przez Mirror/Pnącza podczas tury czarów
				if (current_enemy_health <= 0) {
					clock_t battle_end = clock();
					float seconds = (float)(battle_end - battle_start) / CLOCKS_PER_SEC;
					handle_wave_kill(enemyIndex, seconds);

					current_wave++;
					battle_start = clock();

					if (current_wave > total_waves) {
						std::cout << "\tGratulacje! Przeszłeś poziom " << player_level << "!\n" << std::endl;
						player_level++;
						player_escape_count = 0;
						overkill_stored = 0;
						save_game();
						pause_game();
						return;
					}
					if (enemy[enemyIndex].boss) {
						float hpMultiplier = 1.0f + ((current_wave - 1) * 0.25f);
						float damageMultiplier = 1.0f + ((current_wave - 1) * 0.15f);
						current_enemy_max_health = (int)(enemy[enemyIndex].health * hpMultiplier);
						current_enemy_health = current_enemy_max_health;
						current_enemy_damage = (int)(enemy[enemyIndex].damage * damageMultiplier);
					}
					else {
						current_enemy_max_health = enemy[enemyIndex].health;
						current_enemy_health = current_enemy_max_health;
						current_enemy_damage = enemy[enemyIndex].damage;
					}
					pause_game();
					continue;
				}

				if (precision_turns > 0) precision_turns--;
				if (vampire_turns > 0)   vampire_turns--;

				// Cooldowny tickują po akcji czarów
				tick_cooldowns();

				// Regeneracja many na koniec tury
				regen_mana();
			}

			else if (choice == 2) {

				battle_items_menu();
			}

			else if (choice == 3) {

				int maxEscapes = 1;

				if (player_escape_count >= maxEscapes) {
					clear_screen();
					std::cout << std::endl;
					std::cout << "\tMożesz uciec tylko raz na poziom!\n" << std::endl;
					pause_game();
					continue;
				}

				clear_screen();
				int heal = player_maxhealth / 2;
				int mana_restore = player_maxmana / 2;
				player_health += heal;
				player_mana += mana_restore;
				if (player_health > player_maxhealth)
					player_health = player_maxhealth;
				if (player_mana > player_maxmana)
					player_mana = player_maxmana;

				player_escape_count++;

				std::cout << std::endl;
				std::cout << "\tUciekasz z pola walki!\n";
				std::cout << "\tOdzyskano " << heal << " HP!\n";
				std::cout << "\tOdzyskano " << mana_restore << " many!\n" << std::endl;
				pause_game();
				return;
			}
		}
	}
}

void battle_items_menu() {

	int choice = 0;
	int keyboard_button;

	const int item_amount = 4;

	std::string items[item_amount] = {
		"Mikstura Zdrowia",
		"Eliksir Precyzji",
		"Koktajl Wampira",
		"Powrót"
	};

	while (true) {

		clear_screen();

		print_colored("\n\tPrzedmioty\n\n", COLOR_BRIGHT_WHITE);

		std::cout << "\t";
		print_colored("Mikstura Zdrowia: ", COLOR_GREEN);
		if (player_health_potion > 0)
			number_colored(player_health_potion, COLOR_GREEN);
		else
			number_colored(player_health_potion, COLOR_RED);

		std::cout << "\n\t";
		print_colored("Eliksir Precyzji: ", COLOR_CYAN);
		if (player_precision_potion > 0)
			number_colored(player_precision_potion, COLOR_CYAN);
		else
			number_colored(player_precision_potion, COLOR_RED);

		std::cout << "\n\t";
		print_colored("Koktajl Wampira: ", COLOR_PURPLE);
		if (player_vampire_potion > 0)
			number_colored(player_vampire_potion, COLOR_PURPLE);
		else
			number_colored(player_vampire_potion, COLOR_RED);

		std::cout << "\n\n";

		for (int i = 0; i < item_amount; i++) {
			if (i == choice) {
				std::cout << "\t►  "; print_colored(items[i], COLOR_BRIGHT_WHITE); std::cout << std::endl;
			}
			else
				std::cout << "\t  " << items[i] << std::endl;
		}

		keyboard_button = _getch();

		if (keyboard_button == 224) {

			keyboard_button = _getch();

			if (keyboard_button == 72) {

				choice--;

				if (choice < 0)
					choice = item_amount - 1;
			}
			else if (keyboard_button == 80) {

				choice++;

				if (choice >= item_amount)
					choice = 0;
			}
		}
		else if (keyboard_button == 13) {

			// Mikstura Zdrowia
			if (choice == 0) {

				if (player_health_potion <= 0) {

					clear_screen();

					std::cout << "\n\tNie masz Mikstur Zdrowia!\n\n";

					pause_game();
					continue;
				}

				int healAmount = (int)(25 + (player_maxhealth * 0.07f));

				if (healAmount > 250)
					healAmount = 250;

				player_health += healAmount;

				if (player_health > player_maxhealth)
					player_health = player_maxhealth;

				player_health_potion--;

				clear_screen();

				std::cout << "\n\t";
				print_colored("Użyto Mikstury Zdrowia!", COLOR_GREEN);
				std::cout << std::endl;
				std::cout << "\t";
				print_colored("Odzyskano ", COLOR_GREEN);
				number_colored(healAmount, COLOR_GREEN);
				print_colored(" HP!", COLOR_GREEN);
				std::cout << "\n\n";

				pause_game();
				return;
			}

			// Eliksir Precyzji
			else if (choice == 1) {

				if (player_precision_potion <= 0) {

					clear_screen();

					std::cout << "\n\tNie masz Eliksiru Precyzji!\n\n";

					pause_game();
					continue;
				}

				if (precision_turns > 0) {

					clear_screen();

					std::cout << "\n\tEfekt Eliksiru Precyzji jest już aktywny!\n\n";

					pause_game();
					continue;
				}

				precision_bonus = 50;
				precision_turns = 7;

				player_precision_potion--;

				clear_screen();

				std::cout << "\n\t";
				print_colored("Użyto Eliksiru Precyzji!", COLOR_CYAN);
				std::cout << "\n\t";
				print_colored("+50% szansy trafienia na 7 tur!", COLOR_CYAN);
				std::cout << "\n\n";
				pause_game();
				return;
			}

			// Koktajl Wampira
			else if (choice == 2) {

				if (player_vampire_potion <= 0) {

					clear_screen();

					std::cout << "\n\tNie masz Koktajlu Wampira!\n\n";

					pause_game();
					continue;
				}

				if (vampire_turns > 0) {

					clear_screen();

					std::cout << "\n\tEfekt Koktajlu Wampira jest już aktywny!\n\n";

					pause_game();
					continue;
				}

				vampire_bonus = 10;
				vampire_turns = 10;

				player_vampire_potion--;

				clear_screen();

				std::cout << "\n\t";
				print_colored("Użyto Koktajlu Wampira!", COLOR_PURPLE);
				std::cout << "\n\t";
				print_colored("+10% lifestealu na 10 tur!", COLOR_PURPLE);
				std::cout << "\n\n";
				pause_game();
				return;
			}

			// Powrót
			else if (choice == 3) {

				return;
			}
		}
		else if (keyboard_button == 27) {

			return;
		}
	}
}
