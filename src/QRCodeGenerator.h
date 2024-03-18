#ifndef QRCODEGENERATOR_H
#define QRCODEGENERATOR_H

#include "MenuBar.h"
#include "OperationWidget.h"
#include "Printer.h"
#include "QRCodeWidget.h"
#include <QMainWindow>

class QRCodeGenerator : public QMainWindow
{
    Q_OBJECT

public:
    QRCodeGenerator(QWidget *parent = nullptr);
    ~QRCodeGenerator();

private:
    bool checkQRCode(const QImage &qrcode);

private slots:
    void saveAs();
    void printPreview();
    void printerSetup();
    void print();
    void exit();

private:
    MenuBar *mMenuBar;
    QRCodeWidget *mQRCodeWidget;
    OperationWidget *mOperationWidget;
    Printer *mPrinter;

private:
    static constexpr char VERSION[] = "v0.0.1";
};
#endif // QRCODEGENERATOR_H
