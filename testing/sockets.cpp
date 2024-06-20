#include "sockets.hpp"

/****************************/
/*        CONSTRUCTORS      */
/****************************/

serverManagerClass::serverManagerClass()
{
    _listenfd = setup_serv_sock();
    if( _listenfd == -1)
        exit(1);
        // throw(listeningException());
    
    _sigtype = SIGEMPTY;

    _instance = this;
}

serverManagerClass::~serverManagerClass()
{

}

// const char * serverManagerClass::listeningException::what() const throw()
// {
//     return ("Listening error.");
// }

int serverManagerClass::getListenFd() const
{
    return(_listenfd);
}

/****************************/
/*          METHODS         */
/****************************/

int serverManagerClass::setup_serv_sock() {
    // Setup the listening socket
    int sockfd, portno;
    struct sockaddr_in serv_addr;
    int opt = 1;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "Error opening socket" << std::endl;
        exit(1);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = 5001;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Error on binding" << std::endl;
        exit(1);
    }
    listen(sockfd, 5);
    return sockfd;
}



void serverManagerClass::launch()
{
    fd_set master_fds, read_fds;
    FD_ZERO(&master_fds);
    FD_ZERO(&read_fds);
    FD_SET(_listenfd, &master_fds);
    _fdmax = _listenfd;

    this->setAsSignalHandler();


    while (1) {

        read_fds = master_fds; // copy it
        if (select(_fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(4);
        }

        // run through the existing connections looking for data to read
        for(int i = 0; i <= _fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) { // we got one!!
                if (i == _listenfd) {
                    // handle new connections
                    struct sockaddr_storage remoteaddr; // client address
                    socklen_t addrlen = sizeof remoteaddr;
                    int newfd = accept(_listenfd, (struct sockaddr *)&remoteaddr, &addrlen);

                    if (newfd == -1) {
                        perror("accept");
                    } else {
                        FD_SET(newfd, &master_fds); // add to master set
                        if (newfd > _fdmax) {    // keep track of the max
                            _fdmax = newfd;
                        }
                        printf("selectserver: new connection on socket %d\n", newfd);
                    }
                } else {
                    // handle data from a client
                    char buf[256];
                    int nbytes = recv(i, buf, sizeof buf, 0);
                    if (nbytes <= 0) {
                        // got error or connection closed by client
                        if (nbytes == 0) {
                            // connection closed
                            printf("selectserver: socket %d hung up\n", i);
                        } else {
                            perror("recv");
                        }
                        close(i); // bye!
                        FD_CLR(i, &master_fds); // remove from master set
                    } else {
                        // we got some data from a client
                        buf[nbytes] = '\0';
                        std::cout << "Received message (" << i << "): " << buf << std::endl;
                        // send a response back to the client
                        const char *response = "Your message was received.\n";
                        if (send(i, response, strlen(response), 0) == -1) {
                            perror("send");
                        }
                    }
                } // END handle data from client
            } // END got new incoming connection
        } // END looping through file descriptors
    } // END for(;;)--and you thought it would never end!


}

int main () {

    serverManagerClass serverManager;

    serverManager.launch();







    return (0);
}