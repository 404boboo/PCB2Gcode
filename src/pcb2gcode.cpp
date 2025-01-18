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


#include "include/pcb2gcode.h"
#include "ui_PCB2Gcode.h"

#include <QProgressDialog>
#include <QKeyEvent>


PCB2Gcode::PCB2Gcode(QWidget *parent) : QMainWindow(parent), ui(new Ui::PCB2Gcode), uart(new UART(this)), gerberManager(new GerberManager()), gcodeConverter(new GCodeConverter(gerberManager))
{
    ui->setupUi(this);

    initUART();
    connectSignals();

}

PCB2Gcode::~PCB2Gcode()
{
    delete ui;
    delete uart;
    delete gerberManager;
    delete gcodeConverter;
}

void PCB2Gcode::connectSignals()
{

    connect(ui->generateCSVButton, &QPushButton::clicked, this, &PCB2Gcode::onGenerateFromCSV);

    connect(ui->generateGerberButton, &QPushButton::clicked, this, &PCB2Gcode::onGenerateFromGerber);

    connect(ui->previewButton, &QPushButton::clicked, this, &PCB2Gcode::onPreview);

    connect(ui->previewTestPointsButton, &QPushButton::clicked, this, &PCB2Gcode::onPreviewTestPoints);

    connect(ui->browseButtonCopper, &QPushButton::clicked, this, &PCB2Gcode::onBrowseCopper);

    connect(ui->browseButtonMask, &QPushButton::clicked, this, &PCB2Gcode::onBrowseMask);

    connect(ui->browseButtonSilk, &QPushButton::clicked, this, &PCB2Gcode::onBrowseSilk);

    connect(ui->browseButtonBoard, &QPushButton::clicked, this, &PCB2Gcode::onBrowseBoard);

    connect(ui->browseTestPointsButton, &QPushButton::clicked, this, &PCB2Gcode::onBrowseTestPoints);

    connect(ui->zoomInButton, &QPushButton::clicked, this, &PCB2Gcode::onZoomIn);

    connect(ui->zoomOutButton, &QPushButton::clicked, this, &PCB2Gcode::onZoomOut);

    connect(ui->zoomOriginalButton, &QPushButton::clicked, this, &PCB2Gcode::onZoomOriginal);

    connect(ui->dragButton, &QPushButton::clicked, this, &PCB2Gcode::onDrag);

    connect(ui->saveImageButton, &QPushButton::clicked, this, &PCB2Gcode::onSaveImage);


}

void PCB2Gcode::enableToolBar(){
    ui->zoomInButton->setEnabled(true);
    ui->zoomOutButton->setEnabled(true);
    ui->zoomOriginalButton->setEnabled(true);
    ui->dragButton->setEnabled(true);
    ui->saveImageButton->setEnabled(true);
}


void PCB2Gcode::onBrowseTestPoints()
{
    QString TestPointsFile = QFileDialog::getOpenFileName(this, tr("Select Test Points File"));
    if (!TestPointsFile.isEmpty()) {
        ui->testPointsPath->setText(TestPointsFile);
    }
    else{
        QMessageBox::critical(this, "ERROR: FILE PATH", "Invaild file Path, seems to be empty.");
    }
}

void PCB2Gcode::onBrowseCopper()
{
    QString copperFile = QFileDialog::getOpenFileName(this, tr("Select Copper Layer Gerber File"));
    if (!copperFile.isEmpty()) {
        ui->copperPath->setText(copperFile);
    }
    else {
        QMessageBox::critical(this, "ERROR: FILE PATH", "Invalid file path. It seems to be empty.");
    }

}
void PCB2Gcode::onBrowseMask(){
    QString maskFile = QFileDialog::getOpenFileName(this, tr("Select Solder Mask Gerber File"));
    if (!maskFile.isEmpty()) {
        ui->maskPath->setText(maskFile);
    }
    else {
        QMessageBox::critical(this, "ERROR: FILE PATH", "Invalid file path. It seems to be empty.");
    }
}

void PCB2Gcode::onBrowseBoard()
{
    QString boardFile = QFileDialog::getOpenFileName(this, tr("Select Board Edge Cuts Gerber File"));
    if (!boardFile.isEmpty()) {
        ui->boardPath->setText(boardFile);
    } else {
        QMessageBox::critical(this, "ERROR: FILE PATH", "Invalid file path. It seems to be empty.");
    }
}

void PCB2Gcode::onBrowseSilk()
{
    QString silkFile = QFileDialog::getOpenFileName(this, tr("Select Silkscreen Gerber File"));
    if (!silkFile.isEmpty()) {
        ui->silkPath->setText(silkFile);
    } else {
        QMessageBox::critical(this, "ERROR: FILE PATH", "Invalid file path. It seems to be empty.");
    }

}

