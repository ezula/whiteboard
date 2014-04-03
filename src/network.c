#include "network.h"

TCPsocket Connect(char* ip_address, int port) {
	IPaddress ip;
	TCPsocket sock;

	if (SDLNet_ResolveHost(&ip, ip_address, port)) {
		printf("SDLNet_ResolveHost: %s\n",SDLNet_GetError());
		exit(EXIT_FAILURE);
	}

	sock=SDLNet_TCP_Open(&ip);

	if(!sock) {
		printf("SDLNet_TCP_Open: %s\n",SDLNet_GetError());
		exit(EXIT_FAILURE);
	}

	return sock;
}

void SendShape(TCPsocket* socket, Shape s) {
	
}