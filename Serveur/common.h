
#ifndef GAME_H
#define GAME_H
#include "server2.h"

static void write_client(SOCKET sock, const char *buffer);
static void send_message_to_clients_from_server(Client *clients, int actual, const char *buffer,Client *allClients,int actualAll);

#endif /* guard */