void PCB2Gcode::onPreview()
{
    // Clear any previously loaded files in GerberManager
    gerberManager->clearGerberFiles();

    QString copperFile = ui->copperPath->text();
    QString maskFile = ui->maskPath->text();
    QString silkFile = ui->silkPath->text();
    QString boardFile = ui->boardPath->text();

    if (copperFile.isEmpty() || maskFile.isEmpty() || silkFile.isEmpty() || boardFile.isEmpty()) {
        QMessageBox::warning(this, tr("Incomplete Selection"),
                             tr("Please select all Gerber files: Copper Layer, Mask, Silkscreen, and Board Edge Cuts."));
        return;
    }

    QStringList filePaths = { copperFile, maskFile, silkFile, boardFile };

    // Load Gerber files
    if (!gerberManager->loadGerberFiles(filePaths)) {
        QMessageBox::critical(this, tr("Loading Failed"),
                              tr("An error occurred while loading the Gerber files. Please check the logs for details."));
        return;
    }

    // Show progress dialog
    QProgressDialog progressDialog(tr("Rendering Gerber files..."), tr("Cancel"), 0, 0, this);
    progressDialog.setWindowModality(Qt::WindowModal);
    progressDialog.show();

    // Render Gerber files
    QPixmap renderedImage = gerberManager->renderGerber(40);

    progressDialog.close();

    if (renderedImage.isNull()) {
        QMessageBox::critical(this, tr("Rendering Failed"),
                              tr("An error occurred during the rendering process. Please check the logs for details."));
        return;
    }

    // Display the rendered image in the UI
    enableToolBar();
    ui->graphicsViewPreview->setScene(new QGraphicsScene(this));
    ui->graphicsViewPreview->scene()->addPixmap(renderedImage);
    ui->graphicsViewPreview->fitInView(ui->graphicsViewPreview->scene()->itemsBoundingRect(), Qt::KeepAspectRatio);
    saveImageRendered = renderedImage;
}

void PCB2Gcode::onPreviewTestPoints(){
    onPreview();
    gerberManager->getBoundingBox();
    QList<TestPoint> testPoints;
    QString testPointsFile = ui->testPointsPath->text();
    if (testPointsFile.isEmpty()) {
        testPoints = gcodeConverter->getTestPointsGerber();
        if (testPoints.isEmpty()){
            QMessageBox::critical(this, tr("Loading Test Points Failed"),
                                  tr("An error occurred while loading test points from the Gerber file. Please check the file format and contents."));
            return;
        }
    }
    else{
        if(gcodeConverter->loadCSVFile(testPointsFile)){
            testPoints = gcodeConverter->getTestPointsCSV();
            if (testPoints.isEmpty()) {
                QMessageBox::critical(this, tr("Loading Test Points Failed"),
                              tr("An error occurred while loading test points from the CSV file. Please check the file format and contents."));
                return;
            }
        }
        else{
            QMessageBox::critical(this, tr("Loading CSV File Failed"),
                                  tr("An error occurred while loading the CSV file. Please check the file format and contents(Seems to be empty)."));
            return;
        }
    }

    if (saveImageRendered.isNull()) {
        QMessageBox::critical(this, tr("Rendering Failed"),
                              tr("No rendered image found. Please render the Gerber files first."));
        return;
    }

    QPixmap finalImage = gerberManager->overlayTestPoints(saveImageRendered, testPoints);

    // Display the final image in the UI
    enableToolBar();
    ui->graphicsViewPreview->setScene(new QGraphicsScene(this));
    ui->graphicsViewPreview->scene()->addPixmap(finalImage);
    ui->graphicsViewPreview->fitInView(ui->graphicsViewPreview->scene()->itemsBoundingRect(), Qt::KeepAspectRatio);

    // Store the final rendered image for saving
    saveImageRendered = finalImage;



}

