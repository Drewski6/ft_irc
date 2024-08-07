#include <Dispatch.hpp>

// Constructors //
Dispatch::Dispatch(std::string password)
: _epollfd(epoll_create1(0)), _serverPassword(password) {
  if (_epollfd == -1) {
    throw ServerException("Error: failed to create epoll instance.");
  }
  // setting signal handler
  setAsSignalHandler();
}

// Destructors //
Dispatch::~Dispatch(void) {};

// Member Functions //
void Dispatch::add(AIO_Event& event) {
  struct epoll_event ev;

  // ev.events = EPOLLIN | EPOLLOUT;
  ev.events = EPOLLIN;// | EPOLLET;
  ev.data.ptr = &event;

  if (epoll_ctl(_epollfd, EPOLL_CTL_ADD, event.getSocket(), &ev) == -1) {
    throw ServerException("Error: failed to add event to epoll instance.");
  }

}

void Dispatch::remove(const AIO_Event& event) {
  if (epoll_ctl(_epollfd, EPOLL_CTL_DEL, event.getSocket(), NULL) == -1) {
    perror("epoll_ctl");
    throw ServerException("Error: failed to remove event from epoll instance.");
  } 
}

void Dispatch::run(void) {
  int nfds = epoll_wait(_epollfd, _events, MAX_EVENTS, -1);
  if (nfds == -1) {
    // catches signals, but not set here. set in signal handlers
  }
  for (int i = 0; i < nfds; ++i) {
    AIO_Event *event = static_cast<AIO_Event *>(_events[i].data.ptr);
    if (_events[i].events & EPOLLIN) {
      event->receive_message();
      if (event->shouldDelete()) {
        delete event;
      }
    }
    if (_events[i].events & EPOLLOUT) {
      event->send_message("");
    }
  }
}

// Getters //

int Dispatch::get_epollfd(void) {
  return _epollfd;
}

int Dispatch::get_sigint_received(void) {
  return _sigint_received;
}

// Signal handler //

bool Dispatch::_sigint_received = false;

void Dispatch::recv_signal(int signal) {
  if (signal == SIGINT) {
    _sigint_received = true;
  }
}

void Dispatch::setAsSignalHandler() {
  if (signal(SIGINT, Dispatch::recv_signal) == SIG_ERR)
  {
    perror("signal");
    return ;
  }            
}


std::string Dispatch::getPassword() const
{
  return (_serverPassword);
}
