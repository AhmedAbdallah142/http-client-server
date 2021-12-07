#ifndef HEADERS_H_INCLUDED
#define HEADERS_H_INCLUDED


#include <iostream>
#include <unistd.h>
#include <fstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sstream>
#include <arpa/inet.h>
#include <cstring>


#define DEFAULT_PORT 80;
#define BUFFER_SIZE 1024



using namespace std;

struct command_attributes
{
    string method,filePath,hostName;
    int portNumber;
};

command_attributes getCommand(string command)
{
    struct command_attributes attr;
    istringstream separatedCommand (command);
    separatedCommand >> attr.method;
    separatedCommand >> attr.filePath;
    separatedCommand >> attr.hostName;
    separatedCommand >> attr.portNumber;
    if (!separatedCommand)
        attr.portNumber = DEFAULT_PORT;
    return attr;
}

int handleResponse(string response){
    string temp;
    size_t fsize = response.find("content-length:");
    if (fsize == string::npos)
        return 0;
    istringstream ss (&response[fsize+16]);
    ss >> fsize;
    return fsize;
}

#endif // HEADERS_H_INCLUDED
