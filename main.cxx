#include <iostream>
#include <sstream>

#include "Server.h"
#include "Host.h"

int main(int argc, char **argv)
{
  std::string origin, appid, user;
  if(argc > 1)
  {
    origin = argv[1];
  }
  else
  {
    origin = "localhost";
  }
  if(argc > 2)
  {
    appid = argv[2];
  }
  else
  {
    appid = origin;
  }
  if(argc > 3)
  {
    user = argv[3];
  }
  else
  {
    user = "localuser";
  }

  U2F::Server::GlobalInit();
  U2F::Host::GlobalInit();

  U2F::Server server(origin, appid);
  U2F::Host host(user);


  const auto regChallenge = server.RegistrationChallenge();
  std::cout << "Registration Challenge:  " << regChallenge << std::endl;

  const auto regResponse = host.Register(origin, regChallenge);
  std::cout << "Registration Response:   " << regResponse << std::endl;

  const auto reg = server.RegistrationVerify(regResponse);
  auto key = server.GetRegistrationInfo(reg);
  std::cout << "Key Handle: " << key.KeyHandle << std::endl;

  for(size_t i = 0; i < 10; ++i)
  {
    const auto authChallenge = server.AuthenticationChallenge(key, "Hello");
    const auto authResponse = host.Authenticate(origin, authChallenge);
    const auto auth = server.AuthenticationVerify(authResponse);
    if(!auth)
    {
      std::cerr << "Authentication failed" << std::endl;
    }
    std::cout << "Authentication Challenge: " << authChallenge << std::endl;
    std::cout << "Authentication Response:  " << authResponse << std::endl;
  }

  U2F::Host::GlobalDone();
  U2F::Server::GlobalDone();

  return 0;
}
