// g++ -std=c++11 -o output game.cpp
// ./output

#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <algorithm> 
#include <memory>
using namespace std;

// set rng
random_device rd;          
mt19937 gen(rd()); 

enum MonsterType {goblin, troll, orc};
enum OperatingSystem {windows, linux};

// detect current os
constexpr OperatingSystem CURR_OS =
#ifdef _WIN32
    windows;
#else
    linux;
#endif

// helper functions
string monsterTypeToString(MonsterType type) {
    // translate the monster type enumerator to string
    switch (type) {
        case goblin: return "Goblin";
        case troll: return "Troll";
        case orc: return "Orc";
        default: return "Unknown Type";
    }
}

vector<string> getNamePool() {
    // random names to be given to the monsters
    // all from the Gundam series
    return {"RX", "Zaku", "Wing", "Zero", "Deathscythe", "Heavyarms", 
        "Sandrock", "Tallgeese", "Unicorn", "Banshee", "Barbatos", "Astaroth", 
        "Exia", "Dynames", "Kyrios", "Virtue", "Strike", "Freedom", "Justice",
        "Providence", "Destiny", "Impulse", "Legend", "Quanta", "OO", "Turna",
        "Burning", "Shining", "Epyon", "Kshatriya", "Sinanju", "ZGMF", "Alex",
        "Jesta", "Nu", "Jegan", "ReZEL", "ReGZ", "Guntank", "Guncannon", 
        "Zeta", "ZZ", "Xi", "Sazabi", "Sinanju", "Duel", "Buster", "Blitz", "Aegis",
        "Astray", "Akatsuki"};
}

string popName(vector<string>& vec) {
    // function that mimics "pop" in other language
    // get the last element and delete it to avoid getting the same names in one run
    if (vec.empty()) {
        throw out_of_range("Cannot pop from an empty vector.");
    }
    string value = vec.back(); // Get the last element
    vec.pop_back();         // Remove the last element
    return value;           // Return the popped value
}

string getColor(string text="default") {
    // return color only if operating system is not windows (i.e., is linux-based)
    if (CURR_OS == linux){
        if (text == "Red"){
            return "\033[31m";
        } else if (text == "Blue") {
            return "\033[34m";
        } else if (text == "default"){
            return "\033[0m"; // might change
        }else{
            return "\033[0m";
        }
    }
    return "";
}

int getPlainTextLength(const std::string& text) {
    int length = 0;
    bool in_skip = false;

    for (int i = 0; i < text.size(); i++) {
        if (text[i] == '\033') { // Start of color sequence
            in_skip = true;
        } else if (in_skip && text[i] == 'm') { // End of color sequence
            in_skip = false;
        } else if (!in_skip) { // Count regular characters other than the color seq
            length++;
        }
    }
    return length;
}


// classes
class Monster {
    // parent class that contains most specs for each monster
    public:
        MonsterType type; // type of monster (in enum)
        string name; // given name
        int max_health; // maximum health for the monster
        int health; // current health
        int damage; // damage (per attack)
        int speed; // speed that decides who attack first
        bool is_alive; // if the monster is currently alive (true/false)
        string team; // place holder for team name

        Monster () {}; // placeholder constructor

        Monster (string given_name) {
            name = given_name;
            is_alive = true;
            team = "Unspecified";
        };

        // default virtual destructor
        virtual ~Monster() = default;

        // defines behavior of attacking, goblin will be different
        virtual void attack(Monster& enemy){
            attack_text(enemy);
            enemy.on_enemy_attack(damage, this);
            cout << endl;
        };

        // defines behavior on being attacked, orc will be different
        virtual void on_enemy_attack(int amount, Monster* enemy = nullptr){
            reduce_health(amount);
            // if (is_alive) {
            //     cout << name + " remain health: " + to_string(health) + "\n";
            // }
        };

        // defines behavior on the end of each turn, troll will be different
        virtual void on_end_turn(){};

        string disp(bool with_team, bool with_color){
            string disp_text = "";
            if (with_team){
                disp_text = disp_text + team + " ";
            }
            disp_text = disp_text + monsterTypeToString(type) + " " + name;
            if (with_color) {
                disp_text = getColor(team) + disp_text + getColor();
            }
            return disp_text;
        };

        void attack_text(Monster& enemy){
            cout << disp(true, true) + " attacks " + enemy.disp(true, true) +
                " for " + to_string(damage) + " damage; ";
        };

        // when the monster's health gets reduced
        void reduce_health(int amount) {
            int reduce_amount;
            if (amount >= health) {
                reduce_amount = health;
            } else {
                reduce_amount = amount;
            }
            health -= reduce_amount;
            cout << "dealing " + to_string(reduce_amount) + "damage; ";
            if (health <= 0) {die();}
        };

        // when monster dies
        void die() {
            is_alive = false;
            cout << disp(true, true) + " has died!\n";
        };

};

class Goblin: public Monster {
    // goblin class
    public:
        int num_attack; // number of attacks delt by goblin in each turn

