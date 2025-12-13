# Raspberry PI to ESP32 UART bidirectional link 
# recieves actuation commands from the esp32s3, sends LIDAR data back to the esp32

# next steps:
# parse webrtc specific messages
# construct messages specific to unitree go2 (using the correct protocol) -> should be fine

# figure out the user input interface 

import serial
import struct
import json
import threading
import time

START_BYTE = 0xAA

TYPE_CMD = 1
TYPE_TELEM = 2

ser = serial.Serial("/dev/serial0", 115200, timeout=0.05)

def calc_crc(data):
    """ CRC byte at the end of each message for error checking """
    crc = 0
    for b in data:
        crc ^= b
    return crc

def send_packet(pkt_type, payload_dict):
    payload = json.dumps(payload_dict).encode("utf-8")
    length = len(payload)

    # Build header
    header = bytearray()
    header.append(START_BYTE)
    header.append((length >> 8) & 0xFF)   # high byte
    header.append(length & 0xFF)         # low byte
    header.append(pkt_type)

    # CRC = xor of length bytes, type, and payload
    crc_data = header[1:] + payload
    crc = calc_crc(crc_data)

    packet = header + payload + bytes([crc])

    ser.write(packet)
    
def send_packet_raw(pkt_type: int, json_str: str):
    """
    Sends a raw JSON string as a UART-framed packet.

    Supports >256 byte JSON by using a 16-bit length field.
    """
    if not isinstance(json_str, str):
        raise ValueError("json_str must be a string")

    payload = json_str.encode("utf-8")
    length = len(payload)

    if length > 65535:
        raise ValueError("Payload too large for protocol")

    # Build header
    START_BYTE = 0xAA
    len_lsb = length & 0xFF
    len_msb = (length >> 8) & 0xFF

    header = bytes([START_BYTE, len_lsb, len_msb, pkt_type])

    # CRC over length LSB, length MSB, type, and payload
    crc_val = 0
    for b in header[1:]:  # skip START_BYTE
        crc_val ^= b
    for b in payload:
        crc_val ^= b

    packet = header + payload + bytes([crc_val])

    ser.write(packet)
    ser.flush()
    print(f"Packet: ({packet})")
    print(f"[UART] Sent raw packet ({length} bytes payload)")


def read_packets():
    expected_len = -1
    buf = bytearray()
    while True:
        b = ser.read(1)
        if not b:
            # 
            # print("nothing received")
            continue # discard byte 
        byte = b[0]

        if byte == START_BYTE and len(buf) == 0:
            buf.append(byte)
            continue # discard 
        
        # start reading a [NEW] packet 

        buf.append(byte)

        if len(buf) == 2:  # we have LENGTH (but nothing else yet)
            expected_len = buf[1]

        if len(buf) >= 3 + expected_len + 1:
            # full packet arrived
            _, length, pkt_type = buf[:3]
            payload = buf[3:-1]
            crc = buf[-1]

            if calc_crc(buf[1:-1]) == crc:
                try:
                    obj = json.loads(payload.decode())
                    print("Received:", obj)
                    # on received packet, send to the WebRTC controller 
                except:
                    print("Bad JSON from ESP")
            else:
                print("CRC error")

            buf = bytearray()

t = threading.Thread(target=read_packets) # reads packets continuously in background thread 
t.daemon = True
t.start()

# Example send:
# start the main thread 
send_packet(TYPE_CMD, {"cmd": "standup", "x": 0.1, "y": 0.0, "z": 0})
long_packet = "{\"type\":\"msg\",\"topic\":\"rt/utlidar/robot_pose\",\"data\":{\"header\":{\"stamp\":{\"sec\":1765056882,\"nanosec\":800237417},\"frame_id\":\"odom\"},\"pose\":{\"position\":{\"x\":-0.029953,\"y\":0.005216,\"z\":0.238949},\"orientation\":{\"x\":0.000776,\"y\":0.014025,\"z\":-0.029462,\"w\":-0.999467}}}}"
# long_packet = "{\"cmd\": \"standup\", \"y\": 0.1, \"x\": 0.0, \"z\": 0}"
#long_packet = "{\"type\":\"msg\",\"topic\":\"rt/lf/lowstate\",\"data\":{\"imu_state\":{\"rpy\":[-0.003042,-0.080703,-0.044255]},\"motor_state\":[{\"q\":-0.057989,\"temperature\":31,\"lost\":5,\"reserve\":[0,651]},{\"q\":1.170907,\"temperature\":30,\"lost\":7,\"reserve\":[0,651]},{\"q\":-2.655401,\"temperature\":34,\"lost\":5,\"reserve\":[0,651]},{\"q\":0.059170,\"temperature\":31,\"lost\":5,\"reserve\":[0,651]},{\"q\":1.170317,\"temperature\":28,\"lost\":5,\"reserve\":[0,651]},{\"q\":-2.645875,\"temperature\":32,\"lost\":5,\"reserve\":[0,651]},{\"q\":-0.318820,\"temperature\":88,\"lost\":7,\"reserve\":[0,651]},{\"q\":1.181408,\"temperature\":33,\"lost\":5,\"reserve\":[0,651]},{\"q\":-2.672982,\"temperature\":35,\"lost\":5,\"reserve\":[0,651]},{\"q\":0.323437,\"temperature\":91,\"lost\":6,\"reserve\":[0,651]},{\"q\":1.186949,\"temperature\":32,\"lost\":6,\"reserve\":[0,651]},{\"q\":-2.663236,\"temperature\":33,\"lost\":5,\"reserve\":[0,651]},{\"q\":0,\"temperature\":0,\"lost\":0,\"reserve\":[0,0]},{\"q\":0,\"temperature\":0,\"lost\":0,\"reserve\":[0,0]},{\"q\":0,\"temperature\":0,\"lost\":0,\"reserve\":[0,0]},{\"q\":0,\"temperature\":0,\"lost\":0,\"reserve\":[0,0]},{\"q\":0,\"temperature\":0,\"lost\":0,\"reserve\":[0,0]},{\"q\":0,\"temperature\":0,\"lost\":0,\"reserve\":[0,0]},{\"q\":0,\"temperature\":0,\"lost\":0,\"reserve\":[0,0]},{\"q\":0,\"temperature\":0,\"lost\":0,\"reserve\":[0,0]}],\"bms_state\":{\"version_high\":1,\"version_low\":11,\"soc\":86,\"current\":-1796,\"cycle\":25,\"bq_ntc\":[27,26],\"mcu_ntc\":[28,28]},\"foot_force\":[108,105,108,106],\"temperature_ntc1\":41,\"power_v\":30.615234}}"
# for i in range(10):
#     send_packet(TYPE_CMD,json.loads(long_packet))
#     time.sleep(3)

print("UART bridge running...")

while True:
    # send_packet(TYPE_CMD, {"cmd": "standup", "x": 0.1, "y": 0.0, "z": 0})
    time.sleep(1)