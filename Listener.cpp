//
//  Listener.cpp
//  networkCounter
//
//  Created by Eric Schneider on 11/30/21.
//

#include "Listener.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <errno.h>
#include <unistd.h>

Listener::Listener(int port, int *pKeepListening, std::shared_ptr<IListenerInputHandler> inputHandler)
: m_pKeepListening(pKeepListening)
, m_masterSocket(0)
, m_clientConnections(k_maxConnections, 0)
, m_inputHandler(inputHandler)
{
    m_address.sin_family = AF_INET;
    m_address.sin_addr.s_addr = INADDR_ANY;
    m_address.sin_port = htons(port);
}

Listener::~Listener()
{
    
}

int Listener::Initialize()
{
    if (CreateMasterSocket() == k_listenerFailure) return k_listenerFailure;
    if (SetSocketOptions() == k_listenerFailure) return k_listenerFailure;
    if (BindSocket() == k_listenerFailure) return k_listenerFailure;
    if (InitSocketListen() == k_listenerFailure) return k_listenerFailure;
    
    return k_listenerSuccess;
}

int Listener::Listen()
{
    if (Initialize() == k_listenerFailure) return k_listenerFailure;
    
    int addressLength = sizeof(m_address);
    while (*m_pKeepListening)
    {
        fd_set readfds;
        int maxSocketDescriptor;
             
        CreateSocketDescriptorSet(maxSocketDescriptor, readfds);
        
        if (WaitForActivity(maxSocketDescriptor, readfds) == k_listenerFailure) return k_listenerFailure;
             
        //If something happened on the master socket ,
        //then its an incoming connection
        HandleNewConnections(addressLength, readfds);
             
        //else its some IO operation on some other socket
        for (int i = 0; i < k_maxConnections; i++)
        {
            int sd = m_clientConnections[i];
            ssize_t bytesRead = 0;
                 
            if (FD_ISSET(sd, &readfds))
            {
                if ((bytesRead = read(sd, m_buffer, k_bufferSize - 1)) == 0)
                {
                    CloseConnection(i);
                }
                     
                //Echo back the message that came in
                else
                {
                    (*m_inputHandler)(this, sd, m_buffer, bytesRead);
                }
            }
        }
    }
    
    return k_listenerSuccess;
}

void Listener::Close()
{
    
}

//
// Private
//

int Listener::WaitForActivity(int maxSocketDescriptor, fd_set &readfds)
{
    int activity = select(maxSocketDescriptor + 1 , &readfds , NULL , NULL , NULL);
    
    if ((activity < 0) && (errno != EINTR))
    {
        return k_listenerFailure;
    }
    return k_listenerSuccess;
}

void Listener::HandleNewConnections(int &addressLength, const fd_set &readfds)
{
    if (FD_ISSET(m_masterSocket, &readfds))
    {
        int newSocket;
        if ((newSocket = accept(m_masterSocket,
                                (struct sockaddr *)&m_address, (socklen_t*)&addressLength))<0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        
        //add new socket to array of sockets
        for (int i = 0; i < k_maxConnections; i++)
        {
            //if position is empty
            if (m_clientConnections[i] == 0)
            {
                m_clientConnections[i] = newSocket;
                break;
            }
        }
    }
}

ssize_t Listener::WriteToSingleConnection(int sd, char *buffer)
{
    return send(sd, buffer, strlen(buffer), 0);
}

void Listener::WriteToAllConnections(char *buffer)
{
    std::for_each(m_clientConnections.begin(), m_clientConnections.end(), [this, buffer](int &connection)
    {
        if (connection > 0)
        {
            WriteToSingleConnection(connection, buffer);
        }
    });
}

void Listener::CloseConnection(int sdIndex)
{
    close(m_clientConnections[sdIndex]);
    m_clientConnections[sdIndex] = 0;
}

int Listener::InitSocketListen()
{
    if (listen(m_masterSocket, k_listenerBacklog) < 0)
    {
        perror("Listener initialization failed");
        return k_listenerFailure;
    }
    return k_listenerSuccess;
}

int Listener::CreateMasterSocket()
{
    if((m_masterSocket = socket(AF_INET , SOCK_STREAM , 0)) == 0)
    {
        perror("Listener initialization failed");
        return k_listenerFailure;
    }
    return k_listenerSuccess;
}

int Listener::SetSocketOptions()
{
    int reuseAddrOption = true;
    if (setsockopt(m_masterSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&reuseAddrOption,
                   sizeof(reuseAddrOption)) < 0 )
    {
        perror("Listener initialization failed");
        return k_listenerFailure;
    }
    return k_listenerSuccess;
}

int Listener::BindSocket() {
    if (bind(m_masterSocket, (struct sockaddr *)&m_address, sizeof(m_address)) < 0)
    {
        perror("Listener initialization failed");
        return k_listenerFailure;
    }
    return k_listenerSuccess;
}

void Listener::CreateSocketDescriptorSet(int &maxSocketDescriptor, fd_set &readfds) {
    FD_ZERO(&readfds);
    
    FD_SET(m_masterSocket, &readfds);
    maxSocketDescriptor = m_masterSocket;
    
    std::for_each(m_clientConnections.begin(), m_clientConnections.end(), [&readfds, &maxSocketDescriptor](int &connection)
    {
        if (connection > 0)
        {
            FD_SET(connection, &readfds);
        }
        
        if (connection > maxSocketDescriptor)
        {
            maxSocketDescriptor = connection;
        }
    });
}
