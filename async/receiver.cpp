#include <iostream>
#include <boost/asio.hpp>

int main() {
    try {
        boost::system::error_code ec;

        // Create an io_context
        boost::asio::io_context io_context;

        // Create a TCP socket
        boost::asio::ip::tcp::socket socket(io_context);

        // Create an endpoint representing the server address and port
        boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 8080);

        // Asynchronously attempt to connect to the server
        // socket.async_connect(endpoint,
        //     [&](const boost::system::error_code& error) {
        //         handle_connect(error, socket);
        //     });

        // Run the io_context to start processing asynchronous operations
        //io_context.run();

        //std::cout << "Connected to server." << std::endl;    
        
        bool weaponServerConnected = false;
        while(!weaponServerConnected)
        { 
            socket.connect(endpoint, ec);            
            if (ec)
            {            
                std::cout << "Attempting to reconnect.\n";
                std::this_thread::sleep_for(std::chrono::seconds(1));
                // Close before reattempting to connect.
                socket.close();
            }
            else
            {
                // Once connected. Wait for timestamp message sent from the message server.
                // Custom timespec struct to get timestamp from HIMARS. Only 32 bits since
                // that is the current architecture of the vehicle's software.
                std::cout << "Waiting for timestamp from the Weapon Server...\n";
                
                std::vector<char> buffer(8); // Assuming we are receiving 2 uint32_t values (8 bytes for total)

                // Receive data asynchronously        
                socket.async_read_some(boost::asio::buffer(buffer), [&](const boost::system::error_code& error, std::size_t bytes_transferred) {
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
                        std::cerr << "Error receiving data: " << error.message() << std::endl;
                    }
                });


                io_context.run();

                weaponServerConnected = true;
                
            }
        }
    

    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
