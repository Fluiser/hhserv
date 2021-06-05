#pragma once
#include "util.cpp"
#include "request_parser.hpp"
#include <boost/tuple/tuple.hpp>
#include <boost/logic/tribool.hpp>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/noncopyable.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <string>
#include <vector>
namespace server {

namespace asio = boost::asio;

    class connection:  public boost::enable_shared_from_this<connection> {
        asio::io_service serv;
        asio::ip::tcp::socket socket;
        boost::asio::io_service::strand strand;
        boost::array<char, 1024*8> buffer;

    public:
        connection(const connection&) = delete;
        connection(boost::asio::io_service& serv,
                   http::r_handler) noexcept: socket(serv), strand(serv) {}

        /*
         * void connection::start()
{
            socket_.async_read_some(boost::asio::buffer(buffer_),
            strand_.wrap(
            boost::bind(&connection::handle_read, shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred)));
            }
         * */

        const decltype(serv)& _serv() const noexcept
        {
            return serv;
        }
        const decltype(socket)& _socket() const noexcept
        {
            return socket;
        }

    };
}
