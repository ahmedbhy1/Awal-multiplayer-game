
#ifndef GAME_H
#define GAME_H
#include "server2.h"

int initiateGame(Client player1,Client player2,char* playerName);
void playGameTurn(Client player,int indexOfPlayer, int indexOfGame,int choosenDigit,char* opponentName);

#endif /* guard */
