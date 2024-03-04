#define BOOST_BIND_GLOBAL_PLACEHOLDERS

#include <iostream>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/json.hpp>
#include <boost/property_tree/json_parser.hpp>

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
            boost::asio::io_context io_context;
            tcp::resolver resolver(io_context);
            beast::websocket::stream<tcp::socket> ws(io_context);
            boost::asio::steady_timer timer(io_context, boost::asio::chrono::microseconds(100));

            // Resolve the WebSocket domain name
            auto const results = resolver.resolve("localhost", "54800");

            std::cout << "Connecting to the server...\n";

            // Connect to the WebSocket server
            boost::asio::connect(ws.next_layer(), results.begin(), results.end());

            std::cout << "Connection to server is established\n";

            // Perform the WebSocket handshake
            ws.handshake("localhost", "/");            
            
            // Multi buffer for incoming messages
            beast::multi_buffer buffer;

            timer.async_wait([&](const boost::system::error_code& ec) {
                if (!ec) {
                    // Check for incoming messages
                    ws.async_read(buffer, [&](beast::error_code ec, std::size_t bytes_transferred) {
                        if (!ec) {
                            // Parse the received message as JSON
                            boost::json::error_code json_ec;
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
                        } else {
                            std::cerr << "WebSocket read error: " << ec.message() << std::endl;
                        }
                    });
                    // Set the timer to check again after 100 microseconds
                    timer.expires_after(boost::asio::chrono::microseconds(100));
                    timer.async_wait([&](const boost::system::error_code& ec) {});
                } else {
                    std::cerr << "Timer error: " << ec.message() << std::endl;
                }
            });



            // Timestamp received_timestamp;
            // beast::flat_buffer buffer;
            // ws.read(buffer);

            // // Parse JSON message
            // std::cout << "Parsing json file...." << std::endl;
            // std::string message = beast::buffers_to_string(buffer.data());
            // std::istringstream iss(message);
            // boost::property_tree::ptree pt;
            // boost::property_tree::json_parser::read_json(iss, pt);

            // // Display content
            // std::cout << "Received JSON message:\n";
            // for (const auto& pair : pt) {
            //     std::cout << pair.first << ": " << pair.second.get_value<std::string>() << std::endl;
            //     if (pair.first == "launchTime") {
            //         launchTime = pair.second.get_value<double>();
            //         launchTimeStampReceived = true;
            //         ws.close(websocket::close_code::normal);
            //         io_context.run_for(std::chrono::seconds(1)); // Allow time for closing
            //     }
            // }                                       
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            sleep(1);
        }
    }
    

    std::cout << "Launch Time = " << launchTime << std::endl;
    return 0;
}
