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
	current_enemy_health = enemy[enemyIndex].health;

	int choice = 0;
	int keyboard_button;

	while (true) {

		clear_screen();
		cout << endl;
		cout << "	Lvl: " << player_level << " | Runda: " << current_wave << " z " << total_waves << "\n\n";

		cout << "	                    " << enemy[enemyIndex].name << "\n	                    ";
		draw_enemy_hp_bar(current_enemy_health, enemy[enemyIndex].health);

		cout << endl;
		cout << "	                    DMG przeciwnika: " << enemy[enemyIndex].damage << endl;
		cout << endl;

		cout << "	" << nickname << ":\n       ";
		draw_player_hp_bar(player_health, player_maxhealth);

		cout << endl;
		cout << "	Broń: " << player_weapon_name << " - " << player_weapon_damage << " DMG" << endl;
		cout << "	Mikstury Zdrowia: " << player_health_potion << "\n\n";

		for (int i = 0; i < choice_amount; i++) {
			if (i == choice)
				cout << "	➤  " << choices[i] << endl;
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
					clear_screen();
					cout << endl << "	Twoja tura!\n" << endl;
					cout << "	Przeciwnik uniknął twojego ataku!\n" << endl;
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

					clear_screen();
					cout << endl << "	Twoja tura!\n" << endl;
					if (critical)
						cout << "\tTRAFIENIE KRYTYCZNE!\n";
					cout << "	Zadałeś " << dealt_damage << " obrażeń!\n";
					cout << "	Przeciwnik ma teraz " << current_enemy_health << " HP!\n";
					if (heal > 0)
						cout << "	Odnowiono " << heal << " HP dzięki Wampirycznemu Ostrzu!\n";
					cout << endl;
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
					cout << endl;
					cout << "	Twoja tura!" << endl;
					cout << "	Przeciwnik pokonany!" << endl;
					cout << "	Otrzymujesz: $" << base_reward << " + $" << bonus_money_amount << " za czas walki!" << endl;
					cout << "	Suma: $" << final_reward << endl;
					current_wave++;

					if (current_wave > total_waves) {
						cout << "	Gratulacje! Przeszłeś poziom " << player_level << "!\n" << endl;
						player_level++;
						player_escape_count = 0;
						save_game();
						pause_game();
						return;
					}

					current_enemy_health = enemy[enemyIndex].health;
					pause_game();
					continue;
				}

				int enemyAttack = rand() % 100;
				if (enemyAttack < enemy[enemyIndex].atk_chance) {
					float reductionPercent = player_armor / (player_armor + 100.0f);

					int originalDamage = enemy[enemyIndex].damage;
					bool enemyCritical = false;

					int enemyCritRoll = rand() % 100;

					if (enemyCritRoll < enemy[enemyIndex].crit_chance)
						enemyCritical = true;

					int enemyDamage = enemy[enemyIndex].damage;

					if (enemyCritical)
						enemyDamage = (int)(enemyDamage * 1.5f);

					int finalDamage = enemyDamage * (1.0f - reductionPercent);

					if (finalDamage < 1)
						finalDamage = 1;

					int amountOfDamageReducted = originalDamage - finalDamage;

					player_health -= finalDamage;

					clear_screen();
					cout << endl << "	Tura przeciwnika!\n" << endl;

					if (enemyCritical)
						cout << "\tPRZECIWNIK TRAFIŁ KRYTYCZNIE!\n";

					if (amountOfDamageReducted > 0) {
						cout << "	Obrażenia przeciwnika zredukowane o: "
							<< amountOfDamageReducted << "!\n";
					}

					cout << "	Przeciwnik zadał Ci "
						<< finalDamage
						<< " obrażeń!\n" << endl;

					pause_game();
				}
				else {
					clear_screen();
					cout << endl << "	Tura przeciwnika!\n" << endl;
					cout << "	Przeciwnik nie trafił! Nie otrzymujesz żadnych obrażeń!\n" << endl;
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

				if (player_escape_master)
					maxEscapes = 2;

				if (player_escape_count >= maxEscapes) {
					clear_screen();
					cout << endl;

					if (player_escape_master)
						cout << "	Wykorzystałeś już obie ucieczki na tym poziomie!\n" << endl;
					else
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

		cout << "\tMikstura Zdrowia: x" << player_health_potion << endl;
		cout << "\tEliksir Precyzji: x" << player_precision_potion << endl;
		cout << "\tKoktajl Wampira: x" << player_vampire_potion << endl;

		cout << endl;

		for (int i = 0; i < item_amount; i++) {

			if (i == choice)
				cout << "\t➤   " << items[i] << endl;
			else
				cout << "\t   " << items[i] << endl;
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

				int healAmount = (int)(20 + (player_maxhealth * 0.05f));

				if (healAmount > 150)
					healAmount = 150;

				player_health += healAmount;

				if (player_health > player_maxhealth)
					player_health = player_maxhealth;

				player_health_potion--;

				clear_screen();

				cout << "\n\tUżyto Mikstury Zdrowia!\n";
				cout << "\tOdzyskano " << healAmount << " HP!\n\n";

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
				precision_turns = 5;

				player_precision_potion--;

				clear_screen();

				cout << "\n\tUżyto Eliksiru Precyzji!\n";
				cout << "\t+50% szansy trafienia na 5 tur!\n\n";

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

				vampire_bonus = 5;
				vampire_turns = 10;

				player_vampire_potion--;

				clear_screen();

				cout << "\n\tUżyto Koktajlu Wampira!\n";
				cout << "\t+5% lifestealu na 10 tur!\n\n";

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