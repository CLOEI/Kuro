const base_address = Process.findModuleByName("libgrowtopia.so").base;
const get_screen_width_address = base_address.add(0x10A8088);

if (!base_address) {
  console.error("Base address not found");
}

console.log("Base address: " + base_address);
console.log("get_screen_width address: " + get_screen_width_address);

Interceptor.attach(get_screen_width_address, {
  onLeave: function(retval) {
    console.log('Original return value:', retval.toUInt32());
  }
});