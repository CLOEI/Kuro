#include "packet.hpp"
#include "types.hpp"
#include <magic_enum/magic_enum.hpp>
#include <android/log.h>

void Packet::handle(uint8_t *data, bool* should_send_packet, bool log_packet) {
    EPacketType packet_type{*reinterpret_cast<uint32_t *>(data)};
    auto name = magic_enum::enum_name(
            magic_enum::enum_value<EPacketType>(packet_type));
    data += 4;
    if (log_packet) {
        __android_log_print(ANDROID_LOG_INFO, "Kuro", "Packet type: %s", name.data());
    }
    switch (packet_type) {
        case NetMessageGamePacket: {
            TankPacket tank_packet{};
            memcpy(&tank_packet, data, sizeof(TankPacket));
            auto pkt_name = magic_enum::enum_name(
                    magic_enum::enum_value<ETankPacketType>(tank_packet.type));
            if (log_packet) {
                __android_log_print(ANDROID_LOG_INFO, "Kuro", "TankPacket type: %s", pkt_name.data());
            }
            if (tank_packet.type == ETankPacketType::NetGamePacketAppIntegrityFail) { // not reallu sure it would do much. but it's worth a try
                *should_send_packet = false;
            }
            break;
        }
        default:
            break;
    }
}
