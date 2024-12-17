// g++ -std=c++14 -o output game.cpp
// ./output

#include <iostream>
#include <vector>
#include <string>
#include <random>
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

// helper functions (that does not require custom classes)

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
    // string will be colored based on team name (currently only support red & blue)
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
    // get the length of plain text without all the coloring escapes
    int length = 0;
    bool in_skip = false;

    for (int i = 0; i < text.size(); i++) {
        if (text[i] == '\033') { // start of color sequence
            in_skip = true;
        } else if (in_skip && text[i] == 'm') { // end of color sequence
            in_skip = false;
        } else if (!in_skip) { // count regular characters other than the color seq
            length++;
        }
    }
    return length;
}

// classes

class ActionLog {
    // records the output of each action in each turn
    public:
        ActionLog () : 
            attempted_damage(-1), actual_damage(-1), reflected_damage(-1) {}

        // Setters
        void set_attempted_damage(int value) {attempted_damage = value;}
        void set_actual_damage(int value) {actual_damage = value;}
        void set_reflected_damage(int value) {reflected_damage = value;}

        string get_action_text() {
            string action_text = " for " + to_string(attempted_damage) + 
                " damage; dealing " + to_string(actual_damage) + " damage; ";
            if (reflected_damage != -1) {
                action_text = action_text + "receiving " + to_string(reflected_damage) + " reflected damage;";
            } 
            return action_text;
        }
    
    private:
        int attempted_damage; // attempted damage from attacker -> opponent
        int actual_damage; // actual damage dealt from attacker -> opponent
        int reflected_damage; // reflected damage from opponent -> attacker

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

        Monster (string given_name): is_alive(true), name(given_name), team("Unspecified") {};

        // default virtual destructor
        virtual ~Monster() = default;

        // defines behavior of attacking, goblin will be different
        virtual void attack(Monster& enemy){
            if (enemy.is_alive){
                ActionLog log;
                log.set_attempted_damage(damage);
                enemy.on_enemy_attack(damage, this, &log);
                cout << attack_text(enemy) + log.get_action_text() + "\n";
            }
            
            enemy.check_death();
            check_death();

        };

        // defines behavior on being attacked, orc will be different
        // retruns actual damage amount & reflected amount
        virtual void on_enemy_attack(int amount, Monster* enemy = nullptr, ActionLog* log = nullptr){
            int reduce_amount = reduce_health(amount);
            log->set_actual_damage(reduce_amount);
        };

        // defines behavior on the end of each turn, troll will be different
        virtual void on_end_turn(){}; // nothing by default

        // get the display name of the monster
        // if with_team==true: <team> <monster type> <monster name>
        // if with_color==true: name will be colored based on team name
        // example: Blue Goblin Alex
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
            return disp(true, true) + " attacks " + enemy.disp(true, true);
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

        Goblin(string given_name): 
            Monster(given_name), num_attack(2) {
            type = goblin;
            max_health = 50;
            health = max_health;
            damage = 30;
            speed = 50;
        };

