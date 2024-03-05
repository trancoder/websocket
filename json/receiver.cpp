#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace asio = boost::asio;
using tcp = boost::asio::ip::tcp;

// Function to run WebSocket communication
void run_websocket(asio::io_context& io_context, double& launchTime) {
    try { 
        bool launchTimeStampReceived = false;

        // Resolve the WebSocket server
        tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve("localhost", "8765");

        // Create a WebSocket stream
        websocket::stream<tcp::socket> ws(io_context);

        // Connect to the WebSocket server
        asio::connect(ws.next_layer(), endpoints);

        // Perform the WebSocket handshake
        ws.handshake("localhost", "/");

        // Read and print data from the server asynchronously
        beast::flat_buffer buffer;        
            
        // Start asynchronous read operation        
        ws.async_read(buffer, [&](beast::error_code ec, std::size_t bytes_transferred) {
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
                            launchTimeStampReceived = true;
                            ws.close(websocket::close_code::normal);
                            io_context.run_for(std::chrono::seconds(1)); // Allow time for closing
                        }
                    }
                } catch (const std::exception& e) {
                    std::cerr << "Exception: " << e.what() << std::endl;
                }
            } else {
                std::cerr << "WebSocket read error: " << ec.message() << std::endl;
            }
        });

        // Run the I/O context to start processing asynchronous operations
        io_context.run();

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

// Function to check user input
void check_user_input(asio::io_context& io_context) {
    std::cout << "Enter a number to stop WebSocket communication and exit." << std::endl;
    int input;
    std::cin >> input;
    if (input != 0) {
        io_context.stop(); // Stop the io_context to interrupt the run() call
    }
}

int main() {
     // Set up the I/O context
    asio::io_context io_context;

    double launchTime = 0;    

    // Start WebSocket communication thread
    std::thread websocket_thread(run_websocket, std::ref(io_context), std::ref(launchTime));

    // Start user input checking thread
    std::thread input_thread(check_user_input, std::ref(io_context));

    // Wait for user input thread to finish
    input_thread.join();    

    // Wait for WebSocket communication thread to finish
    websocket_thread.join();

    return 0;
}
