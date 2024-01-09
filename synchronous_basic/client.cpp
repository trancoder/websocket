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
        asio::ip::tcp::resolver resolver(io_context);
        websocket::stream<asio::ip::tcp::socket> ws(io_context);

        // Resolve the server address
        auto results = resolver.resolve("localhost", "9002");

        // Connect to the server
        asio::connect(ws.next_layer(), results.begin(), results.end());
        ws.handshake("localhost", "/");

        std::cout << "Client connected to server." << std::endl;

        // Keep the client running
        io_context.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
