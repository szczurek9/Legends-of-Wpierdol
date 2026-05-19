#include "battle.h"
#include "globals.h"
#include "utils.h"
#include "save.h"
#include <iostream>
#include <cstdlib>
using namespace std;

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
        cout << "	Twoje HP: ";
        draw_hp_bar(player_health, player_maxhealth);

        cout << endl; 
        cout << "	Broń: " << player_weapon_name << " - " << player_weapon_damage << " DMG" << endl;

        cout << "	" << enemy[enemyIndex].name << ": ";
        draw_hp_bar(current_enemy_health, enemy[enemyIndex].health);

        cout << endl;
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
                    clear_screen();
                    cout << endl << "	Twoja tura!\n" << endl;
                    cout << "	Przeciwnik pokonany!\n" << endl;

                    player_money += enemy[enemyIndex].kill_reward;
                    current_wave++;

                    if (current_wave > total_waves) {
                        cout << "	Gratulacje! Przeszłeś poziom " << player_level << "!\n" << endl;
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

                int enemyAttack = rand() % 100;
                if (enemyAttack < enemy[enemyIndex].atk_chance) {
                    player_health -= enemy[enemyIndex].damage;
                    clear_screen();
                    cout << endl << "	Tura przeciwnika!\n" << endl;
                    cout << "	Przeciwnik zadał Ci " << enemy[enemyIndex].damage << " obrażeń!\n" << endl;
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

                if (player_used_escape) {
                    clear_screen();
                    cout << endl << "	Możesz uciec tylko raz na poziom!\n" << endl;
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
                cout << "	Odzyskano " << heal << " HP!\n" << endl;
                pause_game();
                return;
            }
        }
    }
}