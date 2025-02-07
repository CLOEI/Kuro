#include "packet.hpp"
#include "types.hpp"
#include "variant.hpp"
#include <magic_enum/magic_enum.hpp>
#include <android/log.h>
#include "utils.hpp"

World Packet::world;

void Packet::handle(uint8_t *data, bool *should_process_packet, bool log_packet, _ENetPeer *pPeer,
                    int (*pFunction)(ENetPeer *, enet_uint8, ENetPacket *)) {
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
                case ETankPacketType::NetGamePacketCallFunction: {
                    variantlist_t varlist{};
                    varlist.serialize_from_mem(data + sizeof(TankPacket));
                    auto func = varlist.get(0).get_string();
                    __android_log_print(ANDROID_LOG_INFO, "Kuro", "Function: %s", func.data());

                    if (func == "OnSpawn") {
                        std::string message = varlist.get(1).get_string();
                        std::unordered_map<std::string, std::string> data = parse_and_store_as_map(message, "|");

                        if (data.find("type") != data.end()) {
                            if (data["type"] == "local") {
                                __android_log_print(ANDROID_LOG_INFO, "Kuro", "Local player spawned");
                                data["mstate"] = "1";

                                auto joined = map_join_with_delimiter(data, "|");
                                __android_log_print(ANDROID_LOG_INFO, "Kuro", "Joined: %s", joined.data());
                                varlist[1] = joined;

                                uint32_t size = 0;
                                auto x = varlist.serialize_to_mem(&size, nullptr);
                                auto y = TankPacket{};
                                y.type = ETankPacketType::NetGamePacketCallFunction;
                                y.net_id = -1;
                                y.value = -1;
                                y.flags |= 8;
                                y.extended_data_length = size;

                                ENetPacket *packet = enet_packet_create(nullptr, sizeof(EPacketType) + sizeof(TankPacket) + size, ENET_PACKET_FLAG_RELIABLE);
                                *reinterpret_cast<EPacketType *>(packet->data) = NetMessageGamePacket;
                                memcpy(packet->data + sizeof(EPacketType), &y, sizeof(TankPacket));
                                memcpy(packet->data + sizeof(EPacketType) + sizeof(TankPacket), x, size);

                                if (pFunction(pPeer, 0, packet) == 0) {
                                    __android_log_print(ANDROID_LOG_INFO, "Kuro", "Packet sent successfully");
                                } else {
                                    __android_log_print(ANDROID_LOG_ERROR, "Kuro", "Failed to send packet");
                                }
                            }
                        }
//                        else {
//                            types::Player player;
//                            player.type = data["type"];
//                            player.avatar = data["avatar"];
//                            player.net_id = std::stoi(data["netID"]);
//                            player.online_id = data["onlineID"];
//                            player.e_id = data["eid"];
//                            player.ip = data["ip"];
//                            player.colrect = data["colrect"];
//                            player.title_icon = data["titleIcon"];
//                            player.mstate = std::stoi(data["mstate"]);
//                            player.user_id = std::stoi(data["userID"]);
//                            player.invis = std::stoi(data["invis"]);
//                            player.name = data["name"];
//                            player.country = data["country"];
//                            player.position = {
//                                    std::stoi(data["posXY"].substr(0, data["posXY"].find("|"))),
//                                    std::stoi(data["posXY"].substr(data["posXY"].find("|") + 1))};
//                        }
                    }

                    break;
                }
                case ETankPacketType::NetGamePacketSendMapData: {
//                    world.parse(data + sizeof(TankPacket));
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
