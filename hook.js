const base_address = Process.findModuleByName("libgrowtopia.so").base;
const enet_host_service_address = 0x168790C;
const enet_peer_send_address = 0x1685BD4;
const enet_packet_create_address = 0x1685884;

if (!base_address) {
    console.error("Base address not found");
    return;
}