        Goblin(string given_name): Monster(given_name) {
            type = goblin;
            max_health = 50;
            health = max_health;
            damage = 30;
            speed = 50;
            // type specifics
            num_attack = 2;
        };

        void attack(Monster& enemy) {
            for (int i = 0; i < num_attack; i++) {
                if (enemy.is_alive) {
                    attack_text(enemy);
                    enemy.on_enemy_attack(damage, this);
                    cout << endl;
                    }
                else {break;}
            }
        };
};

class Troll: public Monster {
    // troll class
    public:
        int regen_amount; // amount of health regnerated at the end of (their own) turn

        Troll(string given_name): Monster(given_name) {
            type = troll;
            max_health = 100;
            health = max_health;
            speed = 20;
            damage = 40;
            // type specifics
            regen_amount = 20;
        };

        void on_end_turn(){
            health += regen_amount;
            if (health > max_health) {
                int regenerated = max_health - health;
                health = max_health;
                cout << disp(true, true) + " regenerates " + to_string(regenerated) + 
                    " health to " + to_string(health) + " (max)\n";
            }
            else {
                cout << disp(true, true) + " regenerates " + to_string(regen_amount) + 
                    " health to " + to_string(health) + '\n';
            }
        };
};

class Orc: public Monster {
    // orc class
    public:
        int block_amount;
        int reflect_amount;

        Orc(string given_name): Monster(given_name) {
            type = orc;
            max_health = 70;
            health = max_health;
            speed = 30;
            damage = 30;
            block_amount = 10;
            reflect_amount = 10;
        };

        void on_enemy_attack(int amount, Monster* enemy) override {
            int damage_dealt;
            int damage_reflected;
            
            if (amount > block_amount) {
                damage_dealt = amount - block_amount;

            } else {damage_dealt = 0;}

            // if (amount < reflect_amount) {
            //     damage_reflected = amount;
            // } else {damage_reflected = reflect_amount;}
            reduce_health(damage_dealt);
            cout << to_string(damage_dealt) + " dealt; remain: " + to_string(health) + "; " + 
                to_string(reflect_amount) + " reflected;\n";


            enemy->reduce_health(reflect_amount);
        };
};

class Team {
    public:
        vector<Monster*> monsters;
        Monster* active_monster;
        string name;
        bool is_defeated;
        int n_monsters;

        Team (string team_name, vector<Monster*> monster_list) {
            monsters = monster_list;
            active_monster = monsters[0];
            name = team_name;
            string lineup_text = "Team " + name + " lineup: \n";
            for (const auto& mon : monsters) { // Iterates through all elements
                // set the team variable of Monster to team name
                mon->team = team_name;
                // lineup_text += monsterTypeToString(mon->type) + " " + mon->name + "; ";
            }

            is_defeated = false;
            n_monsters = monsters.size();
            // output line-up text
            // cout << getColor(name) + lineup_text + getColor() << endl;
        };

        void update_team() {
            if (!active_monster->is_alive) {
                bool found_alive = false;
                // iterate through all monsters to check if there is an alive one
                // seems redudant but added just in case I might add a select function
                // at some point
                for (auto& mon : monsters) {
                    if (mon->is_alive) { // Check if the monster is alive
                        active_monster = mon; // Set the alive monster as active
                        found_alive = true;
                        break; // Exit the loop once we find an alive monster
                    }
                }
                if (!found_alive) {
                    // If no alive monster is found, the team is defeated
                    is_defeated = true;
                    cout << name + " Team is defeated!" << endl;
                }
            };

        };

        // void lose() {
        //     cout << name + " Team loses!" << endl;
        // };

};

// battle specific functions
void turn(Monster& mon1, Monster& mon2) {
    // function for each turn, this will run continuously until battle ends
    Monster* faster;
    Monster* slower;
    if (mon1.speed > mon2.speed) {
        faster = &mon1;
        slower = &mon2;
    } else if (mon2.speed > mon1.speed) {
        faster = &mon2;
        slower = &mon1;
    } else {
        // randomly decides who goes first
        cout << "Randomly deciding order\n";
        uniform_int_distribution<int> dist(0, 1);
        if (dist(gen) == 0) {
            faster = &mon1;
            slower = &mon2;
        } else {
            faster = &mon2;
            slower = &mon1;
        }
    }
    // in each turn, faster monster attack first,
    // then faster monster's turn end effect activate
    // if slower monster is still alive: slower monster attack
    // then slower monster's turn end effect activate
    faster->attack(*slower);
    faster->on_end_turn();
    if (slower->is_alive){
        slower->attack(*faster);
        slower->on_end_turn();
    }
};

string get_member_text(Monster& mon1, bool opposite=false){
    if (opposite) {
        return getColor(mon1.team)+
            "[ " + mon1.disp(false, false) + " (" + to_string(mon1.health) + 
            ") | " + mon1.team + " ]" + getColor();
    }
    return  getColor(mon1.team) + 
            "[ " + mon1.team + " | " + mon1.disp(false, false) + 
            " (" + to_string(mon1.health) + 
            ") ]" + getColor();
};

