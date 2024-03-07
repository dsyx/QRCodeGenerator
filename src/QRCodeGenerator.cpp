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
    : QMainWindow(parent)
{
    initMenuBar();
    initPreviewPart();
    initSettingPart();
    initOperationPart();
    initPageSetupDialog();

    QHBoxLayout *psLayout = new QHBoxLayout();
    psLayout->addWidget(mPreviewWidget, 6);
    psLayout->addWidget(mSettingWidget, 4);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addLayout(psLayout, 6);
    layout->addWidget(mOperationWidget, 4);

    QWidget *centeralWidget = new QWidget();
    centeralWidget->setLayout(layout);
    setCentralWidget(centeralWidget);

    resize(minimumSizeHint());
}

QRCodeGenerator::~QRCodeGenerator() {}

void QRCodeGenerator::initMenuBar()
{
    mSaveAsAction = new QAction(QStringLiteral("Save as"));
    mSaveAsAction->setShortcut(QKeySequence::SaveAs);
    connect(mSaveAsAction, &QAction::triggered, this, &QRCodeGenerator::saveAs);

    mPrintPreviewAction = new QAction(QStringLiteral("Print Preview"));
    connect(mPrintPreviewAction, &QAction::triggered, this, &QRCodeGenerator::printPreview);

    mPageSetupAction = new QAction(QStringLiteral("Page Setup"));
    connect(mPageSetupAction, &QAction::triggered, this, &QRCodeGenerator::pageSetup);

    mPrintAction = new QAction(QStringLiteral("Print"));
    mPrintAction->setShortcut(QKeySequence::Print);
    connect(mPrintAction, &QAction::triggered, this, &QRCodeGenerator::print);

    mExitAction = new QAction(QStringLiteral("Exit"));
    connect(mExitAction, &QAction::triggered, this, &QRCodeGenerator::exit);

    mFileMenu = new QMenu(QStringLiteral("File"));
    mFileMenu->addAction(mSaveAsAction);
    mFileMenu->addSeparator();
    mFileMenu->addAction(mPrintPreviewAction);
    mFileMenu->addAction(mPageSetupAction);
    mFileMenu->addAction(mPrintAction);
    mFileMenu->addSeparator();
    mFileMenu->addAction(mExitAction);

    mMenuBar = new QMenuBar();
    mMenuBar->addMenu(mFileMenu);
    setMenuBar(mMenuBar);
}

void QRCodeGenerator::initPreviewPart()
{
    mPreviewWidget = new QLabel(QStringLiteral("QR Code Preview"));
    mPreviewWidget->setAlignment(Qt::AlignCenter);
    mPreviewWidget->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    mPreviewWidget->setMinimumSize(DEFAULT_QR_CODE_LENGTH_OR_WIDTH, DEFAULT_QR_CODE_LENGTH_OR_WIDTH);
}

void QRCodeGenerator::initSettingPart()
{
    mErrorCorrectionHash.insert(QStringLiteral("Low"), QZXing::EncodeErrorCorrectionLevel_L);
    mErrorCorrectionHash.insert(QStringLiteral("Medium"), QZXing::EncodeErrorCorrectionLevel_M);
    mErrorCorrectionHash.insert(QStringLiteral("Quartile"), QZXing::EncodeErrorCorrectionLevel_Q);
    mErrorCorrectionHash.insert(QStringLiteral("High"), QZXing::EncodeErrorCorrectionLevel_H);

    mErrorCorrectionLabel = new QLabel(QStringLiteral("Error Correction:"));
    mErrorCorrectionComboBox = new QComboBox();
    mErrorCorrectionComboBox->insertItem(0, QStringLiteral("Low"));
    mErrorCorrectionComboBox->insertItem(1, QStringLiteral("Medium"));
    mErrorCorrectionComboBox->insertItem(2, QStringLiteral("Quartile"));
    mErrorCorrectionComboBox->insertItem(3, QStringLiteral("High"));
    mErrorCorrectionComboBox->setCurrentIndex(2);

    mSizeLabel = new QLabel(QStringLiteral("Size (Width x Height):"));
    mSizeWidthEdit = new QLineEdit(QString::number(DEFAULT_QR_CODE_LENGTH_OR_WIDTH));
    mSizeWidthEdit->setValidator(new QIntValidator(mSizeWidthEdit));
    mSizeHeightEdit = new QLineEdit(QString::number(DEFAULT_QR_CODE_LENGTH_OR_WIDTH));
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
    mDataEdit = new QPlainTextEdit(QStringLiteral("Hello World"));

    mPrintButton = new QPushButton(QStringLiteral("Print"));
    connect(mPrintButton, &QPushButton::clicked, this, &QRCodeGenerator::print);

    mGenerateButton = new QPushButton(QStringLiteral("Generate"));
    connect(mGenerateButton, &QPushButton::clicked, this, &QRCodeGenerator::generate);

    QVBoxLayout *buttonLayout = new QVBoxLayout();
    QSpacerItem *verticalSpacer = new QSpacerItem(20,
                                                  40,
                                                  QSizePolicy::Minimum,
                                                  QSizePolicy::Expanding);
    buttonLayout->addSpacerItem(verticalSpacer);
    buttonLayout->addWidget(mPrintButton);
    buttonLayout->addWidget(mGenerateButton);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(mDataEdit, 8);
    layout->addLayout(buttonLayout, 2);

    mOperationWidget = new QWidget();
    mOperationWidget->setLayout(layout);
}

void QRCodeGenerator::initPageSetupDialog()
{
    mPageSetupDialog = new PageSetupDialog(this);
}

QMarginsF QRCodeGenerator::millimeterToOther(QMarginsF margins, QPageLayout::Unit unit)
{
    qreal factor = 1.0;
    switch (unit) {
    case QPageLayout::Point:
        factor = 72.0 / 25.4;
        break;
    case QPageLayout::Inch:
        factor = 1.0 / 25.4;
        break;
    case QPageLayout::Pica:
        factor = 6.0 / 25.4;
        break;
    case QPageLayout::Didot:
        factor = 1.0 / 0.375;
        break;
    case QPageLayout::Cicero:
        factor = 1.0 / 4.5;
        break;
    default:
        break;
    }
    return margins * factor;
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
        QMessageBox::critical(this,
                              QStringLiteral("Error"),
                              QStringLiteral("Failed to save QR code."));
    }
}

void QRCodeGenerator::printPreview()
{
    if (mQRCode.isNull()) {
        QMessageBox::critical(this,
                              QStringLiteral("Error"),
                              QStringLiteral(
                                  "No valid QR code found, please generate a QR code first."));
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
    if (mQRCode.isNull()) {
        QMessageBox::critical(this,
                              QStringLiteral("Error"),
                              QStringLiteral(
                                  "No valid QR code found, please generate a QR code first."));
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
    if (QMessageBox::question(this,
                              "Exit",
                              "Are you sure you want to exit?",
                              QMessageBox::Yes | QMessageBox::No)
        == QMessageBox::Yes) {
        qApp->quit();
    }
}

void QRCodeGenerator::generate()
{
    QString data = mDataEdit->toPlainText();
    if (data.isEmpty()) {
        return;
    }
    QZXing::EncodeErrorCorrectionLevel errorCorrection
        = mErrorCorrectionHash[mErrorCorrectionComboBox->currentText()];
    QSize size(mSizeWidthEdit->text().toInt(), mSizeHeightEdit->text().toInt());
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
