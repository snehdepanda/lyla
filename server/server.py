import tornado.ioloop
import tornado.web
import tornado.websocket
from api_utils import *
import cv2 as cv

website_clients = []

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
        self.type = 0
        self.img_num = 0
        self.r = 240/96
        self.dir = '../../datasets/test'
        self.f_name = 'test-{}.jpeg'

    def on_message(self, message):
        # image
        if self.type == 0:
            if isinstance(message, str) or len(message) < 5:
                pass
            else:
                print("Received JPEG image data {}".format(self.img_num))
                self.save_image(message)
                # if self.img_num % 5 == 0:
                #     update_all_clients(message, bin=True)
                self.img_num += 1
                update_all_clients(message, bin=True)
                self.type = 1
        
        # 1: label or not found, 2: x, 3: y, 4: width, 5: height
        elif self.type == 1:
            if message == "reset":
                print('reset')
                self.type = 0
            else:
                self.label = message
                self.type = 2
        elif self.type == 2:
            self.x = list(message)[0]
            self.type = 3
        elif self.type == 3:
            self.y = list(message)[0]
            self.type = 4
        elif self.type == 4:
            self.width = list(message)[0]
            self.type = 5
        elif self.type == 5:
            self.height = list(message)[0]
            print('label: {}, x: {}, y: {}, width: {}, height: {}'.format(self.label, self.x, self.y, self.width, self.height))
            f_path = os.path.join(self.dir, self.f_name.format(self.img_num-1))

            img = cv.imread(f_path)
            x, y, = int(self.x*self.r), int(self.y*self.r)
            end_x, end_y = int(x + self.width*self.r), int(y + self.height*self.r)
            print(x,y,end_x,end_y, self.r)
            color = (255,0,0)
            img = cv.rectangle(img, (x,y), (end_x,end_y), color, 2)
            img = cv.putText(img, self.label, (x,y-5), cv.FONT_HERSHEY_SIMPLEX, 1, color, 2)
            cv.imwrite(f_path, img)
            with open(f_path, 'rb') as file:
                byte_array = file.read()
            print(byte_array[:2], byte_array[-2:])
            update_all_clients(byte_array, bin=True)
            self.type = 0

        # self.api_caller.query(message)


    def save_image(self, image_data):
        # Specify the directory and filename to save the image

        # Ensure the directory exists
        if not os.path.exists(self.dir):
            os.makedirs(self.dir)

        # Construct the full path
        file_path = os.path.join(self.dir, self.f_name.format(self.img_num))

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
