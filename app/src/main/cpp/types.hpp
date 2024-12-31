#pragma once
#include <cstdint>

enum ETankPacketType : uint8_t {
    NetGamePacketState,
    NetGamePacketCallFunction,
    NetGamePacketUpdateStatus,
    NetGamePacketTileChangeRequest,
    NetGamePacketSendMapData,
    NetGamePacketSendTileUpdateData,
    NetGamePacketSendTileUpdateDataMultiple,
    NetGamePacketTileActivateRequest,
    NetGamePacketTileApplyDamage,
    NetGamePacketSendInventoryState,
    NetGamePacketItemActivateRequest,
    NetGamePacketItemActivateObjectRequest,
    NetGamePacketSendTileTreeState,
    NetGamePacketModifyItemInventory,
    NetGamePacketItemChangeObject,
    NetGamePacketSendLock,
    NetGamePacketSendItemDatabaseData,
    NetGamePacketSendParticleEffect,
    NetGamePacketSetIconState,
    NetGamePacketItemEffect,
    NetGamePacketSetCharacterState,
    NetGamePacketPingReply,
    NetGamePacketPingRequest,
    NetGamePacketGotPunched,
    NetGamePacketAppCheckResponse,
    NetGamePacketAppIntegrityFail,
    NetGamePacketDisconnect,
    NetGamePacketBattleJoin,
    NetGamePacketBattleEvent,
    NetGamePacketUseDoor,
    NetGamePacketSendParental,
    NetGamePacketGoneFishin,
    NetGamePacketSteam,
    NetGamePacketPetBattle,
    NetGamePacketNpc,
    NetGamePacketSpecial,
    NetGamePacketSendParticleEffectV2,
    NetGameActivateArrowToItem,
    NetGameSelectTileIndex,
    NetGamePacketSendPlayerTributeData,
    NetGamePacketFTUESetItemToQuickInventory,
    NetGamePacketPVENpc,
    NetGamePacketPVPCardBattle,
    NetGamePacketPVEApplyPlayerDamage,
    NetGamePacketPVENPCPositionUpdate,
    NetGamePacketSetExtraMods,
    NetGamePacketOnStepTileMod
};

struct TankPacket {
    ETankPacketType _type;
    uint8_t unk1;
    uint8_t unk2;
    uint8_t unk3;
    uint32_t net_id;
    uint32_t sec_id;
    uint32_t flags;
    float unk6;
    uint32_t value;
    float vector_x;
    float vector_y;
    float vector_x2;
    float vector_y2;
    float unk12;
    int32_t int_x;
    int32_t int_y;
    uint32_t extended_data_length;
};

enum EPacketType : uint32_t {
    NetMessageUnknown,
    NetMessageServerHello,
    NetMessageGenericText,
    NetMessageGameMessage,
    NetMessageGamePacket,
    NetMessageError,
    NetMessageTrack,
    NetMessageClientLogRequest,
    NetMessageClientLogResponse,
    NetMessageMax,
};