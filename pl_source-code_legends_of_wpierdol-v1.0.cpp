#include <iostream>
#include <clocale>
#include <limits>

#define NOMINMAX
#include <windows.h>
#include <conio.h>
#include <stdlib.h>
#include <fstream>
#include <string>

using namespace std;

static void clear_screen() {
	cout << "\x1B[2J\x1B[H";
}

static void pause_game() {
	cout << "\n	Naciśnij dowolny przycisk...";
	_getch();
}

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

bool player_used_escape = false;

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

		{1, "Menel spod Żabki", 30, 3, 25, 95, 5, 1, 5},
		{2, "Sebix z osiedla", 40, 4, 28, 93, 6, 1, 10},
		{3, "Dresiarz", 55, 5, 30, 92, 8, 1, 12},
		{4, "Sąsiad", 75, 7, 32, 90, 10, 1, 21},
		{5, "Kierownik Żabki", 110, 10, 38, 88, 12, 1, 100},

		{6, "Hazardzista", 80, 6, 34, 88, 10, 1, 30},
		{7, "Speedrunner bez snu", 95, 8, 36, 86, 12, 1, 30},
		{8, "Discord Mod", 120, 10, 38, 84, 15, 1, 10},
		{9, "Eggman po 3 energetykach", 140, 12, 40, 82, 18, 1, 5},
		{10, "Peter Griffin", 180, 15, 45, 80, 20, 1, 100},

		// MID GAME (11–25)

		{11, "Patrick Star", 220, 18, 42, 78, 22, 2, 30},
		{12, "Gracz League po tiltcie", 260, 20, 44, 76, 24, 2, 50},
		{13, "Yone OTP", 300, 22, 46, 74, 26, 2, 160},
		{14, "Edgar z Brawl Stars", 340, 25, 48, 72, 28, 2, 80},
		{15, "Kasjerka w Żabce", 420, 30, 52, 70, 30, 2, 100},

		{16, "Ash Ketchum", 480, 35, 50, 68, 32, 2, 110},
		{17, "Gracz League po 12 rankedach", 550, 40, 52, 66, 34, 2, 200},
		{18, "Akali main (tilt mode)", 650, 45, 54, 64, 36, 2, 210},
		{19, "Dis gdy JD", 750, 50, 56, 62, 38, 2, 150},
		{20, "Adam Smasher z Allegro", 900, 60, 60, 60, 40, 2, 500},

		{21, "Speedrunner DMC", 1000, 65, 58, 58, 42, 2, 500},
		{22, "Moderator Reddita", 1150, 70, 60, 56, 44, 2, 650},
		{23, "Silver Yasuo 0/10", 1300, 80, 62, 54, 46, 2, 800},
		{24, "Speedrunner Dark Souls", 1500, 90, 64, 52, 48, 2, 1000},
		{25, "Dante z DMC", 1800,110, 68, 50, 50, 2, 1300},

		// LATE GAME (26–40)

		{26, "Irak Bomber", 2100,120, 66, 48, 52, 3, 1500},
		{27, "Mariusz Pudzianowski", 2400,130, 67, 46, 54, 3, 2000},
		{28, "Vergil z DMC", 2800,150, 68, 44, 56, 3, 2200},
		{29, "Chester Bernington", 3200,170, 69, 42, 58, 3, 2500},
		{30, "Song So-Mi (Songbird)", 3800,200, 70, 40, 60, 3, 3000},

		{31, "Samsung Galaxy S22", 4500,230, 70, 38, 62, 3, 3500},
		{32, "6pek", 5200,260, 71, 36, 64, 3, 4000},
		{33, "Michael Jackson", 6000,300, 72, 34, 66, 3, 5000},
		{34, "Gordon Freeman", 7000,350, 73, 32, 68, 3, 5100},
		{35, "Gabe Newell", 8200,400, 75, 30, 70, 3, 5500},

		{36, "Kostecka", 9500,450, 74, 28, 72, 3, 5000},
		{37, "Walter White", 11000,500, 75, 26, 74, 3, 6000},
		{38, "Owcacejk", 12500,600, 76, 24, 76, 3, 6767},
		{39, "Pan Ryszard", 15000,700, 77, 22, 78, 3, 7000},
		{40, "Legenda Wpierdolu", 20000,900, 80, 20, 80, 5, 10000}
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
		{"Zardzewiała Katana", 120, 50},
		{"Żelazny Miecz", 200, 71},
		{"Baseball z gwoździami", 260, 86},
		{"Katana", 320, 120},

		// League of Legends references
		{"Pistolet Jhin'a", 444, 224},
		{"Latarnia Thresha", 667, 306},
		{"Miecze Yone", 850, 390},
		{"Młot Mordekaisera", 1200, 475},
		{"Shurikeny Akali", 1600, 527},

		// Devil May Cry references
		{"Red Queen", 2300, 700},
		{"Ebony & Ivory", 5000, 820},

		// Cyberpunk references
		{"Mantis Blades", 6500, 930},
		{"Malorian 3516", 10000, 1000},

		// Meme / late game
		{"GTX 1080 TI", 18000, 1881},
		{"Nokia 3310", 33100, 3331},
	};

