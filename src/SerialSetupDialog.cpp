#include "SerialSetupDialog.h"
#include <QFormLayout>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QMessageBox>
#include <QSerialPortInfo>
#include <QSpacerItem>
#include <QVBoxLayout>

SerialSetupDialog::SerialSetupDialog(QWidget *parent)
    : QDialog{parent}
    , mSerialPort{new QSerialPort()}
    , mSerialNameLabel{new QLabel(QStringLiteral("Serial Port:"))}
    , mSerialNameComboBox{new QComboBox()}
    , mBaudRateLabel{new QLabel(QStringLiteral("BaudRate:"))}
    , mBaudRateComboBox{new QComboBox()}
    , mDataBitsLabel{new QLabel(QStringLiteral("Data Bits:"))}
    , mDataBitsComboBox{new QComboBox()}
    , mStopBitsLabel{new QLabel(QStringLiteral("Stop Bits:"))}
    , mStopBitsComboBox{new QComboBox()}
    , mParityLabel{new QLabel(QStringLiteral("Parity:"))}
    , mParityComboBox{new QComboBox()}
    , mFlowControlLabel{new QLabel(QStringLiteral("Flow Control:"))}
    , mFlowControlComboBox{new QComboBox()}
    , mOkButton{new QPushButton(QStringLiteral("OK"))}
    , mCancelButton{new QPushButton(QStringLiteral("Cancel"))}
{
    // SerialName
    const auto infos = QSerialPortInfo::availablePorts();
    for (const auto &info : infos) {
        mSerialNameComboBox->addItem(info.portName());
    }

    // BaudRate
    mBaudRateComboBox->addItem(QStringLiteral("1200"));
    mBaudRateComboBox->addItem(QStringLiteral("2400"));
    mBaudRateComboBox->addItem(QStringLiteral("4800"));
    mBaudRateComboBox->addItem(QStringLiteral("9600"));
    mBaudRateComboBox->addItem(QStringLiteral("19200"));
    mBaudRateComboBox->addItem(QStringLiteral("38400"));
    mBaudRateComboBox->addItem(QStringLiteral("57600"));
    mBaudRateComboBox->addItem(QStringLiteral("115200"));
    mBaudRateComboBox->setCurrentIndex(mBaudRateComboBox->count() - 1);
    mBaudRateComboBox->setEditable(true);
    mBaudRateComboBox->setValidator(new QIntValidator(mBaudRateComboBox));

    // DataBits
    mDataBitsComboBox->addItem(QStringLiteral("5"), QSerialPort::Data5);
    mDataBitsComboBox->addItem(QStringLiteral("6"), QSerialPort::Data6);
    mDataBitsComboBox->addItem(QStringLiteral("7"), QSerialPort::Data7);
    mDataBitsComboBox->addItem(QStringLiteral("8"), QSerialPort::Data8);
    mDataBitsComboBox->setCurrentIndex(mDataBitsComboBox->count() - 1);

    // StopBits
    mStopBitsComboBox->addItem(QStringLiteral("1"), QSerialPort::OneStop);
    mStopBitsComboBox->addItem(QStringLiteral("1.5"), QSerialPort::OneAndHalfStop);
    mStopBitsComboBox->addItem(QStringLiteral("2"), QSerialPort::TwoStop);
    mStopBitsComboBox->setCurrentIndex(0);

    // Parity
    mParityComboBox->addItem(QStringLiteral("No"), QSerialPort::NoParity);
    mParityComboBox->addItem(QStringLiteral("Even"), QSerialPort::EvenParity);
    mParityComboBox->addItem(QStringLiteral("Odd"), QSerialPort::OddParity);
    mParityComboBox->addItem(QStringLiteral("Space"), QSerialPort::SpaceParity);
    mParityComboBox->addItem(QStringLiteral("Mark"), QSerialPort::MarkParity);
    mParityComboBox->setCurrentIndex(0);

    // FlowControl
    mFlowControlComboBox->addItem(QStringLiteral("No"), QSerialPort::NoFlowControl);
    mFlowControlComboBox->addItem(QStringLiteral("Hardware"), QSerialPort::HardwareControl);
    mFlowControlComboBox->addItem(QStringLiteral("Software"), QSerialPort::SoftwareControl);
    mFlowControlComboBox->setCurrentIndex(0);

    // Button
    connect(mOkButton, &QPushButton::clicked, this, &SerialSetupDialog::updateSerialSetup);
    connect(mCancelButton, &QPushButton::clicked, this, &SerialSetupDialog::reject);

    QFormLayout *formLayout = new QFormLayout();
    formLayout->addRow(mSerialNameLabel, mSerialNameComboBox);
    formLayout->addRow(mBaudRateLabel, mBaudRateComboBox);
    formLayout->addRow(mDataBitsLabel, mDataBitsComboBox);
    formLayout->addRow(mFlowControlLabel, mFlowControlComboBox);
    formLayout->addRow(mParityLabel, mParityComboBox);
    formLayout->addRow(mStopBitsLabel, mStopBitsComboBox);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addSpacerItem(
        new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
    buttonLayout->addWidget(mOkButton);
    buttonLayout->addWidget(mCancelButton);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addLayout(formLayout);
    layout->addLayout(buttonLayout);
    setLayout(layout);

    updateSerialSetup();

    setWindowTitle(QStringLiteral("Serial Setup"));
    resize(sizeHint());
}

SerialSetupDialog::~SerialSetupDialog()
{
    delete mSerialPort;
}

void SerialSetupDialog::updateSerialSetup()
{
    if (mSerialPort->isOpen()) {
        mSerialPort->close();
    }

    mSerialPort->setPortName(mSerialNameComboBox->currentText());

    if (!mSerialPort->setBaudRate(mBaudRateComboBox->currentText().toInt())) {
        QMessageBox::critical(this,
                              QStringLiteral("Error"),
                              QStringLiteral("Failed to set baudrate."));
        return;
    }

    if (!mSerialPort->setDataBits(mDataBitsComboBox->currentData().value<QSerialPort::DataBits>())) {
        QMessageBox::critical(this,
                              QStringLiteral("Error"),
                              QStringLiteral("Failed to set data bits."));
        return;
    }

    if (!mSerialPort->setStopBits(mStopBitsComboBox->currentData().value<QSerialPort::StopBits>())) {
        QMessageBox::critical(this,
                              QStringLiteral("Error"),
                              QStringLiteral("Failed to set stop bits."));
        return;
    }

    if (!mSerialPort->setParity(mParityComboBox->currentData().value<QSerialPort::Parity>())) {
        QMessageBox::critical(this,
                              QStringLiteral("Error"),
                              QStringLiteral("Failed to set parity."));
        return;
    }

    if (!mSerialPort->setFlowControl(
            mFlowControlComboBox->currentData().value<QSerialPort::FlowControl>())) {
        QMessageBox::critical(this,
                              QStringLiteral("Error"),
                              QStringLiteral("Failed to flow control."));
        return;
    }

    if (!mSerialPort->open(QIODevice::ReadWrite)) {
        QMessageBox::critical(this, QStringLiteral("Error"), mSerialPort->errorString());
        return;
    }

    accept();
}
