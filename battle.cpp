#include "battle.h"
#include "globals.h"
#include "utils.h"
#include "save.h"
#include <iostream>
#include <cstdlib>
using namespace std;

// ===== POMOCNICZE: regeneracja many na koniec tury =====
void regen_mana() {
	// Baza: 3% max many na turę + bonus z Tarczy Anioła (100% bazowej regen = +3%)
	int base_regen_bonus_pct = 0;
	for (int i = 0; i < MAX_MAGIC_ITEMS; i++) {
		if (magic_item_slots[i] >= 0) {
			base_regen_bonus_pct += magic_items[magic_item_slots[i]].base_mana_regen_bonus;
		}
	}
	// Bazowa regen = 3% max many
	int base_regen = (int)(player_maxmana * 0.03f);
	// Bonus z itemów (100% bazy = +base_regen)
	int bonus_regen = (base_regen * base_regen_bonus_pct) / 100;
	int total_regen = base_regen + bonus_regen;
	player_mana += total_regen;
	if (player_mana > player_maxmana)
		player_mana = player_maxmana;
}

// ===== CZARY (Spells) =====

// Śmiertelne Pnącza: mały damage + stun 2 tury
void cast_deadly_vines(int enemyIndex) {
	int mana_cost = 40;
	if (player_mana < mana_cost) {
		clear_screen();
		cout << "\n\t";
		print_colored("Za mało many! (potrzebujesz 40)", COLOR_RED);
		cout << "\n\n";
		pause_game();
		return;
	}
	if (enemy_stun_turns > 0) {
		clear_screen();
		cout << "\n\t";
		print_colored("Przeciwnik jest już ogłuszony!", COLOR_YELLOW);
		cout << "\n\n";
		pause_game();
		return;
	}

	player_mana -= mana_cost;

	// Damage magiczny: (20 + spell_power * 0.4), zredukowany przez MR przeciwnika
	int raw_damage = (int)(20 + player_spell_power * 0.4f);

	// Efektywny magic resist po przebiciu
	int enemy_mr = enemy[enemyIndex].magic_resist;
	int effective_mr = enemy_mr - player_magic_resist_pen;
	if (effective_mr < 0) effective_mr = 0;

	float mr_reduction = effective_mr / (effective_mr + 100.0f);
	int dealt_damage = (int)(raw_damage * (1.0f - mr_reduction));
	if (dealt_damage < 1) dealt_damage = 1;
	if (dealt_damage > current_enemy_health) dealt_damage = current_enemy_health;

	current_enemy_health -= dealt_damage;
	enemy_stun_turns = 2;  // blokada ruchu na 2 następne tury

	clear_screen();
	cout << "\n\t";
	print_colored("Rzucono: Śmiertelne Pnącza!", COLOR_DARK_GREEN);
	cout << "\n\t";
	print_colored("Zadano ", COLOR_GREEN);
	number_colored(dealt_damage, COLOR_RED);
	print_colored(" obrażeń magicznych!", COLOR_GREEN);
	cout << "\n\t";
	print_colored("Przeciwnik zostaje ogłuszony na 2 tury!", COLOR_YELLOW);
	cout << "\n\t";
	print_colored("Pozostałe HP przeciwnika: ", COLOR_BRIGHT_WHITE);
	number_colored(current_enemy_health, COLOR_RED);
	cout << "\n\n";
	pause_game();
}

