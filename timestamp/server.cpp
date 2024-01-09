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
        websocket::stream<asio::ip::tcp::socket> ws(io_context);
        asio::ip::tcp::acceptor acceptor(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 9002));
        acceptor.accept(ws.next_layer());

        // Perform WebSocket handshake
        ws.accept();

        // Check if the connection is open
        if (ws.is_open()) {
            std::cout << "Connection established. Sending timestamp..." << std::endl;

            // Get current time with seconds and nanoseconds precision
            auto now = std::chrono::system_clock::now();
            auto time_since_epoch = now.time_since_epoch();
            auto seconds = std::chrono::duration_cast<std::chrono::seconds>(time_since_epoch);
            auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(time_since_epoch - seconds);

            // Send timestamp message containing seconds and nanoseconds
            ws.write(asio::buffer(std::to_string(seconds.count()) + " " + std::to_string(nanoseconds.count())));
        } else {
            std::cerr << "Failed to establish connection." << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