const int shop_weapon_amount = sizeof(shop) / sizeof(shop[0]);

skill_upgrade skills[] = {
// Jak robić wzmocnienia?
// Nazwa, Koszt, Opis
	{"Wampiryczne Ostrze", 600, "	Leczy za 10% zadanych obrazen (maksymalnie 20%)"},
	{"Skała Zdrowia", 200, "	+50 maksymalnego HP"},
	{"Woda Życia", 800, "	+210 maksymalnego HP"},
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

void save_game();
bool load_game();
bool save_exists();

void manual_save();

void game_credits();

// Menu startowe:

int main() {

	SetConsoleTitleA("Legends of Wpierdol | by szczurek9");

	setlocale(LC_ALL, "polish");

	srand(time(NULL));

	const int choice_amount = 2;
	string choices[choice_amount] = { "Graj", "Wyjdz" };

	int choice = 0;
	int keyboard_button;

	while (true) {

		clear_screen();

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

	clear_screen();

	if (save_exists()) {

		const int choice_amount = 2;
		string choices[choice_amount] = {
			"Wczytaj grę",
			"Nowa gra"
		};

		int choice = 0;
		int keyboard_button;

		while (true) {

			clear_screen();

			cout << endl;
			cout << "	Wykryto zapis gry!\n";
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

				// Wczytaj gre
				if (choice == 0) {

					if (load_game()) {

						clear_screen();

						cout << endl;
						cout << "	Gra wczytana!\n";
						cout << endl;

						pause_game();
						game_menu();
						return;
					}
				}

				// Nowa gra
				else if (choice == 1) {

					remove("save.sav");
					break;
				}
			}
		}
	}

	// Funkcja pobierająca nick od gracza:

	remove("save.sav");
	cout << endl;
	cout << "	Podaj swój nick: " << endl;
	cin >> nickname;

	// Intro do gry:

	clear_screen();
	cout << endl;
	cout << "	Witaj " << nickname << "!" << " Witaj w grze Legends of Wpierdol!" << endl;
	cout << "	W skrócie:\n";
	cout << "		- napierdalasz się z różnymi przeciwnikami\n";
	cout << "		- za zdobyte pieniądze kupujesz różne bronie\n";
	cout << "		- resztę ogarniesz sam :)\n";
	cout << endl;
	pause_game();
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

			clear_screen();
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
		cout << "	Lvl: " << player_level << " | Runda: " << current_wave << " z " << total_waves << endl;
		cout << "	Twoje HP: " << player_health << "/" << player_maxhealth << endl;
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

					clear_screen();
					cout << endl;
					cout << "	Twoja tura!\n";
					cout << endl;
					cout << "	Przeciwnik uniknął twojego ataku!\n";
					cout << endl;
					pause_game();
				}
				else {
					int dealt_damage = player_weapon_damage;

					// Overkill protection
					if (dealt_damage > current_enemy_health)
						dealt_damage = current_enemy_health;

					current_enemy_health -= player_weapon_damage;

					int heal = (dealt_damage * player_lifesteal) / 100;

					player_health += heal;

					if (player_health > player_maxhealth)
						player_health = player_maxhealth;

					clear_screen();
					cout << endl;
					cout << "	Twoja tura!\n";
					cout << endl;
					cout << "	Zadałeś " << player_weapon_damage << " obrażeń!\n";
					cout << "	Przeciwnik ma teraz " << current_enemy_health << " HP!\n";

					if (heal > 0)
						cout << "	Odnowiono " << heal << " HP dzięki Wampirycznemu Ostrzu!\n";
					cout << endl;
					pause_game();
				}

				// Check czy przeciwnik jest martwy
				if (current_enemy_health <= 0) {
					clear_screen();
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

						player_used_escape = false;

						save_game();

						pause_game();
						return;
					}

					current_enemy_health = enemy[enemyIndex].health;
					pause_game();
					continue;
				}

				// Atak przeciwnika
				int enemyAttack = rand() % 100;

				if (enemyAttack < enemy[enemyIndex].atk_chance) {
					player_health -= enemy[enemyIndex].damage;
					clear_screen();
					cout << endl;
					cout << "	Tura przeciwnika!\n";
					cout << endl;
					cout << "	Przeciwnik zadał Ci " << enemy[enemyIndex].damage << " obrażeń!\n";
					cout << endl;
					pause_game();
				}
				else {
					clear_screen();
					cout << endl;
					cout << "	Tura przeciwnika!\n";
					cout << endl;
					cout << "	Przeciwnik nie trafił! Nie otrzymujesz żadnych obrażeń!\n";
					cout << endl;
					pause_game();
				}

				// GAME OVER
				if (player_health <= 0) {
					clear_screen();
					cout << endl;
					cout << "	GAME OVER!\n";
					cout << endl;
					pause_game();
					exit(0);
				}
			}

			// Ucieczka
			else if (choice == 1) {

				if (player_used_escape) {

					clear_screen();

					cout << endl;
					cout << "	Możesz uciec tylko raz na poziom!\n";
					cout << endl;

					pause_game();
					continue;
				}

				clear_screen();

				int heal = player_maxhealth / 2;

				player_health += heal;

				if (player_health > player_maxhealth)
					player_health = player_maxhealth;

				player_used_escape = true;

				cout << endl;
				cout << "	Uciekasz z pola walki!\n";
				cout << "	Odzyskano " << heal << " HP!\n";
				cout << endl;

				pause_game();
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

		clear_screen();

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
	clear_screen();

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
		pause_game();
		return;
	}
	else {
		cout << endl;
		cout << "Za mało hajsu!" << endl;
		cout << endl;
		pause_game();
	}
}

