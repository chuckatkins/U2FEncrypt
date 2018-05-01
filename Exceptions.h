#ifndef U2F_Exceptions_H_
#define U2F_Exceptions_H_

#include <stdexcept>

#include <u2f-server.h>
#include <u2f-host.h>

namespace U2F
{

class ServerError : public std::runtime_error
{
public:
  ServerError(u2fs_rc code);
};

class HostError : public std::runtime_error
{
public:
  HostError(u2fh_rc code);
};

}

#endif // U2F_Exceptions_H_
