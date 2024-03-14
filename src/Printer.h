#ifndef PRINTER_H
#define PRINTER_H

#include "PrinterSetupDialog.h"
#include <QImage>
#include <QPrinter>
#include <QSharedPointer>
#include <QWidget>

class Printer : public QWidget
{
    Q_OBJECT

public:
    explicit Printer(QWidget *parent = nullptr);
    ~Printer();

    int preview(const QImage &image);
    void setup();
    void print(const QImage &image);

private:
    void print(QPrinter *printer, const QImage &image);

private:
    QSharedPointer<QPrinter> mPrinter;
    QWidget *mParent;
    bool mScaling;

private:
    PrinterSetupDialog *mPrinterSetupDialog;
};

#endif // PRINTER_H
