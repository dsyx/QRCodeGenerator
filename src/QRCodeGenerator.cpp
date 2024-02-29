#include "QRCodeGenerator.h"
#include <QDebug>
#include <QFileDialog>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QMessageBox>
#include <QPixmap>
#include <QSpacerItem>
#include <QVBoxLayout>

QRCodeGenerator::QRCodeGenerator(QWidget *parent)
    : QMainWindow(parent)
{
    resize(600, 600);

    initMenuBar();
    initPreviewPart();
    initSettingPart();
    initOperationPart();

    setMenuBar(mMenuBar);

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

void QRCodeGenerator::initMenuBar()
{
    mFileMenu = new QMenu(QStringLiteral("&File"));
    mSaveAsAction = new QAction(QStringLiteral("Save &as"));
    connect(mSaveAsAction, &QAction::triggered, this, &QRCodeGenerator::saveAs);
    mFileMenu->addAction(mSaveAsAction);

    mMenuBar = new QMenuBar();
    mMenuBar->addMenu(mFileMenu);
}

void QRCodeGenerator::initPreviewPart()
{
    mPreviewLabel = new QLabel(QStringLiteral("QR Code Preview"));
    mPreviewLabel->setAlignment(Qt::AlignCenter);
    mPreviewLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    QGridLayout *layout = new QGridLayout();
    layout->addWidget(mPreviewLabel);

    mPreviewWidget = new QWidget();
    mPreviewWidget->setLayout(layout);
}

void QRCodeGenerator::initSettingPart()
{
    mErrorCorrectionLabel = new QLabel(QStringLiteral("Error Correction:"));
    mErrorCorrectionComboBox = new QComboBox();
    mErrorCorrectionMap.insert(QStringLiteral("Low"), QZXing::EncodeErrorCorrectionLevel_L);
    mErrorCorrectionMap.insert(QStringLiteral("Medium"), QZXing::EncodeErrorCorrectionLevel_M);
    mErrorCorrectionMap.insert(QStringLiteral("Quartile"), QZXing::EncodeErrorCorrectionLevel_Q);
    mErrorCorrectionMap.insert(QStringLiteral("High"), QZXing::EncodeErrorCorrectionLevel_H);
    mErrorCorrectionComboBox->insertItems(0, mErrorCorrectionMap.keys());

    mSizeLabel = new QLabel(QStringLiteral("Size (Width x Height):"));
    mSizeWidthEdit = new QLineEdit(QStringLiteral("256"));
    mSizeWidthEdit->setValidator(new QIntValidator(mSizeWidthEdit));
    mSizeHeightEdit = new QLineEdit(QStringLiteral("256"));
    mSizeHeightEdit->setValidator(new QIntValidator(mSizeHeightEdit));
    QHBoxLayout *sizeInputLayout = new QHBoxLayout();
    sizeInputLayout->addWidget(mSizeWidthEdit);
    sizeInputLayout->addWidget(new QLabel(QStringLiteral("x")));
    sizeInputLayout->addWidget(mSizeHeightEdit);

    QFormLayout *layout = new QFormLayout();
    layout->addRow(mErrorCorrectionLabel, mErrorCorrectionComboBox);
    layout->addRow(mSizeLabel, sizeInputLayout);

    mSettingWidget = new QWidget();
    mSettingWidget->setLayout(layout);
}

void QRCodeGenerator::initOperationPart()
{
    mDataEdit = new QTextEdit(QStringLiteral("Hello World"));

    mGenerateButton = new QPushButton(QStringLiteral("Generate"));
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

void QRCodeGenerator::saveAs()
{
    if (mQRCode.isNull()) {
        QMessageBox::critical(this,
                              QStringLiteral("Error"),
                              QStringLiteral(
                                  "No valid QR code found, please generate a QR code first."));
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this,
                                                    QStringLiteral("Save QR Code"),
                                                    QString(),
                                                    QStringLiteral("Images (*.png *.xpm *.jpg)"));
    if (fileName.isEmpty()) {
        return;
    }
    if (!mQRCode.save(fileName)) {
        QMessageBox::warning(this,
                             QStringLiteral("Warning"),
                             QStringLiteral("Failed to save QR code."));
    }
}

void QRCodeGenerator::generate()
{
    QString data = mDataEdit->toPlainText();
    if (data.isEmpty()) {
        return;
    }
    QZXing::EncodeErrorCorrectionLevel errorCorrection = mErrorCorrectionMap[mErrorCorrectionComboBox->currentText()];
    QSize size(mSizeWidthEdit->text().toInt(), mSizeHeightEdit->text().toInt());
    mQRCode = QZXing::encodeData(data,
                                 QZXing::EncoderFormat_QR_CODE,
                                 size,
                                 errorCorrection,
                                 false,
                                 false);

    QPixmap pixmap = QPixmap::fromImage(mQRCode);
    pixmap = pixmap.scaled(mPreviewLabel->size());
    mPreviewLabel->setPixmap(pixmap);
}
