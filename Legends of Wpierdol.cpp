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

// Zdefiniowanie wzmocnień:

int player_lifesteal = 0;
int player_bonus_health = 0;
int player_bonus_accuracy = 0;

const int MAX_LIFESTEAL = 20;
const int MAX_ACCURACY = 30;

struct shop_weapons {
	string name;
	int price;
	int damage;
};

struct skill_upgrade {
	string name;
	int price;
	string description;
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

	// EARLY GAME (1–10)

	{1, "Menel spod Żabki", 30, 3, 35, 95, 5, 1, 15},
	{2, "Sebix z osiedla", 40, 4, 38, 93, 6, 1, 20},
	{3, "Dresiarz", 55, 5, 40, 92, 8, 1, 28},
	{4, "Sąsiad", 75, 7, 42, 90, 10, 1, 40},
	{5, "Kierownik Żabki", 110, 10, 45, 88, 12, 1, 70}, // BOSS
		 
	{6, "Kostek", 80, 6, 45, 88, 10, 1, 35},
	{7, "Speedrunner bez snu", 95, 8, 47, 86, 12, 1, 45},
	{8, "Discord Mod", 120, 10, 50, 84, 15, 1, 60},
	{9, "Eggman po 3 energetykach", 140, 12, 52, 82, 18, 1, 85},
	{10, "Tryhard CS2", 180, 15, 55, 80, 20, 1, 120}, // BOSS

	// MID GAME (11–25)

	{11, "Cyberpsychol z Night City", 220, 18, 56, 78, 22,2, 160},
	{12, "Gracz League po tiltcie", 260, 20, 58, 76, 24,2, 200},
	{13, "Yone OTP", 300, 22, 60, 74, 26,2, 260},
	{14, "Edgar z Brawl Stars", 340, 25, 62, 72, 28,2, 320},
	{15, "Kasjerka w Żabce", 420, 30, 65, 70, 30,2, 500}, // BOSS

	{16, "Ash Ketchum", 480, 35, 66, 68, 32,2, 600},
	{17, "Gracz League po 12 rankedach", 550, 40, 68, 66, 34,2, 750},
	{18, "Akali main (tilt mode)", 650, 45, 70, 64, 36,2, 900},
	{19, "Dis gdy JD", 750, 50, 72, 62, 38,2, 1100},
	{20, "Adam Smasher z Allegro", 900, 60, 75, 60, 40,2, 1500}, // BOSS

	{21, "Speedrunner DMC", 1000, 65, 76, 58, 42, 2, 1800},
	{22, "Moderator Reddita", 1150, 70, 78, 56, 44, 2, 2200},
	{23, "Silver Yasuo 0/10", 1300, 80, 80, 54, 46, 2, 2700},
	{24, "Speedrunner Dark Souls", 1500, 90, 82, 52, 48, 2, 3300},
	{25, "Dante z DMC", 1800,110, 85, 50, 50, 2, 4200}, // BOSS

	// LATE GAME (26–40)

	{26, "Cyberpsychol MAX", 2100,120, 86, 48, 52, 3, 5000},
	{27, "Mariusz Pudzianowski", 2400,130, 88, 46, 54, 3, 6000},
	{28, "Vergil z DMC", 2800,150, 90, 44, 56, 3, 7200},
	{29, "Chester Bernington", 3200,170, 92, 42, 58, 3, 8500},
	{30, "Song So-Mi (Songbird)", 3800,200, 94, 40, 60, 3, 10000}, // BOSS
	{31, "Samsung Galaxy S22", 4500,230, 95, 38, 62, 3, 12000},
	{32, "6pek", 5200, 260, 96, 36, 64, 3, 14500},
	{33, "Adam Smasher", 6000, 300, 97, 34, 66, 3, 17000},
	{34, "Speedrun God", 7000, 350, 98, 32, 68, 3, 20000},
	{35, "Gabe Newell", 8200,400, 99, 30, 70, 3, 24000}, // BOSS
	{36, "Legenda Night City", 9500,450, 99, 28, 72, 3, 28000},
	{37, "Walter White", 11000,500, 99, 26, 74, 3, 32000},
	{38, "Owcacejk", 12500,600, 99, 24, 76, 3, 37000},
	{39, "Pan Ryszard", 15000,700, 99, 22, 78, 3, 45000},
	{40, "Legenda Wpierdolu", 20000,900, 99, 20, 80, 5, 100000} // FINAL BOSS

};

