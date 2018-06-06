#pragma once


#include <errno.h>
#include <string.h>




// debug info warnning error

#define LOGD(...) printf("[D] " __VA_ARGS__); printf("\n")
#define LOGI(...) printf("[I] " __VA_ARGS__); printf("\n")
#define LOGW(...) printf("[W] " __VA_ARGS__); printf("\n")
#define LOGE(...) printf("[E] " __VA_ARGS__); printf("\n")
