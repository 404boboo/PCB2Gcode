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


#ifndef UART_H
#define UART_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QStringList>


class UART : public QObject
{
    Q_OBJECT

public:
    explicit UART(QObject *parent = nullptr);
    ~UART();

    bool connectPort(const QString &portName);
    void disconnectPort();
    QStringList availablePorts() const;
    bool isConnected() const;
    void sendData(const QByteArray &data);
    void setIRUN(int irunValue, QString &driverID);
    void setIHOLD(int iholdValue, QString &driverID);
    void setSendDelay(int sendDelay, QString &driverID);
    void setChopperMode(const QString &mode, QString &driverID);
    void setMicrostepping(const QString &microstepping, QString &driverID);
    void setVREF(double vrefValue, QString &driverID);

Q_SIGNALS:
    void connectionStatusChanged(bool Connected);
    void dataReceived(const QByteArray &data);

private:
    QSerialPort *serialPort;
};

#endif // UART_H
