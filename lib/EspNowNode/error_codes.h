#ifndef __ERROR_CODES_H
#define __ERROR_CODES_H

namespace thingnet
{
  const int RESULT_OK = 0x00;
  const int ERR_NODE_NOT_INITIALIZED = 0x01;
  const int ERR_HANDLER_LIMIT_EXCEEDED = 0x02;
  const int ERR_NODE_MANAGER_NOT_SET = 0x03;
}

#define ASSERT_OK(expr)                                                                                 \
  {                                                                                                     \
    int result = expr;                                                                                  \
    if (result != thingnet::RESULT_OK)                                                                            \
    {                                                                                                   \
      LOG_ERROR("Fatal error. Non success error code: (%d) [%s, line %d]", result, __FILE__, __LINE__); \
      exit(result);                                                                                     \
    }                                                                                                   \
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