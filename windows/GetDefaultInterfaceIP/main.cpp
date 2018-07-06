#include <WinSock2.h>
#include <iostream>
#include "GetDefaultInterfaceIP.h"
#include "GetDefaultInterfaceIP-verbose.h"

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Iphlpapi.lib")

int main(int argc, char * argv[])
{
    WSADATA wd;
    WSAStartup(MAKEWORD(2, 0), &wd);
    auto defaultAddress = Debauchee::default_ip_address_verbose();
    std::cout << std::endl;
    std::cout << "Default IP Address (v): " << defaultAddress << std::endl;
    std::cout << "Default IP Address (q): " << Debauchee::default_ip_address() << std::endl;
    ::getchar();
    return 0;
}
