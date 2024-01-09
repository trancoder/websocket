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
        websocket::stream<asio::ip::tcp::socket> ws(io_context);

        // Connect to the server
        ws.next_layer().connect(asio::ip::tcp::endpoint(asio::ip::address::from_string("127.0.0.1"), 9002));
        ws.handshake("127.0.0.1", "/");

        // Receive timestamp message
        beast::flat_buffer buffer;
        ws.read(buffer);

        // Display received timestamp
        std::string timestamp = beast::buffers_to_string(buffer.data());
        std::cout << "Received timestamp: " << timestamp << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
