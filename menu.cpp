#include "menu.h"
#include "globals.h"
#include "utils.h"
#include "battle.h"
#include "shop.h"
#include "save.h"
#include <iostream>
#include <limits>

void debug_console()
{
	clear_screen();
	std::string command;
	int value;

	std::cout << "=== DEBUG CONSOLE ===\n\n";
	std::cout << "Komendy:\n";
	std::cout << "money <liczba> - dodaj pieniądze\n";
	std::cout << "health <liczba> - ustaw ilosc zdrowia\n";
	std::cout << "level <liczba> - ustaw level\n";
	std::cout << "weapon <id> - ustaw broń\n";
	std::cout << "Aby wyjść z konsoli rekomenduje wpisanie np. money 0\n\n";
	std::cout << "> ";

	std::cin >> command >> value;

	if (command == "money")
	{
		if (value < 0)
			value = 0;
		player_money += value;
		std::cout << "\nDodano $" << value << std::endl;
	}
	else if (command == "health")
	{
		if (value < 1)
			value = 1;
		player_health = value;
		if (player_health > player_maxhealth)
			player_health = player_maxhealth;
		std::cout << "\nUstawiono HP na " << player_health << std::endl;
	}
	else if (command == "level")
	{
		if (value < 1)
			value = 1;
		if (value > enemy_amount)
			value = enemy_amount;
		player_level = value;
		std::cout << "\nUstawiono level na " << player_level << std::endl;
	}
	else if (command == "weapon")
	{
		if (value >= 0 && value < shop_weapon_amount)
		{
			player_weapon_name = shop[value].name;
			player_weapon_damage = shop[value].damage;
			std::cout << "\nOtrzymano bron: " << player_weapon_name << std::endl;
		}
		else
		{
			std::cout << "\nNiepoprawne ID broni!\n";
		}
	}
	else
	{
		std::cout << "\nNieznana komenda!\n";
	}

	std::cout << std::endl;
	std::cin.clear();
	pause_game();
}

void game_menu()
{

	const int choice_amount = 4;

	std::string choices[choice_amount] = {
		"Graj",
		"Zbrojownia",
		"Ekwipunek",
		"Opcje" };

	while (true)
	{
		int choice = 0;
		int keyboard_button;

		while (true)
		{
			clear_screen();
			std::cout << std::endl;

			std::cout << "\tNick: ";
			print_colored(nickname, player_nickname_color);

			std::cout << " | ";
			ui_colored("LVL: ", player_level, COLOR_BLUE);

			std::cout << std::endl;

			std::cout << "\t";
			ui_colored("HP: ", player_health, COLOR_GREEN);
			ui_colored("/", player_maxhealth, COLOR_GREEN);

			std::cout << " | ";
			ui_colored("Pancerz: ", player_armor, COLOR_YELLOW);

			std::cout << std::endl;

			std::cout << "\t";
			ui_colored("Hajs: $", player_money, COLOR_DARK_GREEN);

			std::cout << std::endl;

			// Wyświetl klasę postaci
			std::cout << "\t";
			print_colored("Klasa: ", COLOR_BRIGHT_WHITE);
			switch (player_class)
			{
			case CLASS_ASSASSIN:
				print_colored("Zabójca", COLOR_RED);
				break;
			case CLASS_MAGE:
				print_colored("Mag", COLOR_BLUE);
				break;
			case CLASS_TANK:
				print_colored("Tank", COLOR_YELLOW);
				break;
			case CLASS_SAMURAI:
				print_colored("Samuraj", COLOR_CYAN);
				break;
			default:
				std::cout << "Brak";
				break;
			}
			std::cout << " | ";
			print_colored("Mana: ", COLOR_BLUE);
			number_colored(player_mana, COLOR_BLUE);
			std::cout << "/";
			number_colored(player_maxmana, COLOR_BLUE);
			std::cout << std::endl;

			std::cout << "\tBroń: ";
			print_colored(player_weapon_name, COLOR_CYAN);

			std::cout << " - ";
			r_ui_colored(player_weapon_damage, " DMG", COLOR_RED);

			std::cout << " | Kryt: ";
			r_ui_colored(player_crit_chance, "%", COLOR_PURPLE);

			std::cout << std::endl
				<< std::endl;
			for (int i = 0; i < choice_amount; i++)
			{
				if (i == choice)
				{
					std::cout << "	►  ";
					print_colored(choices[i], COLOR_BRIGHT_WHITE);
					std::cout << std::endl;
				}
				else
					std::cout << "	  " << choices[i] << std::endl;
			}

			keyboard_button = _getch();

			if (keyboard_button == 224)
			{
				keyboard_button = _getch();
				if (keyboard_button == 72)
				{
					choice--;
					if (choice < 0)
						choice = choice_amount - 1;
				}
				else if (keyboard_button == 80)
				{
					choice++;
					if (choice >= choice_amount)
						choice = 0;
				}
			}
			else if (keyboard_button == 13)
			{
				break;
			}
			else if (keyboard_button == '`')
			{
				debug_console();
			}
		}

		switch (choice)
		{
		case 0:
			game_battle();
			break;
		case 1:
			game_armory();
			break;
		case 2:
			game_inventory();
			break;
		case 3:
			game_options();
			break;
		}
	}
}

