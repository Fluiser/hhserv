#pragma once
#include <string>
#include <boost/tuple/tuple.hpp>
#include <boost/logic/tribool.hpp>
#include "request.hpp"
#include <boost/noncopyable.hpp>
#include <string>
#include <sstream>
#include "reply.hpp"
#include <fstream>
#include "mime_types.hpp"

namespace http {

class r_handler: private boost::noncopyable {
        std::string doc_root;
    protected:
        static bool url_decode(const std::string& in, std::string& out)
        {
            if(!out.empty()) out.clear();
            out.reserve(in.size());
            for(size_t i = 0; i < in.size(); ++i)
            {
                if(in[i] == '%') {
                    if(i+3 <= in.size()) {
                        int value{0};
                        std::istringstream is(in.substr(i+1, 2));
                        if(is >> std::hex >> value) {
                            out += (char)value;
                            i+=2;
                        } else return false;
                    } else return false;
                } else if(in[i] == '+') {
                    out += ' ';
                } else out += in[i];
            }
            return true;
        }

        void handle(const request_t& req, reply_t& rep)
        {
            // Touch my url.
            std::string req_path;
            if(!url_decode(req.path, req_path)) {
                rep = reply_t::stock_reply(reply_t::bad_request);
                return;
            }

            if(req_path.empty() || req_path[0] != '/' || req_path.find("..") != std::string::npos) {
                rep = reply_t::stock_reply(reply_t::bad_request);
                return;
            }

            if(req_path[req_path.size()-1] == '/')
                req_path += "index.html"; // оАОАООАОАОАОА

            size_t  last_slash_pos = req_path.find_last_of('/'),
                    last_dot_pos = req_path.find_last_of('.');
            std::string ext;
            if(last_dot_pos != std::string::npos && last_dot_pos > last_slash_pos)
                ext = req_path.substr(last_dot_pos+1);

            std::string full_path = doc_root + req_path;
            std::fstream file(full_path.c_str(), std::fstream::in | std::fstream::binary);
            if(!file.is_open()) {
                rep = reply_t::stock_reply(reply_t::not_found);
                return;
            }

            rep.status = reply_t::ok;
            char buffer[512];
            while(file.read(buffer, sizeof(buffer)).gcount() > 0)
                rep.content.append(buffer, file.gcount());
            rep.headers.resize(2);
            rep.headers[0].field = "Content-Length";
            rep.headers[0].value = boost::lexical_cast<std::string>(rep.content.size());
            rep.headers[1].field = "Content-Type";
            rep.headers[1].value = mime_types::extension_to_type(ext);
        }
    public:
        r_handler(const std::string& droot) noexcept: doc_root(droot)
        {}


    };

    class r_parser {
    public:
        r_parser(const std::string& root) noexcept: state(state_t::method_start)
        {

        }

        void reset()
        {
            state = state_t::method_start;
        }

        template<class Iterator>
        boost::tuple<boost::tribool, Iterator> parse(request_t& req,
                                                     Iterator begin,
                                                     Iterator end)
        {
            while(begin != end)
            {
                boost::tribool result = this->consume(req, *begin++);
                if(result || !result)
                    return boost::make_tuple(result, begin);
            }
            boost::tribool result = boost::indeterminate;
            return boost::make_tuple(result, begin);
        }

    protected:
        typedef enum {
                method_start,
                method,
                uri_start,
                uri,
                http_version_h,
                http_version_t_1,
                http_version_t_2,
                http_version_p,
                http_version_slash,
                http_version_major_start,
                http_version_major,
                http_version_minor_start,
                http_version_minor,
                expecting_newline_1,
                header_line_start,
                header_lws,
                header_name,
                space_before_header_value,
                header_value,
                expecting_newline_2,
                expecting_newline_3
        } state_t;

        state_t state;

        static bool is_char(int c)
        {
        	return c >= 0 && c <= 127;
        }

        static bool is_ctl(int c)
        {
        	return (c >= 0 && c <= 31) || (c == 127);
        }

        static bool is_digit(int c)
        {
            return c >= '0' && c <= '9';
        }

        static bool is_tspecial(int c)
        {
        	switch (c)
			{
				case '(': case ')': case '<': case '>': case '@':
				case ',': case ';': case ':': case '\\': case '"':
				case '/': case '[': case ']': case '?': case '=':
				case '{': case '}': case ' ': case '\t':
					return true;
				default:
					return false;
			}
        }

