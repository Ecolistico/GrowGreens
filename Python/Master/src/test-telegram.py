import requests
from credentials import telegram

def telegram_bot_sendtext(bot_message):

    bot_token = telegram['token']
    bot_chatID = telegram['chat']
    send_text = 'https://api.telegram.org/bot' + bot_token + '/sendMessage?chat_id=' + bot_chatID + '&parse_mode=Markdown&text=' + bot_message

    response = requests.get(send_text)

    return response.json()

test = telegram_bot_sendtext("Esta es una prueba\nDe la API de telegram para interactuar con bots")
print(test)
