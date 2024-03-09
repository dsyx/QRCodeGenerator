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
#ifndef NDEBUG
#include <QDebug>
#endif

QRCodeGenerator::QRCodeGenerator(QWidget *parent)
    : QMainWindow{parent}
    , mMenuBar{new MenuBar()}
    , mPreviewWidget{new QLabel(QStringLiteral("QR Code Preview"))}
    , mQRCodeSetupWidget{new QRCodeSetupWidget()}
    , mOperationWidget{new OperationWidget()}
    , mPageSetupDialog{new PageSetupDialog(this)}
{
    // MenuBar
    setMenuBar(mMenuBar);
    connect(mMenuBar, &MenuBar::saveAs, this, &QRCodeGenerator::saveAs);
    connect(mMenuBar, &MenuBar::printPreview, this, &QRCodeGenerator::printPreview);
    connect(mMenuBar, &MenuBar::pageSetup, this, &QRCodeGenerator::pageSetup);
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

    if (mPageSetupDialog->printerName().isEmpty()) {
        QMessageBox::critical(this,
                              QStringLiteral("Error"),
                              QStringLiteral("No printer is selected."));
        return;
    }

    QPrinter printer;
    printer.setPrinterName(mPageSetupDialog->printerName());
    printer.setFullPage(true);
    printer.setPageOrientation(mPageSetupDialog->orientation());
    printer.setPageMargins(mPageSetupDialog->margins(), QPageLayout::Millimeter);
    QPrintPreviewDialog dialog(&printer);
    connect(&dialog, &QPrintPreviewDialog::paintRequested, this, &QRCodeGenerator::paintToPrinter);
    dialog.exec();
}

void QRCodeGenerator::pageSetup()
{
    mPageSetupDialog->exec();
}

void QRCodeGenerator::print()
{
    if (!checkQRCode()) {
        return;
    }

    if (mPageSetupDialog->printerName().isEmpty()) {
        QMessageBox::critical(this,
                              QStringLiteral("Error"),
                              QStringLiteral("No printer is selected."));
        return;
    }

    QPrinter printer;
    printer.setPrinterName(mPageSetupDialog->printerName());
    printer.setFullPage(true);
    printer.setPageOrientation(mPageSetupDialog->orientation());
    printer.setPageMargins(mPageSetupDialog->margins(), QPageLayout::Millimeter);
    paintToPrinter(&printer);

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

void QRCodeGenerator::paintToPrinter(QPrinter *printer)
{
    if (!printer) {
        return;
    }
    if (mQRCode.isNull()) {
        return;
    }

    int resolution = printer->resolution();
    QPageLayout layout = printer->pageLayout();
    QRect paperRect = layout.fullRectPixels(resolution);
    QMargins margins = layout.marginsPixels(resolution);
    QRect paintRect(paperRect.x() + margins.left(),
                    paperRect.y() + margins.top(),
                    paperRect.width() - margins.right(),
                    paperRect.height() - margins.bottom());
    if (mPageSetupDialog->printingMethod() == PageSetupDialog::Normal) {
        paintRect.setWidth(paintRect.width() > mQRCode.width() ? mQRCode.width()
                                                               : paintRect.width());
        paintRect.setHeight(paintRect.height() > mQRCode.height() ? mQRCode.height()
                                                                  : paintRect.height());
    }

    QPainter painter;
    painter.begin(printer);
    painter.drawImage(paintRect, mQRCode);
#ifndef NDEBUG
    QPen paperRectPen;
    paperRectPen.setColor(Qt::red);
    paperRectPen.setStyle(Qt::SolidLine);
    paperRectPen.setWidth(2);
    painter.setPen(paperRectPen);
    painter.drawRect(paperRect);
    qDebug() << "Draw PaperRect:" << paperRect;

    QPen paintRectPen;
    paintRectPen.setColor(Qt::green);
    paintRectPen.setStyle(Qt::DotLine);
    paintRectPen.setWidth(2);
    painter.setPen(paintRectPen);
    painter.drawRect(paintRect);
    qDebug() << "Draw PaintRect:" << paintRect;
#endif
    painter.end();
}
