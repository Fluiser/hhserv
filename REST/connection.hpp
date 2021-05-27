#pragma once
#include "util.cpp"
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
        connection(const connection&) = default;
        connection() noexcept: socket(serv), strand(serv) {}

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

        void handler_read(
                    const boost::system::error_code& ec,
                    size_t bsize
                )
        {
            if(ec)
            {
                boost::tribool res;
                boost:tie(res, boost::tuples::ignore) = http::request_parser;
            }
            else
            {
                d_log("ERROR CODE: ", ec);
            }
        }

        void exe()
        {
            socket.async_read_some(
                        asio::buffer(buffer),
                        strand.wrap(
                                    boost::bind(&connection::handler_read, shared_from_this()/*Shit*/,
                                                asio::placeholders::error,
                                                asio::placeholders::bytes_transferred
                                                )
                                )
                    );

        }


    };
}
