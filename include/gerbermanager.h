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


#ifndef GERBERMANAGER_H
#define GERBERMANAGER_H
#undef slots
#include <pybind11/pybind11.h>
#include <pybind11/embed.h>
#include <QTemporaryFile>
#include <QPixmap>
#include <QSvgRenderer>
#include <QString>
#include <string>
#include "include/gcodeConverter.h"


namespace py = pybind11;



class GCodeConverter;

class GerberManager {
public:
    GerberManager();
    ~GerberManager();
    // PCB Bounding Box
    double minX;
    double minY;
    double maxX;
    double maxY;
    void getBoundingBox();
    bool loadGerberFiles(const QStringList& filePaths);
    void clearGerberFiles();
    QPixmap renderGerber(int dpmm=40);
    QList<TestPoint> getPadInfo();
    QList<Trace> getTraceCoords();
    QPixmap overlayTestPoints(const QPixmap& baseImage, const QList<TestPoint>& points);



private:
    py::object GerberWrapper;
    std::string tempImagePath;
    GCodeConverter* gcodeConverter;
    // Bounding Box coords


};

#endif // GERBERMANAGER_H
