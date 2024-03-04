import socket
import time
import struct

# Host and port to listen on
HOST = 'localhost'
PORT = 8080

def main():
    # Create a TCP/IP socket
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    
    # Allow reusing the address
    server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

    try:
        # Bind the socket to the address
        server_socket.bind((HOST, PORT))
    except socket.error as e:
        print("Error:", e)
        return
    
    # Listen for incoming connections
    server_socket.listen(1)

    print "Server listening on {}:{}".format(HOST, PORT)

    while True:
        # Accept a connection
        client_socket, client_address = server_socket.accept()
        print "Connection from {}".format(client_address)

         # Get the current time in seconds
        current_time_sec = int(time.time())

        # Get the current time in nanoseconds (if supported, otherwise just milliseconds)
        if hasattr(time, 'clock_gettime'):
            current_time_nsec = int(time.clock_gettime(time.CLOCK_REALTIME_NS))
        else:
            current_time_nsec = int(time.time() * 1e9)  # Approximate nanoseconds

        # Pack the current time in seconds and nanoseconds together
        data = struct.pack('LL', current_time_sec, current_time_nsec)

        # Send the packed data to the client
        client_socket.sendall(data)

        # Keep receiving data until the client closes the connection
        while True:
            data = client_socket.recv(1024)
            if not data:
                print("Client closed the connection.")
                break
            print("Received:", data.decode())

        # Close the connection
        client_socket.close()

        # Close the server socket if the client closes the connection
        print("Closing server socket.")
        server_socket.close()

if __name__ == "__main__":
    main()
