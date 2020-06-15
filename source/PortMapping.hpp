#pragma once
#include <miniupnpc/miniupnpc.h>
#include <miniupnpc/upnpcommands.h>

class PortMapping {
  public:
    PortMapping(int publicPort, int localPort);
    ~PortMapping();

    const char *getExternalAddress();

  private:
    int publicPort, localPort;
    int error;
    struct UPNPDev *upnp_dev;
    char lan_address[64];
    char wan_address[64];
    struct UPNPUrls upnp_urls;
    struct IGDdatas upnp_data;
};