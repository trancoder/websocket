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
    bool launchTimeStampReceived = false;
    while (!launchTimeStampReceived){
        try {
            asio::io_context io_context;
            websocket::stream<asio::ip::tcp::socket> ws(io_context);

            //asio::ip::tcp::acceptor acceptor(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 9002));
            std::string AfatdsIp = "127.0.0.1";
            short unsigned int AfatdsPort = 54800;
            boost::asio::ip::tcp::endpoint AfatdsEndpoint(boost::asio::ip::make_address(AfatdsIp), AfatdsPort);
            
            std::cout << "Waiting for the websocket Connection..." << std::endl;

            // sync operation (will block until connection is established)
            asio::ip::tcp::acceptor acceptor(io_context, AfatdsEndpoint);

            //returns the next layer of the WebSocket stream, which is the underlying TCP socket
            acceptor.accept(ws.next_layer());

            // for hand shaking (exeception will be thrown at this step)
            ws.accept();

            std::cout << "Websocket Connection is established succesfully" << std::endl;

            if (ws.is_open()) {
                std::cout << "Waiting for json message..." << std::endl;

                try {
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
                        }
                    }
                }
                catch (const boost::beast::system_error& e)
                {                
                    std::cerr <<  e.what() << std::endl;
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
    }
    

    std::cout << "Launch Time = " << launchTime << std::endl;
    return 0;
}
