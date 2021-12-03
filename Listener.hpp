//
//  Listener.hpp
//  networkCounter
//
//  Created by Eric Schneider on 11/30/21.
//

#ifndef Listener_hpp
#define Listener_hpp

#include "IListenerInputHandler.hpp"

#include <netinet/in.h>
#include <stdio.h>
#include <vector>

const int k_bufferSize = 1024;
const int k_listenerBacklog = 10;
const int k_maxConnections = 1024;

// error codes
const int k_listenerSuccess = 0;
const int k_listenerFailure = 1;

class IListenerInputHandler;

class Listener
{
public:
    Listener(int port, int *pKeepListening, std::shared_ptr<IListenerInputHandler> inputHandler);
    virtual ~Listener();
    
    int Listen();
    void Close();
    
    ssize_t WriteToSingleConnection(int sd, char *buffer);
    void WriteToAllConnections(char *buffer);

    
private:
    int Initialize();
    int InitSocketListen();
    int CreateMasterSocket();
    int SetSocketOptions();
    int BindSocket();
    void CreateSocketDescriptorSet(int &maxSocketDescriptor, fd_set &readfds);
    int WaitForActivity(int maxSocketDescriptor, fd_set &readfds);
    void HandleNewConnections(int &addressLength, const fd_set &readfds);
    void CloseConnection(int sdIndex);
    
    int *m_pKeepListening;
    int m_masterSocket;
    std::vector<int> m_clientConnections;
    struct sockaddr_in m_address;
    char m_buffer[k_bufferSize];
    
    std::shared_ptr<IListenerInputHandler> m_inputHandler;
};

#endif /* Listener_hpp */
