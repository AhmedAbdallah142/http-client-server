#ifndef CONTROLLER_H_INCLUDED
#define CONTROLLER_H_INCLUDED

#include<string>
#include "headers.h"
#include <mutex>

#define Max_Time 20
#define files_location "server_files"

int clients = 0;
mutex mtx;

ssize_t recv_for_time(int fd, void* buf, size_t n, int flags)
{
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(fd, &rfds);

    struct timeval tv;
    tv.tv_usec = 0.0;
    tv.tv_sec = max(Max_Time - clients,1);

    //cout << "time " << tv.tv_sec << endl;

    ssize_t data_size;

    if ((data_size = select(fd + 1, &rfds, NULL, NULL, &tv)) > 0)
    {
        data_size = recv( fd, buf, n, flags);
    }
    else
    {
        string response = "HTTP/1.1 408 Request Timeout\r\ncontent-length: 0\r\n\r\n";
        send(fd, &response[0], response.length(), 0);
    }
    return data_size;
}

bool handlePost (int socket,request_header req,char *data,int data_size)
{
    char buffer [BUFFER_SIZE] = {0};
    string absolutePath = files_location + req.path;
    FILE* fd = fopen(&absolutePath[0], "wb");
    fwrite(data,1,data_size,fd);
    req.content_length -= data_size;
    while (req.content_length)
    {
        if ((data_size = recv_for_time(socket, buffer, BUFFER_SIZE-1, 0)) <= 0)
            return false;
        buffer[data_size] = '\0';
        fwrite(&buffer, 1, data_size, fd);
        //cout << buffer;
        req.content_length -= data_size;
    }
    fclose(fd);
    string response = "HTTP/1.1 200 OK\r\ncontent-length: 0\r\n\r\n";
    send(socket, &response[0], response.length(), 0);
    return true;
}

void handleGET(int socket,string path)
{
    string absolutePath = files_location +path;
    char buffer [BUFFER_SIZE] = {0};
    int bytes_read;
    //cout << path << endl;
    string response = "HTTP/1.1 ";
    FILE *fd = fopen(&absolutePath[0], "rb");
    if (fd != NULL)
    {
        fseek(fd, 0, SEEK_END);
        int fsize = ftell(fd);
        fseek(fd, 0, SEEK_SET);
        response += "200 OK\r\ncontent-length: " + to_string(fsize) + "\r\n\r\n";
        send(socket,&response[0], response.length(), 0);
        while ((bytes_read = fread(&buffer, 1, BUFFER_SIZE-1, fd)) > 0)
            send(socket, buffer, bytes_read, 0);
        fclose(fd);
    }
    else
    {
        response += "404 NOT Found\r\ncontent-length: 0\r\n\r\n";
        send(socket,&response[0], response.length(), 0);
    }

}

void communicate(int socket)
{
    char buffer [BUFFER_SIZE] = {0};
    struct request_header req;
    string request;
    size_t data_size,l;
    int start_index;
    while ((data_size = recv_for_time( socket, buffer, BUFFER_SIZE-1, 0))> 0)
    {
        buffer[data_size] = '\0';
        start_index = request.length();
        request += buffer;
        l = request.find("\r\n\r\n");
        if (l != string::npos)
        {
            request = request.substr(0,l);
            cout << endl <<"request: " << request << endl << "------------------------------------" << endl;
            req = parseRequest(request);
            string absolutePath = files_location + req.path;
            if (req.method == "POST")
            {
                start_index = l - start_index + 4;
                if (!handlePost(socket,req,&buffer[start_index],data_size-start_index))
                    break;
                /*FILE* fd = fopen(&absolutePath[0], "wb");
                fwrite(&buffer[start_index],1,data_size - start_index,fd);
                req.content_length -= (data_size - start_index);
                while (req.content_length)
                {
                    if ((data_size = recv_for_time(socket, buffer, BUFFER_SIZE-1, 0)) <= 0)
                        break;
                    buffer[data_size] = '\0';
                    fwrite(&buffer, 1, data_size, fd);
                    req.content_length-= data_size;
                }
                fclose(fd);
                string response = "HTTP/1.1 200 OK\r\ncontent-length: 0\r\n\r\n";
                send(socket, &response[0], response.length(), 0);*/
            }
            else if (req.method == "GET")
            {
                handleGET(socket,req.path);
            }
            request = "";
        }
    }
    mtx.lock();
    clients--;
    mtx.unlock();
    close(socket);
}


#endif // CONTROLLER_H_INCLUDED
