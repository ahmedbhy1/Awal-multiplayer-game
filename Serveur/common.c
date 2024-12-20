#include <errno.h>
#include "common.h"

static void write_client(SOCKET sock, const char *buffer)
{
   if(send(sock, buffer, strlen(buffer), 0) < 0)
   {
      perror("send()");
      exit(errno);
   }
}

static void send_message_to_clients_from_server(Client *clients, int actual, const char *buffer ,Client *allClients,int actualAll)
{
   printf("we are sending message to %d clients \n", actual);
   int i = 0;
   char message[BUF_SIZE];
   for(i = 0; i < actual; i++)
   {
      int j =0;
      for(int j =0;j<actualAll;j++){
         if (strcmp(clients[i].name,allClients[j].name)==0){
            message[0] = 0;
            printf("the message is sent to : %s \n",clients[i].name);
            strncat(message, buffer, sizeof message - strlen(message) - 1);
            write_client(clients[i].sock, message);
            break;
         }
      }

   }
}
