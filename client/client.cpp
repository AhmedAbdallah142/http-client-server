#include "initialization.cpp"
#include "controller.cpp"


int main(int argc, const char * argv[])
{
    if (argc != 3)
    {
        cout << "client IP or port number not initialized" << endl;
    }
    else
    {
        int clientSocket = createSocket(argv[1],atoi(argv[2]));
        struct sockaddr_in serv_addr;
        struct command_attributes attr;
        string command;
        /*cout << "Enter file Path : ";
        cin >> command;*/
        ifstream fileReader("commands.txt");
        string hostName = "";
        int portNumber = 0;
        while (getline (fileReader, command))
        {
            attr = getCommand(command);
            cout << attr.method << " " << attr.filePath << " " << attr.hostName << " " << attr.portNumber << endl;
            if ((hostName != attr.hostName)||(portNumber != attr.portNumber)){
                //cout << "Changed" << endl;
                close(clientSocket);
                clientSocket = createSocket(argv[1],atoi(argv[2]));
                serv_addr = setServer(attr.hostName,attr.portNumber);
                connectWithServer(clientSocket,serv_addr);
                hostName = attr.hostName;
                portNumber = attr.portNumber;
            }
            handleCommand(clientSocket,attr.method,attr.filePath);
        }
        fileReader.close();
    }
    return 0;
}
