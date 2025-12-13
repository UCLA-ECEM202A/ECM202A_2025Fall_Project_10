import asyncio
import json
import logging
import base64
import serial
import struct
import threading
import time
from typing import Callable, Optional, Any, Dict, Union

from webrtc_go2_connection import Go2Connection, Go2ConnectionError
from constants import RTC_TOPIC, ROBOT_CMD
from command_generator import gen_command, gen_mov_command

START_BYTE = 0xAA

TYPE_CMD = 1
TYPE_TELEM = 2

ser = serial.Serial("/dev/serial0", 115200, timeout=0.05)

logging.basicConfig(level=logging.INFO)
logger = logging.getLogger("go2")

class Go2Client:
    def __init__(self, ip, token):
        self.ip = ip
        self.token = token
        self.conn: Go2Connection | None = None
        self.data_channel = None
        self.loop = None

    async def connect(self):
        """Initialize WebRTC connection and register callbacks."""
        self.conn = Go2Connection(
            robot_ip=self.ip,
            token=self.token,
            on_validated=self._on_validated,
            on_message=self._on_message,
            robot_num=0,
            on_video_frame=None,         # add if needed
            decode_lidar=True
        )

        logging.info("Connecting to Go2 via WebRTC...")
        await self.conn.connect()
        logging.info("Connected! Press Ctrl+C to disconnect safely. ")

    #
    # ----------------------------
    # CALLBACKS
    # ----------------------------
    #

    def _on_validated(self, robot_id=None):
        """Called when SCTP / DTLS / ICE completes and datachannel is open."""
        logging.info("DataChannel validated — ready for commands.")

        self.data_channel = self.conn.data_channel

        logger.info("Subscribing to relevant topics...")
        for topic in RTC_TOPIC.values():
            self.data_channel.send(
            json.dumps({"type": "subscribe", "topic": topic}))

        # Example: stand up automatically
        asyncio.create_task(self.stand_down())
        # asyncio.create_task(self.stand_up())
        # asyncio.create_task(self.move())

    def _on_message(self, _, msg, robot_id=None):
        """Raw incoming WebRTC payloads."""
        # logging.info(f"Got message: {msg}")

    #
    # ----------------------------
    # COMMAND HELPERS
    # ----------------------------
    #
    async def send_esp_command(self, cmd):
        """ send command directly from the UART port from esp32 """
        if hasattr(self.conn, 'data_channel') and self.data_channel:
            # maybe validate message structure as well
            
            logging.info(f"Sending message: {cmd}")
            self.data_channel.send(cmd)
        else:
            logging.warning("Failed to send command, no data channel available")

    async def stand_up(self):
        """Send stand-up command."""
        if hasattr(self.conn, 'data_channel') and self.data_channel:
            logging.info("Sending stand-up command...")
            cmd = gen_command(ROBOT_CMD["Hello"], command_id=2082)
            logging.info(f"Sending message: {cmd}")
            self.data_channel.send(cmd)
        else:
            logging.warning("Failed to send stand-up command, no data channel available")
            
    async def stand_down(self):
        """Send stand-up command."""
        if hasattr(self.conn, 'data_channel') and self.data_channel:
            logging.info("Sending stand-down command...")
            cmd = gen_command(ROBOT_CMD["StandDown"])
            logging.info(f"Sending message: {cmd}")
            self.data_channel.send(cmd)
        else:
            logging.warning("Failed to send stand-up command, no data channel available")
            
    async def move(self):
        """Send move command."""
        if hasattr(self.conn, 'data_channel') and self.data_channel:
            logging.info("Sending move command...")
            cmd = gen_mov_command(x=-0.3,y=-0.3,z=-0.2)
            logging.info(f"Sending message: {cmd}")
            self.data_channel.send(cmd)
        else:
            logging.warning("Failed to send move command, no data channel available")

    async def walk(self, vx, vy, yaw):
        """Send movement command."""
        payload = {
            "type": "cmd",
            "cmd": "move",
            "vx": vx,
            "vy": vy,
            "yaw": yaw
        }
        self.data_channel.send(json.dumps(payload))

    async def shutdown(self):
        """ graceful shutdown """
        print("Go2Client: Starting shutdown")

        # Close data channel safely
        if self.conn is not None and self.conn.data_channel is not None:
            try:
                await self.data_channel.close()
                print("Closed data channel.")
            except Exception as e:
                print(f"Error closing data channel: {e}")
        else:
            print("Data channel is closed / was never open.")

        # Close peer connection safely
        if self.conn is not None and self.conn.pc is not None:
            try:
                await self.pc.close()
                print("Closed peer connection.")
            except Exception as e:
                print(f"Error closing peer connection: {e}")
        else:
            print("Peer connection is closed / was never open.")

        # Close any other transports
        if hasattr(self, "video_track") and self.video_track:
            try:
                await self.video_track.stop()
            except Exception:
                pass

        print("Shutdown complete.")
        
    def calc_crc(self, data):
        """ CRC byte at the end of each message for error checking """
        crc = 0
        for b in data:
            crc ^= b
        return crc

    def send_packet(self, pkt_type, payload_dict):
        payload = json.dumps(payload_dict).encode("utf-8")
        length = len(payload)
        header = bytes([START_BYTE, length, pkt_type])
        crc = self.calc_crc(header[1:] + payload)
        ser.write(header + payload + bytes([crc]))

    def read_packets(self):
        expected_len = -1
        buf = bytearray()
        while True:
            b = ser.read(1)
            if not b:
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

                if self.calc_crc(buf[1:-1]) == crc:
                    try:
                        obj = json.loads(payload.decode())
                        logging.info("Received:", obj)
                        buf = bytearray() # reset the buffer regardless of command corruption
                        asyncio.run_coroutine_threadsafe(
                            self.send_esp_command(payload.decode()),
                            self.loop  # must store loop reference!
                        )
                    except:
                        print("Bad JSON from ESP")
                else:
                    print("CRC error")

                buf = bytearray()

# Example send:
# start the main thread 
# send_packet(TYPE_CMD, {"cmd": "standup", "x": 0.1, "y": 0.0, "z": 0})

# print("UART bridge running...")
#
# ----------------------------
# MAIN LOOP
# ----------------------------
#

async def main():
    
    client = Go2Client(
        ip="192.168.0.139",
        token=""
    )
    t = threading.Thread(target=client.read_packets) # reads packets continuously in background thread 
    t.daemon = True
    t.start()
    client.loop = asyncio.get_running_loop()
    await client.connect()

    # Main control loop
    try:
        while True:
            # Example: send walk commands periodically
            # await client.walk(0.1, 0, 0)   # slowly forward
            await asyncio.sleep(0.1)

    except asyncio.CancelledError:
        print("Received shutdown request... cleaning up")
        await client.shutdown() 
        print("Cleanup complete")
        raise   


if __name__ == "__main__":
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        print("Program terminated by user.")
