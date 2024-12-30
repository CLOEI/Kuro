const base_address = Process.findModuleByName("libgrowtopia.so").base;
const enet_host_service_address = base_address.add(0x168790C);
const enet_peer_send_address = base_address.add(0x1685BD4);
const enet_packet_create_address = base_address.add(0x1685884);

if (!base_address) {
    console.error("Base address not found");
}

const modules = Process.enumerateModules();

for (let module of modules) {
	console.log(module.name);
}

console.log("Base address: " + base_address);
console.log("enet_host_service address: " + enet_host_service_address);
console.log("enet_peer_send address: " + enet_peer_send_address);

const enet_event_type = {
  CONNECT: 1,
  DISCONNECT: 2,
  RECEIVE: 3,
}

let enet_event = null;
let peer = null;
Interceptor.attach(enet_host_service_address, {
    onEnter: function(args) {
      enet_event = args[1];
    },
    onLeave: function(retval) {
        if (retval > 0) {
          let type = enet_event.readU8();
          switch(type) {
            case enet_event_type.CONNECT:
              console.log("CONNECTED");
              break;
            case enet_event_type.DISCONNECT:
              console.log("DISCONNECTED");
              break;
            case enet_event_type.RECEIVE:
              const packet = enet_event.add(0x18).readPointer();
              const packetDataLength = packet.add(0x18).readUInt();
              const packetData = packet.add(0x10).readPointer().readByteArray(packetDataLength);
              console.log(packetData);
              break;
          }
        }
    }
});

Interceptor.attach(enet_peer_send_address, {
    onEnter: function(args) {
        peer = args[0];
        const packet = args[2];
        const packetDataLength = packet.add(0x18).readUInt();
        const packetData = packet.add(0x10).readPointer().readByteArray(packetDataLength);
        console.log(packetData);
    }
});