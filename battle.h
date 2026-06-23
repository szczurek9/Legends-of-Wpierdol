#pragma once
void game_battle();
void battle_items_menu();
void battle_spells_menu(int enemyIndex, bool& spell_used);
int get_max_magic_items();
int get_total_ap();
void adept_book_on_spell(int ap_override = 0);
