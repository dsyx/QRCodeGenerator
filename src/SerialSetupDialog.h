#ifndef SERIALSETUPDIALOG_H
#define SERIALSETUPDIALOG_H

#include <QComboBox>
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QSerialPort>

class SerialSetupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SerialSetupDialog(QWidget *parent = nullptr);
    ~SerialSetupDialog();

    QSerialPort *serialPort() { return mSerialPort; }

private slots:
    void updateSerialSetup();

private:
    QSerialPort *mSerialPort;

private:
    QLabel *mSerialNameLabel;
    QComboBox *mSerialNameComboBox;
    QLabel *mBaudRateLabel;
    QComboBox *mBaudRateComboBox;
    QLabel *mDataBitsLabel;
    QComboBox *mDataBitsComboBox;
    QLabel *mStopBitsLabel;
    QComboBox *mStopBitsComboBox;
    QLabel *mParityLabel;
    QComboBox *mParityComboBox;
    QLabel *mFlowControlLabel;
    QComboBox *mFlowControlComboBox;

    QPushButton *mOkButton;
    QPushButton *mCancelButton;
};

#endif // SERIALSETUPDIALOG_H
