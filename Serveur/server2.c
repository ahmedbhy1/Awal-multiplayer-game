#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "server2.h"
#include "client2.h"


#define MAX_GAMES 255
#define MAX_OBSERVERS 10
#define MAX_GAMETABLES 10
#define MAX_GAMES 10

struct House
{
   int numberOfSeeds;
};

struct GameTable {
   int index;
   struct House table[6][2];
   int seedsWonByP1;
   int seedsWonByP2;
};

struct Game {
   int id;
   Client player1;
   Client player2;
   Client observers[MAX_OBSERVERS];
   Client winner;
   struct GameTable gameTables[MAX_GAMETABLES];
   int lastGameTableIndex;
};

int indexOfGame = 0;
struct Game listOfGames[MAX_GAMES];

void addGameTable(int indexOfGame, int lastGameTableIndex) {
   printf("we are adding Game Table \n");
   for (int i = 0; i < 6; i++) {
      for (int j = 0; j < 2; j++) {
         listOfGames[indexOfGame].gameTables[lastGameTableIndex].table[i][j].numberOfSeeds = 4;
      }
   }
   listOfGames[indexOfGame].lastGameTableIndex +=1;
}


void showGameTable(int indexOfGame, int indexOfTableGame) {
   printf("we are showing Game Table \n");
   char GameTableMessage[256] = ""; 
   char temp[10];  
   for (int i = 0; i < 6; i++) {
      for (int j = 0; j < 2; j++) {
         sprintf(temp, "%d", listOfGames[indexOfGame].gameTables[indexOfTableGame].table[i][j].numberOfSeeds);
         strcat(GameTableMessage, temp);
      }
      strcat(GameTableMessage, "\n");
   }

   printf("%s \n", GameTableMessage);
   printf("%s \n", listOfGames[indexOfGame].player1.name);
   printf("%s \n", listOfGames[indexOfGame].gameTables[indexOfTableGame].seedsWonByP1);
   printf("%s \n", listOfGames[indexOfGame].player2.name);
   printf("%s \n", listOfGames[indexOfGame].gameTables[indexOfTableGame].seedsWonByP2);


}


struct void initiateGame(Client player1,Client player2){
   printf("we are initiating Game \n");
   listOfGames[indexOfGame].id = indexOfGame;
   listOfGames[indexOfGame].player1 = player1;
   listOfGames[indexOfGame].player2 = player2;
   listOfGames[indexOfGame].lastGameTableIndex = 0;
   addGameTable(indexOfGame,0);
   showGameTable(indexOfGame,0);
   indexOfGame = indexOfGame + 1;
}


static void init(void)
{
#ifdef WIN32
   WSADATA wsa;
   int err = WSAStartup(MAKEWORD(2, 2), &wsa);
   if(err < 0)
   {
      puts("WSAStartup failed !");
      exit(EXIT_FAILURE);
   }
#endif
}

static void end(void)
{
#ifdef WIN32
   WSACleanup();
#endif
}

static void app(void)
{
   SOCKET sock = init_connection();
   char buffer[BUF_SIZE];
   /* the index for the array */
   int actual = 0;
   int max = sock;
   /* an array for all clients */
   Client clients[MAX_CLIENTS];

   fd_set rdfs;

   while(1)
   {
      int i = 0;
      FD_ZERO(&rdfs);

      /* add STDIN_FILENO */
      FD_SET(STDIN_FILENO, &rdfs);

      /* add the connection socket */
      FD_SET(sock, &rdfs);

      /* add socket of each client */
      for(i = 0; i < actual; i++)
      {
         FD_SET(clients[i].sock, &rdfs);
      }

      if(select(max + 1, &rdfs, NULL, NULL, NULL) == -1)
      {
         perror("select()");
         exit(errno);
      }

      /* something from standard input : i.e keyboard */
      if(FD_ISSET(STDIN_FILENO, &rdfs))
      {
         /* stop process when type on keyboard */
         break;
      }
      else if(FD_ISSET(sock, &rdfs))
      {
         printf("new client connected \n");
         /* new client */
         
         SOCKADDR_IN csin = { 0 };
         size_t sinsize = sizeof csin;
         int csock = accept(sock, (SOCKADDR *)&csin, &sinsize);
         if(csock == SOCKET_ERROR)
         {
            perror("accept()");
            continue;
         }

         /* after connecting the client sends its name */
         if(read_client(csock, buffer) == -1)
         {
            /* disconnected */
            continue;
         }

         /* what is the new maximum fd ? */
         max = csock > max ? csock : max;

         FD_SET(csock, &rdfs);

         Client c = { csock };
         strncpy(c.name, buffer, BUF_SIZE - 1);
         clients[actual] = c;
         actual++;
         
         print_client_names(clients, actual);
         
      }
      else
      {
         int i = 0;
         for(i = 0; i < actual; i++)
         {
            /* a client is talking */
            if(FD_ISSET(clients[i].sock, &rdfs))
            {
               Client client = clients[i];
               int c = read_client(clients[i].sock, buffer);
               /* client disconnected */
               if(c == 0)
               {
                  closesocket(clients[i].sock);
                  remove_client(clients, i, &actual);
                  strncpy(buffer, client.name, BUF_SIZE - 1);
                  strncat(buffer, " disconnected !", BUF_SIZE - strlen(buffer) - 1);
                  send_message_to_all_clients(clients, client, actual, buffer, 1);
               }
               else
               {
                  printf("%s",buffer);
                  printf("\n");
                  doCommend(buffer,client,clients,actual);
                  //for the moment we wont send messages to the other users!
                  //send_message_to_all_clients(clients, client, actual, buffer, 0);
               }
               break;
            }
         }
      }
   }

   clear_clients(clients, actual);
   end_connection(sock);
}


