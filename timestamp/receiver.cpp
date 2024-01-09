#include <iostream>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/websocket.hpp>

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

        asio::ip::tcp::acceptor acceptor(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 9002));
        acceptor.accept(ws.next_layer());

        ws.accept();

        if (ws.is_open()) {
            std::cout << "Waiting for timestamp (seconds and nanoseconds)..." << std::endl;

            Timestamp received_timestamp;
            beast::flat_buffer buffer;
            ws.read(buffer);

            if (ws.got_text()) {
                std::cerr << "Payload should be binary but received a text frame." << std::endl;
            } else if (buffer.size() != sizeof(received_timestamp)) {
                std::cerr << "Received payload size is not valid for timestamp." << std::endl;
            } else {
                std::memcpy(&received_timestamp, buffer.data().data(), sizeof(received_timestamp));

                std::cout << "Received timestamp (seconds and nanoseconds) - Seconds: " << received_timestamp.seconds
                          << ", Nanoseconds: " << received_timestamp.nanoseconds << std::endl;
            }
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
