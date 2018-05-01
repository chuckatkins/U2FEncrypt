#include "Exceptions.h"

namespace U2F
{

ServerError::ServerError(u2fs_rc code)
: std::runtime_error(u2fs_strerror(code))
{
}

HostError::HostError(u2fh_rc code)
: std::runtime_error(u2fh_strerror(code))
{
}

}

