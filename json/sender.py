import json
import websocket
import time

def send_json_file(file_path, uri):
    # Keep attempting to establish the WebSocket connection until successful
    while True:
        try:
            # Attempt to establish WebSocket connection
            ws = websocket.create_connection(uri)
            print("WebSocket connection established successfully.")
            break  # Break out of the loop if connection is successful
        except Exception as e:
            print("Failed to establish WebSocket connection:", str(e))
            print("Retrying in 1 seconds...")
            time.sleep(1)  # Wait for 1 seconds before retrying

    # Wait for the user to hit Enter before sending JSON data
    user_input = raw_input("Press Enter to send the JSON file:")

    # Read JSON data from file
    with open(file_path, 'r') as file:
        print("Scanning thru JSON file...")
        json_data = file.read()

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
    ws_uri = "ws://localhost:54800"  # Update with your WebSocket URI

    # Send JSON file over WebSocket
    send_json_file(json_file_path, ws_uri)
