#pragma once
#include <string>
#include <vector>

namespace http {
    enum request_types {
        GET,
        POST,
        _DELETE,
        UNKNOWN
    };

    typedef struct header {
        std::string field;
        std::string value;
    } header_t;

    typedef struct request {
        std::string method;
        std::string path;
        std::vector<header_t> headers;

        int http_ver_major{0}; // ?
        int http_ver_minor{0}; //?
    } request_t;
}