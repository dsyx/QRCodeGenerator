#include "Printer.h"
#include <QPainter>
#include <QPrintPreviewDialog>

Printer::Printer(QWidget *parent)
    : QWidget{parent}
    , mPrinter{QSharedPointer<QPrinter>::create(QPrinter::HighResolution)}
    , mParent{parent}
    , mPrinterSetupDialog{new PrinterSetupDialog{mPrinter, mParent}}
{
    mScaling = mPrinterSetupDialog->scaling();
}

Printer::~Printer() {}

int Printer::preview(const QImage &image)
{
    QPrintPreviewDialog dialog(mPrinter.get());
    connect(&dialog, &QPrintPreviewDialog::paintRequested, this, [&](QPrinter *printer) {
        print(printer, image);
    });
    return dialog.exec();
}

void Printer::setup()
{
    if (mPrinterSetupDialog->exec() == QDialog::Accepted) {
        mScaling = mPrinterSetupDialog->scaling();
    }
}

void Printer::print(const QImage &image)
{
    print(mPrinter.get(), image);
}

void Printer::print(QPrinter *printer, const QImage &image)
{
    if (!printer) {
        return;
    }

    QRect pageRect{printer->pageLayout().paintRectPixels(printer->resolution())};
    QRect paintRect{0,
                    0,
                    mScaling ? pageRect.width() : image.width(),
                    mScaling ? pageRect.height() : image.height()};

    QPainter painter{printer};
    painter.drawImage(paintRect, image);
}
