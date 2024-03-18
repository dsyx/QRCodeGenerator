#include "QRCodeGenerator.h"
#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QVBoxLayout>

QRCodeGenerator::QRCodeGenerator(QWidget *parent)
    : QMainWindow{parent}
    , mMenuBar{new MenuBar{}}
    , mQRCodeWidget{new QRCodeWidget{}}
    , mOperationWidget{new OperationWidget{}}
    , mPrinter{new Printer(this)}
{
    // MenuBar
    {
        setMenuBar(mMenuBar);
        connect(mMenuBar, &MenuBar::saveAsRequested, this, &QRCodeGenerator::saveAs);
        connect(mMenuBar, &MenuBar::printPreviewRequested, this, &QRCodeGenerator::printPreview);
        connect(mMenuBar, &MenuBar::printerSetupRequested, this, &QRCodeGenerator::printerSetup);
        connect(mMenuBar, &MenuBar::printRequested, this, &QRCodeGenerator::print);
        connect(mMenuBar, &MenuBar::exitRequested, this, &QRCodeGenerator::exit);
    }

    // OperationWidget
    {
        connect(mOperationWidget, &OperationWidget::printRequested, this, &QRCodeGenerator::print);
        connect(mOperationWidget, &OperationWidget::generateRequested, mQRCodeWidget, &QRCodeWidget::setQRCodeData);
    }

    // Layout
    {
        QVBoxLayout *layout{new QVBoxLayout{}};
        layout->addWidget(mQRCodeWidget, 6);
        layout->addWidget(mOperationWidget, 4);

        QWidget *centeralWidget{new QWidget{}};
        centeralWidget->setLayout(layout);
        setCentralWidget(centeralWidget);
    }

    setWindowTitle(QString("QRCodeGenerator %1").arg(VERSION));
    resize(minimumSizeHint());
}

QRCodeGenerator::~QRCodeGenerator() {}

bool QRCodeGenerator::checkQRCode(const QImage &qrcode)
{
    if (qrcode.isNull()) {
        QMessageBox::critical(this, QStringLiteral("Error"), QStringLiteral("QRCode not available."));
        return false;
    }

    return true;
}

void QRCodeGenerator::saveAs()
{
    QImage qrcode{mQRCodeWidget->qrcode()};
    if (!checkQRCode(qrcode)) {
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this,
                                                    QStringLiteral("Save QR Code"),
                                                    QString{},
                                                    QStringLiteral("Images (*.png *.xpm *.jpg)"));
    if (fileName.isEmpty()) {
        return;
    }

    if (!qrcode.save(fileName)) {
        QMessageBox::critical(this, QStringLiteral("Error"), QStringLiteral("Failed to save QR code."));
    }
}

void QRCodeGenerator::printPreview()
{
    QImage qrcode{mQRCodeWidget->qrcode()};
    if (!checkQRCode(qrcode)) {
        return;
    }

    mPrinter->preview(qrcode);
}

void QRCodeGenerator::printerSetup()
{
    mPrinter->setup();
}

void QRCodeGenerator::print()
{
    QImage qrcode{mQRCodeWidget->qrcode()};
    if (!checkQRCode(qrcode)) {
        return;
    }

    mPrinter->print(qrcode);
    QMessageBox::information(this, QStringLiteral("Info"), QStringLiteral("Sent to print queue."));
}

void QRCodeGenerator::exit()
{
    qApp->quit();
}
