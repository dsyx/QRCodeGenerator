#ifndef SERIALSETUPDIALOG_H
#define SERIALSETUPDIALOG_H

#include "Serial.h"
#include <QComboBox>
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QSharedPointer>
#include <QSpinBox>

class SerialSetupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SerialSetupDialog(QSharedPointer<Serial> sp, QWidget *parent = nullptr);
    ~SerialSetupDialog();

private slots:
    void updateSetup();
    void restoreSetup();
    void updateSetupResult(bool ok, const QString &description);

private:
    QSharedPointer<Serial> mSerial;

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
    QLabel *mTimeoutLabel;
    QSpinBox *mTimeoutSpinBox;

    QPushButton *mOkButton;
    QPushButton *mCancelButton;

private:
    static constexpr int TIMEOUT_MIN_VALUE = 0;
    static constexpr int TIMEOUT_MAX_VALUE = 60000;
    static constexpr char TIMEOUT_SUFFIX[] = "  ms";
};

#endif // SERIALSETUPDIALOG_H
