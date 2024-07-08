#include "ChannelManager.hpp"

// CONSTRUCTORS & DESTRUCTOR

ChannelManager::ChannelManager(void) 
  : _maxChannelsForClient(10) {};

ChannelManager::~ChannelManager(void) {
  // Delete all channels
  for (std::set<Channel *>::iterator it = _channels.begin(); it != _channels.end(); it++) {
    delete *it;
  }
};

// METHODS

Channel *ChannelManager::addChannel(std::string name, Client *client) {

  if (client == NULL) {
    // No client to add to channel
    throw ServerException("No client to add to channel");
  }

  // Check if channel already exists by this name
  for (std::set<Channel *>::iterator it = _channels.begin(); it != _channels.end(); it++) {
    if ((*it)->getName() == name) {
      // name already exists
      throw ServerException("Channel already exists");
    }
  }
  Channel *newChannel = new Channel(name);
  if (newChannel == NULL) {
    // Error creating channel
    throw ServerException("Error creating channel");
  }
  newChannel->addClient(client);
  _channels.insert(newChannel);
  return newChannel;
};

void ChannelManager::removeChannel(std::string name) {
  // Look for channel by name
  for (std::set<Channel *>::iterator it = _channels.begin(); it != _channels.end(); it++) {
    if ((*it)->getName() == name) {
      delete *it;
      return;
    }
  }
};

// GETTERS //

size_t ChannelManager::getNumChannels(void) {
  return _channels.size();
};

size_t ChannelManager::getNumClientsInChannel(std::string name) {
  // Look for channel by name
  for (std::set<Channel *>::iterator it = _channels.begin(); it != _channels.end(); it++) {
    if ((*it)->getName() == name) {
      return (*it)->getChanSize();
    }
  }
  return 0;
};

Channel *ChannelManager::getChannel(std::string name) {
  // Look for channel by name
  for (std::set<Channel *>::iterator it = _channels.begin(); it != _channels.end(); it++) {
    if ((*it)->getName() == name) {
      return *it;
    }
  }
  return NULL;
};

// returns the number of channels the given client is in
int ChannelManager::getClientChannelCount(Client *client) {
  int count = 0;

  if (client == NULL) {
    return count;
  }

  for (std::set<Channel *>::iterator it = _channels.begin(); it != _channels.end(); it++) {
    std::map<Client *, bool> &clients = (*it)->getClients();
    for (std::map<Client *, bool>::iterator it2 = clients.begin(); it2 != clients.end(); it2++) {
      if (it2->first == client) {
        count++;
      }
    }
  }
  return count;
}

int ChannelManager::getMaxChannelsForClient(void) {
  return _maxChannelsForClient;
};