#######################
# pip install git+https://github.com/dpallot/simple-websocket-server.git
#######################

import json
import time
from SimpleWebSocketServer import SimpleWebSocketServer, WebSocket

class WebSocketServer(WebSocket):    
    def handleConnected(self):
        print("Client connected.")
        try:
            # Read JSON data from file
            with open("sample.json", "r") as file:
                json_data = file.read()

            print("Sending JSON data...")

            # Send the JSON data over the WebSocket
            self.sendMessage(json_data)

            print("JSON data sent successfully")
        except Exception as e:
            print("Error occurred: {}".format(e))        

    def handleClose(self):
        print("Client disconnected.")

def main():
    # Create WebSocket server
    server = SimpleWebSocketServer('', 54800, WebSocketServer)
    print("Server started, waiting for connections...")
    server.serveforever()

if __name__ == "__main__":
    main()
