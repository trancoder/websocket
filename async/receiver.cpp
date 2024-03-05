#include <iostream>
#include <boost/asio.hpp>
#include <atomic>
#include <thread>

std::atomic<bool> stopAsyncRead(false); // Atomic flag to stop async read

int main() {
    try {
        boost::system::error_code ec;

        // Create an io_context
        boost::asio::io_context io_context;

        // Create a TCP socket
        boost::asio::ip::tcp::socket socket(io_context);

        // Create an endpoint representing the server address and port
        boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 8080);

        bool weaponServerConnected = false;
        while (!weaponServerConnected) {
            socket.connect(endpoint, ec);
            if (ec) {
                std::cout << "Attempting to reconnect.\n";
                std::this_thread::sleep_for(std::chrono::seconds(1));
                // Close before reattempting to connect.
                socket.close();
            } else {
                // Once connected. Wait for timestamp message sent from the message server.
                std::cout << "Waiting for timestamp from the Weapon Server...\n";

                std::vector<char> buffer(8); // Assuming we are receiving 2 uint32_t values (8 bytes for total)

                // Receive data asynchronously
                boost::asio::async_read(socket, boost::asio::buffer(buffer),
                                         [&](const boost::system::error_code &error, std::size_t bytes_transferred) {
                                             if (!error) {
                                                 // Assuming the data received is 2 uint32_t values
                                                 if (bytes_transferred == 8) {
                                                     uint32_t sec, nsec;
                                                     std::memcpy(&sec, buffer.data(), sizeof(uint32_t));
                                                     std::memcpy(&nsec, buffer.data() + sizeof(uint32_t), sizeof(uint32_t));
                                                     std::cout << "Received time: " << sec << " seconds, " << nsec << " nanoseconds" << std::endl;
                                                 } else {
                                                     std::cerr << "Received unexpected data size: " << bytes_transferred << std::endl;
                                                 }
                                             } else {
                                                 if (error == boost::asio::error::operation_aborted) {
                                                     std::cout << "Async read operation aborted." << std::endl;
                                                 } else {
                                                     std::cerr << "Error receiving data: " << error.message() << std::endl;
                                                 }
                                             }
                                         });

                // Run the io_context to start processing asynchronous operations
                std::thread io_thread([&io_context]() { io_context.run(); });

                // Wait for flag to be set to stop async read
                while (!stopAsyncRead.load()) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }

                // Stop the io_context and join the thread
                io_context.stop();
                io_thread.join();

                weaponServerConnected = true;
            }
        }

    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}