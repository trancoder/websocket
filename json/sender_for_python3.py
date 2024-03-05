import json
import asyncio
import websockets

# Define the path to the JSON file
JSON_FILE = "sample.json"

# Read JSON data from the file
def read_json_data():
    with open(JSON_FILE, "r") as file:
        return file.read()

# Define the WebSocket server handler
async def server(websocket, path):
    print("Client connected")
    try:
        # Wait for the user on the server to hit Enter
        input("Press Enter to send JSON data to the client")

        # Send JSON data from the file to the client
        json_data = read_json_data()
        await websocket.send(json_data)

        print("JSON data sent to the client")

    except websockets.exceptions.ConnectionClosedError:
        print("Client disconnected")

# Start the WebSocket server
start_server = websockets.serve(server, "localhost", 8765)

# Run the event loop
asyncio.get_event_loop().run_until_complete(start_server)
asyncio.get_event_loop().run_forever()
