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



#ifndef PCB2GCODE_H
#define PCB2GCODE_H

#include "include/gcodeConverter.h"
#include "include/gerbermanager.h"
#include "include/settings.h"
#include "include/uart.h"

#include <QWidget>
#include <QPixmap>
#include <QPushButton>
#include <QLineEdit>
#include <QMessageBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QMainWindow>
#include <QTabWidget>
#include <QObject>
#include <QFileDialog>



QT_BEGIN_NAMESPACE
namespace Ui {
class PCB2Gcode;
}
QT_END_NAMESPACE

class PCB2Gcode : public QMainWindow
{
    Q_OBJECT

public:
    explicit PCB2Gcode(QWidget *parent = nullptr);
    ~PCB2Gcode();

private Q_SLOTS:
    // Gcode Converter Tab
    void onBrowseTestPoints();
    void onBrowseCopper();
    void onBrowseMask();
    void onBrowseSilk();
    void onBrowseBoard();
    void onGenerateFromGerber();
    void onGenerateFromCSV();
    void onPreview();
    void onPreviewTestPoints();
    void onZoomIn();
    void onZoomOut();
    void onZoomOriginal();
    void onDrag();
    void onSaveImage();
    void enableToolBar();
    void keyPressEvent(QKeyEvent *event);


private:
    Ui::PCB2Gcode *ui;
    Settings appSettings;
    UART *uart;
    QTabWidget* tabWidget;
    GerberManager *gerberManager;
    GCodeConverter *gcodeConverter;
    QPixmap  saveImageRendered;

    void connectSignals();
    void initUART();
    bool isImageRendered = false;
    bool isDragEnabled = false;


};

#endif // PCB2GCODE_H
