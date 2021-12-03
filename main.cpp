//
//  main.cpp
//  networkCounter
//
//  Created by Eric Schneider on 11/30/21.
//

#include "Listener.hpp"
#include "CountHandler.hpp"

#include <csignal>
#include <iostream>

const int k_port = 8089;

static int g_runFlag = true;

void SignalHandler(int signal)
{
    g_runFlag = false;
}

int main(int argc, const char * argv[]) {
    std::cout << "Network Counter start" << std::endl;
    
    // Install a signal handler
    std::signal(SIGTERM, SignalHandler);

    Listener portListener(k_port, &g_runFlag, std::make_shared<CountHandler>());
    return portListener.Listen();
}
