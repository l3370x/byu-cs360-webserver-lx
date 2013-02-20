#include "clientdata.h"

ClientData::ClientData(bool showDebug)
{
    this->showDebug = showDebug;
}

void ClientData::add(int c)
{
    Client toAdd;
    toAdd.fd = c;
    toAdd.buffer = "";
    toAdd.idle = time(NULL);
    clients.push_back(toAdd);
}

void ClientData::remove(int fd)
{
    int pos = 0;
    vector<Client>::iterator it;
    for ( it=clients.begin() ; it < clients.end(); it++ ){
        Client c = *it;
        if(c.fd == fd){
            clients.erase(clients.begin() + pos);
            break;
        }
        pos++;
    }

}

int ClientData::timeout(int t)
{
    for ( int j = 0 ; j < clients.size() ; j++){
        time_t currentTime;
        currentTime = time(NULL);
        int idle = currentTime - clients[j].idle;
        if(showDebug)
            cout << "client " << clients[j].fd << " has been idle for "
                 << idle << " seconds." << endl;
        if(idle > t){
            return clients[j].fd;
        }
    }
    return 0;
}

void ClientData::zeroTimeout(int c)
{
    for ( int j = 0 ; j < clients.size() ; j++){
        if(clients[j].fd == c){
            time_t currentTime;
            currentTime = time(NULL);
            clients[j].idle = currentTime;
        }
    }
}

void ClientData::recv(int c, string buf)
{
    for ( int j = 0 ; j < clients.size() ; j++){
        if(clients[j].fd == c){
            clients[j].buffer += buf;

        }

    }
}

Client *ClientData::getClient(int c){
    for ( int j = 0 ; j < clients.size() ; j++){
        if(clients[j].fd == c){
            return &clients[j];
        }
    }
    return NULL;
}
