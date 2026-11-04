#ifndef __CONFIG_H__
#define __CONFIG_H__

#define BUILD     "build/"
#define SRC       "src/"
#define SRC_BUILD "src_build/"
#define INCLUDE_FLAG "I" SRC

const char* const INCLUDES[] = {
    "-I" SRC "include"
};
const int INCLUDES_LEN = sizeof(INCLUDES) / sizeof(INCLUDES[0]);

#endif  // __CONFIG_H__
