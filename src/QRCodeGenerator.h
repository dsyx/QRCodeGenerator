#ifndef QRCODEGENERATOR_H
#define QRCODEGENERATOR_H

#include <QImage>
#include <QLabel>
#include <QMainWindow>
#include <QPrinter>
#include "MenuBar.h"
#include "OperationWidget.h"
#include "PageSetupDialog.h"
#include "QRCodeSetupWidget.h"

class QRCodeGenerator : public QMainWindow
{
    Q_OBJECT

public:
    QRCodeGenerator(QWidget *parent = nullptr);
    ~QRCodeGenerator();

private:
    bool checkQRCode();

private slots:
    void saveAs();
    void printPreview();
    void pageSetup();
    void print();
    void exit();
    void generate();
    void paintToPrinter(QPrinter *printer);

private:
    QImage mQRCode;

private:
    MenuBar *mMenuBar;
    QLabel *mPreviewWidget;
    QRCodeSetupWidget *mQRCodeSetupWidget;
    OperationWidget *mOperationWidget;
    PageSetupDialog *mPageSetupDialog;

private:
    static constexpr int DEFAULT_QR_CODE_LENGTH_OR_WIDTH = 256;
};
#endif // QRCODEGENERATOR_H
