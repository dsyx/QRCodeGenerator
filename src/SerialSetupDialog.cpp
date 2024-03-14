#include "SerialSetupDialog.h"
#include <QFormLayout>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QList>
#include <QMessageBox>
#include <QSerialPortInfo>
#include <QSpacerItem>
#include <QVBoxLayout>

SerialSetupDialog::SerialSetupDialog(QSharedPointer<QSerialPort> sp, QWidget *parent)
    : QDialog{parent}
    , mSerialPort{sp}
    , mSerialNameLabel{new QLabel{QStringLiteral("Serial Port:")}}
    , mSerialNameComboBox{new QComboBox{}}
    , mBaudRateLabel{new QLabel{QStringLiteral("BaudRate:")}}
    , mBaudRateComboBox{new QComboBox{}}
    , mDataBitsLabel{new QLabel{QStringLiteral("Data Bits:")}}
    , mDataBitsComboBox{new QComboBox{}}
    , mStopBitsLabel{new QLabel{QStringLiteral("Stop Bits:")}}
    , mStopBitsComboBox{new QComboBox{}}
    , mParityLabel{new QLabel{QStringLiteral("Parity:")}}
    , mParityComboBox{new QComboBox{}}
    , mFlowControlLabel{new QLabel{QStringLiteral("Flow Control:")}}
    , mFlowControlComboBox{new QComboBox{}}
    , mReadTimeoutLabel{new QLabel{QStringLiteral("Read Timeout:")}}
    , mReadTimeoutSpinBox{new QSpinBox{}}
    , mOkButton{new QPushButton{QStringLiteral("OK")}}
    , mCancelButton{new QPushButton{QStringLiteral("Cancel")}}
{
    // SerialName
    {
        const QList<QSerialPortInfo> infos{QSerialPortInfo::availablePorts()};
        for (const auto &info : infos) {
            mSerialNameComboBox->addItem(info.portName());
        }
        const QString name{mSerialPort->portName()};
        name.isEmpty() ? mSerialNameComboBox->setCurrentIndex(-1)
                       : mSerialNameComboBox->setCurrentText(name);
    }

    // BaudRate
    {
        mBaudRateComboBox->addItem(QStringLiteral("1200"));
        mBaudRateComboBox->addItem(QStringLiteral("2400"));
        mBaudRateComboBox->addItem(QStringLiteral("4800"));
        mBaudRateComboBox->addItem(QStringLiteral("9600"));
        mBaudRateComboBox->addItem(QStringLiteral("19200"));
        mBaudRateComboBox->addItem(QStringLiteral("38400"));
        mBaudRateComboBox->addItem(QStringLiteral("57600"));
        mBaudRateComboBox->addItem(QStringLiteral("115200"));
        mBaudRateComboBox->setEditable(true);
        mBaudRateComboBox->setValidator(new QIntValidator{mBaudRateComboBox});
        mBaudRateComboBox->setCurrentText(QString::number(mSerialPort->baudRate()));
    }

    // DataBits
    {
        mDataBitsComboBox->addItem(QStringLiteral("5"), QSerialPort::Data5);
        mDataBitsComboBox->addItem(QStringLiteral("6"), QSerialPort::Data6);
        mDataBitsComboBox->addItem(QStringLiteral("7"), QSerialPort::Data7);
        mDataBitsComboBox->addItem(QStringLiteral("8"), QSerialPort::Data8);
        mDataBitsComboBox->setCurrentIndex(mDataBitsComboBox->findData(mSerialPort->dataBits()));
    }

    // StopBits
    {
        mStopBitsComboBox->addItem(QStringLiteral("1"), QSerialPort::OneStop);
        mStopBitsComboBox->addItem(QStringLiteral("1.5"), QSerialPort::OneAndHalfStop);
        mStopBitsComboBox->addItem(QStringLiteral("2"), QSerialPort::TwoStop);
        mStopBitsComboBox->setCurrentIndex(mStopBitsComboBox->findData(mSerialPort->stopBits()));
    }

    // Parity
    {
        mParityComboBox->addItem(QStringLiteral("No"), QSerialPort::NoParity);
        mParityComboBox->addItem(QStringLiteral("Even"), QSerialPort::EvenParity);
        mParityComboBox->addItem(QStringLiteral("Odd"), QSerialPort::OddParity);
        mParityComboBox->addItem(QStringLiteral("Space"), QSerialPort::SpaceParity);
        mParityComboBox->addItem(QStringLiteral("Mark"), QSerialPort::MarkParity);
        mParityComboBox->setCurrentIndex(mParityComboBox->findData(mSerialPort->parity()));
    }

    // FlowControl
    {
        mFlowControlComboBox->addItem(QStringLiteral("No"), QSerialPort::NoFlowControl);
        mFlowControlComboBox->addItem(QStringLiteral("Hardware"), QSerialPort::HardwareControl);
        mFlowControlComboBox->addItem(QStringLiteral("Software"), QSerialPort::SoftwareControl);
        mFlowControlComboBox->setCurrentIndex(mFlowControlComboBox->findData(mSerialPort->flowControl()));
    }

    // ReadTimeout
    {
        mReadTimeoutSpinBox->setRange(READ_TIMEOUT_MIN_VALUE, READ_TIMEOUT_MAX_VALUE);
        mReadTimeoutSpinBox->setSuffix(READ_TIMEOUT_SUFFIX);
        mReadTimeoutSpinBox->setAlignment(Qt::AlignRight);
        mReadTimeoutSpinBox->setValue(READ_TIMEOUT_DEFAULT_VALUE);
    }

    // Button
    {
        connect(mOkButton, &QPushButton::clicked, this, &SerialSetupDialog::updateSetup);
        connect(mCancelButton, &QPushButton::clicked, this, &SerialSetupDialog::restoreSetup);
    }

    // Layout
    {
        QFormLayout *formLayout{new QFormLayout{}};
        formLayout->addRow(mSerialNameLabel, mSerialNameComboBox);
        formLayout->addRow(mBaudRateLabel, mBaudRateComboBox);
        formLayout->addRow(mDataBitsLabel, mDataBitsComboBox);
        formLayout->addRow(mFlowControlLabel, mFlowControlComboBox);
        formLayout->addRow(mParityLabel, mParityComboBox);
        formLayout->addRow(mStopBitsLabel, mStopBitsComboBox);
        formLayout->addRow(mReadTimeoutLabel, mReadTimeoutSpinBox);

        QHBoxLayout *buttonLayout{new QHBoxLayout{}};
        buttonLayout->addSpacerItem(new QSpacerItem{40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum});
        buttonLayout->addWidget(mOkButton);
        buttonLayout->addWidget(mCancelButton);

        QVBoxLayout *layout{new QVBoxLayout{}};
        layout->addLayout(formLayout);
        layout->addLayout(buttonLayout);
        setLayout(layout);
    }

    setWindowTitle(QStringLiteral("Serial Setup"));
    resize(sizeHint());
}

