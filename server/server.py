import tornado.ioloop
import tornado.web
import tornado.websocket
from api_utils import *

website_clients = []
image_num = 0

class MainHandler(tornado.web.RequestHandler):
    def get(self):
        # self.render("index.html")
        self.render("webcam.html")

# handler for user
class WebSocketHandler(tornado.websocket.WebSocketHandler):
    def open(self):
        print("WebSocket opened")
        website_clients.append(self)

    def on_message(self, message):
        # self.write_message("Echo: " + message)
        update_all_clients("Echo: " + message)
        print("Received message: {}".format(message))

    def on_close(self):
        print("WebSocket closed")
        website_clients.remove(self)

# handler for esp32
class WebSocketHandlerESP32(tornado.websocket.WebSocketHandler):
    def check_origin(self, origin):
        return True

    def open(self):
        print("ESP32 WebSocket opened")
        self.api_caller = Caller()

    def on_message(self, message):
        global image_num
        print("Received JPEG image data {}".format(image_num))
        if image_num <= 60:
            self.save_image(message, image_num)
        if image_num % 10 == 0:
            update_all_clients(message, bin=True)
        image_num += 1
        # self.api_caller.query(message)

    # def on_message(self, message):
    #     self.write_message("ESP32: " + message)
    #     print("Received message: {}".format(message))
    #     update_all_clients(message)

    def save_image(self, image_data, num):
        # Specify the directory and filename to save the image
        save_directory = 'y-lab'
        filename = 'y-lab-{}.jpeg'.format(num)

        # Ensure the directory exists
        if not os.path.exists(save_directory):
            os.makedirs(save_directory)

        # Construct the full path
        file_path = os.path.join(save_directory, filename)

        # Write the image data to a file
        with open(file_path, 'wb') as f:
            f.write(image_data)
        print(f"Image saved to {file_path}")


    def on_close(self):
        print("ESP32 WebSocket closed")

def make_app():
    # multiple endpoints
    return tornado.web.Application([
        (r"/", MainHandler),
        (r"/websocket", WebSocketHandler),
        (r"/websocket_esp32", WebSocketHandlerESP32)
    ])

def update_all_clients(message, bin=False):
    if bin == True:
        for client in website_clients:
            print('sending data...')
            client.write_message(message, binary=True)
    else:
        for client in website_clients:
            client.write_message(message)

if __name__ == "__main__":
    app = make_app()
    app.listen(8888)
    print("Server is running at http://10.105.100.183:8888")      # eduroam
    # print("Server is running at http://172.20.20.20:8888")          # 828
    # print("Server is running at http://3.144.73.255:8888")        # aws
    # print("Server is running at http://192.168.4.82:8888")        # asbury
    tornado.ioloop.IOLoop.current().start()
