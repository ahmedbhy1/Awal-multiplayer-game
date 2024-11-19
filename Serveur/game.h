
#ifndef GAME_H
#define GAME_H
#include "server2.h"

int initiateGame(Client player1,Client player2);
void playGameTurn(Client player,int indexOfPlayer, int indexOfGame,int choosenDigit);

#endif /* guard */
