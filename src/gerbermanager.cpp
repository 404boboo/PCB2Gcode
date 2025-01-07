#include "include/gerbermanager.h"
#include <QDebug>
#include <QFile>
#include <QPainter>
#include <QString>
namespace py = pybind11;


GerberManager::GerberManager() {
    try {
        py::initialize_interpreter();
        gcodeConverter = new GCodeConverter(this);
        PyGILState_STATE gstate = PyGILState_Ensure();

        py::module_ sys = py::module_::import("sys");

        // Log the Python executable being used
        qDebug() << "Python executable:" << QString::fromStdString(sys.attr("executable").cast<std::string>());

        // Set the PYTHONPATH environment in Python
        py::list path = sys.attr("path");
        path.append("C:/Users/ahmed/OneDrive/Desktop/Projects/PCB2Gcode/python"); // 'gerber_wrapper.py' directory

        // Log modified Python path
        qDebug() << "Python path after appending:" << QString::fromStdString(py::str(path).cast<std::string>());

        // Import the custom gerber_wrapper module
        py::module_ gerberWrapper = py::module_::import("gerber_wrapper");

        // Instantiate the GerberWrapper class
        GerberWrapper  = gerberWrapper.attr("GerberWrapper")();

        PyGILState_Release(gstate);
    }
    catch (const py::error_already_set& e) {
        qDebug() << "Python initialization error:" << QString::fromStdString(e.what());
    }
}

GerberManager::~GerberManager() {
    try {
        // Remove the temporary file if it exists
        QFile::remove(QString::fromStdString(tempImagePath));
        delete gcodeConverter;
        py::finalize_interpreter();
    }
    catch (...) {
        qDebug() << "Error finalizing Python interpreter.";
    }
}

bool GerberManager::loadGerberFiles(const QStringList& filePaths) {
    try {
        py::gil_scoped_acquire acquire;
        py::list py_file_paths;
        for (const QString& path : filePaths) {
            py_file_paths.append(path.toStdString());
        }
        GerberWrapper.attr("loadGerberFiles")(py_file_paths);
        qDebug() << "Gerber files loaded successfully.";
        return true;
    }
    catch (const py::error_already_set& e) {
        qDebug() << "Python error while loading Gerber files:" << QString::fromStdString(e.what());
        return false;
    }
}

void GerberManager::clearGerberFiles() {
    try {
        GerberWrapper.attr("clearGerberFiles")();
        qDebug() << "Cleared all loaded Gerber files.";
    } catch (const py::error_already_set& e) {
        qDebug() << "Python error while clearing Gerber files:" << QString::fromStdString(e.what());
    }
}

QPixmap GerberManager::renderGerber(int dpmm) {
    try {
        py::gil_scoped_acquire acquire;

        py::object pyRenderToPng = GerberWrapper.attr("renderToPng");
        std::string tempFilePath = pyRenderToPng(dpmm).cast<std::string>();

        if (tempFilePath.empty()) {
            qDebug() << "Failed to render the image, temporary file path is empty.";
            return QPixmap();
        }

        QPixmap pixmap;

        if (pixmap.load(QString::fromStdString(tempFilePath))){
            qDebug() << "Successfully loaded rendered PNG into QPixmap.";
            QFile::remove(QString::fromStdString(tempFilePath));
            return pixmap;
        }
        else{
            qDebug() << "Failed to laod rendered PNG into QPixmap.";
            return QPixmap();
        }

    }
    catch (const py::error_already_set& e) {
        qDebug() << "Python error during rendering:" << QString::fromStdString(e.what());
        return QPixmap();
    }
}

void GerberManager::getBoundingBox(){
    try{
        py::gil_scoped_acquire aquire;
        py::object pyGetBoundingBox = GerberWrapper.attr("getBoundingBox");
        py::object boundingBox = pyGetBoundingBox();

        if (boundingBox.is_none()) {
            qDebug() << "Bounding box is None. Cannot proceed.";
            return;
        }

        // Extract bounding box values
        minX = boundingBox["minX"].cast<double>();
        minY = boundingBox["minY"].cast<double>();
        maxX = boundingBox["maxX"].cast<double>();
        maxY = boundingBox["maxY"].cast<double>();

        qDebug() << "Bounding Box: min_x=" << minX << " min_y=" << minY << " max_x=" << maxX << " max_y=" << maxY;
    }
    catch (const py::error_already_set& e) {
        qDebug() << "Python error during fetching bounding box:" << QString::fromStdString(e.what());
    }
}

