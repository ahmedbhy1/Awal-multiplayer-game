#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "playerState.h"
#include "server2.h"
#include "common.c"
#include "game.h"

#define MAX_CLIENTS 25
#define MAX_GAMES 255
#define MAX_OBSERVERS 10
#define MAX_RECIEVED_GAME_CLIENTS 12
#define MAX_GAMETABLES 10
#define MAX_GAMES 10


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
         bool isNameDuplicate = false;
         for (int i=0;i<actual;i++){
            printf("letts see the client %d name %s =? %s\n",i,clients[i].name,c.name);
            if (strcmp(clients[i].name, c.name) == 0){
               isNameDuplicate = true;
               break;
            }
         }
         if (isNameDuplicate)
         {
            printf("Duplicate name detected: %s\n", buffer);
            const char *errorMessage = "Name already in use. Please retry with a different name.\n";
            send(csock, errorMessage, strlen(errorMessage), 0);
            closesocket(csock); // Close the connection
         }
         else{
            clients[actual] = c;
            actual++;
            if (!search(c.name)){
               State state = {0, 0 , 0 ,NULL,false};
               insert(c.name,state);
            }
            State *player = search(c.name);
            if (player && player->opponentName && player->state==3 ){
               char message[256];
               strcpy(message, "reconnecting to the game with ");
               strcat(message, player->opponentName);
               strcat(message, "...\n here is the Last Table State: \n");
               write_client(c.sock,message);
               char* nextPlayerName;
               if (player->isPlayerTurn){
                  nextPlayerName =  c.name;
               }
               else{
                  nextPlayerName = player->opponentName;
               }
               showGameTable(player->currentIndexOfGame,nextPlayerName ,clients,actual);
            }
            else{
               write_client(c.sock,"write {commands} to get the full command list \n");
            }
            print_client_names(clients, actual);
            display_players();
            }
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
      State *player = search(clients[i].name);
      int remaining_space = BUF_SIZE - strlen(message) - 1;
      if (remaining_space > 0) {
         char* temp;
         if (player->state==0)temp = " (waiting for a game request)";
         if (player->state==1 || player->state==2)temp = " (in a game request)";
         if (player->state==3)temp = " (in a game)";
         if (player->state==4)temp = " (is watching a game a game)";
         strncat(message, clients[i].name, remaining_space - 1);
         remaining_space = BUF_SIZE - strlen(message) - 1;
         strncat(message, temp, remaining_space - 1);
         strncat(message, "\n", remaining_space - strlen(message) - 1);
      } else {
         break;
      }
   }
   
   write_client(receiver.sock, message);
}

static void getCommendList(const char *ch, Client client, Client *clients, int actual) {
   char message[BUF_SIZE] = "";  // Initialize message as an empty string

   // Safely append strings to the message
   if (strlen(message) + strlen("get the list of Players that are not in a game: 1\n") < BUF_SIZE) {
      strcat(message, "get the list of Players that are not in a game: 1\n");
   }

   // Safely append strings to the message
   if (strlen(message) + strlen("request Starting Game With Player : 2 {player Name}\n") < BUF_SIZE) {
      strcat(message, "request Starting Game With Player : 2 {player Name}\n");
   }

   // Safely append strings to the message
   if (strlen(message) + strlen("get the list of Games : g\n") < BUF_SIZE) {
      strcat(message, "get the list of Games : g\n");
   }

   // Safely append strings to the message
   if (strlen(message) + strlen("join a Game as an observer :o {index of active game to join}\n") < BUF_SIZE) {
      strcat(message, "join a Game as an observer :o {index of active game to join}\n");
   }

   // Safely append strings to the message
   if (strlen(message) + strlen("quit a Game as an observer :q\n") < BUF_SIZE) {
      strcat(message, "quit a Game as an observer :q\n");
   }

   // Safely append strings to the message
   if (strlen(message) + strlen("surrender from a Game as a player :sr\n") < BUF_SIZE) {
      strcat(message, "surrender from a Game as a player :sr\n");
   }

   // Safely append strings to the message
   if (strlen(message) + strlen("show game history : gh [index_of_game]\n") < BUF_SIZE) {
      strcat(message, "show game history : gh [index_of_game]\n");
   }

   // Send the message to the client (assuming client.sock is the socket)
   write_client(client.sock, message);
}



