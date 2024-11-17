


#define MAX_CLIENTS 25
#define MAX_GAMES 255
#define MAX_OBSERVERS 10
#define MAX_RECIEVED_GAME_CLIENTS 12
#define MAX_GAMETABLES 10
#define MAX_GAMES 10
#include "game.h"
#include "common.c"

struct House
{
   int numberOfSeeds;
};

struct GameTable {
   int index;
   //  the first 6 Houses Are for PlayerOne
   //  the second 6 Houses Are for the PlayerTwo
   struct House table[12];
   int seedsWonByP1;
   int seedsWonByP2;
};

struct Game {
   int id;
   Client player1;
   Client player2;
   Client observers[MAX_OBSERVERS];
   int indexLastObserver;
   Client winner;
   struct GameTable gameTables[MAX_GAMETABLES];
   int lastGameTableIndex;
};

int indexOfGame = 0;
struct Game listOfGames[MAX_GAMES];

void addGameTable(int indexOfGame, int indexOfTableGame) {
   printf("we are adding Game Table \n");
   for (int i = 0; i < 12; i++) {
      listOfGames[indexOfGame].gameTables[indexOfTableGame].table[i].numberOfSeeds = 4;
   }
   listOfGames[indexOfGame].gameTables[indexOfTableGame].seedsWonByP1 = 0;
   listOfGames[indexOfGame].gameTables[indexOfTableGame].seedsWonByP2 = 0;
   listOfGames[indexOfGame].lastGameTableIndex +=1;
}

void showGameTable(int indexOfGame, int indexOfTableGame) {
    char message[1024] = ""; // Ensure the buffer is large enough for the full message
    char temp[256]; // Temporary buffer for individual parts of the message
    
    strcat(message, "we are showing Game Table\n");
    
    char GameTableMessage[256] = "";
    for (int i = 0; i < 12; i++) {
        if (i && i % 6 == 0) {
            strcat(GameTableMessage, "\n");
        }
        sprintf(temp, "%d", listOfGames[indexOfGame].gameTables[indexOfTableGame].table[i].numberOfSeeds);
        strcat(GameTableMessage, temp);
    }
    strcat(message, GameTableMessage);
    strcat(message, "\n");
    
    sprintf(temp, "seeds won by: %s\n", listOfGames[indexOfGame].player1.name);
    strcat(message, temp);
    sprintf(temp, "%d\n", listOfGames[indexOfGame].gameTables[indexOfTableGame].seedsWonByP1);
    strcat(message, temp);
    
    sprintf(temp, "seeds won by: %s\n", listOfGames[indexOfGame].player2.name);
    strcat(message, temp);
    sprintf(temp, "%d\n", listOfGames[indexOfGame].gameTables[indexOfTableGame].seedsWonByP2);
    strcat(message, temp);
    
    Client listOfClientsToRecieveGameTable[MAX_RECIEVED_GAME_CLIENTS];
    listOfClientsToRecieveGameTable[0] = listOfGames[indexOfGame].player1;
    listOfClientsToRecieveGameTable[1] = listOfGames[indexOfGame].player2;
    
    for (int i = 0; i < listOfGames[indexOfGame].indexLastObserver; i++) {
        listOfClientsToRecieveGameTable[i + 2] = listOfGames[indexOfGame].observers[i];
    }
    printf("message: %s \n", message);
    send_message_to_clients_from_server(listOfClientsToRecieveGameTable,listOfGames[indexOfGame].indexLastObserver+2,message);
}

static void addGameObserver(Client observer){
   listOfGames[indexOfGame].observers[listOfGames[indexOfGame].indexLastObserver] = observer;
   listOfGames[indexOfGame].indexLastObserver+=1;
}
/*
static void startGame(Client player1,Client player2, int indexOfGame,int choosenDigit){
   int i=0;
   while (i<10){
      i++;
      playGameTurn(player1,indexOfGame,choosenDigit);
      showGameTable(indexOfGame,listOfGames[indexOfGame].lastGameTableIndex);
      if (isGameOver(indexOfGame)){break;}
      playGameTurn(player2,indexOfGame,choosenDigit);
      showGameTable(indexOfGame,listOfGames[indexOfGame].lastGameTableIndex);
      if (isGameOver(indexOfGame)){break;}
   }
   //printf("%s the winner of the game is  \n", winnerPlayer(indexOfGame).name);
}
*/
int initiateGame(Client player1,Client player2){
   printf("we are initiating Game \n");
   printf("indexOfGame \n");
   printf("%d \n",indexOfGame);
   listOfGames[indexOfGame].id = indexOfGame;
   listOfGames[indexOfGame].player1 = player1;
   listOfGames[indexOfGame].player2 = player2;
   listOfGames[indexOfGame].lastGameTableIndex = 0;
   addGameTable(indexOfGame,0);
   showGameTable(indexOfGame,0);
   indexOfGame = indexOfGame + 1;
   return indexOfGame - 1;
}

/*

static int isGameOver(int indexOfGame){

}

static Client winnerPlayer(int indexOfGame){

}

static void playGameTurn(Client player,int indexOfPlayer, int indexOfGame,int choosenDigit){
   printf("shoose a number from 1 to 6 in your zone");

}
*/

