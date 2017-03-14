/***************************************************************************
* RxTx Test  Copyright (C) 2017  Гайдук К.С.
***************************************************************************/

#include <QCoreApplication>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QObject>

#include <stdio.h>
#include <string.h>

#include "qrxtxtest.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QRxTxTest rx_tx;
    if (rx_tx.init(argc, argv)) {
        rx_tx.openPort();
    } else return 0;

    return a.exec();
}
