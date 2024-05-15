from api_utils import *
import tornado.ioloop
import tornado.web
import tornado.websocket

website_clients = []
mp3_created = False

class MainHandlerMP3(tornado.web.RequestHandler):
    def get(self):
        # if mp3_created is True:
        # mp3_created = False
        print("Sending MP3 file...")
        filepath = "elevenlabs_test.mp3"  # Specify the path to your MP3 file
        self.set_header('Content-Type', 'audio/mpeg')
        with open(filepath, "rb") as f:
            self.write(f.read())
        self.finish()

# class MainHandlerWebsite(tornado.web.RequestHandler):
#     def get(self):
#         self.render("index.html")

# # handler for user
# class WebSocketHandler(tornado.websocket.WebSocketHandler):
#     def open(self):
#         print("WebSocket opened")
#         website_clients.append(self)

#     def on_message(self, message):
#         # self.write_message("Echo: " + message)
#         update_all_clients("Echo: " + message)
#         print("Received message: {}".format(message))

#     def on_close(self):
#         print("WebSocket closed")
#         website_clients.remove(self)

# # handler for esp32
# class WebSocketHandlerESP32(tornado.websocket.WebSocketHandler):
#     def check_origin(self, origin):
#         return True

#     def open(self):
#         print("ESP32 WebSocket opened")
#         self.api_caller = Caller()
#         website_clients.append(self)

#     def on_message(self, message):
#         update_all_clients("Message: " + message)
#         print("Received message: {}".format(message))
#         #mp3_created = True
#         # print("Received base 64 image data")
#         # update_all_clients(message, bin=False)
#         self.api_caller.query(message)

#     def on_close(self):
#         print("ESP32 WebSocket closed")

# def update_all_clients(message, bin=False):
#     if bin == True:
#         for client in website_clients:
#             print('sending data...')
#             client.write_message(message, binary=True)
#     else:
#         for client in website_clients:
#             client.write_message(message)

def make_app():
    return tornado.web.Application([
    #     (r"/", MainHandlerWebsite),
    #     (r"/websocket", WebSocketHandler),
    #     (r"/websocket_esp32", WebSocketHandlerESP32),
        (r"/mp3", MainHandlerMP3),
    ])

if __name__ == "__main__":
    app = make_app()
    app.listen(8888)
    print("Server is running on http://localhost:8888/mp3")
    tornado.ioloop.IOLoop.current().start()