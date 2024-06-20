#include "ServerManager.hpp"

// Constructors , Copy Constructor, Destructor
ServerManager::ServerManager(int portno_val) : _portno(portno_val){
  //std::cout << "Constructor called for ServerManager" << std::endl;
  _setupServSock();
  if( -1 == _listenfd )
      throw ServerManagerException();
};

ServerManager::ServerManager(ServerManager const &source) { *this = source; };

ServerManager::~ServerManager(void){
  //std::cout << "Destructor called for ServerManager" << std::endl;
  _closefds(_clientfds);
  
};

// Overloaded Operators
ServerManager &ServerManager::operator=(ServerManager const &rhs) {
  if (this == &rhs)
    return (*this);
  // Additional code here if you need a deep copy.
  return (*this);
};

int ServerManager::getListenFd()
{
  return (_listenfd);
}

void ServerManager::_setupServSock() {
  // Setup the listening socket
  int sockfd;
  struct sockaddr_in serv_addr;
  int opt = 1;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    throw ServerManagerException("Error: failed to open socket.");
  }
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(_portno);
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));
  if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
    throw ServerManagerException("Error: failed to bind.");
  }
  listen(sockfd, 1024);
  _clientfds.push_back(sockfd);
  _listenfd = sockfd;
}

int ServerManager::_closefds(std::vector<int> &fds) {
  for (std::vector<int>::iterator it = fds.begin(); it != fds.end(); ++it) {
    close(*it);
  }
  return 0;
}

// Run server //

void ServerManager::runServer(int listenfd) {
  fd_set master_fds, read_fds;
  FD_ZERO(&master_fds);
  FD_ZERO(&read_fds);
  FD_SET(listenfd, &master_fds);
  int fdmax = listenfd;

  setAsSignalHandler();

  while (1) {

    read_fds = master_fds;
    if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
      throw ServerManagerException("Error: failed to select.");
      //exit(4);
    }

    // run through the existing connections looking for data to read
    for(int i = 0; i <= fdmax; i++) {
      if (FD_ISSET(i, &read_fds)) {
        if (i == listenfd) {
          // handle new connections
          struct sockaddr_storage remoteaddr; // client address
          socklen_t addrlen = sizeof remoteaddr;
          int newfd = accept(listenfd, (struct sockaddr *)&remoteaddr, &addrlen);
          if (newfd == -1) {
            throw ServerManagerException("Error: failed to accept.");
          } else {
            FD_SET(newfd, &master_fds); // add to master set
            if (newfd > fdmax) {    // keep track of the max
              fdmax = newfd;
            }
            _clientfds.push_back(newfd);
            std::cout << "selectserver: new connection from " << newfd << std::endl;
          }
        } else {
          // handle data from a client
          char buf[256];
          int nbytes = recv(i, buf, sizeof buf, 0);
          if (nbytes <= 0) {
            // got error or connection closed by client
            if (nbytes == 0) {
              // connection closed
              std::cout << "selectserver: socket " << i << " hung up" << std::endl;
            } else {
              throw ServerManagerException("Error: failed to receive.");
            }
            close(i); // close fd of closed connection
            FD_CLR(i, &master_fds); // remove from master set
          } else {
            // we got some data from a client
            buf[nbytes] = '\0';
            std::cout << "Received message (" << i << "): " << buf << std::endl;
            // send a response back to the client
            const char *response = "Your message was received.\n";
            if (send(i, response, strlen(response), 0) == -1) {
              throw ServerManagerException("Error: failed to send.");
            }
          }
        }
      }
    }
  }
}

ServerManager::ServerManagerException::ServerManagerException() 
  : _message("Error: error not defined.") {};
ServerManager::ServerManagerException::ServerManagerException(std::string const &message_val) 
  : _message(message_val) {};
ServerManager::ServerManagerException::~ServerManagerException() throw(){};
const char *ServerManager::ServerManagerException::what() const throw() {
  return (_message.c_str());
}

// Signal handler //

void ServerManager::recv_signal(int signal) {
  if (signal == SIGINT) {}
}

void ServerManager::setAsSignalHandler() {
  if (signal(SIGINT, recv_signal) == SIG_ERR)
  {
    perror("signal");
    return ;
  }            
}