static void requestOrAcceptGameFromPlayer(Client *clients, Client sender, const char *playerName, int actual,const char *ch) {
   int i = 0;
   char message[BUF_SIZE];
   message[0] = 0;
   char buffer[BUF_SIZE];
   for(i = 0; i < actual; i++)
   {
      /* we don't send message to the sender */
      if(sender.sock != clients[i].sock)
      {
         if (strcmp(clients[i].name, playerName) == 0)
         {
            State *player1 = search(sender.name);
            State *player2 = search(playerName);
            printf("player1= %s player2= %s \n",sender.name,playerName);
            if(player1 && player2){
               //enable the ovserver to start or get a play request
               if (strcmp(ch,"")==0 && (player1->state==0 || player1->state==4) && (player2->state==0||player2->state==4) ){
                  modify_player_state(sender.name,1,0,0,playerName,false);
                  modify_player_state(playerName,2,0,1,sender.name,false);
                  snprintf(message, BUF_SIZE, "The Player %s asks you for a game (y/n)!", sender.name);
               }

               // Assuming receiver is stored in clients[actual] based on the context
               write_client(clients[i].sock, message);
               if (strcmp(ch,"y")==0 && player1->state==2 && player2->state==1){
                  int index = initiateGame(clients[i],sender,playerName,clients,actual);
                  modify_player_state(sender.name,3,index,NULL,NULL,false);
                  modify_player_state(playerName,3,index,NULL,NULL,true);
                  printf("index of the created game! ");
                  printf("%d \n",index);
               }
            }
            
            
         }
      }
   }
}



