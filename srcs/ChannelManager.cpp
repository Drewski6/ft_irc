#include "ChannelManager.hpp"

// CONSTRUCTORS & DESTRUCTOR

ChannelManager::ChannelManager(void) {};

ChannelManager::~ChannelManager(void) {
  // Delete all channels
  for (std::set<Channel *>::iterator it = _channels.begin(); it != _channels.end(); it++) {
    delete *it;
  }
};

// METHODS

void ChannelManager::addChannel(std::string name, Client *client, std::string key_val) {

  if (client == NULL) {
    // No client to add to channel
    return;
  }

  // Check if channel already exists by this name
  for (std::set<Channel *>::iterator it = _channels.begin(); it != _channels.end(); it++) {
    if ((*it)->getName() == name) {
      // name already exists
      return;
    }
  }
  Channel *newChannel = new Channel(name, key_val);
  newChannel->addClient(client);
  _channels.insert(newChannel);
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




// GETTERS

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