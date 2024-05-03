# import tornado.ioloop
# import tornado.web
# import tornado.websocket

# class WebSocketHandler(tornado.websocket.WebSocketHandler):
#     def open(self):
#         print("WebSocket opened")

#     def on_message(self, message):
#         if message == "REQUEST_FILE":
#             self.send_file()

#     def on_close(self):
#         print("WebSocket closed")

#     def send_file(self):
#         chunk_size = 1024  # Send 1KB at a time
#         with open("audio.mp3", "rb") as f:
#             while (chunk := f.read(chunk_size)):
#                 self.write_message(chunk, binary=True)

# def make_app():
#     return tornado.web.Application([
#         (r"/websocket", WebSocketHandler),
#     ])

# if __name__ == "__main__":
#     app = make_app()
#     app.listen(8888)
#     print("Server is running on http://localhost:8888")
#     tornado.ioloop.IOLoop.current().start()

import tornado.ioloop
import tornado.web

class MainHandler(tornado.web.RequestHandler):
    def get(self):
        filepath = "elevenlabs_test.mp3"  # Specify the path to your MP3 file
        self.set_header('Content-Type', 'audio/mpeg')
        with open(filepath, "rb") as f:
            self.write(f.read())
        self.finish()

def make_app():
    return tornado.web.Application([
        (r"/mp3", MainHandler),
    ])

if __name__ == "__main__":
    app = make_app()
    app.listen(8888)
    print("Server is running on http://localhost:8888/mp3")
    tornado.ioloop.IOLoop.current().start()