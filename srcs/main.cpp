#include <iostream>

#include "Dispatch.hpp"
#include "Server.hpp"
#include "ArgParse.hpp"

int main(int argc, char **argv) {

  int port = 0;
  std::string passStr;

  try {
    ArgParse::numArgs(argc);
    port = ArgParse::parsePort(argv[1]);
    passStr = ArgParse::parsePass(argv[2]);
  } catch (const ServerException& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  // DEBUG: show port and password
  std::cout << "Port: " << port << std::endl;
  std::cout << "Password: " << passStr << std::endl;

  Dispatch d;
  Server s(d, port);

  d.add(s);

  while (!d.get_sigint_received()) {
    try {
      d.run();
    } catch (const ServerException& e) {
      std::cerr << e.what() << std::endl;
    }
  }

  return 0;
}