static void doCommend(const char *ch,Client client ,Client *clients, int actual) {
   printf("are we in doCommend ! \n");

   if (strcmp(ch, "commands") == 0) {
      getCommendList(clients,client, clients,actual);
   }
   if (strcmp(ch, "1") == 0) {
      sendPlayersNamesToClient(clients,client, actual);
   }
   if (strcmp(ch,"s") == 0){
      display_players();
   }
   if (strncmp(ch, "p ", 2) == 0) {
      printf("we are here p\n");

      int indexToPlay;
      // Try to extract an integer from the string starting at position 2
      if (sscanf(ch + 2, "%d", &indexToPlay) == 1 && indexToPlay >= 0 && indexToPlay <= 6) {
         printf("Index to play extracted: %d\n", indexToPlay);
         State *state = search(client.name);
         if (state->isPlayerTurn){
            //printf("playGameTurn params client name; index player; current index of game, indexToPlay :%s,%d,%d,%d \n",client.name,state->playerIndex,state->currentIndexOfGame,indexToPlay);
            bool valid = playGameTurn(client,state->playerIndex,state->currentIndexOfGame,indexToPlay-1,state->opponentName,clients,actual);
            printf("%s is the previous player turn \n",client.name);
            printf("%s its your turn \n",state->opponentName);
            if (valid){
               modify_player_state(client.name,-1,NULL,NULL,NULL,false);
               modify_player_state(state->opponentName,-1,NULL,NULL,NULL,true);
            }
            else{
               write_client(client.sock,"Your Game Play is Not Valid! \n You can't choose Houses of Seeds 0 !\n");
            }
            if (showPlayerWinIfGameOver(state->currentIndexOfGame,clients,actual)){
               modify_player_state(client.name,0,NULL,NULL,NULL,false);
               modify_player_state(state->opponentName,0,NULL,NULL,NULL,true);

            }
         }
      } else {
         printf("Invalid index or input format. Ensure the index is between 0 and 6.\n");
      }
   }
   if (strncmp(ch, "2 ", 2) == 0) {  // Check if the string starts with "2 " or with y
      char playerName[256];
      sscanf(ch + 2, "%s", playerName);  // Extract player name from the string starting after "2 "
      printf("we are requesting game to player! \n");
      requestOrAcceptGameFromPlayer(clients,client,playerName,actual,"");
   }
   if (strncmp(ch, "m ", 2) == 0) {  // Check if the string starts with "2 " or with y
      //char playerMessage[256];
      //sscanf(ch + 2, "%s", playerMessage);  // Extract player name from the string starting after "2 "
      //State *result = search(client.name);
      //sendPlyerMassageToTheGameChat(clients,client,playerName,actual,"");
   }
   if (strncmp(ch, "o ", 2) == 0) {  // Check if the string starts with "2 " or with y
      int indexOfGame;
      // Try to extract an integer from the string starting at position 2
      if (sscanf(ch + 2, "%d", &indexOfGame) == 1 && isValidGameIndexToJoinAsOb(indexOfGame)) {
         joinClientAsObserver(client,indexOfGame);
         modify_player_state(client.name,4,NULL,NULL,NULL,false);
         write_client(client.sock,"you are watching the game! waite for somone to play a move! \n if you want to leave type q [enter] \n");
      }
   }

   if (strncmp(ch, "g", 2) == 0) {
      showGames(client);
   }

   if (strncmp(ch, "gh ", 3) == 0) {  // Check if the string starts with "2 " or with y
      int indexOfGame;
      // Try to extract an integer from the string starting at position 2
      if (sscanf(ch + 3, "%d", &indexOfGame) == 1 && isValidGameIndexToGetHistoryFrom(indexOfGame)) {
         printf("show index of game %d \n",indexOfGame);
         showGameHistory(client,indexOfGame);
      }
      else{
         write_client(client.sock,"you should enter a valid game index of a terminated game! \n");
      }
   }

   if (strcmp(ch, "y") == 0) {
      printf("we are accepting game from player! \n");
      State *result = search(client.name);
      if (result){
         requestOrAcceptGameFromPlayer(clients,client,result->opponentName,actual,"y");
      }
       
   }

   if (strcmp(ch, "sr") == 0) {
      printf("sr from the game from player! \n");
      State *result = search(client.name);
      if (result && result->state == 3){
         modify_player_state(client.name,0,NULL,NULL,NULL,false);
         modify_player_state(result->opponentName,0,NULL,NULL,NULL,false);
         printf("sr from the game from player 2 ! \n");
         for(int i=0;i<actual;i++){
            if (strcmp(clients[i].name,result->opponentName)==0) {

               printf("sr from the game from player 3 ! \n");
               surrenderFromGame(clients[i],result->currentIndexOfGame);      
               write_client(clients[i].sock,"Congrats ! you win the game your opponant surrnadered ! \n write {commands} to get the full command list \n");
            }
         }
         write_client(client.sock,"you lost the game (you abandonned the game)! \n write {commands} to get the full command list \n");
      }
      else{
         write_client(client.sock,"you need to be observer first so that you can quit the game \n");
      }   
   }

   if (strcmp(ch, "q") == 0) {
      printf("quit from observing the game from player! \n");
      State *result = search(client.name);
      if (result && result->state == 4){
         modify_player_state(client.name,0,NULL,NULL,NULL,false);
         write_client(client.sock,"write {commands} to get the full command list \n");
      }
      else{
         write_client(client.sock,"you need to be observer first so that you can quit the game \n");
      }   
   }

   if (strcmp(ch, "n") == 0) {
      printf("we are rejecting game from player! \n");
      State *result = search(client.name);
      if (result && result->state==2){
         printf("we are resetting player state to 0 ! \n");
         modify_player_state(client.name,0,NULL,NULL,NULL,false);         
         modify_player_state(result->opponentName,0,NULL,NULL,NULL,false);
         State *result = search(client.name);
         printf("player1: %s ;state%d \n" ,client.name, result->state);
         write_client(client.sock,"write {commands} to get the full command list \n");
         for(int i=0;i<actual;i++){
            if (strcmp(clients[i].name,result->opponentName)==0) {
               write_client(clients[i].sock,"your game request is denied ! \nwrite {commands} to get the full command list \n");
               break;
            }
         }
      }
       
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


static void init_PlayersState(){
    init_table();

    // Create some State structs
    State state1 = {0, 0 , 0 ,NULL,false};
    State state2 = {0, 0 , 1 ,NULL,true};

    // Insert into the hash table
    insert("ahmed", state1);
    insert("alii", state2);

    // Search in the hash table
    State *result = search("ahmed");
    if (result) {
        printf("Found: ID=%d, currentIndexOfTheGame=%d\n, OpponantName=%s\n", result->state, result->currentIndexOfGame, result->opponentName);
    } else {
        printf("Key not found\n");
    }

    result = search("alii");
    if (result) {
        printf("Found: ID=%d, currentIndexOfTheGame=%d\n, OpponantName=%s\n", result->state, result->currentIndexOfGame, result->opponentName);
    } else {
        printf("Key not found\n");
    }
}


int main(int argc, char **argv)
{
   init();

   init_table();
   
   app();

   free_table();

   end();

   return EXIT_SUCCESS;
}
