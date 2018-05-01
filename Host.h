#ifndef U2F_Host_H_
#define U2F_Host_H_

#include <memory>
#include <string>

namespace U2F
{

class Host
{
public:
  static void GlobalInit();
  static void GlobalDone();

  Host(const std::string user = "localuser");
  ~Host();

  const char * Register(const std::string origin, const char *challenge);
  const char * Authenticate(const std::string origin, const char *challenge);

private:
  void Init();

  struct HostImpl;
  std::unique_ptr<HostImpl> Impl;
};

} // end namespace U2F;

#endif // U2F_Host_H_

