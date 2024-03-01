#ifndef QRCODEGENERATOR_H
#define QRCODEGENERATOR_H

#include <QAction>
#include <QComboBox>
#include <QImage>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QMap>
#include <QMenu>
#include <QMenuBar>
#include <QPushButton>
#include <QTextEdit>
#include <QWidget>
#include "QZXing.h"

class QRCodeGenerator : public QMainWindow
{
    Q_OBJECT

public:
    QRCodeGenerator(QWidget *parent = nullptr);
    ~QRCodeGenerator();

private:
    void initMenuBar();
    void initPreviewPart();
    void initSettingPart();
    void initOperationPart();

private slots:
    void saveAs();
    void generate();
    void print();
    void generateAndPrint();

private:
    QImage mQRCode;

    QMenuBar *mMenuBar;
    QMenu *mFileMenu;
    QAction *mSaveAsAction;

    QLabel *mPreviewLabel;
    QWidget *mPreviewWidget;

    QLabel *mErrorCorrectionLabel;
    QComboBox *mErrorCorrectionComboBox;
    QMap<QString, QZXing::EncodeErrorCorrectionLevel> mErrorCorrectionMap;
    QLabel *mSizeLabel;
    QLineEdit *mSizeWidthEdit;
    QLineEdit *mSizeHeightEdit;
    QLabel *mPrinterLabel;
    QComboBox *mPrinterComboBox;
    QWidget *mSettingWidget;

    QTextEdit *mDataEdit;
    QPushButton *mGenerateAndPrintButton;
    QPushButton *mPrintButton;
    QPushButton *mGenerateButton;
    QWidget *mOperationWidget;

private:
    static constexpr int DEFAULT_QR_CODE_LENGTH_OR_WIDTH = 256;
};
#endif // QRCODEGENERATOR_H
