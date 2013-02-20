// C includes
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <iostream>
#include <fstream>
#include <string>

#include "Config.h"
#include "HTTPRequest.h"
#include "HTTPResponse.h"
#include "UsefulFunctions.h"
#include "clientdata.h"

using namespace std;

class Handler {
public:
    Handler(Config c, bool sD);
    ~Handler();
    Config cfg;
    ClientData * cd;
    bool showDebug;

    // Handle a client request, returns true if the server should keep handling
    // requests for this client, otherwise returns false.
    bool handle(int client, ClientData * cd);

    bool doResponse(HTTPResponse resp, int client, bool head = false, int fdOut = 0,
                    int len = 0, string mod = "", string type = "",
                    bool range = false, int start = 0, int end = 0, bool isDir = false, string dirs = "", string CGIpath = "",
                    HTTPRequest req = HTTPRequest());
    bool checkBuffer(int client);
    bool parseBuffer(int client, int found, Client *c);
private:
    char buf_[1600];
};

