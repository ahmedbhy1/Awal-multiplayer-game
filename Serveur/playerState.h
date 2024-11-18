#ifndef PLAYERSTATE_H
#define PLAYERSTATE_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "server2.h"
#define TABLE_SIZE 100

// Define the State structure
typedef struct State {
    int state; // 0 player is waiting | 1 player is requesting a game | 2 player is playing | 3 player is watching
    int currentIndexOfGame ;
    int playerIndex ; // 0 or 1
    char* opponentName ;
} State;

// Define the hash table entry
typedef struct HashNode {
    char *key;          // Key (string)
    State value;        // Value (State struct)
    struct HashNode *next; // Handle collisions via chaining
} HashNode;

// Declare the hash table
extern HashNode *playersState[TABLE_SIZE];

// Function prototypes
void init_table();
void insert(const char *key, State value);
State* search(const char *key);
void free_table();
HashNode** get_playersState();

#endif