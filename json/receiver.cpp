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
using tcp = boost::asio::ip::tcp;     // from <boost/asio/ip/tcp.hpp>

// Define a struct to hold timestamp data (seconds and nanoseconds in UTC)
struct Timestamp {
    uint32_t seconds;
    uint32_t nanoseconds;
};

int main() {
    double launchTime = 0;
    bool launchTimeStampReceived = false;
    while (!launchTimeStampReceived){
        try {
            // Prepare boost::asio objects
            asio::io_context io_context;
            tcp::resolver resolver(io_context);
            beast::websocket::stream<tcp::socket> ws(io_context);

            // Resolve the WebSocket domain name
            auto const results = resolver.resolve("localhost", "54800");

            std::cout << "Connecting to the AFATDS...\n";

            // Connect to the WebSocket server
            asio::connect(ws.next_layer(), results.begin(), results.end());            

            // Perform the WebSocket handshake
            ws.handshake("localhost", "/");
            
            Timestamp received_timestamp;
            beast::flat_buffer buffer;
            ws.read(buffer);

            // Parse JSON message
            std::cout << "Parsing json file...." << std::endl;
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
                    launchTimeStampReceived = true;
                    ws.close(websocket::close_code::normal);
                    io_context.run_for(std::chrono::seconds(1)); // Allow time for closing
                }
            }                                       
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            sleep(1);
        }
    }
    

    std::cout << "Launch Time = " << launchTime << std::endl;
    return 0;
}
