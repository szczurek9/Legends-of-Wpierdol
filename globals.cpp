#include <clocale>
#include "globals.h"

string nickname;
int player_level = 1;
int player_money = 5;
int player_health = 100;
int player_maxhealth = 100;
string player_weapon_name = "nic";
int player_weapon_damage = 0;
bool player_used_escape = false;

int player_lifesteal = 0;
int player_bonus_health = 0;
int player_bonus_accuracy = 0;

int player_armor = 0;

const int MAX_LIFESTEAL = 20;
const int MAX_ACCURACY = 30;
const int MAX_CRIT = 100;

bool player_second_breath = false;
int player_escape_count = 0;

int current_wave;
int total_waves;
int current_enemy_health;
int current_enemy_damage;

int player_weapon_price = 0;

int player_health_potion = 0;

int player_precision_potion = 0;
int player_vampire_potion = 0;

int precision_bonus = 0;
int precision_turns = 0;

int vampire_bonus = 0;
int vampire_turns = 0;

int player_crit_chance = 0;

battle_enemy enemy[] = {
	//   ID,  Name,                           HP,    DMG,  Atk, Crit, PAtk, Dge, Boss, Wve,   Krd

	{    1, "Menel spod Żabki",               30,    3,    25,  2,    95,   5,   false,  1,   5    },
	{    2, "Sebix z osiedla",                40,    4,    28,  2,    93,   6,   false,  1,   10   },
	{    3, "Dresiarz",                       55,    5,    30,  3,    92,   8,   false,  1,   15   },
	{    4, "Sąsiad",                         75,    7,    32,  3,    90,   10,  false,  1,   25   },
	{    5, "Kierownik Żabki",                110,   10,   38,  5,    88,   12,   true,  1,   80   },

	{    6, "Hazardzista",                    80,    6,    34,  4,    88,   10,  false,  1,   30   },
	{    7, "Speedrunner bez snu",            95,    8,    36,  5,    86,   12,  false,  1,   35   },
	{    8, "Discord Mod",                    120,   10,   38,  5,    84,   15,  false,  1,   40   },
	{    9, "Eggman po 3 energetykach",       140,   12,   40,  6,    82,   18,  false,  1,   45   },
	{    10, "Peter Griffin",                 180,   15,   45,  7,    80,   20,   true,  1,   100  },

	{    11, "Patrick Star",                  220,   18,   42,  7,    78,   22,  false,  2,   45   },
	{    12, "Gracz League po tiltcie",       260,   20,   44,  8,    76,   24,  false,  2,   50   },
	{    13, "Yone OTP",                      300,   22,   46,  10,   74,   26,  false,  2,   100  },
	{    14, "Edgar z Brawl Stars",           340,   25,   48,  10,   72,   28,  false,  2,   100  },
	{    15, "Kasjerka w Żabce",              420,   30,   52,  12,   70,   30,   true,  2,   150  },

	{    16, "Ash Ketchum",                   480,   35,   50,  12,   68,   32,  false,  2,   100  },
	{    17, "Gracz League po 12 rankedach",  550,   40,   52,  13,   66,   34,  false,  2,   120  },
	{    18, "Akali main (tilt mode)",        650,   45,   54,  15,   64,   36,  false,  2,   125  },
	{    19, "Dis gdy JD",                    750,   50,   56,  15,   62,   38,  false,  2,   150  },
	{    20, "Adam Smasher z Allegro",        1000,  60,   60,  18,   60,   40,   true,  2,   175  },

	{    21, "Speedrunner DMC",               1200,  65,   58,  18,   58,   42,  false,  2,   200  },
	{    22, "Moderator Reddita",             1400,  70,   60,  20,   56,   44,  false,  2,   210  },
	{    23, "Silver Yasuo 0/10",             1600,  80,   62,  22,   54,   46,  false,  2,   220  },
	{    24, "Speedrunner Elden Ring",        2000,  90,   64,  22,   52,   48,  false,  2,   220  },
	{    25, "Dante z DMC",                   2500,  110,  68,  25,   50,   50,   true,  2,   240  },

	{    26, "Irak Bomber",                   3000,  120,  66,  25,   48,   52,  false,  3,   230  },
	{    27, "Mariusz Pudzianowski",          3800,  130,  67,  28,   46,   54,  false,  3,   240  },
	{    28, "Vergil z DMC",                  4000,  150,  68,  30,   44,   56,  false,  3,   240  },
	{    29, "Chester Bernington",            4500,  170,  69,  30,   42,   58,  false,  3,   240  },
	{    30, "Song So-Mi (Songbird)",         5000,  200,  70,  32,   40,   60,   true,  3,   275  },

	{    31, "Samsung Galaxy S22",            5000,  230,  70,  32,   38,   62,  false,  3,   300  },
	{    32, "6pek",                          6000,  260,  71,  34,   36,   64,  false,  3,   300  },
	{    33, "Michael Jackson",               7000,  300,  72,  35,   34,   66,  false,  3,   320  },
	{    34, "Gordon Freeman",                8000,  350,  73,  35,   32,   68,  false,  3,   330  },
	{    35, "Gabe Newell",                   10000, 400,  75,  38,   30,   70,   true,  3,   350  },

	{    36, "Kostecka",                      13000, 450,  74,  38,   28,   72,  false,  3,   330  },
	{    37, "Walter White",                  15000, 500,  75,  40,   26,   74,  false,  3,   340  },
	{    38, "Owcacejk",                      20000, 600,  76,  40,   24,   76,  false,  3,   367  },
	{    39, "Pan Ryszard",                   25000, 700,  80,  40,   22,   78,  false,  3,   400  },
	{    40, "Legenda Wpierdolu",             30000, 750,  90,  40,   20,   80,   true,  5,   500  }
};

