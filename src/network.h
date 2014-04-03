#ifndef NETWORK_H
#define NETWORK_H

#include <SDL2/SDL_net.h>
#include "shape.h"

#define MSG_TYPE_COORD 1

typedef struct header_struct {
	char version;
	char msg_type;
} header;

typedef struct coord_struct {
	uint32_t shape_id;
	int32_t x;
	int32_t y;
} coord;

typedef struct shape_struct {
	uint32_t id;
	uint8_t thickness;
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
	uint32_t num_points;
} netshape;

TCPsocket Connect(char* ip_address, int port);
void SendShape(TCPsocket* socket, Shape s);

#endif