string get_vs_status_text(Monster& mon1, Monster& mon2, string vs_text=" ... "){
    return  get_member_text(mon1, false) + vs_text + get_member_text(mon2, true) + "\n";
};

void battle(Team* team1, Team* team2) {
    // takes two teams, end after monsters in one team are all dead
    int turn_idx = 1;

    vector<string> team1_vs_texts;
    vector<string> team2_vs_texts;
    int max_team1_len = 0;
    for (int i = 0; i < team1->n_monsters; i++) {
        string text = get_member_text(*(team1->monsters[i]));
        int plain_length = getPlainTextLength(text);
        team1_vs_texts.push_back(get_member_text(*(team1->monsters[i])));
        if (plain_length > max_team1_len) {
            max_team1_len = plain_length;
        }
    }
    for (int i = 0; i < team2->n_monsters; i++) {
        team2_vs_texts.push_back(get_member_text(*(team2->monsters[i])));
    }

    int n_less = min(team1_vs_texts.size(), team2_vs_texts.size());

    for (int i = 0; i < n_less; i++) {
        string text1 = get_member_text(*(team1->monsters[i]));
        cout << text1 + string(max_team1_len-getPlainTextLength(text1), ' ') + 
            "   " + get_member_text(*(team2->monsters[i]), true) + "\n";
    }
    if (team1->n_monsters > n_less) {
        for (int i = n_less; i < team1->n_monsters; i++) {
            cout << get_member_text(*(team1->monsters[i])) + "\n";
        }
    } else if (team2->n_monsters > n_less) {
        for (int i = n_less; i < team2->n_monsters; i++) {
            cout << string(max_team1_len+3, ' ') + // +3: corresponding to the length of text in the middle
                get_member_text(*(team2->monsters[i]), true) + "\n";
        }
    }

    
    while ((!team1->is_defeated) && (!team2->is_defeated)) {
        cout << "\nTurn " + to_string(turn_idx) + "\n";

        cout << get_vs_status_text(*(team1->active_monster), *(team2->active_monster));
        turn(*(team1->active_monster), *(team2->active_monster));
        team1->update_team();
        team2->update_team();
        turn_idx++;
        if (turn_idx>100){break;}
    }
    cout << "Battle Over!\n";

    cout << "\n-----------------------------------------------------------------------------------------------------------------------\n";

};

vector<Monster*> monster_picker(vector<string>& namepool, int n) {
    vector<Monster*> selected;
    uniform_int_distribution<> dis(0, 2);
    for (int i = 0; i < n; i++) {
        MonsterType type = static_cast<MonsterType>(dis(gen));
        switch (type) {
            case goblin: 
                selected.emplace_back(new Goblin(popName(namepool)));
                break;
            case troll: 
                selected.emplace_back(new Troll(popName(namepool)));
                break;
            case orc: 
                selected.emplace_back(new Orc(popName(namepool)));
                break;
            default: 
                selected.emplace_back(new Monster(popName(namepool))); // this should not happen (ideally)
                break;
        }
    }
    return selected;
};


// main code
int main() {

    vector<string> namepool = getNamePool();
    shuffle(namepool.begin(), namepool.end(), gen);

    int battle_idx = 1;

    cout << "\n=======================================================================================================================\n";

    // battle 1
    cout << "\nBattle #" + to_string(battle_idx) + "\n";
    battle(new Team("Red", {new Goblin(popName(namepool))}), 
        new Team("Blue", {new Troll(popName(namepool))}));
    battle_idx++;

    // battle 2
    cout << "\nBattle #" + to_string(battle_idx) + "\n";
    battle(new Team("Red", {new Goblin(popName(namepool))}), 
        new Team("Blue", {new Troll(popName(namepool)), new Troll(popName(namepool))}));
    battle_idx++;

    // battle 3
    cout << "\nBattle #" + to_string(battle_idx) + "\n";
    battle(new Team("Red", {new Troll(popName(namepool))}), 
        new Team("Blue", {new Orc(popName(namepool))}));
    battle_idx++;

    // battle 4
    cout << "\nBattle #" + to_string(battle_idx) + "\n";
    battle(new Team("Red", {new Troll(popName(namepool))}), 
        new Team("Blue", {new Orc(popName(namepool)), new Orc(popName(namepool))}));
    battle_idx++;

    // battle 5
    cout << "\nBattle #" + to_string(battle_idx) + "\n";
    battle(new Team("Red", {new Orc(popName(namepool))}), 
        new Team("Blue", {new Goblin(popName(namepool))}));
    battle_idx++;

    // battle 6
    cout << "\nBattle #" + to_string(battle_idx) + "\n";
    battle(new Team("Red", {new Orc(popName(namepool))}), 
        new Team("Blue", {new Goblin(popName(namepool)), new Goblin(popName(namepool))}));
    battle_idx++;

    // battle 7
    cout << "\nBattle #" + to_string(battle_idx) + "\n";
    battle(new Team("Red", monster_picker(namepool, 4)), 
        new Team("Blue", monster_picker(namepool, 4)));
    battle_idx++;

    return 0;
}