shop_weapons shop[] = {
	{"Patyk", 3, 3},
	{"Trzon od siekiery", 10, 6},
	{"Zardzewiała Rura", 15, 8},
	{"Gazrurka", 20, 10},
	{"Nóż kuchenny", 50, 15},
	{"Maczeta", 70, 26},
	{"Zardzewiała Katana", 120, 50},
	{"Żelazny Miecz", 200, 71},
	{"Baseball z gwoździami", 260, 86},
	{"Katana", 320, 120},
	{"Pistolet Jhin'a", 444, 224},
	{"Latarnia Thresha", 667, 306},
	{"Miecze Yone", 850, 390},
	{"Shurikeny Akali", 1200, 527},
	{"Yamato", 1650, 700},
	{"Ebony & Ivory", 2000, 820},
	{"Mantis Blades", 2700, 930},
	{"Malorian 3516", 3000, 1000},
	{"GTX 1080 TI", 3600, 1800},
	{"Nokia 3310", 4800, 3310}
};

upgrade_item upgrades[] = {

	{"Skała Zdrowia", 180, "   +50 maksymalnego ♥"},
	{"Woda Życia", 560, "   +160 maksymalnego ♥"},
	{"Kolczuga", 220, "   +15 pancerza"}
};

skill_item abilities[] = {

	{"Wampiryczne Ostrze", 580, "   Leczy za 10% zadanych obrazeń (maksymalnie 20%)"},
	{"Kryształ Skupienia", 420, "   15% mniejsza szansa na nietrafienie ataku (maksymalnie 30%)"},
	{"Pierścień Zabójcy", 500, "   10% szansy na trafienie krytyczne (do 50%)"},
	{"Płaszcz Assasyna", 1000, "   10% szansy na trafienie krytyczne (powyżej 50%)"},
	{"Drugie Tchnienie", 3000, "   Pokonanie przeciwnika leczy cię o 25% maksymalnego ♥"}
};

consumable_item consumables[] = {

	{"Mikstura Zdrowia", 60, "   Leczy 25 + 7% maksymalnego ♥ podczas walki (maksymalnie leczy ♥ 250) "},
	{"Eliksir Precyzji", 350, "   Zwiększa twoją szanse na atak o 50% na 7 tur (nie stackuje się)"},
	{"Koktajl Wampira", 200, "   Leczysz się za 10% zadanych obrażeń na 10 tur (nie stackuje się) "}
};

const int enemy_amount = sizeof(enemy) / sizeof(enemy[0]);
const int shop_weapon_amount = sizeof(shop) / sizeof(shop[0]);
const int upgrades_amount = sizeof(upgrades) / sizeof(upgrades[0]);
const int abilities_amount = sizeof(abilities) / sizeof(abilities[0]);
const int consumables_amount = sizeof(consumables) / sizeof(consumables[0]);

InventoryWeapon inventory[max_inventory];

int inventory_count = 0;
