#include "QRCodeWidget.h"
#include <QFormLayout>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QPixmap>

QRCodeWidget::QRCodeWidget(QWidget *parent)
    : QWidget{parent}
    , mPreviewWidget{new QLabel{QStringLiteral("QR Code Preview")}}
    , mErrorCorrectionLabel{new QLabel{QStringLiteral("Error Correction:")}}
    , mErrorCorrectionComboBox{new QComboBox{}}
    , mSizeLabel{new QLabel{QStringLiteral("Size (Width x Height):")}}
    , mSizeWidthEdit{new QLineEdit{QString::number(DEFAULT_LENGTH_OR_WIDTH)}}
    , mSizeHeightEdit{new QLineEdit{QString::number(DEFAULT_LENGTH_OR_WIDTH)}}
    , mSetupWidget{new QWidget{}}
{
    // PreviewWidget
    {
        mPreviewWidget->setAlignment(Qt::AlignCenter);
        mPreviewWidget->setFrameStyle(QFrame::Panel | QFrame::Sunken);
        mPreviewWidget->setMinimumSize(DEFAULT_LENGTH_OR_WIDTH, DEFAULT_LENGTH_OR_WIDTH);
    }

    // SetupWidget
    {
        mErrorCorrectionComboBox->insertItem(0, QStringLiteral("Low"), QZXing::EncodeErrorCorrectionLevel_L);
        mErrorCorrectionComboBox->insertItem(1, QStringLiteral("Medium"), QZXing::EncodeErrorCorrectionLevel_M);
        mErrorCorrectionComboBox->insertItem(2, QStringLiteral("Quartile"), QZXing::EncodeErrorCorrectionLevel_Q);
        mErrorCorrectionComboBox->insertItem(3, QStringLiteral("High"), QZXing::EncodeErrorCorrectionLevel_H);
        mErrorCorrectionComboBox->setCurrentIndex(2);

        mSizeWidthEdit->setValidator(new QIntValidator{mSizeWidthEdit});
        mSizeHeightEdit->setValidator(new QIntValidator{mSizeHeightEdit});

        QHBoxLayout *sizeLayout{new QHBoxLayout{}};
        sizeLayout->addWidget(mSizeWidthEdit);
        sizeLayout->addWidget(new QLabel{QStringLiteral("x")});
        sizeLayout->addWidget(mSizeHeightEdit);

        QFormLayout *layout{new QFormLayout{}};
        layout->addRow(mErrorCorrectionLabel, mErrorCorrectionComboBox);
        layout->addRow(mSizeLabel, sizeLayout);

        mSetupWidget->setLayout(layout);
    }

    QHBoxLayout *layout{new QHBoxLayout{}};
    layout->addWidget(mPreviewWidget, 6);
    layout->addWidget(mSetupWidget, 4);
    setLayout(layout);
}

QRCodeWidget::~QRCodeWidget() {}

QImage QRCodeWidget::qrcode() const
{
    return mQRCode;
}

void QRCodeWidget::setQRCodeData(const QString &data)
{
    mPreviewWidget->clear();

    mQRCode = QZXing::encodeData(data, QZXing::EncoderFormat_QR_CODE, size(), errorCorrection(), false, false);

    QPixmap pixmap{QPixmap::fromImage(mQRCode).scaled(mPreviewWidget->size())};
    mPreviewWidget->setPixmap(pixmap);
}

QZXing::EncodeErrorCorrectionLevel QRCodeWidget::errorCorrection() const
{
    return mErrorCorrectionComboBox->currentData().value<QZXing::EncodeErrorCorrectionLevel>();
}

QSize QRCodeWidget::size() const
{
    return QSize{mSizeWidthEdit->text().toInt(), mSizeHeightEdit->text().toInt()};
}
