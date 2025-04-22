#include "server.h"
#include <iostream>


int main()
{
    std::cout << "hello, server1\n";
    sServer.init(2, 8000);
    sServer.run();
    return 0;
}