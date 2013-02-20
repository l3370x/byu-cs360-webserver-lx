// C includes
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>

// C++ includes
#include <iostream>

// local includes
#include "handler.h"
#include "Config.h"
#include "clientdata.h"

#define MAX_EVENTS 1000

using namespace std;

int
main(int argc, char **argv)
{
    struct sockaddr_in server,client;
    socklen_t clientlen = sizeof(client);
    int option, port, s, c;
    int opt;
    bool showDebug;
    int timeout;

    // setup default arguments
    port = 8080;
    showDebug = false;

    // process command line options using getopt()
    // see "man 3 getopt"
    while ((option = getopt(argc,argv,"p:d")) != -1) {
        switch (option) {
        case 'p':
            port = atoi(optarg);
            break;
        case 'd':
            showDebug = true;
            break;
        default:
            cout << "server [-p port] [-d showDebug]" << endl;
            exit(EXIT_FAILURE);
        }
    }


    //parse config file
    Config cfg;
    cfg.parse("web.conf");

    //set up handler
    Handler h(cfg,showDebug);

    //set timeout parameter
    timeout = atoi(cfg.parameter("timeout").c_str());
    if(showDebug)
        cout << "the server's timeout is set to " << timeout << endl;

    // setup socket address structure
    memset(&server,0,sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = INADDR_ANY;

    // create socket
    s = socket(PF_INET,SOCK_STREAM,0);
    if (!s) {
        perror("socket");
        exit(-1);
    }

    // set socket to immediately reuse port when the application closes
    opt = 1;
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        exit(-1);
    }

    // call bind to associate the socket with our local address and
    // port
    if (bind(s,(const struct sockaddr *)&server,sizeof(server)) < 0) {
        perror("bind");
        exit(-1);
    }

    // convert the socket to listen for incoming connections
    if (listen(s,SOMAXCONN) < 0) {
        perror("listen");
        exit(-1);
    }

    int epfd = epoll_create(1);

    // add listening socket to poller
    static struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = s;
    epoll_ctl(epfd, EPOLL_CTL_ADD, s, &ev);

    //add clientData holder;
    ClientData cd(showDebug);

    while (1) {
        // do poll
        //loop through all clients and do timeout
        int to = cd.timeout(timeout);
        while(to){
            if(showDebug)
                cout << "removing client: " << to << endl;
            ev.events = EPOLLIN;
            ev.data.fd = to;
            epoll_ctl(epfd, EPOLL_CTL_DEL, to, &ev);
            close(to);
            cd.remove(to);
            to = cd.timeout(timeout);
        }
        struct epoll_event events[MAX_EVENTS];
        int nfds = epoll_wait(epfd, events, MAX_EVENTS, 1000);
        if(showDebug)
            cout << nfds << " events happened!" << endl;
        if (nfds < 0) {
            perror("epoll");
            exit(EXIT_FAILURE);
        }
        if (nfds == 0){
            continue;
        }
        
        // handle sockets that are ready
        for(int i = 0; i < nfds; i++) {

            int fd = events[i].data.fd;
            if (fd == s) {
                c = accept(s,(struct sockaddr *)&client,&clientlen);
                if (c < 0) {
                    perror("accept");
                    break;
                }
                if(showDebug)
                    cout << "added client: " << c << " to the poller." << endl;
                // add new client to poller
                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = c;
                epoll_ctl(epfd, EPOLL_CTL_ADD, c, &ev);

                //Add to clients to keep track of buffer and timeout
                cd.add(c);
            } else {
                // handle client
                if(showDebug)
                    cout << "handling client: " << fd << endl;
                //zero timeout for client
                cd.zeroTimeout(fd);

                bool result = h.handle(fd,&cd);
                if (!result) {
                    // socket closed, so remove it from poller
                    if(showDebug)
                        cout << "removing client: " << fd << endl;
                    ev.events = EPOLLIN;
                    ev.data.fd = fd;
                    epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &ev);
                    close(fd);

                    //remove from clients
                    cd.remove(fd);
                }
            }
        }
    }
    close(s);
}
