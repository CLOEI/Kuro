function waitForModule(name) {
  return new Promise((resolve) => {
    const interval = setInterval(() => {
      const module = Process.findModuleByName(name);
      if (module) {
        clearInterval(interval);
        resolve(module);
      }
    }, 100);
  });
}

waitForModule("libgrowtopia.so").then((module) => {
  const base_address = module.base;
  const get_screen_width_address = base_address.add(0x10A8088);
  const enet_host_service_address = base_address.add(0x1085258);

  console.log("Base address: " + base_address);
  console.log("get_screen_width address: " + get_screen_width_address);
  console.log("enet_host_service address: " + enet_host_service_address);

  Interceptor.attach(enet_host_service_address.add(1), {
    onEnter: function (args) {
      console.log("enet_host_service called");
    }
  });

  Interceptor.attach(get_screen_width_address.add(1), {
    onEnter: function (args) {
      console.log("get_screen_width called");
    },
    onLeave: function (retval) {
      console.log("get_screen_width returned: " + parseInt(retval, 16));
    }
  });
  
  
});