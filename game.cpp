#include <iostream>
#include <clocale>
#include <limits>

#define NOMINMAX
#include <windows.h>
#include <conio.h>
#include <stdlib.h>

using namespace std;

// Zdefiniowanie zmiennych:

string nickname;
int player_level = 1;
int player_money = 5;
int player_health = 100;
int player_maxhealth = 100;
string player_weapon_name = "nic";
int player_weapon_damage = 0;

int current_wave;
int total_waves;

// Tablica broni:

struct shop_weapons {
	string name;
	int price;
	int damage;
};

struct battle_enemy {
	int lvl;
	string name;
	int health;
	int damage;
	int atk_chance;
	int player_atk_chance;
	int dodge_chance;
	int wave;
	int kill_reward;
};

battle_enemy enemy[] = {
// Jak robić kolejnych przeciwników?
// ID, Nazwa, HP, DMG, Szansa na atak, Szansa na bycie zaatakowanym przez gracza, Szansa na dodge, Ilość fal, Nagroda za zabójstwo.
// ID,  Name,    HP, DMG,Atk,Gatk,Dge,Wve,Krd
	{1, "Człowiek", 30, 3, 10, 50, 10, 1, 17},
	{2, "Człowiek", 50, 5, 15, 50, 12, 2, 10},
	{3, "Człowiek", 60, 6, 16, 52, 15, 3, 9}
};

int current_enemy_health;

shop_weapons shop[] = {
// Jak robić bronie?
// Nazwa, Koszt, DMG
	{"Patyk", 3, 3},
	{"Trzon od siekiery", 10, 7},
	{"Nóż kuchenny", 50, 20},
	{"Maczeta", 70, 30},
	{"Kij Triple-T", 670, 670},
	{"Nokia 3310", 3310, 600}
};

const int shop_weapon_amount = sizeof(shop) / sizeof(shop[0]);

// Zadeklarowanie funkcji:

void game_menu();
void game_battle();
void game_shop();
void game_options();
void game_intro();
void game_help(); // Pomoc
void game_help_battle(); // Pomoc - Walka
void game_help_shop(); // Pomoc - Sklep
void shop_buy_weapon(int id);
void battle_attack();
void battle_run();

// Menu startowe:

int main() {

	setlocale(LC_ALL, "polish");

	srand(time(NULL));

	const int choice_amount = 2;
	string choices[choice_amount] = { "Graj", "Wyjdz" };

	int choice = 0;
	int keyboard_button;

	while (true) {

		system("cls");

		cout << endl;
		cout << "    Legends of Wpierdol\n\n";

		for (int i = 0; i < choice_amount; i++) {
			if (i == choice)
				cout << "    > " << choices[i] << endl;
			else
				cout << "      " << choices[i] << endl;
		}

		keyboard_button = _getch();

		if (keyboard_button == 224) {
			keyboard_button = _getch();

			if (keyboard_button == 72) { // strzałka w górę
				choice--;
				if (choice < 0)
					choice = choice_amount - 1;
			}
			else if (keyboard_button == 80) { // strzałka w dół
				choice++;
				if (choice >= choice_amount)
					choice = 0;
			}
		}
		else if (keyboard_button == 13) { // ENTER
			break;
		}
	}

	switch (choice) {

	case 0:
		game_intro();
		break;

	case 1:
		exit(0);
		break;
	}

	return 0;
}

// Intro:

void game_intro() {
	system("cls");

	// Funkcja pobierająca nick od gracza:

	cout << endl;
	cout << "	Podaj swój nick: " << endl;
	cin >> nickname;

	// Intro do gry:

	system("cls");
	cout << endl;
	cout << "	Witaj " << nickname << "!" << " Witaj w grze Legends of Wpierdol!" << endl;
	cout << "	W skrócie:\n";
	cout << "		- napierdalasz się z różnymi przeciwnikami\n";
	cout << "		- za zdobyte pieniądze kupujesz różne bronie\n";
	cout << "		- resztę ogarniesz sam :)\n";
	cout << endl;
	system("pause");
	game_menu();
}

// Menu główne gry:

