#include "OperationWidget.h"
#include <QDebug>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QSpacerItem>
#include <QVBoxLayout>

OperationWidget::OperationWidget(QWidget *parent)
    : QTabWidget{parent}
    , mSerialPort{QSharedPointer<QSerialPort>::create()}
    , mSerialPortRxTimer{new QTimer{this}}
    , mTextModeDataEdit{new QPlainTextEdit{QStringLiteral("Hello World")}}
    , mTextModePrintButton{new QPushButton{QStringLiteral("Print")}}
    , mTextModeGenerateButton{new QPushButton{QStringLiteral("Generate")}}
    , mTextModeWidget{new QWidget{}}
    , mSerialModeDataEdit{new QPlainTextEdit{}}
    , mSerialModeCmdEdit{new QLineEdit{}}
    , mSerialModeCmdTailLFCheckBox{new QCheckBox{QStringLiteral("LF")}}
    , mSerialModeCmdTailCRLFCheckBox{new QCheckBox{QStringLiteral("CRLF")}}
    , mSerialModeSetupButton{new QPushButton{QStringLiteral("Setup Serial")}}
    , mSerialModePrintButton{new QPushButton{QStringLiteral("Print")}}
    , mSerialModeGenerateButton{new QPushButton{QStringLiteral("Generate")}}
    , mSerialModeWidget{new QWidget{}}
    , mSerialSetupDialog{new SerialSetupDialog{mSerialPort, this}}
{
    // SerialPort
    {
        connect(mSerialPort.get(), &QSerialPort::errorOccurred, this, &OperationWidget::serialModeErrorOccurredHandler);
        connect(mSerialPort.get(), &QSerialPort::readyRead, this, &OperationWidget::serialModeReadyReadHandler);
    }

    // SerialPortRxTimer
    {
        mSerialPortRxTimer->setSingleShot(true);
        connect(mSerialPortRxTimer, &QTimer::timeout, this, &OperationWidget::serialModeReadTimeoutHandler);
    }

    // TextMode
    {
        connect(mTextModePrintButton, &QPushButton::clicked, this, &OperationWidget::printRequested);
        connect(mTextModeGenerateButton, &QPushButton::clicked, this, &OperationWidget::textModeGenerate);

        QVBoxLayout *buttonLayout{new QVBoxLayout{}};
        buttonLayout->addSpacerItem(new QSpacerItem{20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding});
        buttonLayout->addWidget(mTextModePrintButton);
        buttonLayout->addWidget(mTextModeGenerateButton);

        QHBoxLayout *layout{new QHBoxLayout{}};
        layout->addWidget(mTextModeDataEdit, 8);
        layout->addLayout(buttonLayout, 2);

        mTextModeWidget->setLayout(layout);
    }

    // SerialMode
    {
        mSerialModeDataEdit->setReadOnly(true);

        connect(mSerialModeCmdTailLFCheckBox, &QCheckBox::clicked, this, [&]() {
            if (mSerialModeCmdTailCRLFCheckBox->isChecked()) {
                mSerialModeCmdTailCRLFCheckBox->setChecked(false);
            }
        });
        connect(mSerialModeCmdTailCRLFCheckBox, &QCheckBox::clicked, this, [&]() {
            if (mSerialModeCmdTailLFCheckBox->isChecked()) {
                mSerialModeCmdTailLFCheckBox->setChecked(false);
            }
        });

        connect(mSerialModeSetupButton, &QPushButton::clicked, this, &OperationWidget::serialModeSetup);
        connect(mSerialModePrintButton, &QPushButton::clicked, this, &OperationWidget::printRequested);
        connect(mSerialModeGenerateButton, &QPushButton::clicked, this, &OperationWidget::serialModeGenerate);

        QVBoxLayout *cmdLayout{new QVBoxLayout{}};
        cmdLayout->addSpacerItem(new QSpacerItem{20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding});
        cmdLayout->addWidget(mSerialModeCmdEdit);

        QVBoxLayout *tailLayout{new QVBoxLayout{}};
        tailLayout->addWidget(mSerialModeCmdTailLFCheckBox);
        tailLayout->addWidget(mSerialModeCmdTailCRLFCheckBox);

        QHBoxLayout *cmdAndTailLayout{new QHBoxLayout{}};
        cmdAndTailLayout->addLayout(cmdLayout, 9);
        cmdAndTailLayout->addLayout(tailLayout, 1);

        QVBoxLayout *dataAndCmdAndTailLayout{new QVBoxLayout{}};
        dataAndCmdAndTailLayout->addWidget(mSerialModeDataEdit);
        dataAndCmdAndTailLayout->addLayout(cmdAndTailLayout);

        QVBoxLayout *buttonLayout{new QVBoxLayout{}};
        buttonLayout->addWidget(mSerialModeSetupButton);
        buttonLayout->addSpacerItem(new QSpacerItem{20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding});
        buttonLayout->addWidget(mSerialModePrintButton);
        buttonLayout->addWidget(mSerialModeGenerateButton);

        QHBoxLayout *layout{new QHBoxLayout{}};
        layout->addLayout(dataAndCmdAndTailLayout, 8);
        layout->addLayout(buttonLayout, 2);

        mSerialModeWidget->setLayout(layout);
    }

    addTab(mTextModeWidget, QStringLiteral("Text"));
    addTab(mSerialModeWidget, QStringLiteral("Serial"));
}