void game_help_battle()
{
	int keyboard_button;
	while (true)
	{
		clear_screen();
		std::cout << "ESC - Powrót\n\n";
		std::cout << "	Walka\n";
		std::cout << "		1. Walka w Legends of Wpierdol działa podobnie do systemu walki w Pokemonach.\n";
		std::cout << "		   Gracz atakuje swoją bronią a gra wylicza prawdopodobieństwo trafienia ataku:\n";
		std::cout << "		   Jeżeli trafi - zadaje obrażenia przeciwnikowi, jeśli nie - przeciwnikowi\n";
		std::cout << "		   nic się nie stanie. Potem przeciwnik atakuje w taki sam sposób.\n";
		keyboard_button = _getch();
		if (keyboard_button == 27)
			return;
	}
}

void game_help_shop()
{
	int keyboard_button;
	while (true)
	{
		clear_screen();
		std::cout << "ESC - Powrót\n\n";
		std::cout << "	Zbrojownia\n";
		std::cout << "		1. W Legends of Wpierdol sklep działa w taki sposób że zakup broni wrzuca\n";
		std::cout << "		   aktualną broń z twojego ekwipunku.\n\n";
		std::cout << "		2. Wzmocnienia dają dodatkowe statystyki do gracza.\n";
		std::cout << "		3. Umiejętności dają dodatkowe, unikalne statystyki.\n";
		keyboard_button = _getch();
		if (keyboard_button == 27)
			return;
	}
}

void game_help()
{
	const int choice_amount = 2;
	std::string choices[choice_amount] = { "Walka", "Sklep, Wzmocnienia i Umiejętności" };

	while (true)
	{
		int choice = 0;
		int keyboard_button;

		while (true)
		{
			clear_screen();
			std::cout << "ESC - Powrót\n\n";
			std::cout << "	Pomoc\n\n	Wybierz temat:\n\n";

			for (int i = 0; i < choice_amount; i++)
			{
				if (i == choice)
				{
					std::cout << "	►  ";
					print_colored(choices[i], COLOR_BRIGHT_WHITE);
					std::cout << std::endl;
				}
				else
					std::cout << "	  " << choices[i] << std::endl;
			}

			keyboard_button = _getch();

			if (keyboard_button == 224)
			{
				keyboard_button = _getch();
				if (keyboard_button == 72)
				{
					choice--;
					if (choice < 0)
						choice = choice_amount - 1;
				}
				else if (keyboard_button == 80)
				{
					choice++;
					if (choice >= choice_amount)
						choice = 0;
				}
			}
			else if (keyboard_button == 13)
				break;
			else if (keyboard_button == 27)
				return;
		}

		switch (choice)
		{
		case 0:
			game_help_battle();
			break;
		case 1:
			game_help_shop();
			break;
		}
	}
}

void game_credits()
{
	int keyboard_button;
	while (true)
	{
		clear_screen();
		std::cout << "ESC - Powrót\n\n";
		std::cout << "	Twórcy\n\n";
		std::cout << "	Legends of Wpierdol - by MSLG Studios\n\n";
		std::cout << "	Programowanie:\n	szczurek9\n\n";
		std::cout << "	Balancing:\n	Owcacejk i na pewno nie szczurek9\n\n";
		std::cout << "	Game Testing:\n	Owcacejk\n\n";
		std::cout << "	Bug Fixer:\n	Mispolarny1\n\n";
		std::cout << "	Specjalne podziękowania:\n";
		std::cout << "	Maximum412\n";
		std::cout << "	Galaxy S22\n	Akali mains\n	Valve za przycisk `\n\n";
		std::cout << "	Version: 1.5.2\n\n";
		keyboard_button = _getch();
		if (keyboard_button == 27)
			return;
	}
}

