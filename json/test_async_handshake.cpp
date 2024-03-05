#include <iostream>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

// Function to handle the WebSocket handshake completion
void on_handshake(const beast::error_code& ec) {
    if (ec) {
        std::cerr << "Handshake failed: " << ec.message() << std::endl;
        return;
    }
    std::cout << "Handshake successful!" << std::endl;

    // Continue with WebSocket operations...
}

// Function to handle the connection completion
void on_connect(const beast::error_code& ec, websocket::stream<tcp::socket>& ws) {
    if (ec) {
        std::cerr << "Connect failed: " << ec.message() << std::endl;
        return;
    }

    // Perform WebSocket handshake asynchronously
    //ws.async_handshake("localhost", "/", &on_handshake);
    ws.async_handshake("localhost", "/", [&](const beast::error_code& handshake_ec) {
        if (handshake_ec) {
            std::cerr << "Handshake failed: " << handshake_ec.message() << std::endl;
            return;
        }
        std::cout << "Handshake successful!" << std::endl;

        // Continue with WebSocket operations...
    });
}

int main() {
    net::io_context ioc;

    // These objects perform our I/O
    tcp::resolver resolver(ioc);
    websocket::stream<tcp::socket> ws(ioc);

    // Look up the domain name
    auto const results = resolver.resolve("localhost", "54800");

    // Asynchronously connect to the server
    net::async_connect(ws.next_layer(), results.begin(), results.end(), std::bind(&on_connect, std::placeholders::_1, std::ref(ws)));    

    

    // Run the I/O service to start the asynchronous operations
    ioc.run();

    return 0;
}

