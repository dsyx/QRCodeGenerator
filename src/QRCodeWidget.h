#ifndef QRCODEWIDGET_H
#define QRCODEWIDGET_H

#include <QComboBox>
#include <QImage>
#include <QLabel>
#include <QLineEdit>
#include <QSize>
#include <QWidget>
#include "QZXing.h"

Q_DECLARE_METATYPE(QZXing::EncodeErrorCorrectionLevel)

class QRCodeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit QRCodeWidget(QWidget *parent = nullptr);
    ~QRCodeWidget();

    QImage qrcode() const;

public slots:
    void setQRCodeData(const QString &data);

private:
    QZXing::EncodeErrorCorrectionLevel errorCorrection() const;
    QSize size() const;

private:
    QImage mQRCode;

private:
    QLabel *mPreviewWidget;

    QLabel *mErrorCorrectionLabel;
    QComboBox *mErrorCorrectionComboBox;
    QLabel *mSizeLabel;
    QLineEdit *mSizeWidthEdit;
    QLineEdit *mSizeHeightEdit;
    QWidget *mSetupWidget;

private:
    static constexpr int DEFAULT_LENGTH_OR_WIDTH = 256;
};

#endif // QRCODEWIDGET_H
