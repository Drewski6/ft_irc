#include <iostream>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <vector>

#define SIGEMPTY 0

class serverManagerClass {

	public:
		serverManagerClass();
		~serverManagerClass();

		int setup_serv_sock();

		int getListenFd() const;

		void launch();

		static void	recv_signal(int signal)
		{
			if (signal == SIGINT)
			{
				std::cout << "YOOOOOOOOOOOOOOOOOOO" << std::endl;

				for (int i = 0; i <= _instance->_fdmax; ++i)
				{
					if (fcntl(i, F_GETFD) != 0)
						close(i);
				}
				close(_instance->_listenfd);
			}

		}

		void setAsSignalHandler()
		{
			if (signal(SIGINT, recv_signal) == SIG_ERR)
			{
				perror("signal");
				return ;
			}            
		}

		// class listeningException: public std::exception
		// {
		//     public:
		//         virtual const char* what() const throw();

		// };

	private:
		int _signal;
		int _listenfd;
		int _fdmax;
		int _sigtype;

		static serverManagerClass * _instance;

};
