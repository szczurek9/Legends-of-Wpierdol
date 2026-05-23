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
		if (player_weapon_name != "nic") {

			inventory[inventory_count].name = player_weapon_name;
			inventory[inventory_count].damage = player_weapon_damage;
			inventory[inventory_count].price = player_weapon_price;

			inventory_count++;
		}
		player_weapon_name = shop[id].name;
		player_weapon_damage = shop[id].damage;
		player_weapon_price = shop[id].price;
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
				cout << "	➤  " << shop[i].damage << " DMG | $" << shop[i].price << " - " << shop[i].name << endl;
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

void game_abilities() {

	int choice = 0;
	int keyboard_button;

	while (true) {

		clear_screen();

		cout << "ESC - Powrót\n\n";
		cout << "	Umiejętności\n\n";

		cout << "	Hajs: $" << player_money << endl << endl;

		for (int i = 0; i < abilities_amount; i++) {

			if (i == choice)
				cout << "	➤  ";
			else
				cout << "	  ";

			cout << abilities[i].name << " | $" << abilities[i].price << endl;
			cout << "	   " << abilities[i].description << endl;
		}

		keyboard_button = _getch();

		if (keyboard_button == 224) {

			keyboard_button = _getch();

			if (keyboard_button == 72) {

				choice--;

				if (choice < 0)
					choice = abilities_amount - 1;
			}
			else if (keyboard_button == 80) {

				choice++;

				if (choice >= abilities_amount)
					choice = 0;
			}
		}
		else if (keyboard_button == 13) {

			if (player_money < abilities[choice].price) {

				clear_screen();

				cout << "\n	Za mało hajsu!\n\n";

				pause_game();
				continue;
			}

			switch (choice) {

			case 0:

				if (player_lifesteal >= MAX_LIFESTEAL) {

					clear_screen();

					cout << "\n	Osiągnięto maksymalny poziom Wampirycznego Ostrza!\n\n";

					pause_game();
					continue;
				}

				player_lifesteal += 10;

				if (player_lifesteal > MAX_LIFESTEAL)
					player_lifesteal = MAX_LIFESTEAL;

				break;

			case 1:

				if (player_bonus_accuracy >= MAX_ACCURACY) {

					clear_screen();

					cout << "\n	Osiągnięto maksymalną ilość Kryształów Skupienia!\n\n";

					pause_game();
					continue;
				}

				player_bonus_accuracy += 15;

				if (player_bonus_accuracy > MAX_ACCURACY)
					player_bonus_accuracy = MAX_ACCURACY;

				break;
			case 2:
				if (player_crit_chance >= MAX_CRIT) {

					clear_screen();

					cout << "\n	Osiągnięto maksymalną szansę na atak krytyczny!\n\n";

					pause_game();
					continue;
				}

				player_crit_chance += 10;

				if (player_crit_chance > MAX_CRIT)
					player_crit_chance = MAX_CRIT;

			case 3:

				if (player_escape_master) {

					clear_screen();

					cout << "\n	Masz już to ulepszenie!\n\n";

					pause_game();
					continue;
				}

				player_escape_master = true;

				break;
			}

			player_money -= abilities[choice].price;

			clear_screen();

			cout << "\n	Kupiono: " << abilities[choice].name << "!\n\n";

			pause_game();
		}
		else if (keyboard_button == 27) {

			return;
		}
	}
}

void game_upgrades() {

	int choice = 0;
	int keyboard_button;

	while (true) {

		clear_screen();

		cout << "ESC - Powrót\n\n";
		cout << "	Wzmocnienia\n\n";

		cout << "	Hajs: $" << player_money << "\n";
		cout << "	HP: " << player_health << "/" << player_maxhealth << "\n";
		cout << "	Pancerz: " << player_armor << "\n\n";

		for (int i = 0; i < upgrades_amount; i++) {

			if (i == choice)
				cout << "	➤  ";
			else
				cout << "	  ";

			cout << upgrades[i].name << " | $" << upgrades[i].price << endl;
			cout << "	   " << upgrades[i].description << endl;
		}

		keyboard_button = _getch();

		if (keyboard_button == 224) {

			keyboard_button = _getch();

			if (keyboard_button == 72) {

				choice--;

				if (choice < 0)
					choice = upgrades_amount - 1;
			}
			else if (keyboard_button == 80) {

				choice++;

				if (choice >= upgrades_amount)
					choice = 0;
			}
		}
		else if (keyboard_button == 13) {

			if (player_money < upgrades[choice].price) {

				clear_screen();

				cout << "\n	Za mało hajsu!\n\n";

				pause_game();
				continue;
			}

			switch (choice) {

			case 0:

				player_bonus_health += 50;
				player_maxhealth += 50;
				player_health += 50;

				break;

			case 1:

				player_bonus_health += 160;
				player_maxhealth += 160;
				player_health += 160;

				break;

			case 2:

				if (player_armor >= 90) {

					clear_screen();

					cout << "\n	Osiągnięto maksymalny poziom pancerza!\n\n";

					pause_game();
					continue;
				}

				player_armor += 15;

				if (player_armor > 90)
					player_armor = 90;

				break;
			}

			player_money -= upgrades[choice].price;

			clear_screen();

			cout << "\n	Kupiono: " << upgrades[choice].name << "!\n\n";

			pause_game();
		}
		else if (keyboard_button == 27) {

			return;
		}
	}
}

void game_consumables() {

	int choice = 0;
	int keyboard_button;

	while (true) {

		clear_screen();

		cout << "ESC - Powrót\n\n";
		cout << "	Alchemik\n\n";

		cout << "	Hajs: $" << player_money << endl;
		cout << "	Posiadane mikstury:\n";
		if (player_health_potion > 0) cout << "\t\tMikstura Zdrowia x" << player_health_potion << "\n";
		if (player_precision_potion > 0) cout << "\t\tEliksir Precyzji x" << player_precision_potion << "\n";
		if (player_vampire_potion > 0) cout << "\t\tKoktajl Wampira x" << player_vampire_potion << "\n";
		if (player_health_potion <= 0  && player_precision_potion <= 0 && player_vampire_potion <= 0) cout << "\t\tBrak\n";
		cout << "\n";
		for (int i = 0; i < consumables_amount; i++) {

			if (i == choice)
				cout << "	➤  ";
			else
				cout << "	  ";

			cout << consumables[i].name << " | $" << consumables[i].price << endl;
			cout << "	   " << consumables[i].description << endl;
		}

		keyboard_button = _getch();

		if (keyboard_button == 224) {

			keyboard_button = _getch();

			if (keyboard_button == 72) {

				choice--;

				if (choice < 0)
					choice = consumables_amount - 1;
			}
			else if (keyboard_button == 80) {

				choice++;

				if (choice >= consumables_amount)
					choice = 0;
			}
		}
		else if (keyboard_button == 13) {

			if (player_money < consumables[choice].price) {

				clear_screen();

				cout << "\n	Za mało hajsu!\n\n";

				pause_game();
				continue;
			}

			switch (choice) {

			case 0:
				player_health_potion++;
				break;

			case 1:
				player_precision_potion++;
				break;

			case 2:
				player_vampire_potion++;
				break;
			}

			player_money -= consumables[choice].price;

			clear_screen();

			cout << "\n	Kupiono: " << consumables[choice].name << "!\n";

			pause_game();
		}
		else if (keyboard_button == 27) {

			return;
		}
	}
}