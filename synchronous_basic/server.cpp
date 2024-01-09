#include <iostream>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/websocket.hpp>

namespace asio = boost::asio;
namespace beast = boost::beast;
namespace websocket = beast::websocket;

int main() {
    try {
        asio::io_context io_context;

        // Create and bind the acceptor to listen for incoming connections
        asio::ip::tcp::acceptor acceptor(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 9002));

        std::cout << "Server is waiting for client...." << std::endl;

        while (true) {
            // Wait for and accept a new connection
            asio::ip::tcp::socket socket(io_context);
            acceptor.accept(socket);

            // WebSocket upgrade
            websocket::stream<asio::ip::tcp::socket> ws(std::move(socket));
            ws.accept();

            std::cout << "Server connected to client." << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
