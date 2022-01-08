#ifndef __ERROR_CODES_H
#define __ERROR_CODES_H

namespace thingnet
{

  const int RESULT_OK = 0x00;
  const int RESULT_DUPLICATE = 0x01;
  const int RESULT_NO_EXIST = 0x02;

  const int RESULT_SUCCESS_BOUNDARY = 0x0F;

  const int ERR_NODE_NOT_INITIALIZED = 0x10;
  const int ERR_HANDLER_LIMIT_EXCEEDED = 0x12;
  const int ERR_NODE_PROFILE_NOT_SET = 0x12;
  const int ERR_PEER_REGISTRATION_FAILED = 0x13;
  const int ERR_PEER_UNREGISTRATION_FAILED = 0x14;
}

#define ASSERT_OK(expr)                                                                                   \
  {                                                                                                       \
    int __result = expr;                                                                                  \
    if (__result > thingnet::RESULT_SUCCESS_BOUNDARY)                                                     \
    {                                                                                                     \
      LOG_ERROR("Fatal error. Non success error code: (%d) [%s, line %d]", __result, __FILE__, __LINE__); \
      exit(__result);                                                                                     \
    }                                                                                                     \
  }

#define ASSERT_TRUE(expr)                                                                                \
  {                                                                                                      \
    if (!(expr))                                                                                         \
    {                                                                                                    \
      LOG_ERROR("Fatal error. Did not evaluate to true: (%s) [%s, line %d]", #expr, __FILE__, __LINE__); \
      exit(1);                                                                                           \
    }                                                                                                    \
  }

#endif