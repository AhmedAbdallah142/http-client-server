#ifndef CONTROLLER_H_INCLUDED
#define CONTROLLER_H_INCLUDED

#include "headers.h"
#include<string>
#define files_location "client_files"

string absolutePath;

char buffer [BUFFER_SIZE] = {0};

void sendPostRequest(int socket,string path)
{
    FILE *fd = fopen(&absolutePath[0], "rb");
    if (fd == NULL){
        cout << "File :"<< path <<"Not Found" << endl;
        return;
    }
    fseek(fd, 0, SEEK_END);
    int fsize = ftell(fd);
    fseek(fd, 0, SEEK_SET);
    string request = "POST "+ path +" HTTP/1.1\r\ncontent-length: " + to_string(fsize) + "\r\n\r\n";
    send(socket, &request[0], request.length(),0 );
    int bytes_read;
    while ((bytes_read = fread(&buffer, 1, BUFFER_SIZE, fd)) > 0)
    {
        send(socket, buffer, bytes_read, 0);
    }
    fclose(fd);
}

void recieveResponse(int socket, string path)
{
    int start_index = 0 ;
    string response;
    size_t datasize,l;
    while ((datasize = recv( socket, buffer, BUFFER_SIZE, 0))> 0)
    {
        buffer[datasize] = '\0';
        //start_index = response.length();
        response += buffer;
        l = response.find("\r\n\r\n");
        if (l != string::npos)
        {
            response = response.substr(0,l);
            cout << endl <<"response: " << response << endl << "------------------------------------" << endl;
            int content_length = handleResponse(response);
            response = "";
            if (content_length)
            {
                start_index = l - start_index + 4;
                FILE* fd = fopen(&absolutePath[0], "wb");
                fwrite(&buffer[start_index],1,datasize - start_index,fd);
                content_length -= (datasize - start_index);
                while (content_length){
                    if ((datasize = recv(socket, buffer, BUFFER_SIZE-1, 0)) > 0)
                    {
                        buffer[datasize] = '\0';
                        fwrite(&buffer, 1, datasize, fd);
                        content_length -= datasize;
                    }
                }
                /*while ((datasize = recv(socket, buffer, BUFFER_SIZE-1, 0)) > 0)
                {
                    buffer[datasize] = '\0';
                    //cout << buffer;
                    if ((buffer[datasize-2] == '\r')&&(buffer[datasize-1] == '\r'))
                    {
                        fwrite(&buffer, 1, datasize - 2, fd);
                        break;
                    }
                    fwrite(&buffer, 1, datasize, fd);
                }*/
                fclose(fd);
            }
            break;
        }
        start_index+=datasize;
    }
}


void handleCommand(int socket,string method, string filePath)
{
    absolutePath = files_location + filePath;
    string request;
    if (method == "client_get")
    {
        request = "GET "+filePath +" HTTP/1.1\r\n"+ "content-length: 0\r\n" + "\r\n";
        send(socket,&request[0],request.length(),0);
        recieveResponse(socket,filePath);
    }
    else if(method == "client_post")
    {
        sendPostRequest(socket,filePath);
        recieveResponse(socket, "");
    }
}

#endif // CONTROLLER_H_INCLUDED
