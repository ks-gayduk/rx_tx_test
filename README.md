﻿# RxTx Test
Приложение <em>RxTx Test</em> предназначено для тестирования физического либо виртуального нуль-модемного соединения на предмет наличия потерь данных при заданных скорости обмена и размере буфера приема.

Интерфейс командной строки:
<pre>
RxTxTest COMx MODE1 BAUDRATE TIME DIVIDER FLOW_CTRL OUT MODE2

COMx      – название COM-порта
MODE1     – режим: t – передатчик, r – приемник
BAUDRATE  – скорость в бодах; должна быть кратна 5000
TIME      – время обмена в секундах
DIVIDER   – делитель размера буфера; размер буфера рассчитывается в соответствии 
            с выражением bs = BAUDRATE / (8 * DIVIDER) байт
FLOW_CTRL – управление потоком: f – аппаратное (RTS / CTS), n – нет управления
OUT       – файл для сохранения номеров принятых посылок
MODE2     – режим обмена: c – непрерывный; s – одноразовый
</pre>
Приложение в своей работе использует обмен пакетами следующей структуры:
<pre>
typedef struct _PACKET {
    unsigned long num;  // Номер отсчета (с нуля).
                        // 4 байта, DWORD, 0 – 4.294.967.295
    unsigned char sign; // 0x55.
} PACKET, *PPACKET;
</pre>
Основу программы составляет прецизионный таймер с периодом 10 мс.<br>
При обмене используется стандартный формат посылки 8-N-1. Т.о., один байт при отправке обрамляется одним стартовым и одним стоповым битами (итого, на 1 байт – 10 бит). Один пакет вышеуказанной структуры при обмене будет иметь фактический размер 50 бит. С учетом периода таймера, минимальная поддерживаемая программой скорость обмена – 5000 бод.<br>
Если задана скорость обмена, кратная 5000, то за раз в порт отправляется n = BAUDRATE / 5000 пакетов.<br>

Для проверки целостности потока данных, по окончании обмена в файл OUT сохраняются номера всех принятых пакетов (нумерация с нуля).

Приложение реализовано с применением фреймворка C++ Qt 5.8, поэтому для запуска требуются соответствующие библиотеки.

Пример использования:

Запуск приемника:

RxTxTest COM8 r 150000 70 1 f rx_data1.txt c

Запуск передатчика:

RxTxTest COM9 t 150000 60 1 f rx_data2.txt c

В итоге, будет выполнен обмен данными на скорости 150000 бод на протяжении 60 секунд.