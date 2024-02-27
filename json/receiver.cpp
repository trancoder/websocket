#define BOOST_BIND_GLOBAL_PLACEHOLDERS

#include <iostream>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/bind/bind.hpp> // Add this header
#include <boost/bind/placeholders.hpp> // Add this header

namespace asio = boost::asio;
namespace beast = boost::beast;
namespace websocket = beast::websocket;

// Define a struct to hold timestamp data (seconds and nanoseconds in UTC)
struct Timestamp {
    uint32_t seconds;
    uint32_t nanoseconds;
};

int main() {
    double launchTime = 0;
    try {
        asio::io_context io_context;
        websocket::stream<asio::ip::tcp::socket> ws(io_context);

        asio::ip::tcp::acceptor acceptor(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 9002));
        acceptor.accept(ws.next_layer());

        ws.accept();

        if (ws.is_open()) {
            std::cout << "Waiting for json message..." << std::endl;

            Timestamp received_timestamp;
            beast::flat_buffer buffer;
            ws.read(buffer);

             // Parse JSON message
            std::string message = beast::buffers_to_string(buffer.data());
            std::istringstream iss(message);
            boost::property_tree::ptree pt;
            boost::property_tree::json_parser::read_json(iss, pt);

            // Display content
            std::cout << "Received JSON message:\n";
            for (const auto& pair : pt) {
                std::cout << pair.first << ": " << pair.second.get_value<std::string>() << std::endl;
                if (pair.first == "launchTime") {
                    launchTime = pair.second.get_value<double>();
                }
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

    std::cout << "Launch Time = " << launchTime << std::endl;
    return 0;
}