void PCB2Gcode::onGenerateFromCSV(){
    QString testPointsFile = ui->testPointsPath->text();
    QStringList copperFile = { ui->copperPath->text() };

    if (testPointsFile.isEmpty()) {
        QMessageBox::critical(this, tr("Error"), tr("Please select a test points file."));
        return;
    }

    if (copperFile.isEmpty()) {
        QMessageBox::critical(this, tr("Error"), tr("Please select a test points file."));
        return;
    }

    if (!gcodeConverter->loadCSVFile(testPointsFile)) {
        QMessageBox::critical(this, tr("Error"), tr("Failed to load test points file. Please check the file format."));
        return;
    }

    if (!gerberManager->loadGerberFiles(copperFile)) {
        QMessageBox::critical(this, tr("Error"), tr("Failed to load test points file. Please check the file format."));
        return;
    }

    // Filter and group test points
    QList<TestPoint> csvTestPoints = gcodeConverter->getTestPointsCSV();
    if (csvTestPoints.isEmpty()) {
        QMessageBox::critical(this, tr("Error"), tr("No test points found in the CSV file."));
        return;
    }
    QMap<QString, QList<TestPoint>> groupedPoints = gcodeConverter->groupByNet(csvTestPoints);

    if (groupedPoints.isEmpty()) {
        QMessageBox::critical(this, tr("Error"), tr("No nets found in the test points."));
        return;
    }


    gerberManager->getTraceCoords();
    QString gCode = gcodeConverter->generateGCode(groupedPoints);
    if (gCode.isEmpty()) {
        QMessageBox::critical(this, tr("Error"), tr("Failed to generate G-Code."));
        return;
    }

    QString savePath = QFileDialog::getSaveFileName(this, tr("Save G-Code File"), "", tr("G-Code Files (*.gcode)"));
    if (savePath.isEmpty()) {
        return;
    }

    if (!gcodeConverter->saveGCodeToFile(savePath, gCode)) {
        QMessageBox::critical(this, tr("Error"), tr("Failed to save G-code file."));
    } else {
        QMessageBox::information(this, tr("Success"), tr("G-code generated and saved successfully."));
    }
}

void PCB2Gcode::onGenerateFromGerber(){

    QString copperFile = ui->copperPath->text();

    if (copperFile.isEmpty()) {
        QMessageBox::warning(this, tr("Missing Copper Layer File"),
                             tr("Please select the Copper Layer, Mask, Silkscreen, and Board Edge Cuts."));
        return;
    }

    QStringList filePaths = { copperFile };

    // Load Gerber files
    if (!gerberManager->loadGerberFiles(filePaths)) {
        QMessageBox::critical(this, tr("Loading Failed"),
                              tr("An error occurred while loading the Gerber files. Please check the logs for details."));
        return;
    }
    QList<TestPoint> testPoints = gcodeConverter->getTestPointsGerber();
    if(testPoints.isEmpty()){
        QMessageBox::critical(this, tr("Error"), tr("No pad infromation found."));
        return;
    }
    QMap<QString, QList<TestPoint>> groupedTestPoints = gcodeConverter->groupByNet(testPoints);
    QString gCode = gcodeConverter->generateGCode(groupedTestPoints);
    gerberManager->getTraceCoords();
    QString savePath = QFileDialog::getSaveFileName(this, tr("Save G-Code File"), "", tr("G-Code Files (*.gcode)"));
    if (savePath.isEmpty()) {
        return;
    }

    if (!gcodeConverter->saveGCodeToFile(savePath, gCode)) {
        QMessageBox::critical(this, tr("Error"), tr("Failed to save G-code file."));
    } else {
        QMessageBox::information(this, tr("Success"), tr("G-code generated and saved successfully."));
    }


}


void PCB2Gcode::onZoomIn(){
    ui->graphicsViewPreview->scale(1.1, 1.1);

}
void PCB2Gcode::onZoomOut(){
    ui->graphicsViewPreview->scale(1/1.1, 1/1.1);

}
void PCB2Gcode::onZoomOriginal(){
    ui->graphicsViewPreview->resetTransform();
    if (ui->graphicsViewPreview->scene()) {
        ui->graphicsViewPreview->fitInView(ui->graphicsViewPreview->scene()->itemsBoundingRect(), Qt::KeepAspectRatio);
    }
}

void PCB2Gcode::onDrag(){
    if (isDragEnabled){
        ui->graphicsViewPreview->setDragMode(QGraphicsView::NoDrag);
        isDragEnabled = false;
    }
    else{
        ui ->graphicsViewPreview->setDragMode(QGraphicsView::ScrollHandDrag);
        isDragEnabled = true;
    }
}

void PCB2Gcode::keyPressEvent(QKeyEvent *event){
    if (event->key() == Qt::Key_Escape && isDragEnabled){
        ui->graphicsViewPreview->setDragMode(QGraphicsView::NoDrag);
        isDragEnabled = false;
    }
    else{
        return;
    }
    QWidget::keyPressEvent(event);
}

void PCB2Gcode::onSaveImage(){
    QString fileName = QFileDialog::getSaveFileName(
        this,
        tr("Save Image"),
        "",
        tr("PNG Files (*.png);;JPEG Files (*.jpg);;All Files (*)")
        );

    if (!fileName.isEmpty()){
        if (!saveImageRendered.save(fileName)){
            QMessageBox::warning(this, tr("Save Failed"), tr("Failed to save the image."));
        }
        else{
            qDebug() << "Image saved to:" << fileName;
        }
    }
    else{
        qDebug() << "Save operation canceled.";
    }
}

