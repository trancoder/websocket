#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <iostream>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

int main() {    

    net::io_context ioc;

    // These objects perform our I/O
    tcp::resolver resolver(ioc);
    websocket::stream<tcp::socket> ws(ioc);

    // Look up the domain name
    auto const results = resolver.resolve("127.0.0.1", "54800");

    // Connect to the server
    net::connect(ws.next_layer(), results.begin(), results.end());

    // Read and print data from the server asynchronously
    beast::flat_buffer buffer;

    double launchTime = 0;


    // Perform WebSocket handshake asynchronously
    ws.async_handshake("127.0.0.1", "/",
        [&](const beast::error_code& ec) {
            if (ec) {
                std::cerr << "Handshake failed: " << ec.message() << std::endl;
                return;
            }
            std::cout << "Handshake successful!" << std::endl;
        
            // Start asynchronous read operation        
            ws.async_read(buffer, [&](beast::error_code read_ec, std::size_t bytes_transferred) {
                
                if (!ec) {
                    
                    try {                        
                        //Parse the received message as JSON                        
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
                                //ws.close(websocket::close_code::normal);                                
                            }
                        }
                    } catch (const std::exception& e) {
                        std::cerr << "Exception: " << e.what() << std::endl;
                    }
                } else {
                    std::cerr << "WebSocket read error: " << read_ec.message() << std::endl;
                }
            });
        });

    // Run the I/O service to start the asynchronous operations
    ioc.run();

    return 0;
}
