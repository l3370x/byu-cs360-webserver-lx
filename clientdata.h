#ifndef CLIENTDATA_H
#define CLIENTDATA_H

#include <vector>
#include <iostream>

#include "client.h"

using namespace std;

class ClientData
{
public:
    ClientData(bool showDebug);
    void add(int c);
    void remove(int c);
    int timeout(int t);
    bool showDebug;
    vector<Client> clients;
    void zeroTimeout(int c);
    void recv(int c, string buf);


    Client *getClient(int c);
};

#endif // CLIENTDATA_H
