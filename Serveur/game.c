


#define MAX_CLIENTS 25
#define MAX_GAMES 255
#define MAX_OBSERVERS 10
#define MAX_RECIEVED_GAME_CLIENTS 12
#define MAX_GAMETABLES 10
#define MAX_GAMES 10
#include "game.h"
#include "common.c"
#include <stdbool.h>
struct House
{
   int numberOfSeeds;
};

struct GameTable {
   int index;
   //  the first 6 Houses Are for PlayerOne
   //  the second 6 Houses Are for the PlayerTwo
   struct House table[2][6];
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
   for (int i = 0; i < 2; i++) {
      for(int j = 0;j < 6;j++){
         listOfGames[indexOfGame].gameTables[indexOfTableGame].table[i][j].numberOfSeeds = 4;
      }
   }
   listOfGames[indexOfGame].gameTables[indexOfTableGame].seedsWonByP1 = 0;
   listOfGames[indexOfGame].gameTables[indexOfTableGame].seedsWonByP2 = 0;
   //listOfGames[indexOfGame].lastGameTableIndex +=1;
}

void showGameTable(int indexOfGame,char* nextPlayerName) {
    char message[1024] = ""; // Ensure the buffer is large enough for the full message
    char temp[256]; // Temporary buffer for individual parts of the message
    int indexOfTableGame = listOfGames[indexOfGame].lastGameTableIndex;
    strcat(message, "we are showing Game Table \n");
    
    char GameTableMessage[256] = "";
      for (int i = 0; i < 2; i++) {
         for (int j = 0; j < 6; j++) {
            sprintf(temp, "%d ", listOfGames[indexOfGame].gameTables[indexOfTableGame].table[i][j].numberOfSeeds);
            strcat(GameTableMessage, temp);
         }

         if (i == 0) {
            sprintf(temp, "  %s is getting %d seeds.", listOfGames[indexOfGame].player1.name, listOfGames[indexOfGame].gameTables[indexOfTableGame].seedsWonByP1);
         }
         if (i == 1) {
            sprintf(temp, "  %s is getting %d seeds.", listOfGames[indexOfGame].player2.name, listOfGames[indexOfGame].gameTables[indexOfTableGame].seedsWonByP2);
         }
         strcat(GameTableMessage, temp);
         strcat(GameTableMessage, "\n");
      }

    strcat(message, GameTableMessage);

    Client listOfClientsToRecieveGameTable[MAX_RECIEVED_GAME_CLIENTS];
    listOfClientsToRecieveGameTable[0] = listOfGames[indexOfGame].player1;
    listOfClientsToRecieveGameTable[1] = listOfGames[indexOfGame].player2;
    
    for (int i = 0; i < listOfGames[indexOfGame].indexLastObserver; i++) {
        listOfClientsToRecieveGameTable[i + 2] = listOfGames[indexOfGame].observers[i];
    }
    sprintf(temp, "it's the turn of %s type p number_from_1_to_6 to play\n", nextPlayerName);
    strcat(message, temp);
    send_message_to_clients_from_server(listOfClientsToRecieveGameTable,listOfGames[indexOfGame].indexLastObserver+2,message);
}

void addGameObserver(Client observer){
   listOfGames[indexOfGame].observers[listOfGames[indexOfGame].indexLastObserver] = observer;
   listOfGames[indexOfGame].indexLastObserver+=1;
}


int initiateGame(Client player1,Client player2,char* playerName){
   printf("we are initiating Game \n");
   printf("indexOfGame \n");
   indexOfGame = indexOfGame + 1;
   printf("%d \n",indexOfGame);
   listOfGames[indexOfGame].id = indexOfGame;
   listOfGames[indexOfGame].player1 = player1;
   listOfGames[indexOfGame].player2 = player2;
   listOfGames[indexOfGame].lastGameTableIndex = 0;
   addGameTable(indexOfGame,0);
   showGameTable(indexOfGame,playerName);
   return indexOfGame ;
}



void collectSeeds(Client player, int indexOfGame,int i,int j,int count,int nbSeeds,int nbColledSeeds){


}

bool isGameOver(){
   return false;
}

