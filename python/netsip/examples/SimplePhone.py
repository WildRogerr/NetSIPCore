#
# SimplePhone example using NetSIP
#
# Copyright (C) 2026 WildRogerr
#
# This file is part of NetSIP.
#
# NetSIP is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 2
# as published by the Free Software Foundation.
#
# This example demonstrates how to use the NetSIP Python library.
#
# Features:
# - SIP registration
# - Make and receive calls
# - RTP audio playback
# - Speaker control
# - Microphone control
# - Automatic conversation mode
#

from netsip import SIPManager
import asyncio
import threading
from pathlib import Path


class SimplePhone:
    
    def __init__(self):
        self.caller = SIPManager()
        self.reg_state = False
        self.microphone = True
        self.speaker = True
        self.conversation = False
        self.answer_call = False
        self.autospeak = False
        self.subscriber = {}


    async def main(self):
        
        while True:
            print('  Simple Phone Menu')
            print('---------------------------')
            print('  1) Registration')
            print('  2) Disconnect')
            print('  3) Make Call')
            print('  4) Answer')
            print('  5) Hang Up')
            print('  6) Toggle Auto Conversation')
            print('---------------------------')
            print('  7) Mute/Unmute')
            print('  8) Microphone Off/On')
            print('  0) Exit')
            print('---------------------------')
            menu_number = await asyncio.to_thread(input, '  Enter number: ')
            
            if menu_number == "1":
                print()
                await self.registration()
                print()
            elif menu_number == "2":
                print()
                await self.disconnect()
                print()
            elif menu_number == "3":
                print()
                await self.make_call()
                print()
            elif menu_number == "4":
                print()
                await self.answer()
                print()
            elif menu_number == "5":
                print()
                await self.hangup()
                print()
            elif menu_number == "6":
                print()
                await self.send_audio()
                print()
            elif menu_number == "7":
                print()
                await self.speaker_off_on()
                print()
            elif menu_number == "8":
                print()
                await self.mic_off_on()
                print()
            elif menu_number == "0":
                await self.caller.close_sipcore()
                await asyncio.sleep(1)
                return
            
            
    async def registration(self):
        if self.reg_state:
            print('Subscriber registered!')
            return
        server = await asyncio.to_thread(input, 'Enter server: ')
        proxy = await asyncio.to_thread(input, 'Enter proxy server or skip: ')
        login = await asyncio.to_thread(input, 'Enter number: ')
        username = await asyncio.to_thread(input, 'Enter username or skip: ')
        password = await asyncio.to_thread(input, 'Enter password: ')
        self.subscriber = {
            "server":server,
            "proxy":proxy,
            "login":login,
            "username":username,
            "password":password
        }
        await self.caller.subscriber_registration(self.subscriber["server"],self.subscriber["login"],self.subscriber["password"],self.subscriber["proxy"],self.subscriber["username"])
        if self.caller.last_registration_state:
            self.reg_state = True
            asyncio.create_task(self.ringing_indicator())
            asyncio.create_task(self.answer_indicator())
            asyncio.create_task(self.hangup_indicator())
            self.audio_thread = threading.Thread(
                target=self.caller.audio_manager_thread,
                args=(self.subscriber["login"], "", ""),
                daemon=True
            )
            self.audio_thread.start()
            print('Subscriber registered.')
        else:
            print('Registration failed.')
            
            
    async def disconnect(self):
        if not self.reg_state:
            print('Subscriber not registered.')
            return
        await self.caller.subscriber_disconnect(self.subscriber["login"])
        print('Subscriber disconnected.')
        self.reg_state = False
        self.answer_call=False
        self.conversation=False
        self.autospeak=False
        self.microphone=True
        self.speaker=True
        self.caller.autospeak = False
            
            
    async def make_call(self):
        if not self.reg_state:
            print('Subscriber not registered.')
            return
        if not self.conversation:
            called_number = await asyncio.to_thread(input,'Enter called number: ')
            print(f'Calling to {called_number}.')
            await self.caller.call(self.subscriber["login"], called_number)
            
            
    async def answer(self):
        if not self.reg_state:
            print('Subscriber not registered.')
            return
        if not self.answer_call:
            await self.caller.answer(self.subscriber["login"])
        

    async def hangup(self):
        if not self.reg_state:
            print('Subscriber not registered.')
            return
        if self.conversation:
            await self.caller.hang_up(self.subscriber["login"])
            
            
    async def send_audio(self):
        if not self.reg_state:
            print('Subscriber not registered.')
            return
        if not self.autospeak:
            path = Path(await asyncio.to_thread(input, "Enter wav file path: ")).expanduser().resolve()
            await self.caller.micoff(self.subscriber["login"])
            self.microphone = False
            self.caller.wf_a = str(path)
            self.autospeak = True
            self.caller.autospeak = True
            print('Microphone Off')
            print('Auto Conversation ON')
        else:
            self.autospeak = False
            self.caller.autospeak = False
            await self.caller.micon(self.subscriber["login"])
            self.microphone = True
            print('Microphone On')
            print('Auto Conversation OFF')
            
            
    async def speaker_off_on(self):
        if not self.reg_state:
            print('Subscriber not registered.')
            return
        if self.speaker:
            await self.caller.mute(self.subscriber["login"])
            self.speaker = False
            print('Speaker Off')
        else:
            await self.caller.unmute(self.subscriber["login"])
            self.speaker = True
            print('Speaker On')

            
    async def mic_off_on(self):
        if not self.reg_state:
            print('Subscriber not registered.')
            return
        if self.microphone:
            await self.caller.micoff(self.subscriber["login"])
            self.microphone = False
            print('Microphone Off')
        else:
            await self.caller.micon(self.subscriber["login"])
            self.microphone = True
            print('Microphone On')
            
            
    async def ringing_indicator(self):
    
        while self.reg_state:

            number = self.subscriber.get("login", "")
            client = self.caller.clients.get(number)

            if client:
                if self.caller.clients[number]['current_state'] in ('incoming','ringing'):
                    remote = self.caller.clients[number]['remote_number']
                    print(f'Ringing from {remote}!')

            await asyncio.sleep(0.3)
            
            
    async def answer_indicator(self):
        
        while self.reg_state:
            
            if not self.answer_call:

                number = self.subscriber.get("login", "")
                client = self.caller.clients.get(number)
                
                if client:
                    if self.caller.clients[number]['current_state'] == 'confirmed':
                        remote = self.caller.clients[number]['remote_number']
                        print(f'Answer {remote}!')
                        self.answer_call = True
                        self.conversation = True
                        
            await asyncio.sleep(0.3)


    async def hangup_indicator(self):

        while self.reg_state:

            if self.conversation:
                number = self.subscriber.get("login", "")
                client = self.caller.clients.get(number)
                
                if client:
                    if self.caller.clients[number]['current_state'] == 'disconnected':
                        remote = self.caller.clients[number]['remote_number']
                        print(f'Hang Up {remote}!')
                        self.answer_call = False
                        self.conversation = False

            await asyncio.sleep(0.3)
        
        
        
if __name__ == "__main__":
    phone = SimplePhone()
    asyncio.run(phone.main())