QPixmap GerberManager::overlayTestPoints(const QPixmap& baseImage, const QList<TestPoint>& points){

    if (baseImage.isNull()){
        qDebug() << "Base image is null.";
        return baseImage;
    }
    QSize imageSize = baseImage.size();
    qDebug() << "Base image size: width = " << imageSize.width() << ", height =" << imageSize.height();
    qDebug() << "Bounding box: min_x = " << minX << ", min_y = " << minY << ", max_x = " << maxX << ", max_y = " << maxY;

    double pcbbWidthMM = maxX - minX;
    double pcbHeightMM = maxY - minY;


    qDebug() << "PCB width in mm: " << pcbbWidthMM << ", height in mm: " << pcbbWidthMM;

    if (pcbbWidthMM <= 0 || pcbHeightMM <= 0) {
        qDebug() << "Invalid bounding box dimensions.";
        return baseImage;
    }

    double scaleX = imageSize.width() / pcbbWidthMM;
    double scaleY = imageSize.height() / pcbHeightMM;

    qDebug() << "ScaleX: " << scaleX << ", ScaleY: " << scaleY;


    QPixmap imageTestPoints = baseImage;
    QPainter painter(&imageTestPoints);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QPen pen(Qt::yellow, 2);   // Brighter color with thickness
    painter.setPen(pen);
    QBrush brush(Qt::yellow);
    painter.setBrush(brush);
    for (const auto& tp : points) {
        double scaledX = (tp.x * scaleX); // Already subtracted minX and mixY from GerberWrapepr so we add them for rendering
        double scaledY = imageSize.height() - (tp.y * scaleY) ;
        qDebug() << "Original coordinates (" << tp.x << "," << tp.y << ") scaled to: (" << scaledX << "," << scaledY << ")";

        if (scaledX < 0 || scaledX >= imageSize.width() || scaledY < 0 || scaledY >= imageSize.height()) {
            qDebug() << "Point out of bounds: (" << scaledX << "," << scaledY << "), Image Width: " << imageSize.width() << " Image Height: " << imageSize.height();
        }
        else {
            qDebug() << "Drawing point at scaled coordinates (" << scaledX << "," << scaledY << ")";
            painter.drawEllipse(QPointF(scaledX, scaledY), 20, 20);

            painter.drawLine(QPointF(scaledX - 10, scaledY), QPointF(scaledX + 10, scaledY)); // Horizontal line
            painter.drawLine(QPointF(scaledX, scaledY - 10), QPointF(scaledX, scaledY + 10)); // Vertical line
        }
    }
    painter.end();
    return imageTestPoints;
}


QList<TestPoint> GerberManager::getPadInfo(){
    try{
        py::gil_scoped_acquire acquire;
        py::object extractPadInfo = GerberWrapper.attr("extractPadInfo");
        py::object extractedInfo= extractPadInfo();
        QList<TestPoint> padInfo;

        if(py::isinstance<py::sequence>(extractedInfo)){ // if returned object is iterable
            for(auto item : extractedInfo){
                if(py::isinstance<py::dict>(item)){
                    py::dict padDict = item.cast<py::dict>();
                    TestPoint pad;
                    pad.x = padDict["x"].cast<double>();
                    pad.y = padDict["y"].cast<double>();
                    //pad.aperture = padDict["aperture"].cast<QString>();
                    pad.type = QString::fromStdString(padDict["type"].cast<std::string>());
                    pad.net = QString::fromStdString(padDict["net"].cast<std::string>());
                    pad.source = QString::fromStdString(padDict["source"].cast<std::string>());
                    pad.pin = padDict["pin"].cast<int>();
                    padInfo.emplace_back(pad);
                }
            }
        }
        return padInfo;

    }
    catch(const py::error_already_set& e){
        qDebug() << "Python error while extracting pad coordinates: " << QString::fromStdString(e.what());
        return {};
    }
}

QList<Trace> GerberManager::getTraceCoords(){
    try{
        py::gil_scoped_acquire acquire;
        py::object extractTraceCoords = GerberWrapper.attr("extractTraceCoords");
        py::object extractedCoords= extractTraceCoords();
        QList<Trace> traceCoords;
        if(py::isinstance<py::sequence>(extractedCoords)){
            for(auto item : extractedCoords){
                if(py::isinstance<py::dict>(item)){

                    py::dict coordinate = item.cast<py::dict>();
                    Trace trace;
                    trace.startX = coordinate["startX"].cast<double>();
                    trace.startY = coordinate["startY"].cast<double>();
                    trace.endX = coordinate["endX"].cast<double>();
                    trace.endY = coordinate["endY"].cast<double>();
                    trace.net = QString::fromStdString(coordinate["net"].cast<std::string>());
                    traceCoords.emplace_back(trace);
                    qDebug() << "StartXY: (" << trace.startX << ", " << trace.startY << ") EndXY: (" << trace.endX << ", " << trace.endY << "), Net: " << trace.net;
                }
            }
        }
        return traceCoords;
    }
    catch(const py::error_already_set& e){
        qDebug() << "Python error while extracting trace coordinates: " << QString::fromStdString(e.what());
        return {};
        }


}