OperationWidget::~OperationWidget() {}

void OperationWidget::textModeGenerate()
{
    QString data{mTextModeDataEdit->toPlainText()};
    emit generateRequested(data);
}

void OperationWidget::serialModeSetup()
{
    if (mSerialSetupDialog->exec() == QDialog::Accepted) {
        mSerialPortRxTimer->setInterval(mSerialSetupDialog->readTimeout());
    }
}

void OperationWidget::serialModeGenerate()
{
    mSerialModeDataEdit->clear();
    serialModeDisableOp();

    QString cmd{mSerialModeCmdEdit->text()};
    if (cmd.isEmpty()) {
        serialModeEnableOp();
        return;
    }
    if (mSerialModeCmdTailLFCheckBox->isChecked()) {
        cmd.append("\n");
    } else if (mSerialModeCmdTailCRLFCheckBox->isChecked()) {
        cmd.append("\r\n");
    }

    if (!mSerialPort->isOpen()) {
        if (!mSerialPort->open(QIODevice::ReadWrite)) {
            QMessageBox::critical(this, QStringLiteral("Error"), mSerialPort->errorString());
            serialModeEnableOp();
            return;
        }
    }

    mSerialPort->clear();

    if (mSerialPort->write(cmd.toUtf8()) == -1) {
        QMessageBox::critical(this, QStringLiteral("Error"), mSerialPort->errorString());
        serialModeEnableOp();
        return;
    }

    mSerialPortRxTimer->start();
}

void OperationWidget::serialModeDisableOp()
{
    setDisabled(true);
    mSerialModeCmdEdit->setDisabled(true);
    mSerialModeCmdTailLFCheckBox->setDisabled(true);
    mSerialModeCmdTailCRLFCheckBox->setDisabled(true);
    mSerialModeSetupButton->setDisabled(true);
    mSerialModePrintButton->setDisabled(true);
    mSerialModeGenerateButton->setDisabled(true);
}

void OperationWidget::serialModeEnableOp()
{
    setDisabled(false);
    mSerialModeCmdEdit->setDisabled(false);
    mSerialModeCmdTailLFCheckBox->setDisabled(false);
    mSerialModeCmdTailCRLFCheckBox->setDisabled(false);
    mSerialModeSetupButton->setDisabled(false);
    mSerialModePrintButton->setDisabled(false);
    mSerialModeGenerateButton->setDisabled(false);
}

void OperationWidget::serialModeErrorOccurredHandler(QSerialPort::SerialPortError)
{
    qDebug() << "Serial Port Error:" << mSerialPort->errorString();
}

void OperationWidget::serialModeReadyReadHandler()
{
    if (mSerialPortRxTimer->isActive()) {
        mSerialPortRxTimer->stop();

        QString data{QString::fromUtf8(mSerialPort->readAll())};
        mSerialModeDataEdit->setPlainText(data);
        serialModeEnableOp();

        emit generateRequested(data);
    }
}

void OperationWidget::serialModeReadTimeoutHandler()
{
    QMessageBox::information(this, QStringLiteral("Info"), QStringLiteral("Serial Read Timeout"));
    serialModeEnableOp();
}