static void clear_clients(Client *clients, int actual)
{
   int i = 0;
   for(i = 0; i < actual; i++)
   {
      closesocket(clients[i].sock);
   }
}


static void print_client_names(Client *clients, int actual){
   int i=0;
   for(i = 0; i < actual; i++)
   {
      printf("%s",clients[i].name);
      printf("\n");
   }   
   
}
static void sendPlayersNamesToClient(Client *clients, Client receiver, int actual) {
   int i = 0;
   char message[BUF_SIZE] = {0};
   
   for (i = 0; i < actual; i++) {
      int remaining_space = BUF_SIZE - strlen(message) - 1;
      if (remaining_space > 0) {
         strncat(message, clients[i].name, remaining_space - 1);
         strncat(message, "\n", remaining_space - strlen(message) - 1);
      } else {
         break;
      }
   }
   
   printf("%s",message,"\n");
   write_client(receiver.sock, message);
}

static void getCommendList(const char *ch, Client client, Client *clients, int actual) {
   char message[BUF_SIZE] = "";  // Initialize message as an empty string
   
   // Safely append strings to the message
   if (strlen(message) + strlen("getPlayersList: 1\n") < BUF_SIZE) {
      strcat(message, "getPlayersList: 1\n");
   }

   // Safely append strings to the message
   if (strlen(message) + strlen("request Starting Game With Player : 2 {player Name}\n") < BUF_SIZE) {
      strcat(message, "request Starting Game With Player : 2 {player Name}\n");
   }
   // Print message to the console (for debugging, if needed)
   printf("%s", message);
   
   // Send the message to the client (assuming client.sock is the socket)
   write_client(client.sock, message);
}



static void requestGameFromPlayer(Client *clients, Client sender, const char *playerName, int actual,const char *ch) {
   int i = 0;
   char message[BUF_SIZE];
   message[0] = 0;
   char buffer[BUF_SIZE];
   for(i = 0; i < actual; i++)
   {
      /* we don't send message to the sender */
      if(sender.sock != clients[i].sock)
      {
         printf("%s\n",playerName);
         if (strcmp(clients[i].name, playerName) == 0)
         {
            snprintf(message, BUF_SIZE, "The Player %s asks you for a game (y/n)!", sender.name);
            printf("%s\n", message);

            // Assuming receiver is stored in clients[actual] based on the context
            write_client(clients[i].sock, message);
            if (strcmp(ch,"y")==0){
               initiateGame(sender,clients[i]);
            }
            
         }
      }
   }
}

static void startGame(){


}

static void doCommend(const char *ch,Client client ,Client *clients, int actual) {
   if (strcmp(ch, "commands") == 0) {
      getCommendList(clients,client, clients,actual);
   }
   if (strcmp(ch, "1") == 0) {
      sendPlayersNamesToClient(clients,client, actual);
   }
   if (strncmp(ch, "2 ", 2) == 0 || strcmp(ch, "y")) {  // Check if the string starts with "2 "
      char playerName[256];
      sscanf(ch + 2, "%s", playerName);  // Extract player name from the string starting after "2 "
      printf("we are requestingGameFromPlayer \n ");
      requestGameFromPlayer(clients,client,playerName,actual,ch);
   }
}



static void remove_client(Client *clients, int to_remove, int *actual)
{
   /* we remove the client in the array */
   memmove(clients + to_remove, clients + to_remove + 1, (*actual - to_remove - 1) * sizeof(Client));
   /* number client - 1 */
   (*actual)--;
}

static void send_message_to_all_clients(Client *clients, Client sender, int actual, const char *buffer, char from_server)
{
   int i = 0;
   char message[BUF_SIZE];
   message[0] = 0;
   for(i = 0; i < actual; i++)
   {
      /* we don't send message to the sender */
      if(sender.sock != clients[i].sock)
      {
         if(from_server == 0)
         {
            strncpy(message, sender.name, BUF_SIZE - 1);
            strncat(message, " : ", sizeof message - strlen(message) - 1);
         }
         strncat(message, buffer, sizeof message - strlen(message) - 1);
         write_client(clients[i].sock, message);
      }
   }
}

static int init_connection(void)
{
   SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
   SOCKADDR_IN sin = { 0 };

   if(sock == INVALID_SOCKET)
   {
      perror("socket()");
      exit(errno);
   }

   sin.sin_addr.s_addr = htonl(INADDR_ANY);
   sin.sin_port = htons(PORT);
   sin.sin_family = AF_INET;

   if(bind(sock,(SOCKADDR *) &sin, sizeof sin) == SOCKET_ERROR)
   {
      perror("bind()");
      exit(errno);
   }

   if(listen(sock, MAX_CLIENTS) == SOCKET_ERROR)
   {
      perror("listen()");
      exit(errno);
   }

   return sock;
}

static void end_connection(int sock)
{
   closesocket(sock);
}

static int read_client(SOCKET sock, char *buffer)
{
   int n = 0;

   if((n = recv(sock, buffer, BUF_SIZE - 1, 0)) < 0)
   {
      perror("recv()");
      /* if recv error we disonnect the client */
      n = 0;
   }

   buffer[n] = 0;

   return n;
}

static void write_client(SOCKET sock, const char *buffer)
{
   if(send(sock, buffer, strlen(buffer), 0) < 0)
   {
      perror("send()");
      exit(errno);
   }
}


int main(int argc, char **argv)
{
   init();

   app();

   end();

   return EXIT_SUCCESS;
}



