import asyncio
import re
from netsip import SIPManager
caller = SIPManager()



async def registration(start_number:str,
                       end_number:str,
                       password_reg:str,
                       server_sip:str,
                       start_login:str | None = None,
                       proxy:str | None = None):

    start_match = re.search(r"\d+", start_number)
    end_match = re.search(r"\d+", end_number)
    if (start_match is not None) and (end_match is not None):
        start_number_int = int(start_match.group())
        end_number_int = int(end_match.group())
    else:
        print('Numbers invalid!')
        return

    start_login_match = None

    if start_login is not None:
        start_login_match = re.search(r"\d+", start_login)
    
    for subscriber in range(start_number_int, end_number_int + 1):
        reg_subscriber = start_number[:start_match.start()] + str(subscriber) + start_number[start_match.end():]
        reg_login = None
        if start_login is not None and start_login_match is not None:
            reg_login = (
                start_login[:start_login_match.start()]
                + str(subscriber)
                + start_login[start_login_match.end():]
            )
        await caller.subscriber_registration(server_sip,reg_subscriber,password_reg,proxy,reg_login,wait_for_registration = False)
        print(f'Registering {subscriber}')
        print(proxy)


async def answer(number: str, timeout: int = 30) -> bool:
    print(f'Waiting for {number}')

    loop = asyncio.get_running_loop()
    deadline = loop.time() + timeout
    answer_sent = False

    while loop.time() < deadline:
        client = caller.clients.get(number)

        if client is None:
            print(f'Client {number} not found')
            return False

        state = client['current_state']

        if state == 'ringing' and not answer_sent:
            print(f'Answering {number}')
            await caller.answer(number)
            answer_sent = True

        elif state == 'confirmed':
            print(f'{number}: call confirmed')
            return True

        elif state == 'disconnected':
            print(f'{number}: call disconnected')
            return False

        await asyncio.sleep(0.01)

    print(f'Timeout waiting for {number}')
    return False


async def call(start_number: str, 
               end_number: str,
               start_called_number:str, 
               end_called_number:str,
               prefix:str | None = None):
    
    await asyncio.sleep(1)

    start_number_match = re.search(r"\d+", start_number)
    end_number_match = re.search(r"\d+", end_number)
    if (start_number_match is not None) and (end_number_match is not None):
        start_number_int = int(start_number_match.group())
        end_number_int = int(end_number_match.group())
    else:
        print('Numbers invalid!')
        return

    start_called_number_match = re.search(r"\d+", start_called_number)
    end_called_number_match = re.search(r"\d+", end_called_number)
    if (start_called_number_match is not None) and (end_called_number_match is not None):
        start_called_number_int = int(start_called_number_match.group())
        end_called_number_int = int(end_called_number_match.group())
    else:
        print('Numbers invalid!')
        return

    called_number_now = start_called_number_int

    for subscriber in range(start_number_int, end_number_int + 1):
        if called_number_now <= end_called_number_int:
            caller_number = start_number[:start_number_match.start()] + str(subscriber) + start_number[start_number_match.end():]
            called_number = start_called_number[:start_called_number_match.start()] + str(called_number_now) + start_called_number[start_called_number_match.end():]
            called_number_now += 1

            print(f'Calling {caller_number} -> {called_number}')
            await caller.call(caller_number, f'{prefix}{called_number}')

            result = await answer(called_number)
            if result == False:
                print(f'No Answer {called_number}')


async def main():
    await asyncio.sleep(1)
    await caller.device_off()
    await asyncio.sleep(1)
    print('--------------------------------------------------------------------------------------------------------------')
    start_number = input('Enter start number: ')
    end_number = input('Enter end number: ')
    start_login = input('Enter start login: ').strip() or None
    password = input('Enter password: ')
    server = input('Enter server: ')
    proxy_server = input('Enter proxy "server:port": ').strip() or None
    print('--------------------------------------------------------------------------------------------------------------')
    start_called_number = input('Enter start called number: ')
    end_called_number = input('Enter end called number: ')
    start_called_login = input('Enter start called login: ').strip() or None
    called_prefix = input('Enter called prefix: ')
    called_password = input('Enter called password: ')
    called_server = input('Enter called server: ')
    called_proxy_server = input('Enter called proxy "server:port": ').strip() or None
    print('--------------------------------------------------------------------------------------------------------------')
    await registration(str(start_number), str(end_number), str(password), str(server), str(start_login), str(proxy_server))
    await registration(str(start_called_number), str(end_called_number), str(called_password), str(called_server), str(start_called_login), str(called_proxy_server)) 
    yncall = input('Do you call?(y/n): ')
    if yncall == 'y':
        await call(str(start_number), 
                   str(end_number),
                   str(start_called_number),
                   str(end_called_number),
                   str(called_prefix))
    input('Press enter to exit: ')

asyncio.run(main())