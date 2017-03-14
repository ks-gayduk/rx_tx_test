/***************************************************************************
* RxTx Test  Copyright (C) 2017  Гайдук К.С.
***************************************************************************/

#include "qrxtxtest.h"

QRxTxTest::QRxTxTest() {
    connect(&serial, SIGNAL(readyRead()), this, SLOT(readData()));

    timer.setTimerType(Qt::PreciseTimer);
    connect(&timer, SIGNAL(timeout()), this, SLOT(timerOf()));

    t           = 0;
    cnt_pack_rx = 0;
    cnt_pack_tx = 0;
    pack_num    = 0;
}

// Разбор аргументов командной строки.
bool QRxTxTest::init(int argc, char **argv) {
    if (argc < 9) {usage(); return false;}

    char buf[20];
    strcpy(port, argv[1]);

    strcpy(mode, argv[2]);
    if ((mode[0] != 'r') && (mode[0] != 't')) {usage(); return false;}

    strcpy(buf, argv[3]);
    baudrate = QString(buf).toInt();
    if ((baudrate % 5000) != 0) {usage(); return false;}

    strcpy(buf, argv[4]);
    time_exch = QString(buf).toInt();
    if (time_exch <= 0) {usage(); return false;}

    strcpy(buf, argv[5]);
    div_sb = QString(buf).toDouble();

    strcpy(fl_ctrl, argv[6]);
    if ((fl_ctrl[0] != 'f') && (fl_ctrl[0] != 'n')) {usage(); return false;}

    strcpy(fn, argv[7]);

    strcpy(cont, argv[8]);
    if ((cont[0] != 'c') && (cont[0] != 's')) {usage(); return false;}

    return true;
}

// Вывод информации о требуемом формате командной строки.
void QRxTxTest::usage() {
    QTextCodec *codec = QTextCodec::codecForName("CP866");

    QString str = "\nRxTxTest COMx MODE1 BAUDRATE TIME DIVIDER FLOW_CTRL OUT MODE2\n\n\
COMx      - название COM-порта\n\
MODE1     - режим: t - передатчик, r - приемник\n\
BAUDRATE  - скорость в бодах; должна быть кратна 5000\n\
TIME      - время обмена в секундах\n\
DIVIDER   - делитель размера буфера; размер буфера рассчитывается \n\
            в соответствии с выражением bs = BAUDRATE / (8 * DIVIDER) байт\n\
FLOW_CTRL - управление потоком: f - аппаратное (RTS / CTS), n - нет управления\n\
OUT       - файл для сохранения номеров принятых посылок\n\
MODE2     - режим обмена: c - непрерывный; s - одноразовый\n";

    QByteArray encodedString = codec->fromUnicode(str);
    std::cout << encodedString.data();
}

void QRxTxTest::openPort() {
    serial.setPortName(port);
    serial.setBaudRate(baudrate);
    int size_buf = round(baudrate / (8 * div_sb));
    serial.setReadBufferSize(size_buf);
    if (fl_ctrl[0] == 'f') {
        serial.setFlowControl(QSerialPort::HardwareControl);
    }

    if (serial.open(QIODevice::ReadWrite)) {
        printf("\nThe port %s is opened successfully.\n", port);
        printf("Size buffer: %d bytes\n", size_buf);

        timer.start(10);
        start = QTime::currentTime();

        char buf[100];
        memset(buf, 0, 100);
        memcpy(buf, start.toString().toLatin1(), start.toString().length());
        printf("Start time of transmission: %s.%d\n", buf, start.msec());

    } else {
        printf("\nError opening port.\n");
    }
}

QRxTxTest::~QRxTxTest() {
    serial.close();
}

// Опустошение буфера приема.
void QRxTxTest::readData() {
    data_recive.append(serial.readAll());
}

// Обработчик прерываний таймера.
void QRxTxTest::timerOf() {

    t += 0.01;

    PACKET packet;

    if (mode[0] == 'r') {
        // Прием.
        while (data_recive.count() >= (int)sizeof(PACKET)) {
            memcpy(&packet, data_recive.data(), sizeof(PACKET));
            data_recive.remove(0, sizeof(PACKET));

            if (packet.sign == 0x55) {
                cnt_pack_rx++;
                rx_list << packet.num;

                if (cont[0] != 'c') {
                    timer.stop();
                    stop = QTime::currentTime();

                    printf("End of data exchange.\n");
                    printf("Packets send:     %d\n", cnt_pack_tx);
                    printf("Packets received: %d\n", cnt_pack_rx);

                    char buf[100];
                    memset(buf, 0, 100);
                    memcpy(buf, stop.toString().toLatin1(), stop.toString().length());

                    printf("End time of transmission: %s.%d\n", buf, stop.msec());
                    int dt = start.msecsTo(stop);

                    printf("dt: %d\n", dt);

                    saveToFile();
                    exit(0);
                }
            }
        }
    } else if (mode[0] == 't') {
        // Передача.
        packet.num = pack_num;
        packet.sign = 0x55;

        if (cont[0] == 'c') {
            int k = round(baudrate / 5000);
            for (int i = 0; i < k; i++) {
                cnt_pack_tx++;
                pack_num++;
                packet.num = pack_num;
                serial.write((char*)&packet, sizeof(PACKET));
            }
        } else {
            cnt_pack_tx++;
            pack_num++;
            packet.num = pack_num;
            serial.write((char*)&packet, sizeof(PACKET));

            timer.stop();
            stop = QTime::currentTime();

            printf("End of data exchange.\n");
            printf("Packets send:     %d\n", cnt_pack_tx);
            printf("Packets received: %d\n", cnt_pack_rx);

            char buf[100];
            memset(buf, 0, 100);
            memcpy(buf, stop.toString().toLatin1(), stop.toString().length());

            printf("End time of transmission: %s.%d\n", buf, stop.msec());
            int dt = start.msecsTo(stop);

            printf("dt: %d\n", dt);

            saveToFile();
            exit(0);
        }
    }

    if (fabs(t - time_exch) < 0.001) {
        timer.stop();
        stop = QTime::currentTime();

        printf("End of data exchange.\n");
        printf("Packets send:     %d\n", cnt_pack_tx);
        printf("Packets received: %d\n", cnt_pack_rx);

        char buf[100];
        memset(buf, 0, 100);
        memcpy(buf, stop.toString().toLatin1(), stop.toString().length());

        printf("End time of transmission: %s.%d\n", buf, stop.msec());
        int dt = start.msecsTo(stop);

        printf("dt: %d\n", dt);

        saveToFile();
        exit(0);
    }
}

void QRxTxTest::saveToFile() {
    FILE *pf = fopen(fn, "w");

    // Сохранение в файл номеров полученных пакетов.
    for (int i = 0; i < rx_list.count(); i++) {
        fprintf(pf, "%d\n", rx_list.at(i));
    }

    fclose(pf);
}