        boost::tribool consume(request_t& req, char input)
        {
        	switch(state)
        	{
        		case state_t::method_start:
        			if(is_char(input) || is_ctl(input) || is_tspecial(input))
        				return false;
        			else {
        				state = state_t::method;
        				req.method.push_back(input);
        				return boost::indeterminate;
        			}
        		case state_t::method:
        			if(input == ' ')
        			{
        				state = state_t::uri;
        				return boost::indeterminate;
        			}
        			else if(!is_char(input) || is_ctl(input) || is_tspecial(input))
        				return false;
        			else {
        				req.method.push_back(input);
        				return boost::indeterminate;
        			}
        		case state_t::uri_start:
        			if(is_ctl(input))
        				return false;
        			else {
        				state = state_t::uri;
        				req.path.push_back(input);
        				return boost::indeterminate;
        			}
        		case state_t::uri:
        			if(input == ' ') {
        				state = state_t::http_version_h;
        				return boost::indeterminate;
        			} else if(is_ctl(input))
        				return false;
        			else {
        				req.path.push_back(input);
        				return boost::indeterminate;
        			}
        	    case state_t::http_version_h:
        	        if(input == 'H') {
        	            state = state_t::http_version_t_1;
        	            return boost::indeterminate;
        	        } else return false;
        	    case state_t::http_version_t_1:
        	        if(input == 'T') {
        	            state = state_t::http_version_t_2;
        	            return boost::indeterminate;
        	        } else return false;
        	    case state_t::http_version_t_2:
        	        if(input == 'T') {
        	            state = state_t::http_version_p;
        	            return boost::indeterminate;
        	        } else return false;
        	    case state_t::http_version_p:
        	        if(input == 'P') {
        	            state = state_t::http_version_slash;
        	            return boost::indeterminate;
        	        } else return false;
        	    case state_t::http_version_slash:
        	        if(input == '/') {
        	            req.http_ver_minor = req.http_ver_major = 0;
        	            state = http_version_major_start;
        	            return boost::indeterminate;
        	        } else return false;
        	    case state_t::http_version_major_start:
        	        if(is_digit(input)) {
        	            req.http_ver_major = req.http_ver_major*10+input-'0';
        	            state = state_t::http_version_major;
        	            return boost::indeterminate;
        	        } else return false;
        	    case state_t::http_version_major:
        	        if(input == '.') {
        	            state = http_version_minor_start;
        	            return boost::indeterminate;
        	        } else if(is_digit(input)) {
        	            req.http_ver_major = req.http_ver_major*10+input-'0';
        	            return boost::indeterminate;
        	        } else return false;
        	    case state_t::http_version_minor_start:
        	        if(is_digit(input)) {
        	            req.http_ver_minor = req.http_ver_minor*10+input-'0';
        	            state = http_version_minor;
        	            return boost::indeterminate;
        	        } else return false;
                case state_t::http_version_minor: 
                    if(input == '\r') {
                        state = state_t::expecting_newline_1;
                        return boost::indeterminate;
                    } else if(is_digit(input)) {
                        req.http_ver_minor = req.http_ver_major * 10 + input - '\0';
                        return boost::indeterminate;
                    } else return false;
        	    case state_t::expecting_newline_1:
        	        if(input == '\n') {
        	            state = header_line_start;
        	            return boost::indeterminate;
        	        } else return false;
        	    case state_t::header_line_start:
        	        if(input == '\r') {
        	            state = expecting_newline_3;
        	            return boost::indeterminate;
        	        } else if(!req.headers.empty() && (input == ' ' || input == '\t')) {
        	            state = header_lws;
        	            return boost::indeterminate;
        	        } else if(!is_char(input) || is_ctl(input) || is_tspecial(input)) return false;
        	        else {
        	            req.headers.emplace_back();
        	            req.headers.back().field.push_back(input);
        	            state = header_name;
        	            return boost::indeterminate;
        	        }
        	    case state_t::header_lws:
        	        if(input == '\r') {
        	            state = expecting_newline_2;
        	            return boost::indeterminate;
        	        } else if(input == ' ' || input == '\t') return boost::indeterminate;
        	        else if(is_ctl(input)) return false;
        	        else {
        	            state = header_value;
        	            req.headers.back().value.push_back(input);
        	            return boost::indeterminate;
        	        }
        	    case state_t::header_name:
        	        if(input == ':') {
        	            state = space_before_header_value;
        	            return boost::indeterminate;
        	        } else if(is_ctl(input) || !is_char(input) || is_tspecial(input)) return false;
        	        else {
        	            req.headers.back().field.push_back(input);
        	            return boost::indeterminate;
        	        }
        	    case state_t::space_before_header_value:
        	        if(input == ' ') {
        	            state = header_value;
        	            return boost::indeterminate;
        	        } else return false;
        	    case state_t::header_value:
        	        if(input == '\r') {
        	            state = expecting_newline_2;
        	            return boost::indeterminate;
        	        } else {
        	            if(is_ctl(input)) return false;
        	            req.headers.back().value.push_back(input);
        	            return boost::indeterminate;
        	        }
        	    case state_t::expecting_newline_2:
        	        if(input == '\n') {
        	            state = header_line_start;
        	            return boost::indeterminate;
        	        } else return false;
        	    case state_t::expecting_newline_3:
        	        return (input == '\n');
        	    default:
        	        return false;
        	}
        }
    };
}
