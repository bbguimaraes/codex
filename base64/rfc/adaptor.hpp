#ifndef CODEX_BASE64_RFC_ADAPTER_H
#define CODEX_BASE64_RFC_ADAPTER_H

#include <string_view>

extern "C" {
#include "base64.h"
}

/**
 * Adaptor for \ref EncodeFunc and \ref DecodeFunc for the RFC implementation.
 */
struct Base64RFC {
    static void encode(std::string_view src, char *out);
    static char *decode(std::string_view src, char *out);
};

inline void Base64RFC::encode(std::string_view src, char *out) {
    base64_encode(
        src.data(), src.size(),
        out, output_size<4, 3>(src.size()));
}

inline char *Base64RFC::decode(std::string_view src, char *out) {
    std::size_t ret = output_size<3, 4>(src.size());
    if(!base64_decode(src.data(), src.size(), out, &ret))
        return nullptr;
    return out + ret;
}

#endif
