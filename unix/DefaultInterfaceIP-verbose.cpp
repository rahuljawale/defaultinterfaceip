#include "IfAddrsResource.h"
#include "SocketResource.h"
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#ifdef __linux__
#  include <linux/wireless.h>
#else
#  include <net/if.h>
#  include <net/if_media.h>
#  ifdef __APPLE__
#    define ERR_NO_SUPPORT EOPNOTSUPP
#  else // BSD
#    define ERR_NO_SUPPORT EINVAL
#  endif
#endif

#include <string>
#include <cstring>

// verbose-only
#include <iostream>
#include <errno.h>

namespace Debauchee
{

static void err(const char * msg)
{
    std::cerr << msg << " (" << errno << ")" << std::endl;
}

static bool is_wireless(const char * ifname, const SocketResource& fd)
{
#ifdef __linux__

    struct iwreq req { 0 };
    ::strncpy(req.ifr_name, ifname, IFNAMSIZ - 1);
    return ioctl(fd, SIOCGIWMODE, &req) >= 0;

#else // BSD / OSX

    struct ifmediareq req;
    ::memset(&req, 0, sizeof(struct ifmediareq));
    ::strncpy(req.ifm_name, ifname, IFNAMSIZ - 1);
    if (ioctl(fd, SIOCGIFMEDIA, &req) < 0) {
        // fails for interfaces that don't support the ioctl (like loopbacks)
        // only complain if it's a different kind of failure
        if (errno != ERR_NO_SUPPORT) {
            err("ioctl() failed");
        }
    } else {
        if (!(req.ifm_status & IFM_AVALID)) {
            std::cout << "  IFM-Active not valid" << std::endl;
        } else {
            uint64_t type = IFM_TYPE(req.ifm_active);
            if (type == IFM_IEEE80211) {
                return true;
            } else if (type != IFM_ETHER) {
                std::cout << "  Invalid mediareq type (" << type << ")" << std::endl;
            }
        }
    }
    return false;

#endif
}

static bool is_wireless(const char * ifname)
{
    if (ifname) {
        SocketResource fd(AF_INET, SOCK_DGRAM, 0);
        if (!fd.is_valid()) {
            err("socket() failed");
        } else {
            return is_wireless(ifname, fd);
        }
    }
    return false;
}

std::string default_interface_ip_verbose()
{
    std::string defaultAddress;
    std::string wirelessAddress;
    IfAddrsResource ifa;
    if (!ifa.is_valid()) {
        err("getifaddrs() failed");
    } else {
        int id = 1;
        for (struct ifaddrs * next = ifa; next; next = next->ifa_next, ++id) {
            auto sain = (struct sockaddr_in *)next->ifa_addr;
            if (!sain || sain->sin_family != AF_INET) {
                continue;
            }

            std::string address = inet_ntoa(sain->sin_addr);

            std::cout << "Interface " << id << std::endl;
            std::cout << "  Name: " << (next->ifa_name ? next->ifa_name : "--unset--") << std::endl;
            std::cout << "  Address: " << address << std::endl;

            if (next->ifa_flags & IFF_UP) {
                std::cout << "  Up" << std::endl;
            } else {
                std::cout << "  Down" << std::endl;
            }

            if (next->ifa_flags & IFF_RUNNING) {
                std::cout << "  Running" << std::endl;
            } else {
                std::cout << "  Not Running" << std::endl;
            }

            if (next->ifa_flags & IFF_LOOPBACK) {
                std::cout << "  Loopback" << std::endl;
            }

            bool isWireless = is_wireless(next->ifa_name);
            if (isWireless) {
                std::cout << "  Wireless" << std::endl;
            } else {
                std::cout << "  Wired" << std::endl;
            }

            if (
                (next->ifa_flags & IFF_RUNNING) &&
                !(next->ifa_flags & IFF_LOOPBACK)
                ) {
                if (isWireless) {
                    if (wirelessAddress.empty()) {
                        std::cout << "  Selecting as wireless" << std::endl;
                        wirelessAddress = address;
                    }
                } else {
                    if (defaultAddress.empty()) {
                        std::cout << "  Setting as default" << std::endl;
                        defaultAddress = address;
                    }
                }
            }

			std::cout << std::endl;
        }
    }
    return defaultAddress.empty() ? wirelessAddress : defaultAddress;
}

}

