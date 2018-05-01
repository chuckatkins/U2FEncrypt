//
// Base64 algorithm
//
// This is a C++ port if the libb64 project.
// For details, see http://sourceforge.net/projects/libb64
//
#ifndef Base64_H_
#define Base64_H_

#include <memory>

namespace U2F
{

class Base64Encoder
{
public:
  Base64Encoder();
  ~Base64Encoder();

  char EncodeValue(char value_in);
  int EncodeBlock(const char* plaintext_in, int length_in, char* code_out);
  int EncodeBlockEnd(char* code_out);

private:
  struct Base64EncoderImpl;
  std::unique_ptr<Base64EncoderImpl> Impl;
};

}

#endif // Base64_H_

