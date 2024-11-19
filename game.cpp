// g++ -std=c++11 -o output game.cpp
// ./output

#include <iostream>
#include <vector>
#include <string>
#include <random>
using namespace std;

enum MonsterType {goblin, troll, orc};

vector<string> getNamePool() {
    // random names to be given to the monsters
    // all from the Gundam series
    return {"RX", "Zaku", "Wing", "Zero", "Deathscythe", "Heavyarms", 
        "Sandrock", "Tallgeese", "Unicorn", "Banshee", "Barbatos", "Astaroth", 
        "Exia", "Dynames", "Kyrios", "Virtue", "Strike", "Freedom", "Justice",
        "Providence", "Destiny", "Impulse", "Legend", "Quanta", "OO", "Turna",
        "Burning", "Shining", "Epyon", "Kshatriya", "Sinanju", "ZGMF", "Alex",
        "Jesta", "Miu", "Jegan", "ReZEL", "ReGZ"};
}

class Monster {
    public:
        MonsterType type; // type of monster (in enum)
        string name; // given name
        int max_health; // maximum health for the monster
        int health; // current health
        int damage; // damage (per attack)
        int speed; // speed that decides who attack first
        bool alive; // if the monster is currently alive (true/false)

        // defines behavior of attacking, goblin will be different
        virtual void attack(Monster& enemy){
            enemy.on_enemy_attack(damage);
        };

        // defines behavior on being attacked, orc will be different
        virtual void on_enemy_attack(int amount){
            reduce_health(amount);
            if (alive) {
                cout << name + " remain health: " + to_string(health) + "\n";
            }
        };

        // defines behavior on the end of each turn, troll will be different
        virtual void on_end_turn(){}

        // when the monster's health gets reduced
        void reduce_health(int amount) {
            health -= amount;
            if (health <= 0) {
                health = 0;
                die();
            }
        };

        // when monster dies
        void die() {
            alive = false;
            cout << name + " died!\n";
        };

};

class Team {
    public:
        vector<Monster*> monsters;
        Monster* active_monster;
        string name;

        Team (string team_name, vector<Monster*> monster_list) {
            monsters = monster_list;
            active_monster = monsters[0];
            name = team_name;
        };

        void lose() {
            cout << name + " Team loses!" << endl;
        };

};

class Goblin: public Monster {
    public:
        int num_attack;

        Goblin(string given_name) {
            type = goblin;
            name = given_name;
            max_health = 30;
            health = max_health;
            damage = 20;
            speed = 50;
            alive = true;

            // type specifics
            num_attack = 2;
        };

        void attack(Monster& enemy) {
            for (int i = 0; i < num_attack; i++) {
                if (enemy.alive) {enemy.on_enemy_attack(damage);}
                else {break;}
            }
        };
};

class Troll: public Monster {
    public:
        int regen_amount;

        Troll(string given_name) {
            type = troll;
            name = given_name;
            max_health = 100;
            health = max_health;
            speed = 20;
            damage = 20;
            alive = true;

            // type specifics
            regen_amount = 20;
        };

        void on_end_turn(){
            health += regen_amount;
            if (health > max_health) {
                health = max_health;
                cout << name + " regenerated to max health" + '\n';
            }
            else {
                cout << name + "regenerated to health " + to_string(health) + '\n';
            }
        };
};


void turn(Monster& mon1, Monster& mon2) {
    Monster* faster;
    Monster* slower;
    if (mon1.speed > mon2.speed) {
        faster = &mon1;
        slower = &mon2;
    } else if (mon1.speed > mon2.speed) {
        faster = &mon2;
        slower = &mon1;
    } else {
        // generate two random numbers to decide who goes first
        int rand_num1 = rand();
        int rand_num2 = rand();
        faster = (rand_num1 > rand_num2) ? &mon1 : &mon2;
        slower = (rand_num1 > rand_num2) ? &mon2 : &mon1;
    }
    // in each turn, faster monster attack first,
    // then faster monster's turn end effect activate
    // if slower monster is still alive: slower monster attack
    // then slower monster's turn end effect activate
    faster->attack(*slower);
    faster->on_end_turn();
    if (slower->alive){
        slower->attack(*faster);
        slower->on_end_turn();
    }

    // Monster result_arr[2] = {mon1, mon2};
    // return result_arr;
};



int main() {

    vector<string> namepool = getNamePool();

    // set random
    random_device rd;          
    mt19937 gen(rd()); 

    Goblin gob1("goblin1");
    Goblin gob2("goblin2");
    Troll tro1("troll1");
    cout << "Generated: " + gob1.name + "\n";
    cout << "Generated: " + gob2.name + "\n";
    cout << "Generated: " + tro1.name + "\n";
    // gob1.attack(gob2);
    // gob1.attack(tro1);

    

    turn(gob1, tro1);
    turn(gob1, gob2);

    Team red_team("Red", {new Goblin("goblin 1"), new Troll("troll 1")});
    Team blue_team("Blue", {new Troll("troll 2"), new Troll("troll 3")});



    return 0;

}