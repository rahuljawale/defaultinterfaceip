#include <sys/types.h>
#include <ifaddrs.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/wireless.h>
#include <netinet/in.h>
#include <arpa/inet.h>

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
    if (!ifname) {
        return false;
    }

    bool result = false;
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        err("socket() failed");
    } else {
        struct iwreq req { 0 };
        strncpy(req.ifr_name, ifname, IFNAMSIZ - 1);
        result = (ioctl(fd, SIOCGIWMODE, req) >= 0);
        close(fd);
    }
    return result;
}

std::string default_interface_ip()
{
    struct ifaddrs * ifa;
    int result = getifaddrs(&ifa);
    if (result) {
        err("getifaddrs() failed");
    } else {
        int id = 1;
        for (struct ifaddrs * next = ifa; next; next = next->ifa_next, ++id) {
            auto sain = (struct sockaddr_in *)next->ifa_addr;
            if (!sain || sain->sin_family != AF_INET) {
                continue;
            }

            std::cout << "Interface " << id << std::endl;
            std::cout << "  Name: " << (next->ifa_name ? next->ifa_name : "--unset--") << std::endl;
            std::cout << "  Address: " << inet_ntoa(sain->sin_addr) << std::endl;

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

            if (is_wireless(next->ifa_name)) {
                std::cout << "  Wireless" << std::endl;
            } else {
                std::cout << "  Wired" << std::endl;
            }
        }
        freeifaddrs(ifa);
    }

    return "TODO";
}

}