int current_enemy_health;
const int enemy_amount = sizeof(enemy) / sizeof(enemy[0]);

shop_weapons shop[] = {
// Jak robić bronie?
// Nazwa, Koszt, DMG

		// Early game
		{"Patyk", 3, 3},
		{"Trzon od siekiery", 10, 6},
		{"Zardzewiała Rura", 15, 8},
		{"Gazrurka", 20, 10},
		{"Nóż kuchenny", 50, 15},

		// Mid game
		{"Maczeta", 70, 26},
		{"Zardzewiała Katana", 120, 32},
		{"Żelazny Miecz", 200, 45},
		{"Baseball z gwoździami", 260, 55},
		{"Młot Budowlany", 320, 65},

		// League of Legends references
		{"Pistolet Jhin'a", 444, 74},
		{"Latarnia Thresha", 667, 90},
		{"Miecze Yone", 850, 105},
		{"Młot Mordekaisera", 1200, 135},
		{"Shurikeny Akali", 1600, 170},

		// Devil May Cry references
		{"Red Queen", 2300, 210},
		{"Ebony & Ivory", 5000, 380},

		// Cyberpunk references
		{"Mantis Blades", 6500, 450},
		{"Malorian 3516", 10000, 650},

		// Meme / late game
		{"RTX 5090", 15000, 900},
		{"Nokia 3310", 20000, 1200},
	};

const int shop_weapon_amount = sizeof(shop) / sizeof(shop[0]);

skill_upgrade skills[] = {
// Jak robić wzmocnienia?
// Nazwa, Koszt, Opis
	{"Wampiryczne Ostrze", 600, "	Leczy za 10% zadanych obrazen (maksymalnie 20%)"},
	{"Kryształ Życia", 200, "	+50 maksymalnego HP"},
	{"Kryształ Skupienia", 450, "	15% mniejsza szansa na nietrafienie ataku (maksymalnie 30%)"}
};

const int skills_amount = sizeof(skills) / sizeof(skills[0]);

// Zadeklarowanie funkcji:

