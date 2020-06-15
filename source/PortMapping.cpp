#include "PortMapping.hpp"
#include <miniupnpc/miniupnpc.h>
#include <miniupnpc/upnpcommands.h>
#include <string>

PortMapping::PortMapping(int publicPort, int localPort) : publicPort(publicPort), localPort(localPort), error(0), upnp_dev(upnpDiscover(2000, nullptr, nullptr, 0, 0, 2, &error)) { // error condition
    // TODO: Handle error
    int status = UPNP_GetValidIGD(upnp_dev, &upnp_urls, &upnp_data, lan_address, sizeof(lan_address));
    (void)status; // TODO: Use status. "1" means a valid IGD was found

    UPNP_GetExternalIPAddress(upnp_urls.controlURL, upnp_data.first.servicetype, wan_address);

    std::string publicPortStr = std::to_string(publicPort);
    std::string localPortStr = std::to_string(localPort);

    error = UPNP_AddPortMapping(
        upnp_urls.controlURL,
        upnp_data.first.servicetype,
        publicPortStr.c_str(),
        localPortStr.c_str(),
        lan_address,
        "Vokiso voice chat",
        "TCP",
        nullptr,
        "0" // lease duration (seconds). 0 means as long as possible
    );
}

PortMapping::~PortMapping() {
    std::string publicPortStr = std::to_string(publicPort);
    UPNP_DeletePortMapping(upnp_urls.controlURL, upnp_data.first.servicetype, publicPortStr.c_str(), "TCP", nullptr);
}

const char *PortMapping::getExternalAddress() {
    return wan_address;
}
