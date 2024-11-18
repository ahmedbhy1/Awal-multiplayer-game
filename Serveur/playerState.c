#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define TABLE_SIZE 100  // Size of the hash table

// Define the State structure
typedef struct State {
    int state; // 0 player is waiting | 1 player is requesting a game | 2 player is playing | 3 player is watching
    int currentIndexOfGame ;
    int playerIndex ; // 0 or 1
    char* opponentName ;
} State;

// Define the hash table entry
typedef struct HashNode {
    char *playerName;          // playerName (string)
    State value;        // Value (State struct)
    struct HashNode *next; // Handle collisions via chaining
} HashNode;

// Hash table definition
HashNode *playersState[TABLE_SIZE];

// Hash function (djb2 algorithm)
unsigned int hash_function(const char *playerName) {
    unsigned long hash = 5381;
    int c;
    while ((c = *playerName++)) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    return hash % TABLE_SIZE;
}

// Initialize the hash table
void init_table() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        playersState[i] = NULL;
    }
}

// Insert into the hash table
void insert(const char *playerName, State value) {
    unsigned int index = hash_function(playerName);
    HashNode *newNode = malloc(sizeof(HashNode));
    newNode->playerName = strdup(playerName);
    newNode->value = value;
    newNode->next = playersState[index];
    playersState[index] = newNode;
}

// Search in the hash table
State *search(const char *playerName) {
    unsigned int index = hash_function(playerName);
    HashNode *current = playersState[index];
    while (current) {
        if (strcmp(current->playerName, playerName) == 0) {
            return &current->value;
        }
        current = current->next;
    }
    return NULL; // playerName not found
}

int modify_player_state(const char *playerName, int *newState, int *newIndexOfGame, int *newPlayerIndex, const char *newOpponentName) {
    // Search for the player in the hash table
    State *playerState = search(playerName);

    if (playerState == NULL) {
        printf("Player %s not found.\n", playerName);
        return 0; // Player not found
    }

    // Modify the state only if a new value is provided
    if (newState != NULL) {
        playerState->state = *newState;
    }
    if (newIndexOfGame != NULL) {
        playerState->currentIndexOfGame = *newIndexOfGame;
    }
    if (newPlayerIndex != NULL) {
        playerState->playerIndex = *newPlayerIndex;
    }
    if (newOpponentName != NULL) {
        // Free the old opponent name if it exists
        if (playerState->opponentName != NULL) {
            free(playerState->opponentName);
        }
        // Allocate memory for the new opponent's name
        playerState->opponentName = strdup(newOpponentName);
    }

    printf("Player %s's state has been modified.\n", playerName);
    return 1; // Success
}

// Free the hash table
void free_table() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        HashNode *current = playersState[i];
        while (current) {
            HashNode *temp = current;
            current = current->next;
            free(temp->playerName);
            free(temp);
        }
    }
}

/*
int main() {

    return 0;
}
*/