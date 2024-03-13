#ifndef OPERATIONWIDGET_H
#define OPERATIONWIDGET_H

#include <QCheckBox>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTabWidget>
#include <QWidget>
#include "SerialSetupDialog.h"

class OperationWidget : public QTabWidget
{
    Q_OBJECT

public:
    explicit OperationWidget(QWidget *parent = nullptr);
    ~OperationWidget();

    QString data() const;

signals:
    void print();
    void generate();

private:
    void serialSetup();
    void generateFromSerial();

private:
    SerialSetupDialog *mSerialSetupDialog;

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
};

#endif // OPERATIONWIDGET_H
