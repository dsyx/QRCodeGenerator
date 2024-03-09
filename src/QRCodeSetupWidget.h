#ifndef QRCODESETUPWIDGET_H
#define QRCODESETUPWIDGET_H

#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QSize>
#include <QWidget>
#include "QZXing.h"

Q_DECLARE_METATYPE(QZXing::EncodeErrorCorrectionLevel)

class QRCodeSetupWidget : public QWidget
{
    Q_OBJECT

public:
    explicit QRCodeSetupWidget(QWidget *parent = nullptr);
    ~QRCodeSetupWidget();

    QZXing::EncodeErrorCorrectionLevel errorCorrection() const;
    QSize size() const;

private:
    QLabel *mErrorCorrectionLabel;
    QComboBox *mErrorCorrectionComboBox;
    QLabel *mSizeLabel;
    QLineEdit *mSizeWidthEdit;
    QLineEdit *mSizeHeightEdit;

    static constexpr int DEFAULT_LENGTH_OR_WIDTH = 256;
};

#endif // QRCODESETUPWIDGET_H
