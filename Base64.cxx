#include "Base64.h"

#include <cassert>

constexpr int CHARS_PER_LINE = 72;

namespace U2F
{

enum class Base64EncodeStep
{
  StepA, StepB, StepC
};


struct Base64Encoder::Base64EncoderImpl
{
  Base64EncodeStep Step = Base64EncodeStep::StepA;
  char Result = 0;
  int StepCount = 0;
};


Base64Encoder::Base64Encoder()
: Impl(new Base64EncoderImpl)
{
}


Base64Encoder::~Base64Encoder()
{
}


char Base64Encoder::EncodeValue(char value_in)
{
  static const char *encoding =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
  if (value_in > 63)
    return '=';
  return encoding[(int) value_in];
}


int Base64Encoder::EncodeBlock(const char* input, int len, char* out)
{
  const char *plainchar = input;
  const char *const plaintextend = input + len;
  char *codechar = out;
  char result;
  char fragment;

  result = this->Impl->Result;

  switch(this->Impl->Step)
  {
    while(1)
    {
      case Base64EncodeStep::StepA:
        if(plainchar == plaintextend)
        {
          this->Impl->Result = result;
          this->Impl->Step = Base64EncodeStep::StepA;
          return codechar - out;
        }

        fragment = *plainchar++;
        result = (fragment & 0x0fc) >> 2;
        *codechar++ = this->EncodeValue(result);
        result = (fragment & 0x003) << 4;

      case Base64EncodeStep::StepB:
        if(plainchar == plaintextend)
        {
          this->Impl->Result = result;
          this->Impl->Step = Base64EncodeStep::StepB;
          return codechar - out;
        }

        fragment = *plainchar++;
        result |= (fragment & 0x0f0) >> 4;
        *codechar++ = this->EncodeValue(result);
        result = (fragment & 0x00f) << 2;

      case Base64EncodeStep::StepC:
        if(plainchar == plaintextend)
        {
          this->Impl->Result = result;
          this->Impl->Step = Base64EncodeStep::StepC;
          return codechar - out;
        }

        fragment = *plainchar++;
        result |= (fragment & 0x0c0) >> 6;
        *codechar++ = this->EncodeValue(result);
        result = (fragment & 0x03f) >> 0;
        *codechar++ = this->EncodeValue(result);

        ++(this->Impl->StepCount);
        if (this->Impl->StepCount == CHARS_PER_LINE / 4)
        {
          // *codechar++ = '\n';
          this->Impl->StepCount = 0;
        }
    } // while
  } // switch

  assert(false);
  return codechar - out;
}


int Base64Encoder::EncodeBlockEnd(char* out)
{
  char *codechar = out;

  switch(this->Impl->Step)
  {
    case Base64EncodeStep::StepB:
      *codechar++ = this->EncodeValue(this->Impl->Result);
      *codechar++ = '\0';
      *codechar++ = '\0';
      break;
    case Base64EncodeStep::StepC:
      *codechar++ = this->EncodeValue(this->Impl->Result);
      *codechar++ = '\0';
      break;
    case Base64EncodeStep::StepA:
      break;
  }
  *codechar++ = '\0';

  return codechar - out;
}

}