// Umiejętności i Wzmocnienia

void game_skills() {

	int choice = 0;
	int keyboard_button;

	while (true) {

		clear_screen();

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

				switch (choice) {

				case 0:
					if (player_lifesteal >= MAX_LIFESTEAL) {

						clear_screen();

						cout << endl;
						cout << "	Osiagnieto maksymalny poziom Wampirycznego Ostrza!\n";
						cout << endl;

						pause_game();
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
					player_bonus_health += 210;
					player_maxhealth += 210;
					player_health += 210;
					break;

				case 3:
					if (player_bonus_accuracy >= MAX_ACCURACY) {

						clear_screen();

						cout << endl;
						cout << "	Osiagnieto maksymalną ilość Kryształów Skupienia!\n";
						cout << endl;

						pause_game();
						continue;
					}

					player_bonus_accuracy += 15;

					if (player_bonus_accuracy > MAX_ACCURACY)
						player_bonus_accuracy = MAX_ACCURACY;
					break;
				}

				clear_screen();

				cout << endl;
				cout << "	Kupiono: " << skills[choice].name << endl;
				cout << endl;
				player_money -= skills[choice].price;

				pause_game();
			}
			else {

				clear_screen();

				cout << endl;
				cout << "	Za mało hajsu!\n";
				cout << endl;

				pause_game();
			}
		}

		else if (keyboard_button == 27) {
			return;
		}
	}
}

