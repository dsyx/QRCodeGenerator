#ifndef QRCODEGENERATOR_H
#define QRCODEGENERATOR_H

#include <QAction>
#include <QComboBox>
#include <QHash>
#include <QImage>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QPlainTextEdit>
#include <QPrinter>
#include <QPushButton>
#include <QWidget>
#include "PageSetupDialog.h"
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
    void initPageSetupDialog();

    QMarginsF millimeterToOther(QMarginsF margins, QPageLayout::Unit unit);

private slots:
    void saveAs();
    void printPreview();
    void pageSetup();
    void print();
    void exit();
    void generate();
    void paintToPrinter(QPrinter *printer);

private:
    QMenuBar *mMenuBar;
    QMenu *mFileMenu;
    QAction *mSaveAsAction;
    QAction *mPrintPreviewAction;
    QAction *mPageSetupAction;
    QAction *mPrintAction;
    QAction *mExitAction;

    QLabel *mPreviewWidget;

    QLabel *mErrorCorrectionLabel;
    QComboBox *mErrorCorrectionComboBox;
    QLabel *mSizeLabel;
    QLineEdit *mSizeWidthEdit;
    QLineEdit *mSizeHeightEdit;
    QWidget *mSettingWidget;

    QPlainTextEdit *mDataEdit;
    QPushButton *mPrintButton;
    QPushButton *mGenerateButton;
    QWidget *mOperationWidget;

    PageSetupDialog *mPageSetupDialog;

    QImage mQRCode;
    QHash<QString, QZXing::EncodeErrorCorrectionLevel> mErrorCorrectionHash;

private:
    static constexpr int DEFAULT_QR_CODE_LENGTH_OR_WIDTH = 128;
};
#endif // QRCODEGENERATOR_H
