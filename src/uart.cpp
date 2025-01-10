/*

PCB2Gcode - A Flying Probe Tester Application
Copyright (c) 2024 Ahmed Bouras

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/


#include "include/uart.h"
#include "qdebug.h"


UART::UART(QObject *parent)
    : QObject(parent), serialPort(new QSerialPort(this))
{
    connect(serialPort, &QSerialPort::readyRead, this, [=]() {
        emit dataReceived(serialPort->readAll());
    });

    connect(serialPort, &QSerialPort::errorOccurred, this, [](QSerialPort::SerialPortError error) {
        if (error != QSerialPort::NoError)
            qWarning() << "Serial port error:" << error;
    });
}

UART::~UART()
{
    if (serialPort->isOpen())
        serialPort->close();
    delete serialPort;
}

bool UART::connectPort(const QString &portName)
{
    if (serialPort->isOpen())
        serialPort->close();

    serialPort->setPortName(portName);
    if (serialPort->open(QIODevice::ReadWrite)) {
        emit connectionStatusChanged(true);
        return true;
    }
    emit connectionStatusChanged(false);
    return false;
}

void UART::disconnectPort()
{
    if (serialPort->isOpen()) {
        serialPort->close();
        emit connectionStatusChanged(false);
    }
}

QStringList UART::availablePorts() const
{
    QStringList ports;
    for (const QSerialPortInfo &info : QSerialPortInfo::availablePorts()) {
        ports.append(info.portName());
    }
    return ports;
}

bool UART::isConnected() const
{
    return serialPort->isOpen();
}

void UART::sendData(const QByteArray &data)
{
    if (serialPort->isOpen())
        serialPort->write(data);
}
