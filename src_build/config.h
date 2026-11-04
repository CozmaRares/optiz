#ifndef __CONFIG_H__
#define __CONFIG_H__

#define BUILD        "build/"
#define SRC          "src/"
#define SRC_BUILD    "src_build/"
#define INCLUDE_FLAG "I" SRC

const char* const CPP_FLAGS[] = {
    "-Wall",
    "-Wextra",
    "-fno-exceptions",
    "-I" SRC "include"
};
const int CPP_FLAGS_LEN = sizeof(CPP_FLAGS) / sizeof(CPP_FLAGS[0]);

#endif  // __CONFIG_H__
