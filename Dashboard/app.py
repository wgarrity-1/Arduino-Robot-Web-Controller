# Flask Controller - app.py
# William Garrity
# Last modified 5/6/25

# This is a simple Flask web application provides an API for interfacing
# with the Arduino robot over serial ZigBee.

# Flask imports
from flask import Flask, url_for, render_template
from flask_cors import CORS # this is to get JS integration to work
from markupsafe import escape

# for serial communication for ZigBee
import serial
import atexit

# configure serial device, change 'COM6' to current COM port for
# Zigbee controller
ser = serial.Serial('COM6', '57600', timeout=1)

# properly close serial connection upon application termination
def exit_handler():
    ser.close()
    
atexit.register(exit_handler)



app = Flask(__name__)
CORS(app) 


# reads input from serial and returns integer value
def read_serial():
    # read first byte of serial data
    serial_return_size = int.from_bytes(ser.read(), "big")
    
    # reads rest of the bytes of serial data
    serial_return_buffer = bytearray(ser.read(serial_return_size))

    return int.from_bytes(serial_return_buffer, "big")



# Dashboard homepage
@app.route('/')
def main_page():
    url_for('static', filename='dashboard.css')
    url_for('static', filename='main.css')
    url_for('static', filename='dashboard.js')
    return render_template('index.html')


# API Resources


# reads a specific sonar sensor
@app.route("/read/sonar/<sensor>")
def read_sonar(sensor):
    
    # bytearray to send to robot
    send_instruction = bytearray.fromhex('60 00 00 00')
    serial_return_buffer = bytearray()
    
    # only write if valid sensor number (0 - 15)
    if (int(sensor) < 16 and int(sensor) >= 0):
        send_instruction[0] = send_instruction[0] + int(sensor)
        ser.write(send_instruction)
    
    # return value
    return {
        "reading": f"{escape(read_serial())}"
    }


# reads a specifc sensor
@app.route("/read/sensor/<sensor>")
def read_sensor(sensor):
    
    # bytearray to send to robot
    send_instruction = bytearray.fromhex('40 00 00 00')
    
    # only write if valid sensor number (0 - 15)
    if (int(sensor) < 16 and int(sensor) >= 0):
        send_instruction[0] = send_instruction[0] + int(sensor)
        ser.write(send_instruction)
    
    # return value
    return {
        "reading": f"{escape(read_serial())}"
    }


# read general motor status
@app.route("/read/motor/all")
def read_motor_all():
    
    ser.write(bytearray.fromhex('20 00 00 00'))
    
    return_value = int(read_serial())
    return_string = ""
    
    if (return_value == 0):
        return_string = "off"
    elif (return_value == 1):
        return_string = "forward"
    elif (return_value == 2):
        return_string = "backward"
    elif (return_value == 3):
        return_string = "left"
    elif (return_value == 4):
        return_string = "right"
    else:
        return_string = "Invalid"
    
    # return value
    return {
        "reading": f"{escape(return_value)}"
    }


# read left motor speed
@app.route("/read/motor/left")
def read_motor_left():
    
    ser.write(bytearray.fromhex('22 00 00 00'))
    
    # return value
    return {
        "reading": f"{escape(read_serial())}"
    }


# read right motor speed
@app.route("/read/motor/right")
def read_motor_right():
    
    ser.write(bytearray.fromhex('21 00 00 00'))
    
    # return value
    return {
        "reading": f"{escape(read_serial())}"
    }


# read collision status
@app.route("/read/collision")
def read_collision():
    
    ser.write(bytearray.fromhex('01 00 00 00'))
    
    # return value
    return {
        "reading": f"{escape(read_serial())}"
    }




# write settings


@app.route("/write/motor/direction/forward")
def write_motor_forward():
    ser.write(bytearray.fromhex('88 00 00 00'))
    
    return {
        "sent": "yes"
    }

@app.route("/write/motor/direction/backward")
def write_motor_backward():
    ser.write(bytearray.fromhex('84 00 00 00'))
    
    return {
        "sent": "yes"
    }

@app.route("/write/motor/direction/left")
def write_motor_left():
    ser.write(bytearray.fromhex('82 00 00 00'))
    
    return {
        "sent": "yes"
    }

@app.route("/write/motor/direction/right")
def write_motor_right():
    ser.write(bytearray.fromhex('81 00 00 00'))
    
    return {
        "sent": "yes"
    }

@app.route("/write/motor/stop")
def write_motor_stop():
    ser.write(bytearray.fromhex('B0 00 00 00'))
    
    return {
        "sent": "yes"
    }

@app.route("/write/comm-reset")
def communication_reset():
    ser.write(bytearray.fromhex('FF FF FF FF'))
    
    return {
        "sent": "yes"
    }

@app.route("/write/motor/speed/left/<speed>")
def write_motor_left_speed(speed):
    
    # bytearray to send to robot
    send_instruction = bytearray.fromhex('A0 00 00 00')
    
    # only write if valid speed number (0 - 255)
    if (int(speed) < 256 and int(speed) >= 0):
        send_instruction[1] = send_instruction[1] + int(speed)
        ser.write(send_instruction)
    
    return {
        "sent": "yes"
    }


@app.route("/write/motor/speed/right/<speed>")
def write_motor_right_speed(speed):
    
    # bytearray to send to robot
    send_instruction = bytearray.fromhex('A1 00 00 00')
    
    # only write if valid speed number (0 - 255)
    if (int(speed) < 256 and int(speed) >= 0):
        send_instruction[1] = send_instruction[1] + int(speed)
        ser.write(send_instruction)
    
    return {
        "sent": "yes"
    }


if __name__ == '__main__':
    app.run(host='0.0.0.0', threaded=False, use_reloader=False, debug=False)