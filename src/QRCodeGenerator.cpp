#include "QRCodeGenerator.h"
#include <QFormLayout>
#include <QHBoxLayout>
#include <QImage>
#include <QIntValidator>
#include <QPixmap>
#include <QSpacerItem>
#include <QVBoxLayout>

QRCodeGenerator::QRCodeGenerator(QWidget *parent)
    : QMainWindow(parent)
{
    resize(600, 600);

    initPreviewPart();
    initSettingPart();
    initOperationPart();

    QHBoxLayout *psLayout = new QHBoxLayout();
    psLayout->addWidget(mPreviewWidget, 6);
    psLayout->addWidget(mSettingWidget, 4);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addLayout(psLayout, 6);
    layout->addWidget(mOperationWidget, 4);

    QWidget *centeralWidget = new QWidget();
    centeralWidget->setLayout(layout);
    setCentralWidget(centeralWidget);
}

QRCodeGenerator::~QRCodeGenerator() {}

void QRCodeGenerator::initPreviewPart()
{
    mPreviewLabel = new QLabel("QR Code Preview");
    mPreviewLabel->setAlignment(Qt::AlignCenter);
    mPreviewLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    QGridLayout *layout = new QGridLayout();
    layout->addWidget(mPreviewLabel);

    mPreviewWidget = new QWidget();
    mPreviewWidget->setLayout(layout);
}

void QRCodeGenerator::initSettingPart()
{
    mErrorCorrectionLabel = new QLabel("Error Correction:");
    mErrorCorrectionComboBox = new QComboBox();
    mErrorCorrectionMap.insert(QString("Low"), QZXing::EncodeErrorCorrectionLevel_L);
    mErrorCorrectionMap.insert(QString("Medium"), QZXing::EncodeErrorCorrectionLevel_M);
    mErrorCorrectionMap.insert(QString("Quartile"), QZXing::EncodeErrorCorrectionLevel_Q);
    mErrorCorrectionMap.insert(QString("High"), QZXing::EncodeErrorCorrectionLevel_H);
    mErrorCorrectionComboBox->insertItems(0, mErrorCorrectionMap.keys());

    mSizeLabel = new QLabel("Size (Width x Height):");
    mSizeWidthEdit = new QLineEdit("256");
    mSizeWidthEdit->setValidator(new QIntValidator(mSizeWidthEdit));
    mSizeHeightEdit = new QLineEdit("256");
    mSizeHeightEdit->setValidator(new QIntValidator(mSizeHeightEdit));
    QHBoxLayout *sizeInputLayout = new QHBoxLayout();
    sizeInputLayout->addWidget(mSizeWidthEdit);
    sizeInputLayout->addWidget(new QLabel("x"));
    sizeInputLayout->addWidget(mSizeHeightEdit);

    QFormLayout *layout = new QFormLayout();
    layout->addRow(mErrorCorrectionLabel, mErrorCorrectionComboBox);
    layout->addRow(mSizeLabel, sizeInputLayout);

    mSettingWidget = new QWidget();
    mSettingWidget->setLayout(layout);
}

void QRCodeGenerator::initOperationPart()
{
    mDataEdit = new QTextEdit("Hello World");

    mGenerateButton = new QPushButton("Generate");
    connect(mGenerateButton, &QPushButton::clicked, this, &QRCodeGenerator::generate);

    QVBoxLayout *buttonLayout = new QVBoxLayout();
    QSpacerItem *verticalSpacer = new QSpacerItem(20,
                                                  40,
                                                  QSizePolicy::Minimum,
                                                  QSizePolicy::Expanding);
    buttonLayout->addSpacerItem(verticalSpacer);
    buttonLayout->addWidget(mGenerateButton);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(mDataEdit, 8);
    layout->addLayout(buttonLayout, 2);

    mOperationWidget = new QWidget();
    mOperationWidget->setLayout(layout);
}

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