void game_menu();
void game_battle();
void game_shop();
void game_skills();
void game_options();
void game_intro();
void game_help(); // Pomoc
void game_help_battle(); // Pomoc - Walka
void game_help_shop(); // Pomoc - Sklep
void shop_buy_weapon(int id);
void debug_console();

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

	const int choice_amount = 4;
	string choices[choice_amount] = { "Graj", "Sklep", "Wzmocnienia i Umiejętności", "Opcje"};

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
			else if (keyboard_button == '`') {
				debug_console();
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
			game_skills();
			break;

		case 3:
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

	if (enemyIndex >= enemy_amount) {

		system("cls");

		cout << endl;
		cout << "	Ukończyłeś wszystkie dostępne poziomy gry!\n";
		cout << endl;
		cout << "	Dzięki za grę :)\n";
		cout << endl;

		system("pause");
		return;
	}

	total_waves = enemy[enemyIndex].wave;
	current_wave = 1;
	current_enemy_health = enemy[enemyIndex].health;

	int choice = 0;
	int keyboard_button;

	while (true) {

		system("cls");

		cout << endl;
		cout << "	Lvl: " << player_level << " | Runda: " << current_wave << " z " << total_waves << endl;
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

				int final_dodge = enemy[enemyIndex].dodge_chance - player_bonus_accuracy;

				if (final_dodge < 0)
					final_dodge = 0;

				if (miss < final_dodge) {

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

					int heal = (player_weapon_damage * player_lifesteal) / 100;

					player_health += heal;

					if (player_health > player_maxhealth)
						player_health = player_maxhealth;

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

// Umiejętności i Wzmocnienia

void game_skills() {

	int choice = 0;
	int keyboard_button;

	while (true) {

		system("cls");

		cout << "ESC - Powrot\n\n";
		cout << "	Wzmocnienia i Umiejetnosci\n\n";
		cout << "	Hajs: $" << player_money << endl;
		cout << endl;

		for (int i = 0; i < skills_amount; i++) {

			if (i == choice)
				cout << "	> " << skills[i].name << " | $" << skills[i].price << endl;
			else
				cout << "	  " << skills[i].name << " | $" << skills[i].price << endl;

			cout << "	   " << skills[i].description << endl;
		}

		keyboard_button = _getch();

		if (keyboard_button == 224) {

			keyboard_button = _getch();

			if (keyboard_button == 72) {
				choice--;

				if (choice < 0)
					choice = skills_amount - 1;
			}

			else if (keyboard_button == 80) {
				choice++;

				if (choice >= skills_amount)
					choice = 0;
			}
		}

		else if (keyboard_button == 13) {

			if (player_money >= skills[choice].price) {

				player_money -= skills[choice].price;

				switch (choice) {

				case 0:
					if (player_lifesteal >= MAX_LIFESTEAL) {

						system("cls");

						cout << endl;
						cout << "	Osiagnieto maksymalny poziom Wampirycznego Ostrza!\n";
						cout << endl;

						system("pause");
						continue;
					}

					player_lifesteal += 10;

					if (player_lifesteal > MAX_LIFESTEAL)
						player_lifesteal = MAX_LIFESTEAL;
					break;

				case 1:
					player_bonus_health += 50;
					player_maxhealth += 50;
					player_health += 50;
					break;

				case 2:
					if (player_bonus_accuracy >= MAX_ACCURACY) {

						system("cls");

						cout << endl;
						cout << "	Osiagnieto maksymalną ilość Kryształów Skupienia!\n";
						cout << endl;

						system("pause");
						continue;
					}

					player_bonus_accuracy += 15;

					if (player_bonus_accuracy > MAX_ACCURACY)
						player_bonus_accuracy = MAX_ACCURACY;
					break;
				}

				system("cls");

				cout << endl;
				cout << "	Kupiono: " << skills[choice].name << endl;
				cout << endl;

				system("pause");
			}
			else {

				system("cls");

				cout << endl;
				cout << "	Za mało hajsu!\n";
				cout << endl;

				system("pause");
			}
		}

		else if (keyboard_button == 27) {
			return;
		}
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

void debug_console() {

	system("cls");

	string command;
	int value;

	cout << "=== DEBUG CONSOLE ===\n\n";
	cout << "Komendy:\n";
	cout << "money <liczba>\n";
	cout << "health <liczba>\n";
	cout << "level <liczba>\n";
	cout << "weapon <id>\n\n";

	cout << "> ";

	cin >> command >> value;

	// MONEY
	if (command == "money") {

		player_money += value;

		cout << "\nDodano $" << value << endl;
	}

	// HEALTH
	else if (command == "health") {

		player_health = value;

		if (player_health > player_maxhealth)
			player_health = player_maxhealth;

		cout << "\nUstawiono HP na " << player_health << endl;
	}

	// LEVEL
	else if (command == "level") {

		player_level = value;

		cout << "\nUstawiono level na " << player_level << endl;
	}

	// WEAPON
	else if (command == "weapon") {

		if (value >= 0 && value < shop_weapon_amount) {

			player_weapon_name = shop[value].name;
			player_weapon_damage = shop[value].damage;

			cout << "\nOtrzymano bron: " << player_weapon_name << endl;
		}
		else {
			cout << "\nNiepoprawne ID broni!\n";
		}
	}

	else {

		cout << "\nNieznana komenda!\n";
	}

	cout << endl;
	system("pause");
}