void game_options()
{
	const int choice_amount = 5;
	std::string choices[choice_amount] = { "Zapisz grę", "Zmień kolor nicku", "Pomoc", "Twórcy", "Wyjście z gry" };

	while (true)
	{
		int choice = 0;
		int keyboard_button;

		while (true)
		{
			clear_screen();
			std::cout << "ESC - Powrót\n\n";
			std::cout << "	Opcje:\n\n";

			for (int i = 0; i < choice_amount; i++)
			{
				if (i == choice)
				{
					std::cout << "	►  ";
					print_colored(choices[i], COLOR_BRIGHT_WHITE);
					std::cout << std::endl;
				}
				else
					std::cout << "	  " << choices[i] << std::endl;
			}

			keyboard_button = _getch();

			if (keyboard_button == 224)
			{
				keyboard_button = _getch();
				if (keyboard_button == 72)
				{
					choice--;
					if (choice < 0)
						choice = choice_amount - 1;
				}
				else if (keyboard_button == 80)
				{
					choice++;
					if (choice >= choice_amount)
						choice = 0;
				}
			}
			else if (keyboard_button == 13)
				break;
			else if (keyboard_button == 27)
				return;
		}

		switch (choice)
		{
		case 0:
			manual_save();
			break;
		case 1:
			game_nickname_color();
			break;
		case 2:
			game_help();
			break;
		case 3:
			game_credits();
			break;
		case 4:
			exit(0);
			break;
		}
	}
}

void game_inventory()
{

	while (true)
	{

		clear_screen();

		std::cout << "\n\tEkwipunek\n\n";

		// Założone przedmioty magiczne
		bool any_magic = false;
		for (int i = 0; i < get_max_magic_items(); i++)
		{
			if (magic_item_slots[i] >= 0)
			{
				if (!any_magic)
				{
					any_magic = true;
				}
				int idx = magic_item_slots[i];
				std::cout << "\tM" << i + 1 << " ";
				print_colored(magic_items[idx].name, COLOR_BLUE);
				std::cout << " | ";
				r_ui_colored(magic_items[idx].price * 40 / 100, "$", COLOR_DARK_GREEN);
				std::cout << "\n";
			}
		}
		if (any_magic)
			std::cout << "\n";

		if (inventory_count <= 0 && !any_magic)
		{
			std::cout << "\tBrak przedmiotów.\n\n";
			pause_game();
			return;
		}

		if (inventory_count > 0)
		{
			for (int i = 0; i < inventory_count; i++)
			{

				std::cout << "\t" << i + 1 << ". ";

				print_colored(inventory[i].name, COLOR_CYAN);

				std::cout << " | ";

				number_colored(inventory[i].damage, COLOR_RED);

				std::cout << " DMG | ";

				r_ui_colored(inventory[i].price * 40 / 100, "$", COLOR_DARK_GREEN);

				std::cout << std::endl;
			}
		}

		std::cout << "\n\t0. Powrót";
		std::cout << "\n\n\tWybierz: ";

		std::string input;
		std::cin >> input;

		// Slot magiczny: gracz wpisał np. "M1".."M6"
		if (input.size() >= 2 && (input[0] == 'M' || input[0] == 'm'))
		{
			int slot = (input[1] - '1'); // M1 -> 0, M2 -> 1, ...
			if (slot < 0 || slot >= get_max_magic_items() || magic_item_slots[slot] < 0)
			{
				continue;
			}

			int idx = magic_item_slots[slot];
			clear_screen();
			std::cout << "\n\t";
			print_colored(magic_items[idx].name, COLOR_BLUE);
			std::cout << "\n\n\t1. ";
			print_colored("Zdejmij", COLOR_RED);
			std::cout << "\n\t0. Powrót\n\n";

			int magic_action;
			std::cin >> magic_action;

			if (magic_action == 1)
			{
				int sell_price = magic_items[idx].price * 40 / 100;
				magic_item_slots[slot] = -1;
				magic_item_count--;
				player_money += sell_price;
				recalculate_magic_stats();
				clear_screen();
				std::cout << "\n";
				std::cout << "\n\tZdjęto: ";
				print_colored(magic_items[idx].name, COLOR_BLUE);
				std::cout << "\n\tOdzyskano: ";
				r_ui_colored(sell_price, "$", COLOR_DARK_GREEN);
				std::cout << "\n";
			}

			pause_game();
			continue;
		}

		// Broń z ekwipunku
		int choice;
		try
		{
			choice = std::stoi(input);
		}
		catch (...)
		{
			continue;
		}

		if (choice == 0)
			return;

		choice--;

		if (choice < 0 || choice >= inventory_count)
			continue;

		clear_screen();

		std::cout << "\n\t";
		print_colored(inventory[choice].name, COLOR_CYAN);
		std::cout << std::endl;

		std::cout << "\t1. ";
		print_colored("Załóż", COLOR_GREEN);
		std::cout << std::endl;

		std::cout << "\t2. ";
		print_colored("Sprzedaj", COLOR_RED);
		std::cout << std::endl;

		std::cout << "\t0. Powrót\n\n";

		int action;
		std::cin >> action;

		if (action == 1)
		{

			std::string oldName = player_weapon_name;
			int oldDamage = player_weapon_damage;
			int oldPrice = player_weapon_price;

			player_weapon_name = inventory[choice].name;
			player_weapon_damage = inventory[choice].damage;
			player_weapon_price = inventory[choice].price;

			inventory[choice].name = oldName;
			inventory[choice].damage = oldDamage;
			inventory[choice].price = oldPrice;

			std::cout << "\n\tZałożono broń!\n";
		}
		else if (action == 2)
		{

			int money = inventory[choice].price * 40 / 100;

			player_money += money;

			for (int i = choice; i < inventory_count - 1; i++)
			{

				inventory[i] = inventory[i + 1];
			}

			inventory_count--;

			std::cout << "\n\tSprzedano za ";
			r_ui_colored(money, "$", COLOR_DARK_GREEN);
			std::cout << "!\n";
		}

		pause_game();
	}
}

