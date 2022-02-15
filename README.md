# ESP8266 WIFI Master Modbus RTU
ESP8266 WIFI Master Modbus_RTU

Как это работает:


Пользователь создает на своем устойстве (Смартфоне, ПК, Ноутбуке) точку доступа с именем(ssid) и паролем(password) из файла прошивки ESP8266_WIFI_Master_Modbus_RTU.ino модуль ESP8266 подключается и начинает слушать порт 4210 ожидая строку символов.


Получили строку: "07050001ff00"


07		- id устройства в сети rs485 #7

05 		- команда запись данных

0001	- к какой регистр записать

ff00	- что записать


Преобразовали в массив и добавили контрольную сумму: char[] = {0x07, 0x05, 0x00, 0x01, 0xff, 0x00, crc16_H, crc16_L}

Отправили массив в RX TX на модуль сети RS485.
