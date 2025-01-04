#pragma once
#include <cstdint>

class Packet {
public:
    static void handle(uint8_t *data, bool *should_send_packet, bool log_packet);
};