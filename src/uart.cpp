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

bool UART::isConnected() const
{
    return serialPort->isOpen();
}

void UART::disconnectPort()
{
    if (isConnected()) {
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

void UART::sendData(const QByteArray &data)
{
    if (serialPort->isOpen()) {
        qint64 bytesWritten = serialPort->write(data);
        if (bytesWritten == -1) {
            qWarning() << "Failed to write to port" << serialPort->portName() << ", error:" << serialPort->errorString();
        } else if (bytesWritten != data.size()) {
            qWarning() << "Failed to write all bytes to port" << serialPort->portName();
        } else {
            emit dataSent(data); // Emit the sent data signal
            qDebug() << "Sent:" << data;
        }

        // Flush to ensure data is sent immediately
        if (!serialPort->flush()) {
            qWarning() << "Failed to flush data to port" << serialPort->portName();
        }
    } else {
        qWarning() << "Attempted to send data, but serial port is not open.";
    }
}

void UART::handleReadyRead()
{
    QByteArray data = serialPort->readAll();
    emit dataReceived(data);
    qDebug() << "Received:" << data;
}


void UART::setIRUN(int irunValue, QString &driverID)
{
    if (isConnected()) {
        QByteArray command = "SET DRIVER " + driverID.toUtf8() + " IRUN " + QByteArray::number(irunValue) + "\r\n";
        sendData(command);
        qDebug() << "IRUN value set to:" << irunValue;
    } else {
        qWarning() << "Failed to set IRUN. Serial port not open.";
    }
}

void UART::setIHOLD(int iholdValue, QString &driverID)
{
    if (isConnected()) {
        QByteArray command = "SET DRIVER " + driverID.toUtf8() + " IHOLD " + QByteArray::number(iholdValue) + "\r\n";
        sendData(command);
        qDebug() << "IHOLD value set to:" << iholdValue;
    } else {
        qWarning() << "Failed to set IHOLD. Serial port not open.";
    }
}
void UART::setSendDelay(int sendDelay, QString &driverID)
{
    if (isConnected()) {
        QByteArray command = "SET DRIVER " + driverID.toUtf8() + " SENDDELAY " + QByteArray::number(sendDelay) + "\r\n";
        sendData(command);
        qDebug() << "Send delay set to:" << sendDelay;
    } else {
        qWarning() << "Failed to set Send Delay. Serial port not open.";
    }
}

void UART::setChopperMode(const QString &mode, QString &driverID)
{
    if (isConnected()) {
        QByteArray command = "SET DRIVER " + driverID.toUtf8() + " CHOPPER " + mode.toUtf8() + "\r\n";
        sendData(command);
        qDebug() << "Chopper mode set to:" << mode;
    } else {
        qWarning() << "Failed to set Chopper Mode. Serial port not open.";
    }
}

void UART::setMicrostepping(const QString &microstepping, QString &driverID)
{
    if (isConnected()) {
        QByteArray command = "SET DRIVER " + driverID.toUtf8() + " MICROSTEP " + microstepping.toUtf8() + "\r\n";
        sendData(command);
        qDebug() << "Microstepping set to:" << microstepping;
    } else {
        qWarning() << "Failed to set Microstepping. Serial port not open.";
    }
}

void UART::setVREF(double vrefValue, QString &driverID)
{
    if (vrefValue <= 0.0 || vrefValue >= 2.5) {
        qWarning() << "Invalid VREF: Must be between 0.0V and 2.5V.";
        return;
    }

    if (isConnected()) {
        QByteArray command = "SET DRIVER " + driverID.toUtf8() + " VREF " + QByteArray::number(vrefValue, 'f', 3) + "\r\n";
        sendData(command);
        qDebug() << "VREF set to:" << vrefValue << "V";
    } else {
        qWarning() << "Failed to set VREF. Serial port not open.";
    }
}


