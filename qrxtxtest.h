/***************************************************************************
* RxTx Test  Copyright (C) 2017  Гайдук К.С.
***************************************************************************/

#ifndef QRXTXTEST_H
#define QRXTXTEST_H

#include <QSerialPort>
#include <QSerialPortInfo>
#include <QObject>
#include <QByteArray>
#include <QTimer>
#include <QTime>
#include <QList>
#include <QDebug>
#include <QTextCodec>

#include <iostream>

/********************************************************************/

#pragma pack(push, 1)

// Пакет, 5 байт.
typedef struct _PACKET {
    unsigned long num;  // Номер отсчета (с нуля).
                        // 4 байта, DWORD, 0 – 4.294.967.295
    unsigned char sign; // 0x55.
} PACKET, *PPACKET;

#pragma pack(pop)

/********************************************************************/

class QRxTxTest : public QObject
{
    Q_OBJECT

    QSerialPort   serial;      // COM-порт.
    QByteArray    data_recive; // Буфер приема.
    QTimer        timer;       // Таймер, по которому выполняется отправка данных / проверка буфера чтения.
    double        t;           // Время с момента старта таймера.
    int           cnt_pack_rx; // Количество принятых пакетов.
    int           cnt_pack_tx; // Количество отправленных пакетов.
    int           pack_num;    // Номер следующего отправляемого пакета (с нуля).
    QList<int>    rx_list;     // Номера полученных пакетов.
    QTime         start, stop; // Начало / окончание передачи / приема данных.

    char          port[10];    // Название порта.
    char          mode[2];     // Режим работы. t – передатчик, r – приемник.
    int           baudrate;    // Скорость в бодах.
    int           time_exch;   // Время обмена, с.
    double        div_sb;      // Делитель размера буфера.
    char          fl_ctrl[2];  // Управление потоком. f – RTS / CTS, n – без управления потоком.
    char          fn[256];     // Имя файла для сохранения номеров принятых пакетов.
    char          cont[2];     // c – непрерывный обмен, s – одноразовый.

public:
    QRxTxTest();
    void openPort();                  // Открытие порта.
    void saveToFile();                // Сохранение номеров принятых пакетов в файл.
    bool init(int argc, char **argv); // Разбор аргументов командной строки.
    void usage();                     // Вывод информации о требуемом формате командной строки.
    ~QRxTxTest();

private slots:
    void readData();           // Опустошение буфера приема.
    void timerOf();            // Обработчик прерываний таймера.
};

/********************************************************************/

#endif // QRXTXTEST_H
