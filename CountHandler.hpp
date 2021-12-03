//
//  CountHandler.hpp
//  networkCounter
//
//  Created by Eric Schneider on 12/1/21.
//

#ifndef CountHandler_hpp
#define CountHandler_hpp

#include "IListenerInputHandler.hpp"
#include <stdio.h>

const int k_countStringBufferSize = 128;

enum class CountCommands { INCR, DECR, OUTPUT, BAD_COMMAND};

class CountHandler : public IListenerInputHandler
{
public:
    CountHandler();
    virtual ~CountHandler() = default;
    
    void operator()(Listener *listener,
                            const int connectionId,
                            char *inputBuffer,
                            const int inputBufferLength);
private:
    char m_countStringBuffer[k_countStringBufferSize];
    int m_count;
    CountCommands GetCommand(char *inputBuffer);
    int DecrementCount(char *inputBuffer);
    int IncrementCount(char *inputBuffer);
    int GetAmount();
    char *GetCountAsString();
};

#endif /* CountHandler_hpp */
