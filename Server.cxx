#include "Server.h"

#include <cstring>
#include <cstdlib>

#include <sstream>
#include <vector>

#include "Base64.h"
#include "Exceptions.h"

#include <u2f-server.h>

namespace
{

// This is a helper function to perform base64 encoding and is taken
// directly from the libu2f-server library and ported to use the C++ Base64
// wrapper
static u2fs_rc encode_b64u(const char *data, size_t data_len, char *output)
{
  constexpr int _B64_BUFSIZE = 2048;
  U2F::Base64Encoder b64;

  //base64 is 75% efficient (4 characters encode 3 bytes)
  if ((data_len * 4) >= (_B64_BUFSIZE * 3) || output == NULL)
    return U2FS_MEMORY_ERROR;

  int cnt;
  cnt = b64.EncodeBlock(data, data_len, output);
  cnt += b64.EncodeBlockEnd(output + cnt);

  output[cnt] = '\0';

  return U2FS_OK;
}

}

namespace U2F
{

#define CHECK_U2FS_ERROR(rc) if(rc != U2FS_OK) { throw ServerError(rc); }

struct Server::ServerImpl
{
  ServerImpl()
  : Context(nullptr)
  {
  }

  u2fs_ctx_t *Context;
  std::string Origin;
  std::string AppId;
  std::vector<u2fs_reg_res_t*> Registrations;
  std::vector<u2fs_auth_res_t*> Authentications;
};

void Server::GlobalInit()
{
  bool u2fDebug = false;

  if(const char* env_U2F_DEBUG = std::getenv("U2F_DEBUG"))
  {
    size_t len = std::strlen(env_U2F_DEBUG);
    if(len > 0 && !(len == 1 && env_U2F_DEBUG[0] == '0'))
    {
      u2fDebug = true;
    }
  }

  u2fs_rc ret;
  ret = u2fs_global_init(u2fDebug ? U2FS_DEBUG : u2fs_initflags(0));
  CHECK_U2FS_ERROR(ret)
}


void Server::GlobalDone()
{
  u2fs_global_done();
}


Server::Server(const std::string origin, const std::string appid)
: Impl(new Server::ServerImpl)
{
  this->Impl->Origin = origin.empty() ? "localhost" : origin;
  this->Impl->AppId = appid.empty() ? this->Impl->Origin : appid;

  u2fs_rc ret;

  ret = u2fs_init(&this->Impl->Context);
  CHECK_U2FS_ERROR(ret)

  ret = u2fs_set_origin(this->Impl->Context, this->Impl->Origin.c_str());
  CHECK_U2FS_ERROR(ret)

  ret = u2fs_set_appid(this->Impl->Context, this->Impl->AppId.c_str());
  CHECK_U2FS_ERROR(ret)
}


Server::~Server()
{
  for(auto reg : this->Impl->Registrations)
  {
    u2fs_free_reg_res(reg);
  }
  for(auto auth : this->Impl->Authentications)
  {
    u2fs_free_auth_res(auth);
  }
  u2fs_done(this->Impl->Context);
}


void Server::SetChallenge(const std::string challenge)
{
  u2fs_rc ret;
  char rawChallenge[U2FS_CHALLENGE_RAW_LEN];
  std::memset(rawChallenge, 0, U2FS_CHALLENGE_RAW_LEN);
  std::memcpy(rawChallenge, challenge.data(),
    challenge.size() > U2FS_CHALLENGE_RAW_LEN ?
      U2FS_CHALLENGE_RAW_LEN : challenge.size());

  char b64Challenge[U2FS_CHALLENGE_B64U_LEN];
  ret = encode_b64u(rawChallenge, U2FS_CHALLENGE_RAW_LEN, b64Challenge); 
  CHECK_U2FS_ERROR(ret);

  ret = u2fs_set_challenge(this->Impl->Context, b64Challenge);
  CHECK_U2FS_ERROR(ret);
}

const char * Server::RegistrationChallenge(const std::string challenge)
{
  u2fs_rc ret;
  if(!challenge.empty())
  {
    this->SetChallenge(challenge);
  }

  char *regChallenge;
  ret = u2fs_registration_challenge(this->Impl->Context, &regChallenge);
  CHECK_U2FS_ERROR(ret)

  return regChallenge;
}


Server::RegistrationResult Server::RegistrationVerify(const char *response)
{
  u2fs_rc ret;
  u2fs_reg_res_t *regResult;

  ret = u2fs_registration_verify(this->Impl->Context, response, &regResult);
  CHECK_U2FS_ERROR(ret)

  this->Impl->Registrations.push_back(regResult);
  return regResult;
}


KeyInfo Server::GetRegistrationInfo(Server::RegistrationResult regResultHandle)
{
  u2fs_rc ret;
  u2fs_reg_res_t *regResult =
    reinterpret_cast<u2fs_reg_res_t*>(regResultHandle);

  KeyInfo info;
  info.KeyHandle = u2fs_get_registration_keyHandle(regResult);
  info.PublicKey = u2fs_get_registration_publicKey(regResult);

  return info;
}


const char * Server::AuthenticationChallenge(const KeyInfo key,
  const std::string challenge)
{
  u2fs_rc ret;
  if(!challenge.empty())
  {
    this->SetChallenge(challenge);
  }

  ret = u2fs_set_keyHandle(this->Impl->Context, key.KeyHandle);
  CHECK_U2FS_ERROR(ret)

  ret = u2fs_set_publicKey(this->Impl->Context,
    reinterpret_cast<const unsigned char *>(key.PublicKey));
  CHECK_U2FS_ERROR(ret)

  char *authChallenge;
  ret = u2fs_authentication_challenge(this->Impl->Context, &authChallenge);
  CHECK_U2FS_ERROR(ret)

  return authChallenge;
}

Server::AuthenticationResult Server::AuthenticationVerify(const char *response)
{
  u2fs_rc ret;
  u2fs_auth_res_t *authResult;

  ret = u2fs_authentication_verify(this->Impl->Context, response, &authResult);
  CHECK_U2FS_ERROR(ret)

  this->Impl->Authentications.push_back(authResult);
  return authResult;
}

} // end namespace U2F