void game_menu() {

	const int choice_amount = 3;
	string choices[choice_amount] = { "Graj", "Sklep", "Opcje" };

	while (true) {
		int choice = 0;
		int keyboard_button;

		while (true) {

			system("cls");
			cout << endl;
			cout << "	Nick: " << nickname << endl;
			cout << "	LVL: " << player_level << endl;
			cout << "	Hajs: $" << player_money << endl;
			cout << "	Broń: " << player_weapon_name << " - " << player_weapon_damage << " DMG" << endl;
			cout << endl;

			for (int i = 0; i < choice_amount; i++) {
				if (i == choice)
					cout << "	> " << choices[i] << endl;
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
		}

		switch (choice) {
		case 0:
			game_battle();
			break;

		case 1:
			game_shop();
			break;

		case 2:
			game_options();
			break;
		}
	}
}

// Menu i system walki:

void game_battle() {

	const int choice_amount = 2;
	string choices[choice_amount] = { "Walcz", "Uciekaj" };

	int enemyIndex = player_level - 1;

	total_waves = enemy[enemyIndex].wave;
	current_wave = 1;
	current_enemy_health = enemy[enemyIndex].health;

	int choice = 0;
	int keyboard_button;

	while (true) {

		system("cls");

		cout << endl;
		cout << "	Lvl: " << player_level << " | Fala: " << current_wave << " z " << total_waves << endl;
		cout << "	Twoje HP: " << player_health << endl;
		cout << "	Przeciwnik: " << enemy[enemyIndex].name << " | " << current_enemy_health << " HP" << endl;
		cout << "	DMG przeciwnika: " << enemy[enemyIndex].damage << endl;
		cout << endl;

		for (int i = 0; i < choice_amount; i++) {
			if (i == choice)
				cout << "	> " << choices[i] << endl;
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

			// Atak Gracza
			if (choice == 0) {

				int miss = rand() % 100;

				if (miss < enemy[enemyIndex].dodge_chance) {
					system("cls");
					cout << endl;
					cout << "	Twoja tura!\n";
					cout << endl;
					cout << "	Przeciwnik uniknął twojego ataku!\n";
					cout << endl;
					system("pause");
				}
				else {
					current_enemy_health -= player_weapon_damage;
					system("cls");
					cout << endl;
					cout << "	Twoja tura!\n";
					cout << endl;
					cout << "	Zadałeś " << player_weapon_damage << " obrażeń!\n";
					cout << "	Przeciwnik ma teraz " << current_enemy_health << " HP!\n";
					cout << endl;
					system("pause");
				}

				// Check czy przeciwnik jest martwy
				if (current_enemy_health <= 0) {
					system("cls");
					cout << endl;
					cout << "	Twoja tura!\n";
					cout << endl;
					cout << "	Przeciwnik pokonany!\n";
					cout << endl;

					player_money += enemy[enemyIndex].kill_reward;
					current_wave++;

					if (current_wave > total_waves) {
						cout << "	Gratulacje! Przeszłeś poziom " << player_level << "!\n";
						cout << endl;
						player_level++;
						system("pause");
						return;
					}

					current_enemy_health = enemy[enemyIndex].health;
					system("pause");
					continue;
				}

				// Atak przeciwnika
				int enemyAttack = rand() % 100;

				if (enemyAttack < enemy[enemyIndex].atk_chance) {
					player_health -= enemy[enemyIndex].damage;
					system("cls");
					cout << endl;
					cout << "	Tura przeciwnika!\n";
					cout << endl;
					cout << "	Przeciwnik zadał Ci " << enemy[enemyIndex].damage << " obrażeń!\n";
					cout << endl;
					system("pause");
				}
				else {
					system("cls");
					cout << endl;
					cout << "	Tura przeciwnika!\n";
					cout << endl;
					cout << "	Przeciwnik nie trafił! Nie otrzymujesz żadnych obrażeń!\n";
					cout << endl;
					system("pause");
				}

				// GAME OVER
				if (player_health <= 0) {
					system("cls");
					cout << endl;
					cout << "	GAME OVER!\n";
					cout << endl;
					system("pause");
					exit(0);
				}
			}

			// Ucieczka
			else if (choice == 1) {
				system("cls");
				cout << endl;
				cout << "	Uciekasz z pola walki! Regenerujesz swoje zdrowie do: " << player_maxhealth << " HP" << endl;
				cout << endl;

				player_health = player_maxhealth;
				system("pause");
				return;
			}
		}
	}
}

// Menu i system sklepu:

void game_shop() {

	int choice = 0;
	int keyboard_button;

	while (true) {

		system("cls");

		cout << "ESC - Powrót\n";
		cout << endl;
		cout << "	Sklep:\n";
		cout << endl;
		cout << "	Hajs: $" << player_money << endl;
		cout << "	Aktualna Broń: " << player_weapon_name << " - " << player_weapon_damage << " DMG" << endl;
		cout << endl;

		for (int i = 0; i < shop_weapon_amount; i++) {
			if (i == choice)
				cout << "	> " << shop[i].damage << " DMG | $" << shop[i].price << " - " << shop[i].name << endl;
			else
				cout << "	  " << shop[i].damage << " DMG | $" << shop[i].price << " - " << shop[i].name << endl;
		}

		keyboard_button = _getch();

		if (keyboard_button == 224) {
			keyboard_button = _getch();

			if (keyboard_button == 72) {
				choice--;
				if (choice < 0)
					choice = shop_weapon_amount - 1;
			}
			else if (keyboard_button == 80) {
				choice++;
				if (choice >= shop_weapon_amount)
					choice = 0;
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

// Kupowanie broni:

void shop_buy_weapon(int id) {
	system("cls");

	cout << endl;
	cout << "	Wybrałeś: " << shop[id].name << endl;
	cout << "	Cena: $" << shop[id].price << endl;

	if (player_money >= shop[id].price) {
		player_money = player_money - shop[id].price;
		player_weapon_name = shop[id].name;
		player_weapon_damage = shop[id].damage;
		cout << endl;
		cout << "	Kupiono!" << endl;
		cout << endl;
		system("pause");
		return;
	}
	else {
		cout << endl;
		cout << "Za mało hajsu!" << endl;
		cout << endl;
		system("pause");
	}
}

// Menu i opcje gry:

void game_options() {

	const int choice_amount = 2;
	string choices[choice_amount] = { "Pomoc","Wyjście z gry" };

	while (true) {

		int choice = 0;
		int keyboard_button;

		while (true) {

			system("cls");
			cout << "ESC - Powrót\n";
			cout << endl;
			cout << "	Opcje:\n";
			cout << endl;

			for (int i = 0; i < choice_amount; i++) {
				if (i == choice)
					cout << "	> " << choices[i] << endl;
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
			game_help();
			break;

		case 1:
			exit(0);
			break;

		}
	}
}

void game_help() {

	const int choice_amount = 2;
	string choices[choice_amount] = { "Walka","Sklep" };

	while (true) {

		int choice = 0;
		int keyboard_button;

		while (true) {

			system("cls");
			cout << "ESC - Powrót\n";
			cout << endl;
			cout << "	Pomoc\n\n";
			cout << "	Wybierz temat:\n";
			cout << endl;

			for (int i = 0; i < choice_amount; i++) {
				if (i == choice)
					cout << "	> " << choices[i] << endl;
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
			game_help_battle();
			break;

		case 1:
			game_help_shop();
			break;
		}
	}
}

void game_help_battle() {
	int keyboard_button;

	while (true) {
		system("cls");
		cout << "ESC - Powrót\n";
		cout << endl;
		cout << "	Walka\n";
		cout << "		1. Walka w Legends of Wpierdol działa podobnie do systemu walki w Pokemonach.\n";
		cout << "		   Gracz atakuje swoją bronią a gra wylicza prawdopodobieństwo trafienia ataku:\n";
		cout << "		   Jeżeli trafi - zadaje obrażenia przeciwnikowi, jeśli nie - przeciwnikowi\n";
		cout << "		   nic się nie stanie. Potem przeciwnik atakuje w taki sam sposób.\n";

		keyboard_button = _getch();

		if (keyboard_button == 27) {
			return;
		}

	}
}

void game_help_shop() {
	int keyboard_button;

	while (true) {
		system("cls");
		cout << "ESC - Powrót\n";
		cout << endl;
		cout << "	Sklep\n";
		cout << "		1. W Legends of Wpierdol sklep działa w taki sposób że zakup broni usuwa\n";
		cout << "		   aktualną broń z twojego ekwipunku. Bronie kupujesz za pomocą pieniędzy\n";
		cout << "		   zdobytych poprzez walkę z przeciwnikami\n";

		keyboard_button = _getch();

		if (keyboard_button == 27) {
			return;
		}

	}
}
