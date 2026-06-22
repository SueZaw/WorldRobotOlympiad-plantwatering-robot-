from flask import Flask, render_template, Response
import cv2  # OpenCV
import urllib.request  # to open and read URL
import numpy as np

# Initialize Flask app
app = Flask(__name__)

# OBJECT CLASSIFICATION PROGRAM FOR VIDEO IN IP ADDRESS

# Replace with your camera URL
# url = 'http://192.168.200.150/cam-lo.jpg'
url = 'http://172.20.10.10:8080/shot.jpg'

classNames = []
classFile = 'coco.names'
with open(classFile, 'rt') as f:
    classNames = f.read().rstrip('\n').split('\n')

configPath = 'ssd_mobilenet_v3_large_coco_2020_01_14.pbtxt'
weightsPath = 'frozen_inference_graph.pb'

net = cv2.dnn_DetectionModel(weightsPath, configPath)
net.setInputSize(320, 320)
net.setInputScale(1.0 / 127.5)
net.setInputMean((127.5, 127.5, 127.5))
net.setInputSwapRB(True)

esp8266_url = 'http://172.20.10.11/signal'  # Updated ESP8266 IP address
esp8266_no_plant_url = 'http://172.20.10.11/no_signal'  # Updated URL to signal no plant detected

potted_plant_detected = False  # Flag to track detection status

def generate_video_stream():
    global potted_plant_detected
    while True:
        imgResponse = urllib.request.urlopen(url)  # Open the URL
        imgNp = np.array(bytearray(imgResponse.read()), dtype=np.uint8)
        img = cv2.imdecode(imgNp, -1)  # Decode the image

        img = cv2.rotate(img, cv2.ROTATE_90_CLOCKWISE)  # Rotate image

        classIds, confs, bbox = net.detect(img, confThreshold=0.5)

        current_detection = False  # Track if a potted plant is detected in the current frame

        if len(classIds) != 0:
            for classId, confidence, box in zip(classIds.flatten(), confs.flatten(), bbox):
                cv2.rectangle(img, box, color=(0, 255, 0), thickness=3)  # Show detected object
                cv2.putText(img, classNames[classId - 1], (box[0] + 10, box[1] + 30), cv2.FONT_HERSHEY_COMPLEX, 1, (0, 255, 0), 2)

                # Check if detected object is a potted plant (class 64)
                if classId == 64:
                    current_detection = True
                    try:
                        urllib.request.urlopen(esp8266_url)  # Send signal to ESP8266
                        print(f"Signal sent to ESP8266 for {classNames[classId - 1]}")
                    except Exception as e:
                        print(f"Failed to send signal: {e}")
                        # Attempt to send no plant detected signal if sending plant detected signal fails
                        try:
                            urllib.request.urlopen(esp8266_no_plant_url)  # Send no plant detected signal to ESP8266
                            print("Signal sent to ESP8266: Potted plant not detected (fallback)!")
                        except Exception as fallback_e:
                            print(f"Failed to send no plant signal (fallback): {fallback_e}")

        # If no potted plant detected and status has changed
        if not current_detection and potted_plant_detected:
            try:
                urllib.request.urlopen(esp8266_no_plant_url)  # Send no plant detected signal to ESP8266
                print("Signal sent to ESP8266: Potted plant not detected!")
            except Exception as e:
                print(f"Failed to send no plant signal: {e}")

        # Update detection status
        potted_plant_detected = current_detection

        # Convert image to jpeg format for streaming
        ret, jpeg = cv2.imencode('.jpg', img)
        frame = jpeg.tobytes()
        yield (b'--frame\r\n'
               b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n\r\n')

# Define the Flask route to stream video
@app.route('/video_feed')
def video_feed():
    return Response(generate_video_stream(), mimetype='multipart/x-mixed-replace; boundary=frame')

# Define the Flask route for the main page
@app.route('/')
def index():
    return render_template('index.html')

if __name__ == "__main__":
    # Run the Flask web server
    app.run(host='0.0.0.0', port=5000, debug=True)
