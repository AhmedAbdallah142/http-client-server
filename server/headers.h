#ifndef HEADERS_H_INCLUDED
#define HEADERS_H_INCLUDED

#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <sstream>
#include <fstream>

#define BACKLOG 10
#define BUFFER_SIZE 1024


using namespace std;


struct request_header
{
    string method,path;
    int content_length = 0;
};

request_header parseRequest(string request)
{
    string temp;
    struct request_header req ;
    istringstream separatedheader (request);
    separatedheader >> req.method;
    separatedheader >> req.path;
    size_t pos = request.find("content-length:");
    if (pos != string::npos)
    {
        separatedheader.seekg(pos+16);
        separatedheader >> req.content_length;
    }
    return req;
}

#endif // HEADERS_H_INCLUDED
