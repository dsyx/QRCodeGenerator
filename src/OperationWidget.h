#ifndef OPERATIONWIDGET_H
#define OPERATIONWIDGET_H

#include "Serial.h"
#include "SerialSetupDialog.h"
#include <QCheckBox>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSharedPointer>
#include <QTabWidget>
#include <QWidget>

class OperationWidget : public QTabWidget
{
    Q_OBJECT

public:
    explicit OperationWidget(QWidget *parent = nullptr);
    ~OperationWidget();

signals:
    void printRequested();
    void generateRequested(const QString &data);

private:
    void textModeGenerate();
    void serialModeSetup();
    void serialModeGenerate();
    void serialModeSetDisabled(bool disable);

private slots:
    void serialModeResponseHandler(const QByteArray &rsp);
    void serialModeTimeoutHandler(const QString &description);
    void serialModeErrorHandler(const QString &description);

private:
    QSharedPointer<Serial> mSerial;

private:
    QPlainTextEdit *mTextModeDataEdit;
    QPushButton *mTextModePrintButton;
    QPushButton *mTextModeGenerateButton;
    QWidget *mTextModeWidget;

    QPlainTextEdit *mSerialModeDataEdit;
    QLineEdit *mSerialModeCmdEdit;
    QCheckBox *mSerialModeCmdTailLFCheckBox;
    QCheckBox *mSerialModeCmdTailCRLFCheckBox;
    QPushButton *mSerialModeSetupButton;
    QPushButton *mSerialModePrintButton;
    QPushButton *mSerialModeGenerateButton;
    QWidget *mSerialModeWidget;

    SerialSetupDialog *mSerialSetupDialog;
};

#endif // OPERATIONWIDGET_H