// Menu i opcje gry:

void game_options() {

	const int choice_amount = 4;
	string choices[choice_amount] = {
		"Pomoc",
		"Zapisz grę",
		"Twórcy",
		"Wyjście z gry"
	};

	while (true) {

		int choice = 0;
		int keyboard_button;

		while (true) {

			clear_screen();
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
			manual_save();
			break;

		case 2:
			game_credits();
			break;

		case 3:
			exit(0);
			break;
		}
	}
}

void game_credits() {

	int keyboard_button;

	while (true) {

		clear_screen();

		cout << "ESC - Powrot\n";
		cout << endl;
		cout << "	Twórcy\n";
		cout << endl;
		cout << "	Legends of Wpierdol\n";
		cout << endl;
		cout << "	Programowanie:\n";
		cout << "	szczurek9\n";
		cout << endl;
		cout << "	Game Design:\n";
		cout << "	szczurek9\n";
		cout << endl;
		cout << "	Balancing:\n";
		cout << "	chyba szczurek9\n";
		cout << endl;
		cout << "	Writing:\n";
		cout << "	też szczurek9\n";
		cout << endl;
		cout << "	Specjalne podziękowania:\n";
		cout << "	Owcacejk, Mispolarny1, maximum412, Toster57\n";
		cout << "	Galaxy S22\n";
		cout << "	Akali mains\n";
		cout << "	Valve za przycisk `\n";
		cout << endl;
		cout << "	Version: 1.0\n";
		cout << endl;

		keyboard_button = _getch();

		if (keyboard_button == 27) {
			return;
		}
	}
}

void game_help() {

	const int choice_amount = 2;
	string choices[choice_amount] = { "Walka","Sklep, Wzmocnienia i Umiejętności" };

	while (true) {

		int choice = 0;
		int keyboard_button;

		while (true) {

			clear_screen();
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
		clear_screen();
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
		clear_screen();
		cout << "ESC - Powrót\n";
		cout << endl;
		cout << "	Sklep, Wzmocnienia i Umiejętności\n";
		cout << "		1. W Legends of Wpierdol sklep działa w taki sposób że zakup broni usuwa\n";
		cout << "		   aktualną broń z twojego ekwipunku. Bronie kupujesz za pomocą pieniędzy\n";
		cout << "		   zdobytych poprzez walkę z przeciwnikami\n";
		cout << "\n";
		cout << "		2. W Legends of Wpierdol wzmocnienia dają dodatkowe statystyki do gracza.\n";
		cout << "		   Umiejętności pozwalają na różne bonusy takie jak lifesteal czy większa\n";
		cout << "		   szansa na trafienie przeciwników. Kupujesz je za pomocą pieniędzy\n";

		keyboard_button = _getch();

		if (keyboard_button == 27) {
			return;
		}

	}
}

void debug_console() {

	clear_screen();

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

		if (value < 0)
			value = 0;

		player_money += value;

		cout << "\nDodano $" << value << endl;
	}

	// HEALTH
	else if (command == "health") {

		if (value < 1)
			value = 1;

		player_health = value;

		if (player_health > player_maxhealth)
			player_health = player_maxhealth;

		cout << "\nUstawiono HP na " << player_health << endl;
	}

	// LEVEL
	else if (command == "level") {

		if (value < 1)
			value = 1;

		if (value > enemy_amount)
			value = enemy_amount;

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

	cin.clear();
	cin.ignore(numeric_limits<streamsize>::max(), '\n');

	pause_game();
}

void manual_save() {

	clear_screen();

	save_game();

	cout << endl;
	cout << "	Gra została zapisana!\n";
	cout << endl;

	pause_game();
}

bool save_exists() {

	ifstream file("save.sav");

	return file.good();
}

void save_game() {

	ofstream file("save.sav");

	if (!file)
		return;

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

	if (!file)
		return false;

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