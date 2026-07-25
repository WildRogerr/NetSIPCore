#
# NetSIP
#
# Python library for controlling the NetSIPCore SIP engine.
#
# Copyright (C) 2026 WildRogerr
#
# This file is part of NetSIP.
#
# NetSIP is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 2
# as published by the Free Software Foundation.
#
# NetSIP is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the GNU General Public License for more details.
#

import asyncio
import socket
import json
import time
import queue
import subprocess
import threading
from pathlib import Path



class SIPManager():

    def __init__(self):
        self.sipcore_log = []
        MODULE_DIR = Path(__file__).resolve().parent
        NetSIPCore = MODULE_DIR / "NetSIPCore.exe"
        self.sipcore_process = subprocess.Popen([NetSIPCore],
                                                stdout=subprocess.PIPE,
                                                stderr=subprocess.STDOUT,
                                                text=True,
                                                bufsize=1)
        threading.Thread(target=self.read_sipcore_output,daemon=True).start()

        self.start_manager = True
        self.clients = {}
        self.reg_events = {}
        self.sip_core_host = "127.0.0.1"
        self.sip_core_port = 4890
        self.socket_queue = queue.Queue()
        self.registration_state = False
        self.stop_audio = True
        self.autospeak = False
        threading.Thread(target=self.tcp_thread, daemon=True).start()

        # Default audio file paths
        self.wf_a = 'resources/subscriber_a.wav'
        self.wf_b = 'resources/subscriber_b.wav'
        self.wf_c = 'resources/subscriber_c.wav'
        
        self.play_a = False
        self.play_b = False
        self.play_c = False


    def read_sipcore_output(self):
        while self.sipcore_process.stdout is None:
            time.sleep(0.3)
            continue
        
        for line in self.sipcore_process.stdout:
            line = line.rstrip()

            if not line:
                continue

            self.sipcore_log.append(line)

            print("[SIPCORE]", line)


    async def subscriber_registration(self, server:str, number:str, password:str):
        self.registration_state = False

        event = asyncio.Event()
        self.reg_events[number] = event

        registration_data = {
            'server': server,
            'username': number,
            'password': password,
            'audio_state': 'stop',
            'current_state': 'registering',
            'command': 'registration'
        }

        client = {
            'server': server,
            'username': number,
            'audio_state': 'stop',
            'current_state': 'registering',
            'command': 'registration'
        }

        self.clients[number] = client
        await self.send_json(registration_data)

        try:
            await asyncio.wait_for(event.wait(), timeout=10)
        except asyncio.TimeoutError:
            print(f"❌ {number}: Registration timeout")
            self.reg_events.pop(number, None)
            return

        self.registration_state = True
        self.stop_audio = False


    async def subscriber_disconnect(self, number:str):
        client = self.clients.get(number)
        if client is None:
            return
        self.stop_audio = True
        self.registration_state = False
        client['command'] = 'disconnect'
        await self.send_json(client)
        if number in self.clients:
            del self.clients[number]


    async def call(self, number: str, calling_number:str):
        client = self.clients.get(number)
        if client is None:
            return
        client['command'] = 'call'
        client['remote'] = calling_number
        await self.send_json(client)

    
    async def answer(self, number: str):
        client = self.clients.get(number)
        if client is None:
            return
        client['command'] = 'answer'
        await self.send_json(client)
    
    
    async def hang_up(self, number: str):
        client = self.clients.get(number)
        if client is None:
            return
        client['command'] = 'hangup'
        await self.send_json(client)


    async def mute(self, number: str):
        client = self.clients.get(number)
        if client is None:
            return
        client['command'] = 'mute'
        client['device'] = 'speaker'
        asyncio.create_task(self.send_json(client))

    
    async def unmute(self, number: str):
        client = self.clients.get(number)
        if client is None:
            return
        client['command'] = 'unmute'
        client['device'] = 'speaker'
        asyncio.create_task(self.send_json(client))


    async def micoff(self, number: str):
        client = self.clients.get(number)
        if client is None:
            return
        client['command'] = 'mute'
        client['device'] = 'microphone'
        asyncio.create_task(self.send_json(client))


    async def micon(self, number: str):
        client = self.clients.get(number)
        if client is None:
            return
        client['command'] = 'unmute'
        client['device'] = 'microphone'
        asyncio.create_task(self.send_json(client))


    def audio_manager_thread(self, number_a:str, number_b:str, number_c:str):
        self.wf_a
        self.wf_b
        self.wf_c
        
        while not self.stop_audio:
            if not self.autospeak:
                time.sleep(0.3)
                continue

            client_a = self.clients.get(number_a)
            client_b = self.clients.get(number_b)
            client_c = self.clients.get(number_c)
            
            if (client_a
            and (client_a['current_state'] == 'confirmed' or client_a['current_state'] == 'streaming')
            and not self.play_a
            and not self.play_b
            and not self.play_c
            and client_a['audio_state'] == 'stop'):
                self.play_a = True
                asyncio.run(self.send_audio(number_a, self.wf_a))

            if client_a and client_a['audio_state'] == 'stop':
                self.play_a = False
            
            if self.stop_audio:
                self.play_a = False
                self.play_b = False
                self.play_c = False
                break
            
            if (client_b
            and (client_b['current_state'] == 'confirmed' or client_b['current_state'] == 'streaming')
            and not self.play_a
            and not self.play_b
            and not self.play_c
            and client_b['audio_state'] == 'stop'):
                self.play_b = True
                asyncio.run(self.send_audio(number_b, self.wf_b))

            if client_b and client_b['audio_state'] == 'stop':
                self.play_b = False
            
            if self.stop_audio:
                self.play_a = False
                self.play_b = False
                self.play_c = False
                break
            
            if (client_c
            and (client_c['current_state'] == 'confirmed' or client_c['current_state'] == 'streaming')
            and not self.play_a
            and not self.play_b
            and not self.play_c
            and client_c['audio_state'] == 'stop'):
                self.play_c = True
                asyncio.run(self.send_audio(number_c, self.wf_c))

            if client_c and client_c['audio_state'] == 'stop':
                self.play_c = False
            
            if self.stop_audio:
                self.play_a = False
                self.play_b = False
                self.play_c = False
                break

            time.sleep(0.3)


    async def send_audio(self, number:str, wavefile:str):
        client = self.clients.get(number)
        if client is None:
            return
        client['command'] = 'send_audio'
        client['audio_path'] = wavefile
        client['audio_state'] = 'play'
        await self.send_json(client)


    def tcp_thread(self):

        self.sock = None
        recv_buffer = ""

        while self.start_manager:

            try:

                if self.sock is None:
                    self.sock = socket.socket(
                        socket.AF_INET,
                        socket.SOCK_STREAM
                    )
                    print("CONNECTING...")
                    self.sock.connect((self.sip_core_host, self.sip_core_port))
                    self.sock.settimeout(0.5)
                    print("TCP connected")

                # RECEIVE

                try:

                    data = self.sock.recv(4096)
                    if not data:
                        raise Exception("Disconnected")

                    recv_buffer += data.decode()
                    while "\n" in recv_buffer:

                        msg, recv_buffer = recv_buffer.split("\n", 1)

                        if not msg.strip():
                            continue

                        # print("RECV:", msg)
                        data_json = json.loads(msg)
                        username = data_json['username']
                        state = data_json.get('state', '')
                        audio_state = data_json.get('audio_state', 'stop')
                        remote_number = data_json.get('remote', '')
                        client = self.clients.get(username)

                        if not client:
                            continue

                        client['current_state'] = state
                        client['audio_state'] = audio_state
                        client['remote_number'] = remote_number
                        
                        if state == "registered":
                            event = self.reg_events.get(username)

                            if event:
                                event.set()

                except socket.timeout:
                    pass

                # SEND

                try:
                    data = self.socket_queue.get_nowait()

                    msg = json.dumps(data) + "\n"

                    # print("TCP SEND:", msg)

                    self.sock.sendall(msg.encode())

                    if data['command'] == 'destroy':
                        self.start_manager = False

                except queue.Empty:
                    pass

                except Exception as e:
                    # print("SEND ERROR:", repr(e))
                    raise

            except Exception as e:
                print("TCP ERROR:", repr(e))
                time.sleep(1)
                try:
                    if self.sock:
                        self.sock.close()
                except:
                    pass
                self.sock = None


    async def send_json(self, json:dict):
        # print("QUEUE SEND:", json)
        self.socket_queue.put(json.copy())


    def close_sipcore(self):
        shutdown = {}
        shutdown['command'] = 'destroy'
        asyncio.create_task(self.send_json(shutdown))