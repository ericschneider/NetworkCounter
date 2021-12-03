//
//  IListenerInputHandler.hpp
//  networkCounter
//
//  Created by Eric Schneider on 12/1/21.
//

#ifndef IListenerInputHandler_hpp
#define IListenerInputHandler_hpp
class Listener;

class IListenerInputHandler
{
public:
    virtual void operator()(Listener *listener,
                            const int connectionId,
                            char *inputBuffer,
                            const int inputBufferLength) = 0;
};

#endif /* IListenerInputHandler_hpp */
