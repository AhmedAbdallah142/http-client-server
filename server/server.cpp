#include "controller.cpp"
#include <thread>


/*
* create a socket to the server 
* then bind this socket to specified ip and portnumber
* listen to clients 
* if client accepted open a new thread to serve the client
*/
int main(int argc, const char * argv[])
{
    if (argc != 2)
    {
        cout << "port number not defined" << endl;
    }
    else
    {
        int serverPort = atoi(argv[1]);

        cout << "server port: " << serverPort << endl;

        int server_fd, new_socket;
        struct sockaddr_in address;
        int opt = 1;
        int addrlen = sizeof(address);


        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
        {
            perror("socket failed");
            exit(EXIT_FAILURE);
        }

        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,&opt, sizeof(opt)))
        {
            perror("failure in setsocketopt");
            exit(EXIT_FAILURE);
        }

        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons( serverPort );

        if (bind(server_fd, (struct sockaddr *)&address,sizeof(address)) < 0)
        {
            perror("bind failed");
            exit(EXIT_FAILURE);
        }
        while (1)
        {
            if (listen(server_fd, BACKLOG) < 0)
            {
                perror("listen failed");
                continue;
            }

            if ((new_socket = accept(server_fd, (struct sockaddr *)&address,(socklen_t*)&addrlen))<0)
            {
                perror("accept failed");
                continue;
            }
            mtx.lock();
            clients++;
            mtx.unlock();
            new thread (communicate, new_socket);
        }
    }
    return 0;
}
