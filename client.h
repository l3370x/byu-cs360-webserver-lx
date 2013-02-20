#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <time.h>

using namespace std;

class Client
{
public:
    Client();
    int fd;
    string buffer;
    time_t idle;
};

#endif // CLIENT_H
