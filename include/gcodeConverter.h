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


#ifndef GCODECONVERTER_H
#define GCODECONVERTER_H


#include <QString>
#include <QList>
#include <QMap>
#include <cmath>
#include <QPointF>


struct TestPoint{
    double x;
    double y;
    QString source;
    QString type;
    QString net;
    int pin;

};

struct Trace{
    double startX;
    double startY;
    double endX;
    double endY;
    QString net;

};


class GerberManager;

class GCodeConverter
{
public:
    explicit GCodeConverter(GerberManager* gerberManager);

    bool loadCSVFile(const QString &filePath);
    bool saveGCodeToFile(const QString &filePath, const QString &gCodeContent);
    QList<TestPoint> getTestPointsCSV();
    QList<TestPoint> getTestPointsGerber();
    QMap<QString, QList<TestPoint>> groupByNet(const QList<TestPoint> &testPoints) const;
    QString generateGCode(const QMap<QString, QList<TestPoint>> &groupedTestPoints) const;
    QMap<QString, QPair<QList<TestPoint>, QList<TestPoint>>>divideTestPointsForProbes(const QMap<QString, QList<TestPoint>>& groupedTestPoints) const;
    QMap<QString, QList<TestPoint>>prioritizeEdgesAndSingleTracePoints(const QList<TestPoint> &testPoints) const;
    int calculateTotalDistance(const TestPoint& lowerProbePosition,
                               const TestPoint& upperProbePosition,
                               const QList<TestPoint>& netTestPoints) const;
    QString selectNextNet( const QMap<QString, QPair<QList<TestPoint>, QList<TestPoint>>>& dividedTestPoints,
                          const QList<QString>& remainingNets,
                          const QPointF& lowerProbePos,
                          const QPointF& upperProbePos) const;
    void assignPointsByMinimizingLocalMaximum(
        const TestPoint& probe1, const TestPoint& probe2,
        const QList<TestPoint>& edgePoints,
        QList<TestPoint>& upperProbePoints, QList<TestPoint>& lowerProbePoints) const;

private:
    QList<TestPoint> testPointsCSV;
    QList<TestPoint> testPointsGerber;
    QList<Trace> Traces;
    GerberManager* gerberManager;
};

#endif // GCODECONVERTER_H
