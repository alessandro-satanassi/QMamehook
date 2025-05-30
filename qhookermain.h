#ifndef QHOOKERMAIN_H
#define QHOOKERMAIN_H

#include <QCoreApplication>
#include <QObject>
#include <QSettings>
#include <QTcpSocket>
#include <QTextStream>
#include <QSerialPort>
#include <QSerialPortInfo>

class qhookerMain : public QObject
{
    Q_OBJECT

private:
    QCoreApplication *mainApp;

    QSettings *settings = nullptr;

    QTcpSocket tcpSocket;

    QVector<QSerialPort*> serialPort;

    QStringList buffer;

    QByteArray gameName;

    QList<QSerialPortInfo> validDevices;

    QSet<uint32_t> validIDs;

    QHash<QString, QString> settingsMap;

    void LoadConfig(const QString &);

    void SerialInit();

    void AddNewDevices(QList<QSerialPortInfo> &);

    bool GameSearching(const QString & = "");

    bool GameStarted(const QString & = "");

    void ReadyRead();

public:
    enum {
        sortPIDascend = 0,
        sortPIDdescend,
        sortPortAscend,
        sortPortDescend
    } sortTypes_e;

    explicit qhookerMain(QObject *parent = 0);

    bool verbosity = false;

    bool customPathSet = false;

    bool closeOnDisconnect = false;

    int sortType = sortPIDascend;

    QString customPath;

    void quit();

signals:
    void finished();

public slots:
    void run();

    void aboutToQuitApp();

//private slots:

};

#endif // QHOOKERMAIN_H