SerialSetupDialog::~SerialSetupDialog() {}

QSharedPointer<QSerialPort> SerialSetupDialog::serialPort() const
{
    return mSerialPort;
}

int SerialSetupDialog::readTimeout() const
{
    return mReadTimeoutSpinBox->value();
}

#define VERIFY(job, msg) \
    do { \
        if (!(job)) { \
            QMessageBox::critical(this, QStringLiteral("Error"), msg); \
            return; \
        } \
    } while (0)

void SerialSetupDialog::updateSetup()
{
    if (mSerialPort->isOpen()) {
        mSerialPort->close();
    }

    const QString name{mSerialNameComboBox->currentText()};
    const qint32 baudrate{mBaudRateComboBox->currentText().toInt()};
    const QSerialPort::DataBits databits{mDataBitsComboBox->currentData().value<QSerialPort::DataBits>()};
    const QSerialPort::StopBits stopbits{mStopBitsComboBox->currentData().value<QSerialPort::StopBits>()};
    const QSerialPort::Parity parity{mParityComboBox->currentData().value<QSerialPort::Parity>()};
    const QSerialPort::FlowControl flowControl{mFlowControlComboBox->currentData().value<QSerialPort::FlowControl>()};

    mSerialPort->setPortName(name);
    VERIFY(mSerialPort->setBaudRate(baudrate), QStringLiteral("Failed to set baudrate."));
    VERIFY(mSerialPort->setDataBits(databits), QStringLiteral("Failed to set data bits."));
    VERIFY(mSerialPort->setStopBits(stopbits), QStringLiteral("Failed to set stop bits."));
    VERIFY(mSerialPort->setParity(parity), QStringLiteral("Failed to set parity."));
    VERIFY(mSerialPort->setFlowControl(flowControl), QStringLiteral("Failed to flow control."));
    VERIFY(mSerialPort->open(QIODevice::ReadWrite), mSerialPort->errorString());

    accept();
}

void SerialSetupDialog::restoreSetup()
{
    const QString name{mSerialPort->portName()};

    name.isEmpty() ? mSerialNameComboBox->setCurrentIndex(-1)
                   : mSerialNameComboBox->setCurrentText(name);
    mBaudRateComboBox->setCurrentText(QString::number(mSerialPort->baudRate()));
    mDataBitsComboBox->setCurrentIndex(mDataBitsComboBox->findData(mSerialPort->dataBits()));
    mStopBitsComboBox->setCurrentIndex(mStopBitsComboBox->findData(mSerialPort->stopBits()));
    mParityComboBox->setCurrentIndex(mParityComboBox->findData(mSerialPort->parity()));
    mFlowControlComboBox->setCurrentIndex(mFlowControlComboBox->findData(mSerialPort->flowControl()));

    reject();
}
