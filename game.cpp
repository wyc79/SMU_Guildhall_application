// g++ -std=c++11 -o output game.cpp
// ./output

#include <iostream>
#include <vector>
#include <string>
#include <random>
// #include <algorithm> 
// #include <memory>
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

int getPlainTextLength(const string& text) {
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

class ActionLog {
    public:
        int attempted_damage; // attempted damage from attacker -> opponent
        int actual_damage; // actual damage dealt from attacker -> opponent
        int reflected_damage; // reflected damage from opponent -> attacker

        ActionLog () : 
            attempted_damage(-1), actual_damage(-1), reflected_damage(-1) {}

        // Setters
        void setAttemptedDamage(int value) {attempted_damage = value;}
        void setActualDamage(int value) {actual_damage = value;}
        void setReflectedDamage(int value) {reflected_damage = value;}

        string getActionText() {
            string action_text = " for " + to_string(attempted_damage) + 
                " damage; dealing " + to_string(actual_damage) + " damage; ";
            if (reflected_damage != -1) {
                action_text = action_text + "receiving " + to_string(reflected_damage) + " reflected damage;";
            } 
            return action_text;
        }
};

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
            ActionLog log;
            log.setAttemptedDamage(damage);
            enemy.on_enemy_attack(damage, this, &log);
            // attempted_damage = damage;

            cout << attack_text(enemy) + log.getActionText() + "\n";
            check_death();
            enemy.check_death();

        };

        // defines behavior on being attacked, orc will be different
        // retruns actual damage amount & reflected amount
        virtual void on_enemy_attack(int amount, Monster* enemy = nullptr, ActionLog* log = nullptr){
            int reduce_amount = reduce_health(amount);
            log->setActualDamage(reduce_amount);
        };

        // defines behavior on the end of each turn, troll will be different
        virtual void on_end_turn(){}; // nothing by default

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

        string attack_text(Monster& enemy){
            return disp(true, true) + " attacks " + enemy.disp(true, true);// +
                // " for " + to_string(damage) + " damage; ";
        };

        // when the monster's health gets reduced
        int reduce_health(int amount) {
            int reduce_amount;
            if (amount >= health) {
                reduce_amount = health;
            } else {
                reduce_amount = amount;
            }
            health -= reduce_amount;
            // cout << "dealing " + to_string(reduce_amount) + "damage; ";
            // if (health <= 0) {die();}
            return reduce_amount;
        };

        // // when monster dies
        // void die() {
        //     is_alive = false;
        //     cout << disp(true, true) + " has died!\n";
        // };

        bool check_death() {
            if (is_alive && health<=0) {
                is_alive = false;
                cout << disp(true, true) + " has died!\n";
                return true;
            } else if (is_alive && health>0) {
                return false;
            } else {return true;}
        }

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
                if (enemy.is_alive && is_alive) {
                    // attack_text(enemy);
                    ActionLog log;
                    log.setAttemptedDamage(damage);
                    enemy.on_enemy_attack(damage, this, &log);

                    cout << attack_text(enemy) + log.getActionText() + "\n";
                    check_death();
                    enemy.check_death();
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
            if (is_alive) {
                health += regen_amount;
                if (health > max_health) {
                    int regenerated = max_health - health;
                    health = max_health;
                    cout << disp(true, true) << " regenerates " << regenerated <<
                        " health to " << health << " (max)\n";
                }
                else {
                    cout << disp(true, true) << " regenerates " << regen_amount <<
                        " health to " << health << "\n";
                }
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

        void on_enemy_attack(int amount, Monster* enemy, ActionLog* log) override {
            int damage_dealt;
            int damage_reflected;
            
            if (amount > block_amount) {
                damage_dealt = amount - block_amount;

            } else {damage_dealt = 0;}

            // if (amount < reflect_amount) {
            //     damage_reflected = amount;
            // } else {damage_reflected = reflect_amount;}
            reduce_health(damage_dealt);
            log->setActualDamage(damage_dealt);
            log->setReflectedDamage(reflect_amount);

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
            }

            is_defeated = false;
            n_monsters = monsters.size();
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
                    cout << getTeamName(true) + " is defeated!\n";
                }
            };
        };

        string getTeamName(bool color = true) {
            string text = name + " Team";
            if (color) {
                text = getColor(name) + text + getColor();
            }
            return text;
        }
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
        // cout << "Randomly deciding order\n";
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
    ActionLog log;
    faster->attack(*slower);
    faster->on_end_turn();

    if (slower->is_alive){
        ActionLog log;
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

    // output line-up text: monsters facing each other in a single file
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

    // while both teams are still standing, combat
    while ((!team1->is_defeated) && (!team2->is_defeated)) {
        cout << "\nTurn " << turn_idx << "\n";
        cout << get_vs_status_text(*(team1->active_monster), *(team2->active_monster));
        turn(*(team1->active_monster), *(team2->active_monster));
        team1->update_team();
        team2->update_team();
        turn_idx++;
        if (turn_idx>100){break;}
    }

    // battle ended; if both team are defeated, tie; if team1 is defeated, team2 wins, vice versa
    cout << "Battle Over! ";
    if (team1->is_defeated && team2->is_defeated) {
        cout << " Tied!";
    } else if (team1->is_defeated){
        cout << team2->getTeamName(true);
    } else if (team2->is_defeated){
        cout << team1->getTeamName(true);
    }
    cout << " wins!\n";
    

    cout << "\n-----------------------------------------------------------------------------------------------------------------------\n";

};

// pick n monsters randomly, and return the selected monsters
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
    cout << "\nBattle #" << battle_idx << "\n";
    battle(new Team("Red", {new Goblin(popName(namepool))}), 
        new Team("Blue", {new Troll(popName(namepool))}));
    battle_idx++;

    // battle 2
    cout << "\nBattle #" << battle_idx << "\n";
    battle(new Team("Red", {new Goblin(popName(namepool))}), 
        new Team("Blue", {new Troll(popName(namepool)), new Troll(popName(namepool))}));
    battle_idx++;

    // battle 3
    cout << "\nBattle #" << battle_idx << "\n";
    battle(new Team("Red", {new Troll(popName(namepool))}), 
        new Team("Blue", {new Orc(popName(namepool))}));
    battle_idx++;

    // battle 4
    cout << "\nBattle #" << battle_idx << "\n";
    battle(new Team("Red", {new Troll(popName(namepool))}), 
        new Team("Blue", {new Orc(popName(namepool)), new Orc(popName(namepool))}));
    battle_idx++;

    // battle 5
    cout << "\nBattle #" << battle_idx << "\n";
    battle(new Team("Red", {new Orc(popName(namepool))}), 
        new Team("Blue", {new Goblin(popName(namepool))}));
    battle_idx++;

    // battle 6
    cout << "\nBattle #" << battle_idx << "\n";
    battle(new Team("Red", {new Orc(popName(namepool))}), 
        new Team("Blue", {new Goblin(popName(namepool)), new Goblin(popName(namepool))}));
    battle_idx++;

    // battle 7
    cout << "\nBattle #" << battle_idx << "\n";
    battle(new Team("Red", monster_picker(namepool, 4)), 
        new Team("Blue", monster_picker(namepool, 4)));
    battle_idx++;

    return 0;
}
