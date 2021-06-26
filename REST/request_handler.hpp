#pragma once
#include "request.hpp"
#include "reply.hpp"
#include <string>
#include <sstream>
#include <hex>
#include <boost/noncopyable.hpp>

namespace http {
    class r_handler: private boost::noncopyable {
        std::string doc_root;

        static bool uri_decode(const std::string& in, std::string& out) noexcept
        {
            out.reserve(in.size());
            for(size_t i = 0; i < in.size(); ++i)
            {
                if(in[i] == '%') {
                    if(i+3 <= in.size()) {
                        int v = 0;
                        std::istringstream is(in.substr(i+1, 2));
                        if(is >> std::hex >> v) {
                            out += (char)v;
                            i += 2;
                        }
                    } else {
                        return false;
                    }
                } else if(in[i] == '+') {
                    out += ' ';
                } else {
                    out += in[i];
                }
            }
            return true;
        }
    public:
        r_handler() noexcept {}

        r_handler(const std::string& root) noexcept: doc_root(root)
        {

        }

        void handle(const request_t& request, reply_t& reply) noexcept
        {
            std::string request_path{};
            if(uri_decode(request.path, request_path)) {
                reply.status = reply::bad_request;
                return;
            }

            if( request_path.empty() ||
                request_path[0] != '/' ||
                request_path.find("..") != std::string::npos) {
                reply = reply::stock_reply(reply::bad_request);
                return;
            }
        }

    };
}