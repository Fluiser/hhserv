#pragma once
#include <string>
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include "request.hpp"

namespace http {
    typedef struct reply {
        enum status_t {
            ok = 200,
            created = 201,
            accepted = 202,
            no_content = 204,
            multiple_choices = 300,
            moved_permanently = 301,
            moved_temporarily = 302,
            not_modified = 304,
            bad_request = 400,
            unauthorized = 401,
            forbidden = 403,
            not_found = 404,
            internal_server_error = 500,
            not_implemented = 501,
            bad_gateway = 502,
            service_unavailable = 503
        };

        std::vector<header_t> headers;
        std::string content;
        status_t status;

        std::vector<boost::asio::const_buffer>&& to_buffer();

        static void stock_reply(reply& r, status_t status);
        static reply stock_reply(status_t status);
    } reply_t;

    namespace status {
        static char ok[] = "HTTP/1.1 200 OK\r\n";
        static char created[] = "HTTP/1.1 201 Created\r\n";
        static char accepted[] = "HTTP/1.1 202 Accepted\r\n";
        static char no_content[] = "HTTP/1.1 204 No Content\r\n";
        static char multiple_choices[] = "HTTP/1.1 300 Multiple Choices\r\n";
        static char moved_permanently[] = "HTTP/1.1 301 Moved Permanently\r\n";
        static char moved_temporarily[] = "HTTP/1.1 302 Moved Temporarily\r\n";
        static char not_modified[] = "HTTP/1.1 304 Not Modified\r\n";
        static char bad_request[] = "HTTP/1.1 400 Bad Request\r\n";
        static char unauthorized[] = "HTTP/1.1 401 Unauthorized\r\n";
        static char forbidden[] = "HTTP/1.1 403 Forbidden\r\n";
        static char not_found[] = "HTTP/1.1 404 Not Found\r\n";
        static char internal_server_error[] = "HTTP/1.1 500 Internal Server Error\r\n";
        static char not_implemented[] = "HTTP/1.1 501 Not Implemented\r\n";
        static char bad_gateway[] = "HTTP/1.1 502 Bad Gateway\r\n";
        static char service_unavailable[] = "HTTP/1.1 503 Service Unavailable\r\n";

        boost::asio::const_buffer to_buffer(http::reply_t::status_t status) noexcept {
            switch (status) {
                case http::reply_t::status_t::ok:
                    return boost::asio::buffer(ok);
                case http::reply_t::status_t::created:
                    return boost::asio::buffer(created);
                case http::reply_t::status_t::no_content:
                    return boost::asio::buffer(no_content);
                case http::reply_t::status_t::multiple_choices:
                    return boost::asio::buffer(multiple_choices);
                case http::reply_t::status_t::moved_permanently:
                    return boost::asio::buffer(moved_permanently);
                case http::reply_t::status_t::moved_temporarily:
                    return boost::asio::buffer(moved_temporarily);
                case http::reply_t::status_t::not_modified:
                    return boost::asio::buffer(not_modified);
                case http::reply_t::status_t::bad_request:
                    return boost::asio::buffer(bad_request);
                case http::reply_t::status_t::unauthorized:
                    return boost::asio::buffer(unauthorized);
                case http::reply_t::status_t::forbidden:
                    return boost::asio::buffer(forbidden);
                case http::reply_t::status_t::not_found:
                    return boost::asio::buffer(not_found);
                case http::reply_t::status_t::internal_server_error:
                    return boost::asio::buffer(internal_server_error);
                case http::reply_t::status_t::not_implemented:
                    return boost::asio::buffer(not_implemented);
            }
        }
    }// namespace status
    namespace misc_strings {
        static char name_value_separator[] = {':', ' '};
        static char crlf[] = {'\r', '\n'};
    }

    std::vector<boost::asio::const_buffer>&& reply_t::to_buffer()
    {
        std::vector<boost::asio::const_buffer> buffers;
        buffers.push_back(status::to_buffer(status));
        for(size_t i = 0; i < headers.size(); ++i)
        {
            header_t& h = headers[i];
            buffers.push_back(boost::asio::buffer(h.field));
            buffers.push_back(boost::asio::buffer(misc_strings::name_value_separator));
            buffers.push_back(boost::asio::buffer(h.value));
            buffers.push_back(boost::asio::buffer(misc_strings::crlf));
        }
        buffers.push_back(boost::asio::buffer(misc_strings::crlf));
        buffers.push_back(boost::asio::buffer(content));
    }
    namespace stock_replies {

