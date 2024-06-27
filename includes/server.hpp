#pragma once

#include <netinet/in.h>
#include <vector>
#include <cstdio>

#include "ServerException.hpp"
#include "io_event.hpp"
#include "client.hpp"
#include "Dispatch.hpp"

class server : public io_event
{
	public:
		//CONSTRUCTORS & DESTRUCTOR
		server(void);
		server(dispatch& d);
		~server(void);

		//METHODS
		void send_message(void) {}
		void receive_message(void);
		int socket_func(void) const;

		//GETTERS
		std::string getType(void);


	private:
		// shared_socket
		int _socket;
		std::string _type;
		struct sockaddr_in _addr;
		socklen_t _client_addr_len;
		dispatch& _d;
		std::vector<client *> _clients;

};