#include "QRCodeGenerator.h"
#include <QDebug>
#include <QImage>
#include <QPixmap>
#include <exception>
#include <QIntValidator>

QRCodeGenerator::QRCodeGenerator(QWidget *parent)
try : QMainWindow(parent)
    , mPreviewLabel(new QLabel("QR Code Preview"))
    , mErrorCorrectionLabel(new QLabel(QString::fromUtf8("Error Correction:")))
    , mErrorCorrectionComboBox(new QComboBox())
    , mSizeLabel(new QLabel(QString::fromUtf8("Image Size:")))
    , mSizeXLabel(new QLabel(QString::fromUtf8("x")))
    , mSizeWidthEdit(new QLineEdit(QString::fromUtf8("256")))
    , mSizeHeightEdit(new QLineEdit(QString::fromUtf8("256")))
    , mDataEdit(new QTextEdit())
    , mSpacer(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum))
    , mGenerateButton(new QPushButton(QString::fromUtf8("Generate")))
    , mSizeEditLayout(new QHBoxLayout())
    , mSettingLayout(new QFormLayout())
    , mPreviewAndSettingLayout(new QHBoxLayout())
    , mButtonLayout(new QHBoxLayout())
    , mMainLayout(new QVBoxLayout())
{
    setMinimumSize(480, 600);

    mPreviewLabel->setAlignment(Qt::AlignCenter);
    mPreviewLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    mErrorCorrectionMap.insert(QString::fromUtf8("Low"), QZXing::EncodeErrorCorrectionLevel_L);
    mErrorCorrectionMap.insert(QString::fromUtf8("Medium"), QZXing::EncodeErrorCorrectionLevel_M);
    mErrorCorrectionMap.insert(QString::fromUtf8("Quartile"), QZXing::EncodeErrorCorrectionLevel_Q);
    mErrorCorrectionMap.insert(QString::fromUtf8("High"), QZXing::EncodeErrorCorrectionLevel_H);
    mErrorCorrectionComboBox->insertItems(0, mErrorCorrectionMap.keys());
    mSettingLayout->addRow(mErrorCorrectionLabel, mErrorCorrectionComboBox);

    mSizeWidthEdit->setValidator(new QIntValidator(mSizeWidthEdit));
    mSizeHeightEdit->setValidator(new QIntValidator(mSizeHeightEdit));
    mSizeEditLayout->addWidget(mSizeWidthEdit);
    mSizeEditLayout->addWidget(mSizeXLabel);
    mSizeEditLayout->addWidget(mSizeHeightEdit);
    mSettingLayout->addRow(mSizeLabel, mSizeEditLayout);

    mPreviewAndSettingLayout->addWidget(mPreviewLabel, 6);
    mPreviewAndSettingLayout->addLayout(mSettingLayout, 4);

    mButtonLayout->addSpacerItem(mSpacer);
    mButtonLayout->addWidget(mGenerateButton);
    connect(mGenerateButton, &QPushButton::clicked, this, &QRCodeGenerator::generate);

    mMainLayout->addLayout(mPreviewAndSettingLayout, 4);
    mMainLayout->addWidget(mDataEdit, 4);
    mMainLayout->addLayout(mButtonLayout, 2);

    QWidget *centeralWidget = new QWidget();
    centeralWidget->setLayout(mMainLayout);
    setCentralWidget(centeralWidget);
} catch (const std::exception &e) {
    qFatal("QRCodeGenerator(QWidget *parent) failed: %s", e.what());
}

QRCodeGenerator::~QRCodeGenerator() {}

void QRCodeGenerator::generate()
{
    QString data = mDataEdit->toPlainText();
    if (data.isEmpty()) {
        return;
    }
    QZXing::EncodeErrorCorrectionLevel errorCorrection = mErrorCorrectionMap[mErrorCorrectionComboBox->currentText()];
    QSize size(mSizeWidthEdit->text().toInt(), mSizeHeightEdit->text().toInt());
    QImage qrcode = QZXing::encodeData(data,
                                       QZXing::EncoderFormat_QR_CODE,
                                       size,
                                       errorCorrection,
                                       false,
                                       false);

    QPixmap pixmap = QPixmap::fromImage(qrcode);
    pixmap = pixmap.scaled(mPreviewLabel->size());
    mPreviewLabel->setPixmap(pixmap);
}
