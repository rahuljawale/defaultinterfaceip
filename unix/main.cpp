#include <iostream>
#include "DefaultInterfaceIP.h"
#include "DefaultInterfaceIP-verbose.h"

int main()
{
    auto defaultAddress = Debauchee::default_interface_ip_verbose();
    std::cout << "Default IP Address (v): " << defaultAddress << std::endl;
    std::cout << "Default IP Address (q): " << Debauchee::default_interface_ip() << std::endl;
    return 0;
}
