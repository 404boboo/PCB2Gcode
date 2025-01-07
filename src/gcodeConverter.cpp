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

    QString gCode;
    gCode += "G21 ; Set units to millimeters\n";
    gCode += "G90 ; Absolute positioning\n";

    for (const QString &net : groupedTestPoints.keys()) {
        gCode += QString("; Net: %1\n").arg(net);

        for (const TestPoint &tp : groupedTestPoints[net]) {
            gCode += QString("G0 X%1 Y%2 ; Move to test point\n").arg(tp.x).arg(tp.y);
            gCode += "G1 Z-1 ; Lower probe\n";
            gCode += "G1 Z1 ; Raise probe\n";
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

