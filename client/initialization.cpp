#ifndef INITIALIZATION_H_INCLUDED
#define INITIALIZATION_H_INCLUDED

#include "headers.h"

int createSocket(string hostName, int portNumber)
{
    int clientSocket = 0 ;
    int opt = 1;
    struct sockaddr_in address;
    if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(clientSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,&opt, sizeof(opt)))
    {
        perror("failure in setsocketopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_port = htons(portNumber);
    if(inet_pton(AF_INET, hostName.c_str(), &address.sin_addr)<=0)
    {
        perror("\nclient address invalid\n");
        exit(EXIT_FAILURE);
    }


    if (bind(clientSocket, (struct sockaddr *)&address,sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    return clientSocket;
}

sockaddr_in setServer(string hostName, int portNumber)
{
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portNumber);

    if(inet_pton(AF_INET,hostName.c_str(), &serv_addr.sin_addr)<=0)
    {
        perror("\nserver address invalid\n");
        exit(EXIT_FAILURE);
    }
    return serv_addr;
}


void connectWithServer(int clientSocket, sockaddr_in serv_addr)
{
    if (connect(clientSocket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("\nconnection failed \n");
        //exit(EXIT_FAILURE);
    }
}

#endif // INITIALIZATION_H_INCLUDED
