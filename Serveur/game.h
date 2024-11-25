
#ifndef GAME_H
#define GAME_H
#include "server2.h"
#include <stdbool.h>

int initiateGame(Client player1,Client player2,char* playerName,Client *clients,int actual);
bool playGameTurn(Client player,int indexOfPlayer, int indexOfGame,int choosenDigit,char* opponentName,Client *clients,int actual);
void showGameTable(int indexOfGame,char* nextPlayerName,Client *clients,int actual);
void showGames(Client client);
bool isValidGameIndexToJoinAsOb(int index);
bool isValidGameIndexToGetHistoryFrom(int index);
bool joinClientAsObserver(Client client,int indexOfGame);
void surrenderFromGame(Client winner,int indexOfGame);
bool showPlayerWinIfGameOver(int indexOfGame,Client* clients,int actual);
void showGameHistory(Client client,int indexOfGame);
#endif /* guard */
