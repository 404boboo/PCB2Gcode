#ifndef GCODECONVERTER_H
#define GCODECONVERTER_H


#include <QString>
#include <QList>
#include <QMap>


struct TestPointCSV {
    QString sourceRefDes;
    QString sourcePad;
    QString net;
    QString netClass;
    QString side;
    double x;
    double y;
    QString padType;
    QString footprintSide;
};

struct TestPointGerber {
    double x;
    double y;
    std::string aperture;
    std::string type;
    std::string net;
    std::string source;
    int pin;
};

class GerberManager;

class GCodeConverter
{
public:
    explicit GCodeConverter(GerberManager* gerberManager);

    bool loadCSVFile(const QString &filePath);
    QList<TestPointCSV> filterTopSidePoints() const;
    QMap<QString, QList<TestPointCSV>> groupByNet(const QList<TestPointCSV> &testPoints) const;
    QString generateGCodeFromCSV(const QMap<QString, QList<TestPointCSV>> &groupedTestPoints) const;
    bool saveGCodeToFile(const QString &filePath, const QString &gCodeContent);
    bool extractPadInfo();
    QString generateGcodeFromGerber();

private:
    QList<TestPointCSV> testPoints;
    std::vector<TestPointGerber> padInfo;
    GerberManager* gerberManager;
};

#endif // GCODECONVERTER_H
