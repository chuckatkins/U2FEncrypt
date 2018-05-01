#include <cstring>
#include <cstdlib>

#include <iostream>
#include <chrono>
#include <thread>

#include "Host.h"
#include "Exceptions.h"

#include <u2f-host.h>

namespace U2F
{

#define CHECK_U2FH_ERROR(rc) if(rc != U2FH_OK) { throw HostError(rc); }

struct Host::HostImpl
{
  HostImpl()
  {
  }

  unsigned int MaxDevices;
  u2fh_devs *Devices;
};

void Host::GlobalInit()
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

  u2fh_rc ret;
  ret = u2fh_global_init(u2fDebug ? U2FH_DEBUG : u2fh_initflags(0));
  CHECK_U2FH_ERROR(ret)
}


void Host::GlobalDone()
{
  u2fh_global_done();
}


Host::Host(const std::string user)
: Impl(new Host::HostImpl)
{
  u2fh_rc ret;

  ret = u2fh_devs_init(&this->Impl->Devices);
  CHECK_U2FH_ERROR(ret)

  ret = u2fh_devs_discover(this->Impl->Devices, &this->Impl->MaxDevices);
  if(ret == U2FH_NO_U2F_DEVICE)
  {
    std::cerr << "No U2F device available, please insert one now..."
      << std::endl;
    do
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      ret = u2fh_devs_discover(this->Impl->Devices, &this->Impl->MaxDevices);
    }
    while(ret == U2FH_NO_U2F_DEVICE);
  }
  CHECK_U2FH_ERROR(ret)
}


Host::~Host()
{
  u2fh_devs_done(this->Impl->Devices);
}


const char * Host::Register(const std::string origin, const char *challenge)
{
  u2fh_rc ret;

  char *responsePtr = nullptr;
  ret = u2fh_register(this->Impl->Devices, challenge,
    origin.c_str(), &responsePtr, U2FH_REQUEST_USER_PRESENCE);

  return responsePtr;
}


const char * Host::Authenticate(const std::string origin,
  const char *challenge)
{
  u2fh_rc ret;

  char *responsePtr = nullptr;
  ret = u2fh_authenticate(this->Impl->Devices, challenge,
    origin.c_str(), &responsePtr, U2FH_REQUEST_USER_PRESENCE);

  return responsePtr;
}

} // end namespace U2F