bool playGameTurn(Client player,int indexOfPlayer, int indexOfGame,int choosenDigit,char* opponentName){
   // index of the player represants 0 or 1 -> the side in wich the player play in !
   int nomberOfSeeds = listOfGames[indexOfGame].gameTables[listOfGames[indexOfGame].lastGameTableIndex].table[indexOfPlayer][choosenDigit].numberOfSeeds;
   if (nomberOfSeeds==0)return false;
   int count = nomberOfSeeds;
   
   printf("player name ( %s ) and player index ( %d ) and \n",player.name,indexOfPlayer);
   printf("nomber of collected seeds %d \n",count);

   listOfGames[indexOfGame].gameTables[listOfGames[indexOfGame].lastGameTableIndex].table[indexOfPlayer][choosenDigit].numberOfSeeds = 0;
   int i = indexOfPlayer;
   int j = choosenDigit;

   //sow the taken seeds to the next houses
   while(count > 0){
         if (i == 0){
            j-=1;
            if (j<0){
               j+=1;
               i=1;
            }
         }
         else{
            j+=1;
            if (j>=6){
               j-=1;
               i=0;
            }
         }

         listOfGames[indexOfGame].gameTables[listOfGames[indexOfGame].lastGameTableIndex].table[i][j].numberOfSeeds+=1;
         //printf("nomber of seeds on t[i][j] %d \n",listOfGames[indexOfGame].gameTables[listOfGames[indexOfGame].lastGameTableIndex].table[i][j].numberOfSeeds);
         count-=1;
      }
   

   printf("i and j after throwing seeds! (%d,%d) \n",i,j);
   count = nomberOfSeeds;
   int nbSeeds=0;
   int nbColledSeeds=0;
   bool areAllOfThemInTheRange = true;
   
    //collecting seeds
   while(count > 0){
      
      printf("collecting seeds (i,j)=(%d,%d)",i,j);


      nbSeeds = listOfGames[indexOfGame].gameTables[listOfGames[indexOfGame].lastGameTableIndex].table[i][j].numberOfSeeds ;
      if (nbSeeds == 2 || nbSeeds == 3){
         nbColledSeeds += nbSeeds;
         listOfGames[indexOfGame].gameTables[listOfGames[indexOfGame].lastGameTableIndex].table[i][j].numberOfSeeds = 0 ;
      
         if (i == 1) {
            j -= 1;
            if (j < 0) {
                  j += 1;
                  i = 0;
            }
         } else {
            j += 1;
            if (j >= 6) {
                  j -= 1;
                  i = 1;
            }
         }

         count-=1;
      }
      else{ 
         areAllOfThemInTheRange = false;
         if (indexOfPlayer == 0){
            printf("is player1 eating the seeds ? %d \n",nbColledSeeds);
            listOfGames[indexOfGame].gameTables[listOfGames[indexOfGame].lastGameTableIndex].seedsWonByP1+=nbColledSeeds;
         }
         else{
            printf("is player2 eating the seeds ? %d \n",nbColledSeeds);
            listOfGames[indexOfGame].gameTables[listOfGames[indexOfGame].lastGameTableIndex].seedsWonByP2+=nbColledSeeds;
         }
         break;
      }
   }
   printf("we want to see areAllOfThemInTheRange: %d \n",areAllOfThemInTheRange);
   printf("we want to see nbColledSeeds: %d \n",nbColledSeeds);
   if (areAllOfThemInTheRange == true){
      if (indexOfPlayer == 0){
         printf("is player1 eating the seeds ? %d \n",nbColledSeeds);
         listOfGames[indexOfGame].gameTables[listOfGames[indexOfGame].lastGameTableIndex].seedsWonByP1+=nbColledSeeds;
      }
      else{
         printf("is player2 eating the seeds ? %d \n",nbColledSeeds);
         listOfGames[indexOfGame].gameTables[listOfGames[indexOfGame].lastGameTableIndex].seedsWonByP2+=nbColledSeeds;
      }
   }


   
   if(!isGameOver()){
      showGameTable(indexOfGame,opponentName);
   }
   return true;
}
