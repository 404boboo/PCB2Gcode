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


#include "include/gcodeConverter.h"
#include "include/gerbermanager.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

GCodeConverter::GCodeConverter(GerberManager* gerberManager):gerberManager(gerberManager)
{

}



bool GCodeConverter::loadCSVFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open CSV file:" << filePath;
        return false;
    }

    QTextStream in(&file);
    QString headerLine = in.readLine(); // Skip header

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList parts = line.split(',');

        if (parts.size() != 9) continue;
        TestPoint tp;
        tp.source = parts[0].remove('"');
        tp.pin = parts[1].toInt();
        tp.net = parts[2].remove('"');
        tp.x = parts[5].toDouble() - gerberManager->minX; // Subtracting boundBox o
        tp.y = parts[6].toDouble() - gerberManager->minY;
        tp.type = parts[7].remove('"');
        testPointsCSV.append(tp);
    }

    file.close();
    return true;
}
QList<TestPoint> GCodeConverter::getTestPointsCSV()
{
    return testPointsCSV;
}
QList<TestPoint> GCodeConverter::getTestPointsGerber(){
    testPointsGerber = gerberManager->getPadInfo();
    return testPointsGerber;
}

QMap<QString, QList<TestPoint>> GCodeConverter::groupByNet(const QList<TestPoint> &testPoints) const
{
    QMap<QString, QList<TestPoint>> grouped;
    for (const TestPoint &tp : testPoints) {
        grouped[tp.net].append(tp);
    }
    return grouped;
}


QString GCodeConverter::generateGCode(const QMap<QString, QList<TestPoint>>& groupedTestPoints) const
{

    double pcbbWidthMM = gerberManager->maxX - gerberManager->minX;
    double pcbHeightMM = gerberManager->maxY - gerberManager->minY;
    QString gCode;
    gCode += QString("; G54 %1 %2\n")
                 .arg(pcbbWidthMM)
                 .arg(pcbHeightMM);

    for (const QString &net : groupedTestPoints.keys()) {
        gCode += QString("; Net: %1\n").arg(net);
        const QList<TestPoint>& testPoints = groupedTestPoints[net];
        for(int i = 0; i < testPoints.size() - 1; i += 2){
            // Probe 1
            gCode += QString("G0 P1 X%1 Y%2 Z1 ;\n").arg(testPoints[i].x).arg(testPoints[i].y);

            // Probe 2
            gCode += QString("G0 P2 X%1 Y%2 Z1 ;\n").arg(testPoints[i+1].x).arg(testPoints[i+1].y);

            // Prefrom test
            gCode += "T1 ;\n";

            // Retract
            gCode += "G0 P1 Z0 ;\n";
            gCode += "G0 P2 Z0 ;\n";
        }
    }

    gCode += "M30 ; End of program\n";
    return gCode;
}



bool GCodeConverter::saveGCodeToFile(const QString &filePath, const QString &gCodeContent)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Failed to save G-code file:" << filePath;
        return false;
    }

    QTextStream out(&file);
    out << gCodeContent;
    file.close();
    return true;
}

