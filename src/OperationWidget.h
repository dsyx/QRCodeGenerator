#ifndef OPERATIONWIDGET_H
#define OPERATIONWIDGET_H

#include "SerialSetupDialog.h"
#include <QCheckBox>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSerialPort>
#include <QSharedPointer>
#include <QTabWidget>
#include <QTimer>
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
    void serialModeDisableOp();
    void serialModeEnableOp();

private slots:
    void serialModeErrorOccurredHandler(QSerialPort::SerialPortError error);
    void serialModeReadyReadHandler();
    void serialModeReadTimeoutHandler();

private:
    QSharedPointer<QSerialPort> mSerialPort;
    QTimer *mSerialPortRxTimer;

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