        // attack multiple times when attacking
        void attack(Monster& enemy) {
            for (int i = 0; i < num_attack; i++) {
                if (enemy.is_alive && is_alive) {
                    // attack_text(enemy);
                    ActionLog log;
                    log.set_attempted_damage(damage);
                    enemy.on_enemy_attack(damage, this, &log);

                    cout << attack_text(enemy) + log.get_action_text() + "\n";
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

        Troll(string given_name): 
            Monster(given_name), regen_amount(20) {
            type = troll;
            max_health = 100;
            health = max_health;
            speed = 20;
            damage = 40;
        };

        // regeneration occurs at the end of their own turn
        void on_end_turn(){
            if (is_alive && health<max_health) {
                health += regen_amount;
                if (health > max_health) {
                    int regenerated = max_health - health;
                    health = max_health;
                    cout << disp(true, true) << " regenerates " << regenerated <<
                        " health to " << health << " (max);\n";
                }
                else {
                    cout << disp(true, true) << " regenerates " << regen_amount <<
                        " health to " << health << ";\n";
                }
            }
            
        };
};

class Orc: public Monster {
    // orc class
    public:
        int block_amount;
        int reflect_amount;

        Orc(string given_name): 
            Monster(given_name), block_amount(10), reflect_amount(10) {
            type = orc;
            max_health = 70;
            health = max_health;
            speed = 30;
            damage = 30;
        };

        // blocking and reflecting occurs when enemy attacks
        void on_enemy_attack(int amount, Monster* enemy, ActionLog* log) override {
            int damage_dealt;
            int damage_reflected;
            
            if (amount > block_amount) {
                damage_dealt = amount - block_amount;

            } else {damage_dealt = 0;}

            int reduce_amount = reduce_health(damage_dealt);
            log->set_actual_damage(reduce_amount);
            log->set_reflected_damage(reflect_amount);

            enemy->reduce_health(reflect_amount);
        };
};

class Team {
    // team class that can store a series (of pointers) of monsters, and if the team is defeated
    public:
        vector<unique_ptr<Monster>> monsters; 
        string name;
        bool is_defeated;
        int n_monsters;

        Team(const string team_name, vector<unique_ptr<Monster>> monster_list)
            : name(team_name), is_defeated(false), n_monsters(monster_list.size()) {

            for (auto& mon : monster_list) {
                mon->team = team_name; // set team name for each monster    
                monsters.push_back(std::move(mon)); // move monster into the team's vector
            }

            // set the first monster as the active monster
            if (!monsters.empty()) {
                active_monster = monsters[0].get();
            } else {
                throw runtime_error(
                    name + " Team has no monsters. Cannot set active_monster.");
            }
            update_active_monster();
        }
        
        // update the team, check if the team is defeated
        void update_team() {
            update_active_monster();
            if (is_defeated) {
                cout << get_team_name(true) + " is defeated!\n";
            }
        };
        
        // get the team's name, optionally with color
        string get_team_name(bool color = true) {
            string text = name + " Team";
            if (color) {
                text = getColor(name) + text + getColor();
            }
            return text;
        }

        // get the active monster since it's provate
        Monster& get_active_monster() const {
            if (!active_monster) throw runtime_error("No active monster available.");
            return *active_monster;
        }

        // update the active monster
        void update_active_monster() {
            // if the active monster is still alive, do nothing
            if (active_monster->is_alive) {return;}
            // else, iterate through the monster list and get the first alive monster
            for (auto& mon : monsters) {
                if (mon->is_alive) {
                    active_monster = mon.get();
                    return;
                }
            }
            // else, if there is no alive monster in the team, the team is defeated 
            // (and set active monster pointer to null)
            is_defeated = true;
            active_monster = nullptr;
        }
    private:
        Monster* active_monster = nullptr; // points to the first alive monster; make it private so it can only be updated by its own class
};

// battle specific functions
void turn(Monster& mon1, Monster& mon2) {
    // function for each turn, this will run continuously until battle ends
    // for each turn, decide the faster and slower monster (based on speed)
    Monster* faster;
    Monster* slower;
    if (mon1.speed > mon2.speed) {
        faster = &mon1;
        slower = &mon2;
    } else if (mon2.speed > mon1.speed) {
        faster = &mon2;
        slower = &mon1;
    } else {
        // randomly decides who goes first if both have the same speed
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
    faster->attack(*slower);
    faster->on_end_turn();

    // the slower one attack if & only if it is still alive after the faster's attack
    if (slower->is_alive){
        slower->attack(*faster);
        slower->on_end_turn();
    }
};

string getMemberText(Monster& mon1, bool opposite=false){
    // get the text to be outputted as a team member
    // [ <team> | <monster type> <monster name> (remaining health) ]
    // example: [ Red | Orc Kyrios (70) ] 
    // if opposite=true: [ Goblin Alex (40) | Blue ]
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

string getVSstatusText(Monster& mon1, Monster& mon2, string vs_text=" ... "){
    // get the line up text at the beginning of each turn and battle
    // first and second monster have the opposite order to "face each other"
    return  getMemberText(mon1, false) + vs_text + getMemberText(mon2, true) + "\n";
};

void battle(unique_ptr<Team> team1, unique_ptr<Team> team2) {
    // function that performs the battle

    // takes two teams, end after monsters in one team are all dead
    int turn_idx = 1;

    // output line-up text: monsters facing each other in a single file
    // gets the member text from all the monsters first
    // for team1 only, get the longest text, use it as reference for padding
    // to ensure team2 can line up in a straight line
    vector<string> team1_vs_texts;
    vector<string> team2_vs_texts;
    int max_team1_len = 0;
    for (const auto& mon : team1->monsters) {
        string text = getMemberText(*mon);
        int plain_length = getPlainTextLength(text);
        team1_vs_texts.push_back(getMemberText(*mon));
        if (plain_length > max_team1_len) {
            max_team1_len = plain_length;
        }
    }
    for (const auto& mon : team2->monsters) {
        team2_vs_texts.push_back(getMemberText(*mon));
    }
    // get the lowest n_monsters (n_less)
    // for the first n_less monsters on each team, have them face to face
    // didn't use getVSstatusText because I want the second team to also line
    // up in a straight line
    int n_less = min(team1_vs_texts.size(), team2_vs_texts.size());
    for (int i = 0; i < n_less; i++) {
        string text1 = getMemberText(*(team1->monsters[i]));
        cout << text1 + string(max_team1_len-getPlainTextLength(text1), ' ') + 
            "   " + getMemberText(*(team2->monsters[i]), true) + "\n";
    }
    // for the rest of the monsters, output to corresponsing locations
    if (team1->n_monsters > n_less) {
        for (int i = n_less; i < team1->n_monsters; i++) {
            cout << getMemberText(*(team1->monsters[i])) + "\n";
        }
    } else if (team2->n_monsters > n_less) {
        for (int i = n_less; i < team2->n_monsters; i++) {
            cout << string(max_team1_len+3, ' ') + // +3: corresponding to the length of text in the middle
                getMemberText(*(team2->monsters[i]), true) + "\n";
        }
    }
    
    // actual combat
    // while both teams are still standing, combat
    while ((!team1->is_defeated) && (!team2->is_defeated)) {
        cout << "\nTurn " << turn_idx << "\n";
        cout << getVSstatusText(team1->get_active_monster(), team2->get_active_monster());
        turn(team1->get_active_monster(), team2->get_active_monster());
        team1->update_team();
        team2->update_team();
        turn_idx++;
        if (turn_idx>100){break;}
    }

    // battle ended; if both team are defeated, tie; if team1 is defeated, team2 wins, vice versa
    cout << "\nBattle Over! ";
    if (team1->is_defeated && team2->is_defeated) {
        cout << "Tied!\n";
    } else if (team1->is_defeated){
        cout << team2->get_team_name(true) << " wins!\n";
    } else if (team2->is_defeated){
        cout << team1->get_team_name(true) << " wins!\n";
    }

    cout << "\n-----------------------------------------------------------------------------------------------------------------------\n";

};

unique_ptr<Monster> getMonster(MonsterType type, vector<string>& namepool) {
    // get a monster based on type
    switch (type) {
        case goblin: return make_unique<Goblin>(popName(namepool));
        case troll: return make_unique<Troll>(popName(namepool));
        case orc: return make_unique<Orc>(popName(namepool));
        default: return make_unique<Monster>(popName(namepool)); // should not happen
    }
};

vector<MonsterType> monsterPicker(int n) {
    // pick n monsters randomly, and return the selected MonsterTypes 
    vector<MonsterType> selected;
    uniform_int_distribution<> dis(0, 2);
    for (int i = 0; i < n; i++) {
        MonsterType type = static_cast<MonsterType>(dis(gen));
        selected.emplace_back(type);
    }
    return selected;
}

void makeBattle(
    // readable wrapper for combat, takes two "lineup" of monster,
    // use it as instruction to build teams
    // then send it to battle()
    const pair<string, vector<MonsterType>>& lineup1, 
    const pair<string, vector<MonsterType>>& lineup2, 
    vector<string>& namepool) {
    vector<unique_ptr<Monster>> team1_monsters;
    for (auto& type : lineup1.second) {
        team1_monsters.emplace_back(getMonster(type, namepool));
    }

    vector<unique_ptr<Monster>> team2_monsters;
    for (auto& type : lineup2.second) {
        team2_monsters.emplace_back(getMonster(type, namepool));
    }

    auto team1 = make_unique<Team>(lineup1.first, std::move(team1_monsters));
    auto team2 = make_unique<Team>(lineup2.first, std::move(team2_monsters));
    battle(std::move(team1), std::move(team2));

};


// main code for running all the battles
int main() {

    vector<string> namepool = getNamePool();
    shuffle(namepool.begin(), namepool.end(), gen);

    int battle_idx = 1;

    cout << "\n=======================================================================================================================\n";


    // battle 1: One goblin vs one troll.
    cout << "\nBattle #" << battle_idx << "\n";
    makeBattle({"Red", {goblin}}, {"Blue", {troll}}, namepool);
    battle_idx++;

    // battle 2: One goblin vs two trolls.
    cout << "\nBattle #" << battle_idx << "\n";
    makeBattle({"Red", {goblin}}, {"Blue", {troll, troll}}, namepool);
    battle_idx++;

    // battle 3: One troll vs one orc.
    cout << "\nBattle #" << battle_idx << "\n";
    makeBattle({"Red", {troll}}, {"Blue", {orc}}, namepool);
    battle_idx++;

    // battle 4: One troll vs two orc.
    cout << "\nBattle #" << battle_idx << "\n";
    makeBattle({"Red", {troll}}, {"Blue", {orc, orc}}, namepool);
    battle_idx++;

    // battle 5: One orc vs one goblin.
    cout << "\nBattle #" << battle_idx << "\n";
    makeBattle({"Red", {orc}}, {"Blue", {goblin}}, namepool);
    battle_idx++;

    // battle 6: One orc vs two goblin.
    cout << "\nBattle #" << battle_idx << "\n";
    makeBattle({"Red", {orc}}, {"Blue", {goblin, goblin}}, namepool);
    battle_idx++;

    // battle 7: 4 random monsters vs 4 random monsters.
    cout << "\nBattle #" << battle_idx << "\n";
    makeBattle({"Red", monsterPicker(4)}, {"Blue", monsterPicker(4)}, namepool);
    battle_idx++;

    return 0;
}