// Gwiezdne Uderzenie: wysoki burst damage, wysoki koszt many
void cast_star_strike(int enemyIndex) {
	int mana_cost = 80;
	if (player_mana < mana_cost) {
		clear_screen();
		cout << "\n\t";
		print_colored("Za mało many! (potrzebujesz 80)", COLOR_RED);
		cout << "\n\n";
		pause_game();
		return;
	}

	player_mana -= mana_cost;

	// Damage magiczny: (80 + spell_power * 1.2), zredukowany przez MR przeciwnika
	int raw_damage = (int)(80 + player_spell_power * 1.2f);

	// Efektywny magic resist po przebiciu
	int enemy_mr = enemy[enemyIndex].magic_resist;
	int effective_mr = enemy_mr - player_magic_resist_pen;
	if (effective_mr < 0) effective_mr = 0;

	float mr_reduction = effective_mr / (effective_mr + 100.0f);
	int dealt_damage = (int)(raw_damage * (1.0f - mr_reduction));
	if (dealt_damage < 1) dealt_damage = 1;
	if (dealt_damage > current_enemy_health) dealt_damage = current_enemy_health;

	current_enemy_health -= dealt_damage;

	clear_screen();
	cout << "\n\t";
	print_colored("Rzucono: Gwiezdne Uderzenie!", COLOR_YELLOW);
	cout << "\n\t";
	print_colored("POTĘŻNY cios magiczny! Zadano ", COLOR_YELLOW);
	number_colored(dealt_damage, COLOR_RED);
	print_colored(" obrażeń!", COLOR_YELLOW);
	cout << "\n\t";
	print_colored("Pozostałe HP przeciwnika: ", COLOR_BRIGHT_WHITE);
	number_colored(current_enemy_health, COLOR_RED);
	cout << "\n\n";
	pause_game();
}

// ===== MENU CZARÓW =====
void battle_spells_menu(int enemyIndex) {

	int choice = 0;
	int keyboard_button;

	const int spell_amount = 3;
	string spells[spell_amount] = {
		"Śmiertelne Pnącza  [40 many] - mały dmg + stun 2 tury",
		"Gwiezdne Uderzenie  [80 many] - burst damage",
		"Powrót"
	};

	while (true) {

		clear_screen();

		print_colored("\n\tCzary\n\n", COLOR_BRIGHT_WHITE);

		cout << "\t";
		print_colored("Mana: ", COLOR_BLUE);
		number_colored(player_mana, COLOR_BLUE);
		cout << " / ";
		number_colored(player_maxmana, COLOR_BLUE);

		cout << "  |  ";
		print_colored("Siła zaklęć: ", COLOR_PURPLE);
		number_colored(player_spell_power, COLOR_PURPLE);

		cout << "\n\n";

		for (int i = 0; i < spell_amount; i++) {
			if (i == choice) {
				cout << "\t►  "; print_colored(spells[i], COLOR_BRIGHT_WHITE); cout << endl;
			}
			else
				cout << "\t  " << spells[i] << endl;
		}

		keyboard_button = _getch();

		if (keyboard_button == 224) {
			keyboard_button = _getch();
			if (keyboard_button == 72) {
				choice--;
				if (choice < 0) choice = spell_amount - 1;
			}
			else if (keyboard_button == 80) {
				choice++;
				if (choice >= spell_amount) choice = 0;
			}
		}
		else if (keyboard_button == 13) {
			if (choice == 0) {
				cast_deadly_vines(enemyIndex);
				return;
			}
			else if (choice == 1) {
				cast_star_strike(enemyIndex);
				return;
			}
			else if (choice == 2) {
				return;
			}
		}
		else if (keyboard_button == 27) {
			return;
		}
	}
}

