#pragma once

#include <set>

#include "Channel.hpp"
#include "Client.hpp"

class Channel;
class Client;

class ChannelManager {
  public:
    ChannelManager(void);
    ~ChannelManager(void);

    void addChannel(std::string name, Client *client = NULL, std::string key_val = "");
    void removeChannel(std::string name);

    // Getters //
    size_t getNumChannels(void);
    size_t getNumClientsInChannel(std::string name);

  private:
    std::set<Channel *> _channels;

};