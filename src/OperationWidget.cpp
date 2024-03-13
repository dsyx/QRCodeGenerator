#include "OperationWidget.h"
#include <QDebug>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QVBoxLayout>

OperationWidget::OperationWidget(QWidget *parent)
    : QTabWidget{parent}
    , mSerialSetupDialog{new SerialSetupDialog(this)}
    , mTextModeDataEdit{new QPlainTextEdit(QStringLiteral("Hello World"))}
    , mTextModePrintButton{new QPushButton(QStringLiteral("Print"))}
    , mTextModeGenerateButton{new QPushButton(QStringLiteral("Generate"))}
    , mTextModeWidget{new QWidget()}
    , mSerialModeDataEdit{new QPlainTextEdit()}
    , mSerialModeCmdEdit{new QLineEdit()}
    , mSerialModeCmdTailLFCheckBox{new QCheckBox(QStringLiteral("LF"))}
    , mSerialModeCmdTailCRLFCheckBox{new QCheckBox(QStringLiteral("CRLF"))}
    , mSerialModeSetupButton{new QPushButton(QStringLiteral("Setup Serial"))}
    , mSerialModePrintButton{new QPushButton(QStringLiteral("Print"))}
    , mSerialModeGenerateButton{new QPushButton(QStringLiteral("Generate"))}
    , mSerialModeWidget{new QWidget()}
{
    // TextMode
    {
        connect(mTextModePrintButton, &QPushButton::clicked, this, &OperationWidget::print);
        connect(mTextModeGenerateButton, &QPushButton::clicked, this, &OperationWidget::generate);

        QVBoxLayout *buttonLayout = new QVBoxLayout();
        buttonLayout->addSpacerItem(
            new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));
        buttonLayout->addWidget(mTextModePrintButton);
        buttonLayout->addWidget(mTextModeGenerateButton);

        QHBoxLayout *layout = new QHBoxLayout();
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

        connect(mSerialModeSetupButton, &QPushButton::clicked, this, &OperationWidget::serialSetup);
        connect(mSerialModePrintButton, &QPushButton::clicked, this, &OperationWidget::print);
        connect(mSerialModeGenerateButton,
                &QPushButton::clicked,
                this,
                &OperationWidget::generateFromSerial);

        QVBoxLayout *cmdLayout = new QVBoxLayout();
        cmdLayout->addSpacerItem(
            new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));
        cmdLayout->addWidget(mSerialModeCmdEdit);

        QVBoxLayout *tailLayout = new QVBoxLayout();
        tailLayout->addWidget(mSerialModeCmdTailLFCheckBox);
        tailLayout->addWidget(mSerialModeCmdTailCRLFCheckBox);

        QHBoxLayout *cmdAndTailLayout = new QHBoxLayout();
        cmdAndTailLayout->addLayout(cmdLayout, 9);
        cmdAndTailLayout->addLayout(tailLayout, 1);

        QVBoxLayout *dataAndCmdAndTailLayout = new QVBoxLayout();
        dataAndCmdAndTailLayout->addWidget(mSerialModeDataEdit);
        dataAndCmdAndTailLayout->addLayout(cmdAndTailLayout);

        QVBoxLayout *buttonLayout = new QVBoxLayout();
        buttonLayout->addWidget(mSerialModeSetupButton);
        buttonLayout->addSpacerItem(
            new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));
        buttonLayout->addWidget(mSerialModePrintButton);
        buttonLayout->addWidget(mSerialModeGenerateButton);

        QHBoxLayout *layout = new QHBoxLayout();
        layout->addLayout(dataAndCmdAndTailLayout, 8);
        layout->addLayout(buttonLayout, 2);

        mSerialModeWidget->setLayout(layout);
    }

    addTab(mTextModeWidget, QStringLiteral("Text"));
    addTab(mSerialModeWidget, QStringLiteral("Serial"));
}

OperationWidget::~OperationWidget() {}

QString OperationWidget::data() const
{
    QWidget *cw = currentWidget();

    if (cw == mTextModeWidget) {
        return mTextModeDataEdit->toPlainText();
    }

    if (cw == mSerialModeWidget) {
        return mSerialModeDataEdit->toPlainText();
    }

    return QString();
}

void OperationWidget::serialSetup()
{
    mSerialSetupDialog->exec();
}

void OperationWidget::generateFromSerial()
{
    mSerialModeDataEdit->clear();
    QSerialPort *sp = mSerialSetupDialog->serialPort();
    if (!sp->isOpen()) {
        sp->open(QIODevice::ReadWrite);
    }
    QString cmd = mSerialModeCmdEdit->text();
    QString tail;
    if (mSerialModeCmdTailLFCheckBox->isChecked()) {
        tail.append("\n");
    } else if (mSerialModeCmdTailCRLFCheckBox->isChecked()) {
        tail.append("\r\n");
    }
    cmd += tail;
    sp->write(cmd.toUtf8());
    if (sp->waitForReadyRead(1000)) {
        QByteArray rsp = sp->readAll();
        mSerialModeDataEdit->setPlainText(QString::fromUtf8(rsp));
        emit generate();
    }
}
