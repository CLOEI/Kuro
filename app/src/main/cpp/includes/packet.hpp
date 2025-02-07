#pragma once
#include <cstdint>
#include <world.hpp>
#include <gtitem.h>
#include "../vendor/enet/include/enet/enet.h"

class Packet {
public:
    static void
    handle(uint8_t *data, bool *should_process_packet, bool log_packet, _ENetPeer *pPeer,
           int (*pFunction)(ENetPeer *, enet_uint8, ENetPacket *));
public:
    static World world;
};