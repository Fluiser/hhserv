#pragma once
#include <string>
#include <boost/tuple/tuple.hpp>
#include <boost/logic/tribool.hpp>
#include "request.hpp"

namespace http {
    class r_handler {
    public:
        r_handler(const std::string& root) noexcept: state(state_t::method_start) 
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
        	}
        }
    };
}
