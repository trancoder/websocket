import json
import websocket

def send_json_file(file_path, uri):
    # Read JSON data from file
    with open(file_path, 'r') as file:
        json_data = file.read()

    # Establish WebSocket connection
    ws = websocket.create_connection(uri)

    try:
        # Send JSON data over WebSocket
        ws.send(json_data)
        print("JSON data sent successfully.")
    except Exception as e:
        print("Failed to send JSON data:", str(e))
    finally:
        # Close WebSocket connection
        ws.close()

if __name__ == "__main__":
    # Specify the file path of the JSON file
    json_file_path = "sample.json"

    # Specify the WebSocket URI
    ws_uri = "ws://localhost:9002"  # Update with your WebSocket URI

    # Send JSON file over WebSocket
    send_json_file(json_file_path, ws_uri)
