#pragma once
#include <cstdint>
#include <world.hpp>

class Packet {
public:
    static void handle(uint8_t *data, bool *should_process_packet, bool log_packet);
public:
    static World world;
};