#pragma once
#include "request.hpp"
#include "reply.hpp"
#include <string>
#include <sstream>
#include <fstream>
#include <boost/noncopyable.hpp>
#include "mime_types.hpp"

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

            if(request_path[request_path.size()-1] == '/') {
                request_path += "index.html";
            }

            size_t last_slash_pos = request_path.find_last_of('/');
            size_t last_dot_pos = request_path.find_last_of('.');
            std::string extension {};

            if(last_dot_pos != std::string::npos && last_dot_pos > last_slash_pos) {
                extension = request_path.substr(last_dot_pos+1);
            }

            std::string full_path = doc_root + request_path;
            std::ifstream is(full_path, std::ios::in | std::ios::binary);
            if(!is) {
                reply = reply::stock_reply(reply::not_found);
                return;
            }

            reply.status = reply_t::ok;
            char buff[512];
            while(is.read(buff, sizeof(buff)).gcount() > 0)
            {
                reply.content.append(buff, is.gcount());
            }

            reply.headers.resize(2);
            reply.headers[0].field = "Content-Length";
            reply.headers[0].value = boost::lexical_cast<std::string>(reply.content.size());
            reply.headers[1].field = "Content-Type";
            reply.headers[1].value = mime_types::extension_to_type(extension);

        }

    };
}