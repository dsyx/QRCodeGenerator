#include "QRCodeGenerator.h"
#include <QApplication>
#include <QFileDialog>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QMessageBox>
#include <QPainter>
#include <QPixmap>
#include <QPrintPreviewDialog>
#include <QSizePolicy>
#include <QSpacerItem>
#include <QVBoxLayout>
#include <QDebug>

QRCodeGenerator::QRCodeGenerator(QWidget *parent)
    : QMainWindow{parent}
    , mMenuBar{new MenuBar()}
    , mPreviewWidget{new QLabel(QStringLiteral("QR Code Preview"))}
    , mQRCodeSetupWidget{new QRCodeSetupWidget()}
    , mOperationWidget{new OperationWidget()}
    , mPrinterSetupDialog{new PrinterSetupDialog(this)}
{
    // MenuBar
    setMenuBar(mMenuBar);
    connect(mMenuBar, &MenuBar::saveAs, this, &QRCodeGenerator::saveAs);
    connect(mMenuBar, &MenuBar::printPreview, this, &QRCodeGenerator::printPreview);
    connect(mMenuBar, &MenuBar::printerSetup, this, &QRCodeGenerator::printerSetup);
    connect(mMenuBar, &MenuBar::print, this, &QRCodeGenerator::print);
    connect(mMenuBar, &MenuBar::exit, this, &QRCodeGenerator::exit);

    // PreviewWidget
    mPreviewWidget->setAlignment(Qt::AlignCenter);
    mPreviewWidget->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    mPreviewWidget->setMinimumSize(DEFAULT_QR_CODE_LENGTH_OR_WIDTH, DEFAULT_QR_CODE_LENGTH_OR_WIDTH);

    // OperationWidget
    connect(mOperationWidget, &OperationWidget::print, this, &QRCodeGenerator::print);
    connect(mOperationWidget, &OperationWidget::generate, this, &QRCodeGenerator::generate);

    QHBoxLayout *psLayout = new QHBoxLayout();
    psLayout->addWidget(mPreviewWidget, 6);
    psLayout->addWidget(mQRCodeSetupWidget, 4);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addLayout(psLayout, 6);
    layout->addWidget(mOperationWidget, 4);

    QWidget *centeralWidget = new QWidget();
    centeralWidget->setLayout(layout);
    setCentralWidget(centeralWidget);

    resize(minimumSizeHint());
}

QRCodeGenerator::~QRCodeGenerator() {}

bool QRCodeGenerator::checkQRCode()
{
    if (mQRCode.isNull()) {
        QMessageBox::critical(this,
                              QStringLiteral("Error"),
                              QStringLiteral("QRCode not available."));
        return false;
    }

    return true;
}

void QRCodeGenerator::saveAs()
{
    if (!checkQRCode()) {
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
        QMessageBox::critical(this,
                              QStringLiteral("Error"),
                              QStringLiteral("Failed to save QR code."));
    }
}

void QRCodeGenerator::printPreview()
{
    if (!checkQRCode()) {
        return;
    }

    QPrinter *printer = mPrinterSetupDialog->printer();
    QPrintPreviewDialog dialog(printer);
    connect(&dialog, &QPrintPreviewDialog::paintRequested, this, &QRCodeGenerator::paintQRCode);
    dialog.exec();
}

void QRCodeGenerator::printerSetup()
{
    mPrinterSetupDialog->exec();
}

void QRCodeGenerator::print()
{
    if (!checkQRCode()) {
        return;
    }

    QPrinter *printer = mPrinterSetupDialog->printer();
    paintQRCode(printer);

    QMessageBox::information(this, QStringLiteral("Info"), QStringLiteral("Sent to print queue."));
}

void QRCodeGenerator::exit()
{
    qApp->quit();
}

void QRCodeGenerator::generate()
{
    QString data = mOperationWidget->data();
    if (data.isEmpty()) {
        return;
    }
    QZXing::EncodeErrorCorrectionLevel errorCorrection = mQRCodeSetupWidget->errorCorrection();
    QSize size = mQRCodeSetupWidget->size();
    mQRCode = QZXing::encodeData(data,
                                 QZXing::EncoderFormat_QR_CODE,
                                 size,
                                 errorCorrection,
                                 false,
                                 false);

    QPixmap pixmap = QPixmap::fromImage(mQRCode);
    pixmap = pixmap.scaled(mPreviewWidget->size());
    mPreviewWidget->setPixmap(pixmap);
}

void QRCodeGenerator::paintQRCode(QPrinter *printer)
{
    if (!printer || !checkQRCode()) {
        return;
    }

    bool scaling = mPrinterSetupDialog->isScaling();
    QRect pageRect = printer->pageLayout().paintRectPixels(printer->resolution());
    QRect paintRect = QRect(0,
                            0,
                            scaling ? pageRect.width() : mQRCode.width(),
                            scaling ? pageRect.height() : mQRCode.height());

    QPainter painter(printer);
    painter.drawImage(paintRect, mQRCode);
}
