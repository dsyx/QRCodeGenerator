#ifndef SERIALSETUPDIALOG_H
#define SERIALSETUPDIALOG_H

#include <QComboBox>
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QSerialPort>
#include <QSharedPointer>
#include <QSpinBox>

class SerialSetupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SerialSetupDialog(QSharedPointer<QSerialPort> sp, QWidget *parent = nullptr);
    ~SerialSetupDialog();

    QSharedPointer<QSerialPort> serialPort() const;
    int readTimeout() const;

private slots:
    void updateSetup();
    void restoreSetup();

private:
    QSharedPointer<QSerialPort> mSerialPort;

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
    QLabel *mReadTimeoutLabel;
    QSpinBox *mReadTimeoutSpinBox;

    QPushButton *mOkButton;
    QPushButton *mCancelButton;

private:
    static constexpr int READ_TIMEOUT_DEFAULT_VALUE = 200;
    static constexpr int READ_TIMEOUT_MIN_VALUE = 0;
    static constexpr int READ_TIMEOUT_MAX_VALUE = 60000;
    static constexpr char READ_TIMEOUT_SUFFIX[] = "  ms";
};

#endif // SERIALSETUPDIALOG_H