//// UART ////

QString PCB2Gcode::getSelectedDriverID()
{
    QString selectedDriverID = ui->driverIDComboBox->currentText();

    if (selectedDriverID.isEmpty()) {
        QMessageBox::warning(this, "Driver ID Missing", "Please select a Driver ID before proceeding.");
    }

    QString driverID = selectedDriverID.split(" ").first(); // take the first part (the number)
    return driverID;
}

void PCB2Gcode::initUART()
{
    // UART connection

    connect(uart, &UART::connectionStatusChanged, this, [=](bool connected) {
        if (connected)
            QMessageBox::information(this, "Connection", "Connected successfully.");
        else
            QMessageBox::critical(this, "Connection", "Failed to connect or disconnected.");
    });

    connect(ui->refreshButton, &QPushButton::clicked, this, [=]() {
        ui->comPortComboBox->clear();
        ui->comPortComboBox->addItems(uart->availablePorts());
    });

    connect(ui->connectButton, &QPushButton::clicked, this, [=]() {
        if (uart->isConnected()) {
            uart->disconnectPort();
            ui->connectButton->setText("Connect");
        } else {
            QString portName = ui->comPortComboBox->currentText();
            if (uart->connectPort(portName))
                ui->connectButton->setText("Disconnect");
        }
    });
    // UART COM PORTS
    ui->comPortComboBox->addItems(uart->availablePorts());
    /// Commands Logger ///

    // Handle incoming data
    connect(uart, &UART::dataReceived, this, [=](const QByteArray &data) {
        QString response = QString::fromUtf8(data);
        ui->recieveLogtextEdit->append("Received: " + response);
    });

    // Handle sent data
    connect(uart, &UART::dataSent, this, [=](const QByteArray &data) {
        QString sentCommand = QString::fromUtf8(data).trimmed();
        ui->sentLogtextEdit->append("Sent: " + sentCommand);
    });
    // Clear button
    connect(ui->clearLogsButton, &QPushButton::clicked, this, [=]() {
        ui->sentLogtextEdit->clear();
        ui->recieveLogtextEdit->clear();
    });

    // Driver IDs
    ui->driverIDComboBox->clear();
    QStringList driverIDs = {"0 (X1)", "1 (Y1)", "2 (X2)", "3 (Y2)"};
    ui->driverIDComboBox->addItems(driverIDs);
    connect(ui->driverIDComboBox, &QComboBox::currentTextChanged, this, [=](const QString &driverID) {
        qDebug() << "Driver ID selected:" << driverID;
    });

    // SET buttons

    // SET IRUN button
    connect(ui->irunButton, &QPushButton::clicked, this, [=]() {
        QString driverID = getSelectedDriverID();
        if (driverID.isEmpty()) return;

        int irunValue = ui->IRUNSpinBox->value();
        uart->setIRUN(irunValue, driverID);
    });

    // SET IHOLD button
    connect(ui->iholdButton, &QPushButton::clicked, this, [=]() {
        QString driverID = getSelectedDriverID();
        if (driverID.isEmpty()) return;

        int iholdValue = ui->IHOLDSpinBox->value();
        uart->setIHOLD(iholdValue, driverID);
    });

    // SET SendDelay button
    connect(ui->sendDelayButton, &QPushButton::clicked, this, [=]() {
        QString driverID = getSelectedDriverID();
        if (driverID.isEmpty()) return;

        int sendDelay = ui->sendDelaySpinBox->value();
        uart->setSendDelay(sendDelay, driverID);
    });

    // SET chopper mode button
    connect(ui->chopperModeButton, &QPushButton::clicked, this, [=]() {
        QString driverID = getSelectedDriverID();
        if (driverID.isEmpty()) return;

        QString chopperMode = ui->chopperModeComboBox->currentText();
        QString chopperID = chopperMode.split(" ").first();
        uart->setChopperMode(chopperMode, driverID);
    });

    // SET microstepping button
    connect(ui->microsteppingButton, &QPushButton::clicked, this, [=]() {
        QString driverID = getSelectedDriverID();
        if (driverID.isEmpty()) return;

        QString microstepping = ui->microsteppingComboBox->currentText();
        uart->setMicrostepping(microstepping, driverID);
    });

    // SET VREF button
    connect(ui->vrefButton, &QPushButton::clicked, this, [=]() {
        QString driverID = getSelectedDriverID();
        if (driverID.isEmpty()) return;

        double vrefValue = ui->changeVrefSpinBox->value();
        uart->setVREF(vrefValue, driverID);
    });

}



