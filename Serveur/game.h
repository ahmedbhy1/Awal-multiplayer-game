
#ifndef GAME_H
#define GAME_H
#include "server2.h"
#include <stdbool.h>

int initiateGame(Client player1,Client player2,char* playerName);
bool playGameTurn(Client player,int indexOfPlayer, int indexOfGame,int choosenDigit,char* opponentName);
void showGameTable(int indexOfGame,char* nextPlayerName);
void showCurrentGames(Client client);
bool isValidGameIndexToJoinAsOb(int index);
bool joinClientAsObserver(Client client,int indexOfGame);
void surrenderFromGame(Client winner,int indexOfGame);
#endif /* guard */
