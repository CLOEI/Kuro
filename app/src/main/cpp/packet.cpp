#include "packet.hpp"
#include "types.hpp"
#include <magic_enum/magic_enum.hpp>
#include <android/log.h>

World Packet::world;

void Packet::handle(uint8_t *data, bool* should_process_packet, bool log_packet) {
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
            switch (tank_packet.type) {
                case ETankPacketType::NetGamePacketAppIntegrityFail: {
                    *should_process_packet = false;  // not reallu sure it would do much. but it's worth a try
                    break;
                }
                case ETankPacketType::NetGamePacketSendMapData: {
                    world.parse(data + sizeof(TankPacket));
                }
                default:
                    break;
            }
            break;
        }
        default:
            break;
    }
}
