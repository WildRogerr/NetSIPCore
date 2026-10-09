import asyncio
from netsip import SIPManager

start_number = input('Enter start number: ')
end_number = input('Enter end number: ')
prefix = input('Enter prefix: ')
password = input('Enter password: ')
server = input('Enter server: ')
caller = SIPManager()


async def registration(start_number:int, end_number:int, password:str, server:str):
    for subscriber in range(start_number, end_number + 1):
        await caller.subscriber_registration(server,str(subscriber),password,wait_for_registration = False)
        print(f'Registering {subscriber}')


async def answer(number: str, timeout: int = 30) -> bool:
    print(f'Waiting for {number}')

    loop = asyncio.get_running_loop()
    deadline = loop.time() + timeout

    while loop.time() < deadline:
        state = caller.clients[number]['current_state']

        if state == 'ringing':
            print(f'Answering {number}')
            await caller.answer(number)
            return True

        if state in ('confirmed', 'disconnected'):
            print(f'{number}: call state is {state}')
            return state == 'confirmed'

        await asyncio.sleep(0.01)

    print(f'Timeout waiting for {number}')
    return False


async def call(start_number: int, end_number: int):
    await asyncio.sleep(2)

    for subscriber in range(start_number, end_number, 2):
        caller_number = str(subscriber)
        called_number = str(subscriber + 1)

        print(f'Calling {caller_number} -> {called_number}')
        await caller.call(caller_number, f'{prefix}{called_number}')

        await answer(called_number)


async def main():
    await caller.device_off()
    while True:
        await registration(int(start_number), int(end_number), password, server)
        yncall = input('Do you call?(y/n): ')
        if yncall == 'y':
            await call(int(start_number), int(end_number))
        else:
            pass
        ynrepeat = input('Do you repeat?(y/n): ')
        if ynrepeat != 'y':
            break


asyncio.run(main())