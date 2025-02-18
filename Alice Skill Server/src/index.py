# coding: utf-8
# Импортирует поддержку UTF-8.
from __future__ import unicode_literals

# Импортируем модули для работы с JSON и логами.
import json
import logging

import sys
sys.path.insert(0, 'src')

# Импортируем модуль связи со шлюзами
import GateProcessor

import socket

# Импортируем подмодули Flask для запуска веб-сервиса.
from flask import Flask, request
app = Flask(__name__)

logging.basicConfig(filename='skill.log', level=logging.DEBUG, filemode='w')

# Хранилище данных о сессиях.
sessionStorage = {}

# Сервер для шлюза
gateproc = GateProcessor.GateProcessor(4551)

# Задаем параметры приложения Flask.
@app.route("/", methods=['POST'])

def main():
# Функция получает тело запроса и возвращает ответ.
	logging.info('Request: %r', request.json)

	response = {
	"version": request.json['version'],
		"session": request.json['session'],
		"response": {
			"end_session": False
		}
	}

	handle_dialog(request.json, response)

	logging.info('Response: %r', response)

	return json.dumps(
		response,
		ensure_ascii=False,
		indent=2
	)

# Функция для непосредственной обработки диалога.
def handle_dialog(req, res):
	user_id = req['session']['user_id']

	# Проверка существования соединения со шлюзом этого пользователя
	gate_serial = gateproc.get_gate_for(user_id)
	if (gate_serial != ''):
		if (gateproc.is_gate_connected(gate_serial)):
			if req['session']['new']:
				# Это новая сессия.
				res['response']['text'] = 'Привет! Что мне включить?'
				return;

			# Обрабатываем ответ пользователя.
			tokens = req['request']['nlu']['tokens']
			gateproc.send_tokens(gate_serial, tokens)
			answer = gateproc.recv_answer(gate_serial)
			logging.info('Gate answer: %s', answer)
			if (answer[0:2] == 'OK'):
				res['response']['text'] = 'Готово!'
			else:
				res['response']['text'] = 'Что-то пошло не так'
		else:
			res['response']['text'] = 'Нет соединения со шлюзом'
	else:
		res['response']['text'] = 'Ваше устройство еще не привязано ни к какому шлюзу.\
Следуйте инструкции по регистрации нового устройства. Ваш uID: {}'.format(user_id)


# Функция возвращает подсказки для ответа.
def get_suggests(user_id):
	session = sessionStorage[user_id]

	# Выбираем подсказки из массива.
	suggests = [
		{'title': suggest, 'hide': False}
		for suggest in session['suggests']
	]

	return suggests

if __name__ == '__main__':
	sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	sock.bind(('', 4443))
	port = sock.getsockname()[1]
	sock.close()
	app.run(ssl_context=('cert.pem', 'key.pem'), host='0.0.0.0', port=port)
