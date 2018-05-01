#ifndef U2F_Server_H_
#define U2F_Server_H_

#include <memory>
#include <string>
#include <utility>

namespace U2F
{

struct KeyInfo
{
  const char *KeyHandle = nullptr;
  const char *PublicKey = nullptr;
};

class Server
{
public:
  using RegistrationResult = void *;
  using AuthenticationResult = void *;

  static void GlobalInit();
  static void GlobalDone();

  Server(const std::string origin = "localhost", const std::string appid = "");
  ~Server();

  const char * RegistrationChallenge(const std::string challenge="");
  RegistrationResult RegistrationVerify(const char *response);
  KeyInfo GetRegistrationInfo(RegistrationResult regResult); 

  const char * AuthenticationChallenge(const KeyInfo key,
    const std::string challenge="");
  AuthenticationResult AuthenticationVerify(const char *response);

private:
  void Init();
  void SetChallenge(const std::string string);

  struct ServerImpl;
  std::unique_ptr<ServerImpl> Impl;
};

} // end namespace U2F;

#endif // U2F_Server_H_