void game_battle() {

	clock_t battle_start = clock();

	// Rozszerzone menu walki
	const int choice_amount = 4;
	string choices[] = {
		"Walcz",
		"Czary",
		"Przedmioty",
		"Uciekaj"
	};

	int enemyIndex = player_level - 1;

	if (enemyIndex >= enemy_amount) {
		clear_screen();
		cout << endl;
		cout << "\tUkończyłeś wszystkie dostępne poziomy gry!\n";
		cout << endl;
		cout << "\tDzięki za grę :)\n";
		cout << endl;
		pause_game();
		return;
	}

	total_waves = enemy[enemyIndex].wave;
	current_wave = 1;
	if (enemy[enemyIndex].boss) {

		float hpMultiplier =
			1.0f + ((current_wave - 1) * 0.25f);

		current_enemy_max_health =
			(int)(enemy[enemyIndex].health * hpMultiplier);

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
		cout << endl;
		print_colored("\tLvl", COLOR_BRIGHT_WHITE);
		cout << ": " << player_level << " | ";

		print_colored("Runda", COLOR_BRIGHT_WHITE);
		cout << ": " << current_wave << "/" << total_waves << "\n\n";

		cout << "\t                    ";
		print_colored(enemy[enemyIndex].name, COLOR_RED);

		// Pokaż ogłuszenie
		if (enemy_stun_turns > 0) {
			print_colored(" [OGŁUSZONY: ", COLOR_YELLOW);
			number_colored(enemy_stun_turns, COLOR_YELLOW);
			print_colored(" tur]", COLOR_YELLOW);
		}

		cout << "\n\t                    ";
		draw_enemy_hp_bar(current_enemy_health, current_enemy_max_health);

		cout << "\n\n\t                    ";

		print_colored("DMG przeciwnika", COLOR_RED);
		cout << ": " << current_enemy_damage;

		// Pancerz i MR przeciwnika
		cout << "  |  ";
		print_colored("Pancerz", COLOR_YELLOW);
		cout << ": " << enemy[enemyIndex].armor;
		cout << "  |  ";
		print_colored("MR", COLOR_CYAN);
		cout << ": " << enemy[enemyIndex].magic_resist;

		cout << endl << endl;

		cout << "\t";
		print_colored(nickname, player_nickname_color);
		cout << ":\n       ";

		draw_player_hp_bar(player_health, player_maxhealth);

		cout << "\n\t";

		// Mana
		draw_player_mana_bar(player_mana, player_maxmana);

		cout << "\n\n\t";

		print_colored("Broń", COLOR_BRIGHT_WHITE);
		cout << ": ";

		print_colored(player_weapon_name, COLOR_CYAN);

		cout << " - ";

		// Całkowity DMG = broń + bonus AD z klasy
		int total_dmg = player_weapon_damage + player_class_bonus_ad;
		number_colored(total_dmg, COLOR_RED);
		print_colored(" DMG", COLOR_RED);

		if (player_class_bonus_ad > 0) {
			print_colored(" (+", COLOR_YELLOW);
			number_colored(player_class_bonus_ad, COLOR_YELLOW);
			print_colored(" AD klasy)", COLOR_YELLOW);
		}

		cout << "\n\t";

		print_colored("Mikstury Zdrowia", COLOR_GREEN);

		cout << ": " << player_health_potion << "\n\n";

		for (int i = 0; i < choice_amount; i++) {
			if (i == choice) {
				cout << "\t►  "; print_colored(choices[i], COLOR_BRIGHT_WHITE); cout << endl;
			}
			else
				cout << "\t  " << choices[i] << endl;
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

				int miss = rand() % 100;
				int bonusAccuracy = player_bonus_accuracy;

				if (precision_turns > 0)
					bonusAccuracy += precision_bonus;

				int final_dodge = enemy[enemyIndex].dodge_chance - bonusAccuracy;
				if (final_dodge < 0) final_dodge = 0;

				if (miss < final_dodge) {
					// ATAK NIETRAFIONY

					clear_screen();
					cout << "\n\t";
					print_colored("Twoja tura!", COLOR_BRIGHT_WHITE);
					cout << "\n";
					cout << "\n\t";
					print_colored("Przeciwnik uniknął twojego ataku!", COLOR_RED);
					cout << endl << endl;

					pause_game();
				}
				else {
					bool critical = false;

					int critRoll = rand() % 100;

					if (critRoll < player_crit_chance) {
						critical = true;
					}

					// Całkowity DMG gracza: broń + bonus AD klasy
					int total_player_dmg = player_weapon_damage + player_class_bonus_ad;
					// Tank: -25% obrażeń dla broni powyżej 500 DMG
					if (player_class == CLASS_TANK && player_weapon_damage > 500) {
						total_player_dmg = (int)(total_player_dmg * 0.75f);
					}
					int dealt_damage = total_player_dmg;

					if (critical)
						dealt_damage = (int)(dealt_damage * 1.5f);

					// Redukcja przez pancerz przeciwnika (uwzględnia armor pen gracza)
					int enemy_armor_val = enemy[enemyIndex].armor;
					int effective_enemy_armor = enemy_armor_val - player_armor_pen;
					if (effective_enemy_armor < 0) effective_enemy_armor = 0;

					float armor_reduction = effective_enemy_armor / (effective_enemy_armor + 100.0f);
					dealt_damage = (int)(dealt_damage * (1.0f - armor_reduction));
					if (dealt_damage < 1) dealt_damage = 1;

					if (dealt_damage > current_enemy_health)
						dealt_damage = current_enemy_health;

					current_enemy_health -= dealt_damage;

					int currentLifesteal = player_lifesteal;

					if (vampire_turns > 0)
						currentLifesteal += vampire_bonus;

					int heal = (dealt_damage * currentLifesteal) / 100;
					player_health += heal;
					if (player_health > player_maxhealth)
						player_health = player_maxhealth;

					// ATAK TRAFIONY

					clear_screen();
					cout << "\n\t";
					print_colored("Twoja tura!", COLOR_BRIGHT_WHITE);
					cout << "\n";
					cout << "\n\t";

					if (critical) {
						print_colored("TRAFIENIE KRYTYCZNE!", COLOR_PURPLE);

						cout << "\n\t";
					}
					print_colored("Zadałeś ", COLOR_GREEN);
					number_colored(dealt_damage, COLOR_RED);
					print_colored(" obrażeń!", COLOR_GREEN);

					if (effective_enemy_armor > 0) {
						cout << "\n\t";
						print_colored("(Pancerz wroga zredukował część obrazen)", COLOR_YELLOW);
					}

					cout << endl;
					cout << "\n\t";
					print_colored("Przeciwnik ma teraz ", COLOR_BRIGHT_WHITE);
					number_colored(current_enemy_health, COLOR_RED);
					print_colored(" HP!", COLOR_BRIGHT_WHITE);

					if (heal > 0) {
						cout << endl;
						cout << "\n\t";
						print_colored("Odnowiono ", COLOR_GREEN);
						number_colored(heal, COLOR_GREEN);
						print_colored(" HP dzięki Wampirycznemu Ostrzu!", COLOR_GREEN);
					}

					cout << endl << endl;
					pause_game();
				}

				if (current_enemy_health <= 0) {
					clock_t battle_end = clock();
					float seconds = (float)(battle_end - battle_start) / CLOCKS_PER_SEC;

					float multiplier = 1.0f;

					if (seconds < 8)
						multiplier = 1.5f;
					else if (seconds < 15)
						multiplier = 1.2f;

					int base_reward = enemy[enemyIndex].kill_reward;
					int final_reward = (int)(base_reward * multiplier);

					player_money += final_reward;

					int bonus_money_amount = final_reward - base_reward;

					clear_screen();

					cout << "\n\t";
					print_colored("Przeciwnik pokonany!", COLOR_GREEN);
					cout << "\n";
					cout << "\n\t";
					print_colored("Otrzymujesz: $", COLOR_DARK_GREEN);
					number_colored(base_reward, COLOR_DARK_GREEN);

					print_colored(" + $", COLOR_YELLOW);
					number_colored(bonus_money_amount, COLOR_YELLOW);

					print_colored(" za czas walki!", COLOR_BRIGHT_WHITE);

					cout << "\n\t";

					print_colored("Suma: $", COLOR_GREEN);
					number_colored(final_reward, COLOR_GREEN);

					cout << endl;
					current_wave++;
					battle_start = clock();

					// Reset stunu po pokonaniu fali
					enemy_stun_turns = 0;

					if (player_second_breath) {

						int healAmount = (int)(player_maxhealth * 0.25f);
						int manaAmount = (int)(player_maxmana * 0.25f);

						player_health += healAmount;
						player_mana += manaAmount;

						if (player_health > player_maxhealth)
							player_health = player_maxhealth;
						if (player_mana > player_maxmana)
							player_mana = player_maxmana;

						cout << "\n\t";
						ui_colored("Drugie Tchnienie przywróciło: ", healAmount, COLOR_GREEN); print_colored(" HP!", COLOR_GREEN);
						cout << "\n\t";
						ui_colored("Drugie Tchnienie przywróciło: ", manaAmount, COLOR_BLUE); print_colored(" many!", COLOR_BLUE); cout << "\n\n";
					}

					if (current_wave > total_waves) {
						cout << "\tGratulacje! Przeszłeś poziom " << player_level << "!\n" << endl;
						player_level++;
						player_escape_count = 0;
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

				// ===== ATAK PRZECIWNIKA =====

				// Sprawdź stun - jeśli ogłuszony, nie atakuje
				if (enemy_stun_turns > 0) {
					enemy_stun_turns--;

					clear_screen();
					cout << "\n\t";
					print_colored("Tura Przeciwnika!", COLOR_BRIGHT_WHITE);
					cout << "\n\n\t";
					print_colored("Przeciwnik jest ogłuszony i nie może zaatakować!", COLOR_YELLOW);
					if (enemy_stun_turns > 0) {
						cout << "\n\t";
						print_colored("Pozostałe tury ogłuszenia: ", COLOR_YELLOW);
						number_colored(enemy_stun_turns, COLOR_YELLOW);
					}
					cout << "\n\n";
					pause_game();
				}
				else {
					int enemyAttack = rand() % 100;
					if (enemyAttack < enemy[enemyIndex].atk_chance) {

						// Pancerz gracza uwzględnia przebicie przeciwnika (per wróg)
						int effective_player_armor = player_armor - enemy[enemyIndex].armor_pen;
						if (effective_player_armor < 0) effective_player_armor = 0;

						float reductionPercent = effective_player_armor / (effective_player_armor + 100.0f);

						int originalDamage = current_enemy_damage;
						bool enemyCritical = false;

						int enemyCritRoll = rand() % 100;

						if (enemyCritRoll < enemy[enemyIndex].crit_chance)
							enemyCritical = true;

						int enemyDamage = current_enemy_damage;

						if (enemyCritical)
							enemyDamage = (int)(enemyDamage * 1.5f);

						int finalDamage = (int)(enemyDamage * (1.0f - reductionPercent));

						if (finalDamage < 1)
							finalDamage = 1;

						// Redukcja MR gracza wobec ewentualnych obrażeń magicznych (currently fizyczne)
						// Pole zostawione na przyszłość

						int amountOfDamageReducted = originalDamage - finalDamage;

						player_health -= finalDamage;
						// ATAK PRZECIWNIKA TRAFIONY

						clear_screen();
						cout << "\n\t";
						print_colored("Tura Przeciwnika!", COLOR_BRIGHT_WHITE);
						cout << "\n";
						cout << "\n\t";

						if (enemyCritical) {
							print_colored("PRZECIWNIK TRAFIŁ KRYTYCZNIE!", COLOR_PURPLE);

							cout << "\n\t";
						}
						if (amountOfDamageReducted > 0) {
							ui_colored("Obrażenia przeciwnika zredukowane o: ", amountOfDamageReducted, COLOR_YELLOW);
							cout << "\n\t";
						}
						print_colored("Przeciwnik zadał Ci ", COLOR_RED);
						number_colored((int)finalDamage, COLOR_RED);
						print_colored(" obrażeń!", COLOR_RED);

						cout << endl << endl;
						pause_game();
					}
					else {
						// NIETRAFIONY
						clear_screen();
						cout << "\n\t";
						print_colored("Tura Przeciwnika!", COLOR_BRIGHT_WHITE);
						cout << "\n";
						cout << "\n\t";

						print_colored("Przeciwnik nie trafił!", COLOR_GREEN);
						cout << "\n\t";
						print_colored("Nie otrzymujesz obrażeń!", COLOR_GREEN);

						cout << endl << endl;
						pause_game();
					}
				}

				if (player_health <= 0) {
					clear_screen();
					cout << endl << "\tGAME OVER!\n" << endl;
					pause_game();
					exit(0);
				}

				if (precision_turns > 0)
					precision_turns--;

				if (vampire_turns > 0)
					vampire_turns--;

				// Regeneracja many na koniec tury
				regen_mana();
			}

			else if (choice == 1) {
				// ===== CZARY =====
				battle_spells_menu(enemyIndex);

				// Po użyciu czaru: sprawdź czy przeciwnik pokonany
				if (current_enemy_health <= 0) {
					clock_t battle_end = clock();
					float seconds = (float)(battle_end - battle_start) / CLOCKS_PER_SEC;

					float multiplier = 1.0f;
					if (seconds < 8) multiplier = 1.5f;
					else if (seconds < 15) multiplier = 1.2f;

					int base_reward = enemy[enemyIndex].kill_reward;
					int final_reward = (int)(base_reward * multiplier);
					player_money += final_reward;
					int bonus_money_amount = final_reward - base_reward;

					clear_screen();
					cout << "\n\t";
					print_colored("Przeciwnik pokonany!", COLOR_GREEN);
					cout << "\n\n\t";
					print_colored("Otrzymujesz: $", COLOR_DARK_GREEN);
					number_colored(base_reward, COLOR_DARK_GREEN);
					print_colored(" + $", COLOR_YELLOW);
					number_colored(bonus_money_amount, COLOR_YELLOW);
					print_colored(" za czas walki!", COLOR_BRIGHT_WHITE);
					cout << "\n\t";
					print_colored("Suma: $", COLOR_GREEN);
					number_colored(final_reward, COLOR_GREEN);
					cout << endl;

					current_wave++;
					battle_start = clock();
					enemy_stun_turns = 0;

					if (player_second_breath) {
						int healAmount = (int)(player_maxhealth * 0.25f);
						int manaAmount = (int)(player_maxmana * 0.25f);
						player_health += healAmount;
						player_mana += manaAmount;
						if (player_health > player_maxhealth) player_health = player_maxhealth;
						if (player_mana > player_maxmana) player_mana = player_maxmana;
						cout << "\n\t";
						ui_colored("Drugie Tchnienie przywróciło: ", healAmount, COLOR_GREEN); print_colored(" HP,", COLOR_GREEN);
						cout << "\n\t";
						ui_colored("oraz: ", manaAmount, COLOR_BLUE); print_colored(" many!", COLOR_BLUE); cout << "\n\n";
					}

					if (current_wave > total_waves) {
						cout << "\tGratulacje! Przeszłeś poziom " << player_level << "!\n" << endl;
						player_level++;
						player_escape_count = 0;
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

				// Po czarze: tura przeciwnika (jeśli nie ogłuszony)
				if (enemy_stun_turns > 0) {
					enemy_stun_turns--;
					clear_screen();
					cout << "\n\t";
					print_colored("Tura Przeciwnika!", COLOR_BRIGHT_WHITE);
					cout << "\n\n\t";
					print_colored("Przeciwnik jest ogłuszony i nie może zaatakować!", COLOR_YELLOW);
					if (enemy_stun_turns > 0) {
						cout << "\n\t";
						print_colored("Pozostałe tury ogłuszenia: ", COLOR_YELLOW);
						number_colored(enemy_stun_turns, COLOR_YELLOW);
					}
					cout << "\n\n";
					pause_game();
				}
				else {
					int enemyAttack = rand() % 100;
					if (enemyAttack < enemy[enemyIndex].atk_chance) {

						int effective_player_armor = player_armor - enemy[enemyIndex].armor_pen;
						if (effective_player_armor < 0) effective_player_armor = 0;

						float reductionPercent = effective_player_armor / (effective_player_armor + 100.0f);
						int originalDamage = current_enemy_damage;
						bool enemyCritical = false;
						int enemyCritRoll = rand() % 100;
						if (enemyCritRoll < enemy[enemyIndex].crit_chance) enemyCritical = true;

						int enemyDamage = current_enemy_damage;
						if (enemyCritical) enemyDamage = (int)(enemyDamage * 1.5f);

						int finalDamage = (int)(enemyDamage * (1.0f - reductionPercent));
						if (finalDamage < 1) finalDamage = 1;

						int amountOfDamageReducted = originalDamage - finalDamage;
						player_health -= finalDamage;

						clear_screen();
						cout << "\n\t";
						print_colored("Tura Przeciwnika!", COLOR_BRIGHT_WHITE);
						cout << "\n\n\t";
						if (enemyCritical) { print_colored("PRZECIWNIK TRAFIŁ KRYTYCZNIE!", COLOR_PURPLE); cout << "\n\t"; }
						if (amountOfDamageReducted > 0) { ui_colored("Obrażenia przeciwnika zredukowane o: ", amountOfDamageReducted, COLOR_YELLOW); cout << "\n\t"; }
						print_colored("Przeciwnik zadał Ci ", COLOR_RED);
						number_colored((int)finalDamage, COLOR_RED);
						print_colored(" obrażeń!", COLOR_RED);
						cout << endl << endl;
						pause_game();
					}
					else {
						clear_screen();
						cout << "\n\t";
						print_colored("Tura Przeciwnika!", COLOR_BRIGHT_WHITE);
						cout << "\n\n\t";
						print_colored("Przeciwnik nie trafił!", COLOR_GREEN);
						cout << "\n\t";
						print_colored("Nie otrzymujesz obrażeń!", COLOR_GREEN);
						cout << endl << endl;
						pause_game();
					}
				}

				if (player_health <= 0) {
					clear_screen();
					cout << endl << "\tGAME OVER!\n" << endl;
					pause_game();
					exit(0);
				}

				if (precision_turns > 0) precision_turns--;
				if (vampire_turns > 0) vampire_turns--;

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
					cout << endl;
					cout << "\tMożesz uciec tylko raz na poziom!\n" << endl;

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

				cout << endl;
				cout << "\tUciekasz z pola walki!\n";
				cout << "\tOdzyskano " << heal << " HP!\n";
				cout << "\tOdzyskano " << mana_restore << " many!\n" << endl;
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

	string items[item_amount] = {
		"Mikstura Zdrowia",
		"Eliksir Precyzji",
		"Koktajl Wampira",
		"Powrót"
	};

	while (true) {

		clear_screen();

		cout << "\n\tPrzedmioty\n\n";

		clear_screen();

		print_colored("\n\tPrzedmioty\n\n", COLOR_BRIGHT_WHITE);

		cout << "\t";
		print_colored("Mikstura Zdrowia: ", COLOR_GREEN);
		if (player_health_potion > 0)
			number_colored(player_health_potion, COLOR_GREEN);
		else
			number_colored(player_health_potion, COLOR_RED);

		cout << "\n\t";
		print_colored("Eliksir Precyzji: ", COLOR_CYAN);
		if (player_precision_potion > 0)
			number_colored(player_precision_potion, COLOR_CYAN);
		else
			number_colored(player_precision_potion, COLOR_RED);

		cout << "\n\t";
		print_colored("Koktajl Wampira: ", COLOR_PURPLE);
		if (player_vampire_potion > 0)
			number_colored(player_vampire_potion, COLOR_PURPLE);
		else
			number_colored(player_vampire_potion, COLOR_RED);

		cout << "\n\n";

		for (int i = 0; i < item_amount; i++) {
			if (i == choice) {
				cout << "\t►  "; print_colored(items[i], COLOR_BRIGHT_WHITE); cout << endl;
			}
			else
				cout << "\t  " << items[i] << endl;
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

					cout << "\n\tNie masz Mikstur Zdrowia!\n\n";

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

				cout << "\n\t";
				print_colored("Użyto Mikstury Zdrowia!", COLOR_GREEN);
				cout << endl;
				cout << "\t";
				print_colored("Odzyskano ", COLOR_GREEN);
				number_colored(healAmount, COLOR_GREEN);
				print_colored(" HP!", COLOR_GREEN);
				cout << "\n\n";

				pause_game();
				return;
			}

			// Eliksir Precyzji
			else if (choice == 1) {

				if (player_precision_potion <= 0) {

					clear_screen();

					cout << "\n\tNie masz Eliksiru Precyzji!\n\n";

					pause_game();
					continue;
				}

				if (precision_turns > 0) {

					clear_screen();

					cout << "\n\tEfekt Eliksiru Precyzji jest już aktywny!\n\n";

					pause_game();
					continue;
				}

				precision_bonus = 50;
				precision_turns = 7;

				player_precision_potion--;

				clear_screen();

				cout << "\n\t";
				print_colored("Użyto Eliksiru Precyzji!", COLOR_CYAN);
				cout << "\n\t";
				print_colored("+50% szansy trafienia na 7 tur!", COLOR_CYAN);
				cout << "\n\n";
				pause_game();
				return;
			}

			// Koktajl Wampira
			else if (choice == 2) {

				if (player_vampire_potion <= 0) {

					clear_screen();

					cout << "\n\tNie masz Koktajlu Wampira!\n\n";

					pause_game();
					continue;
				}

				if (vampire_turns > 0) {

					clear_screen();

					cout << "\n\tEfekt Koktajlu Wampira jest już aktywny!\n\n";

					pause_game();
					continue;
				}

				vampire_bonus = 10;
				vampire_turns = 10;

				player_vampire_potion--;

				clear_screen();

				cout << "\n\t";
				print_colored("Użyto Koktajlu Wampira!", COLOR_PURPLE);
				cout << "\n\t";
				print_colored("+10% lifestealu na 10 tur!", COLOR_PURPLE);
				cout << "\n\n";
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
