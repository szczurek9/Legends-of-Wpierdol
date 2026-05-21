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
    string choices[choice_amount] = {
        "Walcz",
        "Użyj Mikstury",
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
                int final_dodge = enemy[enemyIndex].dodge_chance - player_bonus_accuracy;
                if (final_dodge < 0) final_dodge = 0;

                if (miss < final_dodge) {
                    clear_screen();
                    cout << endl << "	Twoja tura!\n" << endl;
                    cout << "	Przeciwnik uniknął twojego ataku!\n" << endl;
                    pause_game();
                }
                else {
                    int dealt_damage = player_weapon_damage;
                    if (dealt_damage > current_enemy_health)
                        dealt_damage = current_enemy_health;

                    current_enemy_health -= player_weapon_damage;

                    int heal = (dealt_damage * player_lifesteal) / 100;
                    player_health += heal;
                    if (player_health > player_maxhealth)
                        player_health = player_maxhealth;

                    clear_screen();
                    cout << endl << "	Twoja tura!\n" << endl;
                    cout << "	Zadałeś " << player_weapon_damage << " obrażeń!\n";
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

                    if (seconds < 15)
                        multiplier = 2.0f;
                    else if (seconds < 30)
                        multiplier = 1.5f;
                    else if (seconds < 60)
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

                    int finalDamage = enemy[enemyIndex].damage * (1.0f - reductionPercent);

                    if (finalDamage < 1)
                        finalDamage = 1;

                    int amountOfDamageReducted = originalDamage - finalDamage;

                    player_health -= finalDamage;

                    clear_screen();
                    cout << endl << "	Tura przeciwnika!\n" << endl;

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
            }

            else if (choice == 1) {

                if (player_health_potion <= 0) {

                    clear_screen();

                    cout << "\n	Nie masz żadnych mikstur!\n\n";

                    pause_game();
                    continue;
                }

                int healAmount = (int)(20 + (player_maxhealth * 0.05f));

                if (healAmount < 1)
                    healAmount = 1;

                if (healAmount > 150)
                    healAmount = 150;

                player_health += healAmount;

                if (player_health > player_maxhealth)
                    player_health = player_maxhealth;

                player_health_potion--;

                clear_screen();

                cout << "\n	Użyto Mikstury Zdrowia!\n";
                cout << "	Odzyskano " << healAmount << " HP!\n";
                cout << "	Pozostało mikstur: "
                    << player_health_potion
                    << "\n\n";

                pause_game();
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