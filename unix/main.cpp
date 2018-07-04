#include <iostream>
#include "DefaultInterfaceIP.h"

int main()
{
    auto addr = Debauchee::default_interface_ip();
    std::cout << std::endl;
    std::cout << "Default Interface IP: " << addr << std::endl;
    return 0;
}
