#include "Serial.h"
#include <QDebug>
#include <QMutexLocker>
#include <QSerialPortInfo>

Serial::Serial(QObject *parent)
    : QThread{parent}
{
    start();
}

Serial::~Serial()
{
    mMutex.lock();
    mQuit = true;
    mWaitCondition.wakeOne();
    mMutex.unlock();
    wait();
}

SerialConfig Serial::config()
{
    const QMutexLocker locker(&mMutex);
    return mSerialConfig;
}

void Serial::setConfig(const SerialConfig &cfg)
{
    const QMutexLocker locker(&mMutex);

    mSerialConfig = cfg;
    mConfigured = true;

    if (!isRunning()) {
        start();
    } else {
        mWaitCondition.wakeOne();
    }
}

void Serial::request(const QByteArray &req)
{
    const QMutexLocker locker(&mMutex);

    mRequest = req;
    mRequested = true;

    if (!isRunning()) {
        start();
    } else {
        mWaitCondition.wakeOne();
    }
}

void Serial::run()
{
    QSerialPort sp;

    mMutex.lock();
    mSerialConfig = SerialConfig{
        sp.portName(),
        sp.baudRate(),
        sp.dataBits(),
        sp.stopBits(),
        sp.parity(),
        sp.flowControl(),
        mTimeout,
    };
    mMutex.unlock();

    while (!mQuit) {
        const QMutexLocker locker(&mMutex);
        mWaitCondition.wait(&mMutex);

        handleSetConfig(sp);
        handleRequest(sp);
    }
}

void Serial::handleSetConfig(QSerialPort &sp)
{
    if (!mConfigured) {
        return;
    }
    mConfigured = false;

    if (sp.isOpen()) {
        sp.close();
    }

    sp.setPortName(mSerialConfig.name);
    if (!sp.setBaudRate(mSerialConfig.baudRate)) {
        goto failure;
    }
    if (!sp.setDataBits(mSerialConfig.dataBits)) {
        goto failure;
    }
    if (!sp.setStopBits(mSerialConfig.stopBits)) {
        goto failure;
    }
    if (!sp.setParity(mSerialConfig.parity)) {
        goto failure;
    }
    if (!sp.setFlowControl(mSerialConfig.flowControl)) {
        goto failure;
    }
    mTimeout = mSerialConfig.timeout;

    emit setConfigResult(true, QString());
    return;

failure:
    mSerialConfig = SerialConfig{
        sp.portName(),
        sp.baudRate(),
        sp.dataBits(),
        sp.stopBits(),
        sp.parity(),
        sp.flowControl(),
        mTimeout,
    };
    emit setConfigResult(false, QString("Can't set %1: %2").arg(sp.portName(), sp.errorString()));
}

void Serial::handleRequest(QSerialPort &sp)
{
    if (!mRequested || mRequest.isEmpty()) {
        return;
    }
    mRequested = false;

    if (!sp.isOpen()) {
        if (!sp.open(QIODevice::ReadWrite)) {
            emit error(QString("Can't open %1: %2").arg(sp.portName(), sp.errorString()));
            return;
        }
    }

    sp.write(mRequest);
    if (!sp.waitForBytesWritten(mTimeout)) {
        emit timeout(QString("%1: Wait write request timeout.").arg(sp.portName()));
        return;
    }
    if (!sp.waitForReadyRead(mTimeout)) {
        emit timeout(QString("%1: Wait read response timeout.").arg(sp.portName()));
        return;
    }
    QByteArray rsp{sp.readAll()};
    while (sp.waitForReadyRead(20)) {
        rsp += sp.readAll();
    }
    emit response(rsp);
}

QList<QString> Serial::availableSerialName()
{
    const QList<QSerialPortInfo> infos{QSerialPortInfo::availablePorts()};
    QList<QString> names;
    for (const auto &info : infos) {
        names.push_back(info.portName());
    }
    return names;
}
