#include "OperationWidget.h"
#include <QHBoxLayout>
#include <QMessageBox>
#include <QSpacerItem>
#include <QVBoxLayout>

OperationWidget::OperationWidget(QWidget *parent)
    : QTabWidget{parent}
    , mSerial{QSharedPointer<Serial>::create()}
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
    , mSerialSetupDialog{new SerialSetupDialog{mSerial, this}}
{
    // SerialPort
    {
        connect(mSerial.get(), &Serial::response, this, &OperationWidget::serialModeResponseHandler);
        connect(mSerial.get(), &Serial::timeout, this, &OperationWidget::serialModeTimeoutHandler);
        connect(mSerial.get(), &Serial::error, this, &OperationWidget::serialModeErrorHandler);
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
    mSerialSetupDialog->exec();
}

void OperationWidget::serialModeGenerate()
{
    mSerialModeDataEdit->clear();
    serialModeSetDisabled(true);

    QString cmd{mSerialModeCmdEdit->text()};
    if (cmd.isEmpty()) {
        serialModeSetDisabled(false);
        return;
    }
    if (mSerialModeCmdTailLFCheckBox->isChecked()) {
        cmd.append("\n");
    } else if (mSerialModeCmdTailCRLFCheckBox->isChecked()) {
        cmd.append("\r\n");
    }

    mSerial->request(cmd.toUtf8());
}

void OperationWidget::serialModeSetDisabled(bool disable)
{
    setDisabled(disable);
    mSerialModeCmdEdit->setDisabled(disable);
    mSerialModeCmdTailLFCheckBox->setDisabled(disable);
    mSerialModeCmdTailCRLFCheckBox->setDisabled(disable);
    mSerialModeSetupButton->setDisabled(disable);
    mSerialModePrintButton->setDisabled(disable);
    mSerialModeGenerateButton->setDisabled(disable);
}

void OperationWidget::serialModeResponseHandler(const QByteArray &rsp)
{
    QString data{QString::fromUtf8(rsp)};
    mSerialModeDataEdit->setPlainText(data);
    emit generateRequested(data);

    serialModeSetDisabled(false);
}

void OperationWidget::serialModeTimeoutHandler(const QString &description)
{
    QMessageBox::information(this, QStringLiteral("Info"), description);

    serialModeSetDisabled(false);
}

void OperationWidget::serialModeErrorHandler(const QString &description)
{
    QMessageBox::critical(this, QStringLiteral("Error"), description);

    serialModeSetDisabled(false);
}