void game_armory()
{

	const int choice_amount = 5;

	std::string choices[choice_amount] = {
		"Rynek Broni",
		"Wzmocnienia",
		"Umiejętności",
		"Alchemik",
		"Magiczny Rynek" };

	while (true)
	{

		int choice = 0;
		int keyboard_button;

		int menuColors[choice_amount] = {
			COLOR_CYAN,   // Rynek Broni
			COLOR_YELLOW, // Wzmocnienia
			COLOR_PURPLE, // Umiejętności
			COLOR_GREEN,  // Alchemik
			COLOR_BLUE    // Magiczny Rynek
		};

		while (true)
		{

			clear_screen();

			std::cout << "ESC - Powrót\n\n";

			print_colored("\tZbrojownia", COLOR_YELLOW);
			std::cout << "\n\n";

			for (int i = 0; i < choice_amount; i++)
			{

				if (i == choice)
				{

					print_colored("\t ► ", COLOR_BRIGHT_WHITE);
					print_colored(choices[i], menuColors[i]);
				}
				else
				{

					std::cout << "\t  ";
					print_colored(choices[i], menuColors[i]);
				}

				std::cout << std::endl;
			}

			keyboard_button = _getch();

			if (keyboard_button == 224)
			{

				keyboard_button = _getch();

				if (keyboard_button == 72)
				{

					choice--;

					if (choice < 0)
						choice = choice_amount - 1;
				}
				else if (keyboard_button == 80)
				{

					choice++;

					if (choice >= choice_amount)
						choice = 0;
				}
			}
			else if (keyboard_button == 13)
			{
				break;
			}
			else if (keyboard_button == 27)
			{
				return;
			}
		}

		switch (choice)
		{

		case 0:
			game_shop();
			break;

		case 1:
			game_upgrades();
			break;

		case 2:
			game_abilities();
			break;
		case 3:
			game_consumables();
			break;
		case 4:
			game_magic_shop();
			break;
		}
	}
}

void game_nickname_color()
{

	const int color_amount = 15;

	std::string color_names[color_amount] = {
		"Biały",
		"Jasny Biały",

		"Niebieski",
		"Ciemny Niebieski",

		"Zielony",
		"Ciemny Zielony",

		"Cyan",
		"Ciemny Cyan",

		"Czerwony",
		"Ciemny Czerwony",

		"Fioletowy",
		"Ciemny Fioletowy",

		"Żółty",
		"Ciemny Żółty",

		"Szary" };

	int colors[color_amount] = {
		COLOR_WHITE,
		COLOR_BRIGHT_WHITE,

		COLOR_BLUE,
		COLOR_DARK_BLUE,

		COLOR_GREEN,
		COLOR_DARK_GREEN,

		COLOR_CYAN,
		COLOR_DARK_CYAN,

		COLOR_RED,
		COLOR_DARK_RED,

		COLOR_PURPLE,
		COLOR_DARK_PURPLE,

		COLOR_YELLOW,
		COLOR_DARK_YELLOW,

		COLOR_GRAY };

	int choice = 0;

	while (true)
	{

		clear_screen();

		std::cout << "ESC - Powrót\n\n";
		std::cout << "\tKolor Nicku\n\n";

		for (int i = 0; i < color_amount; i++)
		{

			if (i == choice)
				std::cout << "\t► ";
			else
				std::cout << "\t  ";

			set_color(colors[i]);
			std::cout << color_names[i];
			reset_color();

			std::cout << std::endl;
		}

		int key = _getch();

		if (key == 224)
		{

			key = _getch();

			if (key == 72)
			{

				choice--;

				if (choice < 0)
					choice = color_amount - 1;
			}
			else if (key == 80)
			{

				choice++;

				if (choice >= color_amount)
					choice = 0;
			}
		}
		else if (key == 13)
		{

			player_nickname_color = colors[choice];

			clear_screen();

			std::cout << "\n\tZmieniono kolor nicku!\n\n";

			pause_game();

			return;
		}
		else if (key == 27)
		{

			return;
		}
	}
}