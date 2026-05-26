#include "battle.h"
#include "globals.h"
#include "utils.h"
#include "save.h"
#include <iostream>
#include <cstdlib>
using namespace std;

void game_battle() {

	clock_t battle_start = clock();

	const int choice_amount = 3;
	string choices[] = {
		"Walcz",
		"Przedmioty",
		"Uciekaj"
	};

	int enemyIndex = player_level - 1;

	if (enemyIndex >= enemy_amount) {
		clear_screen();
		cout << endl;
		cout << "	Ukończyłeś wszystkie dostępne poziomy gry!\n";
		cout << endl;
		cout << "	Dzięki za grę :)\n";
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

		cout << "\n\t                    ";
		draw_enemy_hp_bar(current_enemy_health, current_enemy_max_health);

		cout << "\n\n\t                    ";

		print_colored("DMG przeciwnika", COLOR_RED);
		cout << ": " << current_enemy_damage << endl << endl;

		cout << "\t";
		print_colored(nickname, player_nickname_color);
		cout << ":\n       ";

		draw_player_hp_bar(player_health, player_maxhealth);

		cout << "\n\n\t";

		print_colored("Broń", COLOR_BRIGHT_WHITE);
		cout << ": ";

		print_colored(player_weapon_name, COLOR_CYAN);

		cout << " - ";

		number_colored(player_weapon_damage, COLOR_RED);

		print_colored(" DMG", COLOR_RED);

		cout << endl;

		cout << "\t";

		print_colored("Mikstury Zdrowia", COLOR_GREEN);

		cout << ": " << player_health_potion << "\n\n";

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

			if (choice == 0) {

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

					int dealt_damage = player_weapon_damage;

					if (critical)
						dealt_damage = (int)(dealt_damage * 1.5f);

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
					if (player_second_breath) {

						int healAmount = (int)(player_maxhealth * 0.25f);

						player_health += healAmount;

						if (player_health > player_maxhealth)
							player_health = player_maxhealth;

						cout << "\n\t";
						ui_colored("Drugie Tchnienie przywróciło: ", healAmount, COLOR_GREEN); print_colored(" HP!", COLOR_GREEN); cout << "\n\n";
					}

					if (current_wave > total_waves) {
						cout << "	Gratulacje! Przeszłeś poziom " << player_level << "!\n" << endl;
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

				int enemyAttack = rand() % 100;
				if (enemyAttack < enemy[enemyIndex].atk_chance) {
					float reductionPercent = player_armor / (player_armor + 100.0f);

					int originalDamage = current_enemy_damage;
					bool enemyCritical = false;

					int enemyCritRoll = rand() % 100;

					if (enemyCritRoll < enemy[enemyIndex].crit_chance)
						enemyCritical = true;

					int enemyDamage = current_enemy_damage;

					if (enemyCritical)
						enemyDamage = (int)(enemyDamage * 1.5f);

					int finalDamage = enemyDamage * (1.0f - reductionPercent);

					if (finalDamage < 1)
						finalDamage = 1;

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

				if (player_health <= 0) {
					clear_screen();
					cout << endl << "	GAME OVER!\n" << endl;
					pause_game();
					exit(0);
				}

				if (precision_turns > 0)
					precision_turns--;

				if (vampire_turns > 0)
					vampire_turns--;
			}

			else if (choice == 1) {

				battle_items_menu();
			}

			else if (choice == 2) {

				int maxEscapes = 1;


				if (player_escape_count >= maxEscapes) {
					clear_screen();
					cout << endl;
						cout << "	Możesz uciec tylko raz na poziom!\n" << endl;

					pause_game();
					continue;
				}

				clear_screen();
				int heal = player_maxhealth / 2;
				player_health += heal;
				if (player_health > player_maxhealth)
					player_health = player_maxhealth;

				player_escape_count++;

				cout << endl;
				cout << "	Uciekasz z pola walki!\n";
				cout << "	Odzyskano " << heal << " HP!\n" << endl;
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
				cout << "	►  "; print_colored(items[i], COLOR_BRIGHT_WHITE); cout << endl;
			}
			else
				cout << "	  " << items[i] << endl;
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