#include <WinSock2.h>
#include <iostream>
#include "GetDefaultInterfaceIP.h"

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Iphlpapi.lib")

int main(int argc, char * argv[])
{
    WSADATA wd;
    WSAStartup(MAKEWORD(2, 0), &wd);
    auto defaultAddress = Debauchee::default_ip_address();
    std::cout << std::endl;
    std::cout << "Default IP Address: " << defaultAddress << std::endl;
    ::getchar();
    return 0;
}
