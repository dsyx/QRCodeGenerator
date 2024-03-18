#ifndef SERIAL_H
#define SERIAL_H

#include <QList>
#include <QMutex>
#include <QSerialPort>
#include <QThread>
#include <QWaitCondition>

struct SerialConfig
{
    QString name;
    qint32 baudRate;
    QSerialPort::DataBits dataBits;
    QSerialPort::StopBits stopBits;
    QSerialPort::Parity parity;
    QSerialPort::FlowControl flowControl;
    int timeout;
};

class Serial : public QThread
{
    Q_OBJECT

public:
    explicit Serial(QObject *parent = nullptr);
    ~Serial();

    SerialConfig config();
    void setConfig(const SerialConfig &cfg);
    void request(const QByteArray &req);

signals:
    void setConfigResult(bool ok, const QString &description);
    void response(const QByteArray &rsp);
    void timeout(const QString &description);
    void error(const QString &description);

private:
    void run() override;
    void handleSetConfig(QSerialPort &sp);
    void handleRequest(QSerialPort &sp);

public:
    static QList<QString> availableSerialName();

private:
    bool mQuit = false;
    bool mConfigured = false;
    bool mRequested = false;
    int mTimeout = 200;
    QByteArray mRequest;
    SerialConfig mSerialConfig;
    QMutex mMutex;
    QWaitCondition mWaitCondition;
};

#endif // SERIAL_H
