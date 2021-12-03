//
//  CountHandler.cpp
//  networkCounter
//
//  Created by Eric Schneider on 12/1/21.
//

#include "CountHandler.hpp"
#include "Listener.hpp"

#include <string>
#include <stdlib.h>

CountHandler::CountHandler()
: m_count(0)
{}

void CountHandler::operator()(Listener *listener,
                        const int connectionId,
                        char *inputBuffer,
                        const int inputBufferLength)
{
    int initialCount = m_count;
    
    // null terminate buffer so we can use it like a string.
    inputBuffer[inputBufferLength] = '\0';
    
    // Painfully simple parser. If this was a real app, I'd make
    // a state machine so I could be more flexible/have better checking
    // but this is just an example.
    CountCommands command = GetCommand(inputBuffer);
    switch (command)
    {
        case CountCommands::INCR:
            if (IncrementCount(inputBuffer) != initialCount)
            {
                listener->WriteToAllConnections(GetCountAsString());
            }
            break;
        case CountCommands::DECR:
            if (DecrementCount(inputBuffer) != initialCount)
            {
                listener->WriteToAllConnections(GetCountAsString());
            }
            break;
        case CountCommands::OUTPUT:
            listener->WriteToSingleConnection(connectionId, GetCountAsString());
            break;
        case CountCommands::BAD_COMMAND:
            // do nothing
            break;
    }
}

CountCommands CountHandler::GetCommand(char *inputBuffer)
{
    char *command = strtok(inputBuffer, " ");
    if (strcmp("INCR", command) == 0)
        return CountCommands::INCR;
    if (strcmp("DECR", command) == 0)
        return CountCommands::DECR;
    if (strncmp("OUTPUT", command, 6) == 0)
        return CountCommands::OUTPUT;
    return CountCommands::BAD_COMMAND;
}

int CountHandler::DecrementCount(char *inputBuffer)
{
    m_count = m_count - GetAmount();
    return m_count;
}

int CountHandler::IncrementCount(char *inputBuffer)
{
    m_count = m_count + GetAmount();
    return m_count;
}

int CountHandler::GetAmount()
{
    int amount = 0;
    char *amountStr = strtok(NULL, "");
    
    if (amountStr)
    {
        // If stoi fails, we just need to catch the exception and return 0,
        // we never recieved a valid integer.
        try
        {
            amount = std::stoi(amountStr);
        }
        catch (...) {}
    }
    return amount;
}

char *CountHandler::GetCountAsString()
{
    sprintf(m_countStringBuffer, "%d\n", m_count);
    return m_countStringBuffer;
}
