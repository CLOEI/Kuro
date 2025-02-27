#pragma once
#include "enet/enet.h"

typedef int (*enet_host_service_t)(ENetHost *host, ENetEvent *event, enet_uint32 timeout);
typedef int (*get_screen_width_t)();
typedef int (*get_screen_height_t)();
typedef int (*send_packet_t)(int type, uint8_t *data, int peer);
typedef int (*send_packet_raw_t)(int type, int data, int packet_length, void *a4, int *peer, int flag);