#include <unordered_map>
#include <vector>
#include <unordered_set>

#include <WinSock2.h>
#pragma comment(lib, "ws2_32")

#include "ringBuffer.h"
#include "common.h"
#include "protocolBuffer.h"
#include "stub.h"

#include "proxy.h"

constexpr int strLen = 1000;
static wchar_t str[strLen];


