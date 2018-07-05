#include "IfAddrsResource.h"
#include "SocketResource.h"
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#ifdef __linux__
#include <linux/wireless.h>
#else
#include <net/if.h>
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

static bool is_wireless(const char * ifname)
{
#ifdef __linux__
    if (ifname) {
        SocketResource fd(AF_INET, SOCK_STREAM, 0);
        if (!fd.is_valid()) {
            err("socket() failed");
        } else {
            struct iwreq req { 0 };
            ::strncpy(req.ifr_name, ifname, IFNAMSIZ - 1);
            return ioctl(fd, SIOCGIWMODE, req) >= 0;
        }
    }
#else
    std::cout << "  Cannot check for wireless" << std::endl;
#endif
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
                        std::cout << "Selecting as wireless" << std::endl;
                        wirelessAddress = address;
                    }
                } else {
                    if (defaultAddress.empty()) {
                        std::cout << "  Setting as default" << std::endl;
                        defaultAddress = address;
                    }
                }
            }
        }
    }
    return defaultAddress.empty() ? wirelessAddress : defaultAddress;
}

}

