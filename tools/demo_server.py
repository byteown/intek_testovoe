from datetime import datetime
from http.server import BaseHTTPRequestHandler, HTTPServer

import json
import sys


sys.stdout.reconfigure(encoding="utf-8")


class DemoHandler(BaseHTTPRequestHandler):
    def do_POST(self):
        body_length = int(self.headers.get("Content-Length", 0))
        raw = self.rfile.read(body_length)
        try:
            content = json.loads(raw.decode("utf-8"))
            info = f"{datetime.now().strftime('%Y-%m-%d, %H:%M:%S')} | {content['agent_id']} | {len(content['payload'])}"
            print(info)
            print(json.dumps(content, ensure_ascii=False, indent=2))
        except UnicodeDecodeError:
            print("Unicode Decode Error")
            self.send_response(400)
            self.end_headers()
            return
        except json.JSONDecodeError:
            print("JSON Decode Error")
            self.send_response(400)
            self.end_headers()
            return
        except KeyError:
            print("Key Error")
            self.send_response(400)
            self.end_headers()
            return

        body = b'{"status":"ok"}'
        self.send_response(200)
        self.send_header("Content-Length", len(body))
        self.send_header("Content-Type", "application/json")
        self.end_headers()
        self.wfile.write(body)

    def log_message(self, format, *args):
        pass


if __name__ == "__main__":
    server = HTTPServer(("127.0.0.1", 8080), DemoHandler)
    
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        print("Server is gonna close")
