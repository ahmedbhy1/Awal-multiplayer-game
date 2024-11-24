
#ifndef GAME_H
#define GAME_H
#include "server2.h"
#include <stdbool.h>

int initiateGame(Client player1,Client player2,char* playerName);
bool playGameTurn(Client player,int indexOfPlayer, int indexOfGame,int choosenDigit,char* opponentName);
void showGameTable(int indexOfGame,char* nextPlayerName);
#endif /* guard */
