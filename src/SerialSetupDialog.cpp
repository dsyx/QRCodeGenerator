#include "SerialSetupDialog.h"
#include <QFormLayout>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QMessageBox>
#include <QSpacerItem>
#include <QVBoxLayout>

SerialSetupDialog::SerialSetupDialog(QSharedPointer<Serial> sp, QWidget *parent)
    : QDialog{parent}
    , mSerial{sp}
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
    , mTimeoutLabel{new QLabel{QStringLiteral("Timeout:")}}
    , mTimeoutSpinBox{new QSpinBox{}}
    , mOkButton{new QPushButton{QStringLiteral("OK")}}
    , mCancelButton{new QPushButton{QStringLiteral("Cancel")}}
{
    const SerialConfig cfg{mSerial->config()};
    connect(mSerial.get(), &Serial::setConfigResult, this, &SerialSetupDialog::updateSetupResult);

    // SerialName
    {
        const QList<QString> names{Serial::availableSerialName()};
        for (const auto &name : names) {
            mSerialNameComboBox->addItem(name);
        }
        cfg.name.isEmpty() ? mSerialNameComboBox->setCurrentIndex(-1)
                           : mSerialNameComboBox->setCurrentText(cfg.name);
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
        mBaudRateComboBox->setCurrentText(QString::number(cfg.baudRate));
    }

    // DataBits
    {
        mDataBitsComboBox->addItem(QStringLiteral("5"), QSerialPort::Data5);
        mDataBitsComboBox->addItem(QStringLiteral("6"), QSerialPort::Data6);
        mDataBitsComboBox->addItem(QStringLiteral("7"), QSerialPort::Data7);
        mDataBitsComboBox->addItem(QStringLiteral("8"), QSerialPort::Data8);
        mDataBitsComboBox->setCurrentIndex(mDataBitsComboBox->findData(cfg.dataBits));
    }

    // StopBits
    {
        mStopBitsComboBox->addItem(QStringLiteral("1"), QSerialPort::OneStop);
        mStopBitsComboBox->addItem(QStringLiteral("1.5"), QSerialPort::OneAndHalfStop);
        mStopBitsComboBox->addItem(QStringLiteral("2"), QSerialPort::TwoStop);
        mStopBitsComboBox->setCurrentIndex(mStopBitsComboBox->findData(cfg.stopBits));
    }

    // Parity
    {
        mParityComboBox->addItem(QStringLiteral("No"), QSerialPort::NoParity);
        mParityComboBox->addItem(QStringLiteral("Even"), QSerialPort::EvenParity);
        mParityComboBox->addItem(QStringLiteral("Odd"), QSerialPort::OddParity);
        mParityComboBox->addItem(QStringLiteral("Space"), QSerialPort::SpaceParity);
        mParityComboBox->addItem(QStringLiteral("Mark"), QSerialPort::MarkParity);
        mParityComboBox->setCurrentIndex(mParityComboBox->findData(cfg.parity));
    }

    // FlowControl
    {
        mFlowControlComboBox->addItem(QStringLiteral("No"), QSerialPort::NoFlowControl);
        mFlowControlComboBox->addItem(QStringLiteral("Hardware"), QSerialPort::HardwareControl);
        mFlowControlComboBox->addItem(QStringLiteral("Software"), QSerialPort::SoftwareControl);
        mFlowControlComboBox->setCurrentIndex(mFlowControlComboBox->findData(cfg.flowControl));
    }

    // ReadTimeout
    {
        mTimeoutSpinBox->setRange(TIMEOUT_MIN_VALUE, TIMEOUT_MAX_VALUE);
        mTimeoutSpinBox->setSuffix(TIMEOUT_SUFFIX);
        mTimeoutSpinBox->setAlignment(Qt::AlignRight);
        mTimeoutSpinBox->setValue(cfg.timeout);
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
        formLayout->addRow(mTimeoutLabel, mTimeoutSpinBox);

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

void SerialSetupDialog::updateSetup()
{
    const SerialConfig cfg{
        mSerialNameComboBox->currentText(),
        mBaudRateComboBox->currentText().toInt(),
        mDataBitsComboBox->currentData().value<QSerialPort::DataBits>(),
        mStopBitsComboBox->currentData().value<QSerialPort::StopBits>(),
        mParityComboBox->currentData().value<QSerialPort::Parity>(),
        mFlowControlComboBox->currentData().value<QSerialPort::FlowControl>(),
        mTimeoutSpinBox->value(),
    };

    mSerial->setConfig(cfg);
}

void SerialSetupDialog::restoreSetup()
{
    const SerialConfig cfg{mSerial->config()};

    cfg.name.isEmpty() ? mSerialNameComboBox->setCurrentIndex(-1)
                       : mSerialNameComboBox->setCurrentText(cfg.name);
    mBaudRateComboBox->setCurrentText(QString::number(cfg.baudRate));
    mDataBitsComboBox->setCurrentIndex(mDataBitsComboBox->findData(cfg.dataBits));
    mStopBitsComboBox->setCurrentIndex(mStopBitsComboBox->findData(cfg.stopBits));
    mParityComboBox->setCurrentIndex(mParityComboBox->findData(cfg.parity));
    mFlowControlComboBox->setCurrentIndex(mFlowControlComboBox->findData(cfg.flowControl));
    mTimeoutSpinBox->setValue(cfg.timeout);

    reject();
}

void SerialSetupDialog::updateSetupResult(bool ok, const QString &description)
{
    if (ok) {
        accept();
    } else {
        QMessageBox::critical(this, QStringLiteral("Error"), description);
    }
}
