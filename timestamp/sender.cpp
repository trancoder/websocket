#include <iostream>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/websocket.hpp>
#include <chrono>

namespace asio = boost::asio;
namespace beast = boost::beast;
namespace websocket = beast::websocket;

// Define a struct to hold timestamp data (seconds and nanoseconds in UTC)
struct Timestamp {
    uint32_t seconds;
    uint32_t nanoseconds;
};

int main() {
    try {
        asio::io_context io_context;
        websocket::stream<asio::ip::tcp::socket> ws(io_context);

        ws.next_layer().connect(asio::ip::tcp::endpoint(asio::ip::address::from_string("127.0.0.1"), 9002));
        ws.handshake("127.0.0.1", "/");

        if (ws.is_open()) {
            std::cout << "Sending timestamp (seconds and nanoseconds) in UTC..." << std::endl;

            // Get current time in UTC with seconds and nanoseconds precision
            auto now = std::chrono::system_clock::now();
            auto time_since_epoch = now.time_since_epoch();
            auto seconds = std::chrono::duration_cast<std::chrono::seconds>(time_since_epoch);
            auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(time_since_epoch - seconds);

            Timestamp timestamp{ static_cast<uint32_t>(seconds.count()), static_cast<uint32_t>(nanoseconds.count()) };

            // Send timestamp struct containing seconds and nanoseconds in UTC as binary data
            ws.binary(true);
            ws.write(asio::buffer(&timestamp, sizeof(timestamp)));
            std::cout << "Seconds = " << timestamp.seconds << " and nanoseconds = " << timestamp.nanoseconds << std::endl;
        } else {
            std::cerr << "Failed to establish connection." << std::endl;
        }

        // Gracefully close the WebSocket connection
        ws.close(websocket::close_code::normal);
        io_context.run_for(std::chrono::seconds(1)); // Allow time for closing
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