        static char ok[] = "";
        static char created[] =
                "<html>"
                "<head><title>Created</title></head>"
                "<body><h1>201 Created</h1></body>"
                "</html>";
        static char accepted[] =
                "<html>"
                "<head><title>Accepted</title></head>"
                "<body><h1>202 Accepted</h1></body>"
                "</html>";
        static char no_content[] =
                "<html>"
                "<head><title>No Content</title></head>"
                "<body><h1>204 Content</h1></body>"
                "</html>";
        static char multiple_choices[] =
                "<html>"
                "<head><title>Multiple Choices</title></head>"
                "<body><h1>300 Multiple Choices</h1></body>"
                "</html>";
        static char moved_permanently[] =
                "<html>"
                "<head><title>Moved Permanently</title></head>"
                "<body><h1>301 Moved Permanently</h1></body>"
                "</html>";
        static char moved_temporarily[] =
                "<html>"
                "<head><title>Moved Temporarily</title></head>"
                "<body><h1>302 Moved Temporarily</h1></body>"
                "</html>";
        static char not_modified[] =
                "<html>"
                "<head><title>Not Modified</title></head>"
                "<body><h1>304 Not Modified</h1></body>"
                "</html>";
        static char bad_request[] =
                "<html>"
                "<head><title>Bad Request</title></head>"
                "<body><h1>400 Bad Request</h1></body>"
                "</html>";
        static char unauthorized[] =
                "<html>"
                "<head><title>Unauthorized</title></head>"
                "<body><h1>401 Unauthorized</h1></body>"
                "</html>";
        static char forbidden[] =
                "<html>"
                "<head><title>Forbidden</title></head>"
                "<body><h1>403 Forbidden</h1></body>"
                "</html>";
        static char not_found[] =
                "<html>"
                "<head><title>Not Found</title></head>"
                "<body><h1>404 Not Found</h1></body>"
                "</html>";
        static char internal_server_error[] =
                "<html>"
                "<head><title>Internal Server Error</title></head>"
                "<body><h1>500 Internal Server Error</h1></body>"
                "</html>";
        static char not_implemented[] =
                "<html>"
                "<head><title>Not Implemented</title></head>"
                "<body><h1>501 Not Implemented</h1></body>"
                "</html>";
        static char bad_gateway[] =
                "<html>"
                "<head><title>Bad Gateway</title></head>"
                "<body><h1>502 Bad Gateway</h1></body>"
                "</html>";
        static char service_unavailable[] =
                "<html>"
                "<head><title>Service Unavailable</title></head>"
                "<body><h1>503 Service Unavailable</h1></body>"
                "</html>";

        std::string to_string(reply::status_t status) {
            switch (status) {
                case reply::ok:
                    return ok;
                case reply::created:
                    return created;
                case reply::accepted:
                    return accepted;
                case reply::no_content:
                    return no_content;
                case reply::multiple_choices:
                    return multiple_choices;
                case reply::moved_permanently:
                    return moved_permanently;
                case reply::moved_temporarily:
                    return moved_temporarily;
                case reply::not_modified:
                    return not_modified;
                case reply::bad_request:
                    return bad_request;
                case reply::unauthorized:
                    return unauthorized;
                case reply::forbidden:
                    return forbidden;
                case reply::not_found:
                    return not_found;
                case reply::internal_server_error:
                    return internal_server_error;
                case reply::not_implemented:
                    return not_implemented;
                case reply::bad_gateway:
                    return bad_gateway;
                case reply::service_unavailable:
                    return service_unavailable;
                default:
                    return internal_server_error;
            }
        }

    } // namespace stock_replies

    void reply_t::stock_reply(reply_t& r, status_t status)
    {
        r.status = status;
        r.content = stock_replies::to_string(status);
        r.headers.resize(2);
        r.headers[0].field = "Content-Length";
        r.headers[0].value = boost::lexical_cast<std::string>(r.content.size());
        r.headers[1].field="Content-Type";
        r.headers[1].value="text/html";
    }

    reply_t reply_t::stock_reply(status_t status)
    {
        reply_t r;
        r.status = status;
        r.content = stock_replies::to_string(status);
        r.headers.resize(2);
        r.headers[0].field = "Content-Length";
        r.headers[0].value = boost::lexical_cast<std::string>(r.content.size());
        r.headers[1].field="Content-Type";
        r.headers[1].value="text/html";
        return r;
    }
}//namespace http