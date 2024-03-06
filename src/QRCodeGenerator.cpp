#include "QRCodeGenerator.h"
#include <QDebug>
#include <QFileDialog>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QList>
#include <QMessageBox>
#include <QPainter>
#include <QPixmap>
#include <QPrinter>
#include <QPrinterInfo>
#include <QSizePolicy>
#include <QSpacerItem>
#include <QVBoxLayout>

QRCodeGenerator::QRCodeGenerator(QWidget *parent)
    : QMainWindow(parent)
{
    initMenuBar();
    initPreviewPart();
    initSettingPart();
    initOperationPart();

    setMenuBar(mMenuBar);

    QHBoxLayout *psLayout = new QHBoxLayout();
    psLayout->addWidget(mPreviewWidget, 6);
    psLayout->addWidget(mSettingWidget, 4);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addLayout(psLayout, 6);
    layout->addWidget(mOperationWidget, 4);

    QWidget *centeralWidget = new QWidget();
    centeralWidget->setLayout(layout);
    setCentralWidget(centeralWidget);
}

QRCodeGenerator::~QRCodeGenerator() {}

void QRCodeGenerator::initMenuBar()
{
    mFileMenu = new QMenu(QStringLiteral("&File"));
    mSaveAsAction = new QAction(QStringLiteral("Save &as"));
    connect(mSaveAsAction, &QAction::triggered, this, &QRCodeGenerator::saveAs);
    mFileMenu->addAction(mSaveAsAction);
    
    mMenuBar = new QMenuBar();
    mMenuBar->addMenu(mFileMenu);
}

void QRCodeGenerator::initPreviewPart()
{
    mPreviewLabel = new QLabel(QStringLiteral("QR Code Preview"));
    mPreviewLabel->setAlignment(Qt::AlignCenter);
    mPreviewLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    mPreviewLabel->setMinimumSize(DEFAULT_QR_CODE_LENGTH_OR_WIDTH, DEFAULT_QR_CODE_LENGTH_OR_WIDTH);

    QGridLayout *layout = new QGridLayout();
    layout->addWidget(mPreviewLabel);

    mPreviewWidget = new QWidget();
    mPreviewWidget->setLayout(layout);
}

void QRCodeGenerator::initSettingPart()
{
    mErrorCorrectionLabel = new QLabel(QStringLiteral("Error Correction:"));
    mErrorCorrectionComboBox = new QComboBox();
    mErrorCorrectionMap.insert(QStringLiteral("Low"), QZXing::EncodeErrorCorrectionLevel_L);
    mErrorCorrectionMap.insert(QStringLiteral("Medium"), QZXing::EncodeErrorCorrectionLevel_M);
    mErrorCorrectionMap.insert(QStringLiteral("Quartile"), QZXing::EncodeErrorCorrectionLevel_Q);
    mErrorCorrectionMap.insert(QStringLiteral("High"), QZXing::EncodeErrorCorrectionLevel_H);
    mErrorCorrectionComboBox->insertItems(0, mErrorCorrectionMap.keys());

    mSizeLabel = new QLabel(QStringLiteral("Size (Width x Height):"));
    mSizeWidthEdit = new QLineEdit(QString::number(DEFAULT_QR_CODE_LENGTH_OR_WIDTH));
    mSizeWidthEdit->setValidator(new QIntValidator(mSizeWidthEdit));
    mSizeHeightEdit = new QLineEdit(QString::number(DEFAULT_QR_CODE_LENGTH_OR_WIDTH));
    mSizeHeightEdit->setValidator(new QIntValidator(mSizeHeightEdit));
    QHBoxLayout *sizeInputLayout = new QHBoxLayout();
    sizeInputLayout->addWidget(mSizeWidthEdit);
    sizeInputLayout->addWidget(new QLabel(QStringLiteral("x")));
    sizeInputLayout->addWidget(mSizeHeightEdit);

    mPrinterLabel = new QLabel(QStringLiteral("Printer:"));
    mPrinterComboBox = new QComboBox();
    QList<QPrinterInfo> printerInfoList = QPrinterInfo::availablePrinters();
    for (const auto &printerInfo : printerInfoList) {
        mPrinterComboBox->addItem(printerInfo.printerName());
    }
#ifndef NDEBUG
    connect(mPrinterComboBox,
            &QComboBox::currentTextChanged,
            this,
            &QRCodeGenerator::printPrinterInfo);
#endif

    mPrintModeLabel = new QLabel(QStringLiteral("Print Mode:"));
    mPrintModeComboBox = new QComboBox();
    mPrintModeMap.insert(QStringLiteral("Normal"), Normal);
    mPrintModeMap.insert(QStringLiteral("Full"), Full);
    mPrintModeComboBox->insertItems(0, mPrintModeMap.keys());

    QFormLayout *layout = new QFormLayout();
    layout->addRow(mErrorCorrectionLabel, mErrorCorrectionComboBox);
    layout->addRow(mSizeLabel, sizeInputLayout);
    layout->addRow(mPrinterLabel, mPrinterComboBox);
    layout->addRow(mPrintModeLabel, mPrintModeComboBox);

    mSettingWidget = new QWidget();
    mSettingWidget->setLayout(layout);
}

void QRCodeGenerator::initOperationPart()
{
    mDataEdit = new QTextEdit(QStringLiteral("Hello World"));

    mGenerateAndPrintButton = new QPushButton(QStringLiteral("Generate And Print"));
    connect(mGenerateAndPrintButton,
            &QPushButton::clicked,
            this,
            &QRCodeGenerator::generateAndPrint);

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
    buttonLayout->addWidget(mGenerateAndPrintButton);
    buttonLayout->addWidget(mPrintButton);
    buttonLayout->addWidget(mGenerateButton);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(mDataEdit, 8);
    layout->addLayout(buttonLayout, 2);

    mOperationWidget = new QWidget();
    mOperationWidget->setLayout(layout);
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
        QMessageBox::warning(this,
                             QStringLiteral("Warning"),
                             QStringLiteral("Failed to save QR code."));
    }
}

void QRCodeGenerator::generate()
{
    QString data = mDataEdit->toPlainText();
    if (data.isEmpty()) {
        return;
    }
    QZXing::EncodeErrorCorrectionLevel errorCorrection = mErrorCorrectionMap[mErrorCorrectionComboBox->currentText()];
    QSize size(mSizeWidthEdit->text().toInt(), mSizeHeightEdit->text().toInt());
    mQRCode = QZXing::encodeData(data,
                                 QZXing::EncoderFormat_QR_CODE,
                                 size,
                                 errorCorrection,
                                 false,
                                 false);

    QPixmap pixmap = QPixmap::fromImage(mQRCode);
    pixmap = pixmap.scaled(mPreviewLabel->size());
    mPreviewLabel->setPixmap(pixmap);
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

    QPrinter printer;
    printer.setPrinterName(mPrinterComboBox->currentText());
    int resolution = printer.resolution();
    QPageLayout layout = printer.pageLayout();
    QRect fullRect = layout.fullRectPixels(resolution);
    QRect paintableRect(fullRect.x(),
                        fullRect.y(),
                        fullRect.width() - (0.16 * resolution),
                        fullRect.height() - (0.16 * resolution));
    QRect paintRect;
    if (mPrintModeMap[mPrintModeComboBox->currentText()] == Normal) {
        int width = paintableRect.width() > mQRCode.width() ? mQRCode.width()
                                                            : paintableRect.width();
        int height = paintableRect.height() > mQRCode.height() ? mQRCode.height()
                                                               : paintableRect.height();
        paintRect.setRect(paintableRect.x(), paintableRect.y(), width, height);
    } else {
        paintRect.setRect(paintableRect.x(),
                          paintableRect.y(),
                          paintableRect.width(),
                          paintableRect.height());
    }

    QPainter painter;
    painter.begin(&printer);
    painter.drawImage(paintRect, mQRCode);
    painter.end();

    QMessageBox::information(this, QStringLiteral("Info"), QStringLiteral("Sent to print queue."));
}

void QRCodeGenerator::generateAndPrint()
{
    generate();
    print();
}

#ifndef NDEBUG
void QRCodeGenerator::printPrinterInfo(const QString &name)
{
    QPrinter printer;
    printer.setPrinterName(name);
    static const char *boolStrings[] = {"False", "True"};
    static const char *colorModeStrings[] = {"GrayScale", "Color"};
    static const char *duplexStrings[] = {"DuplexNone",
                                          "DuplexAuto",
                                          "DuplexLongSide",
                                          "DuplexShortSide"};
    static const char *outputFormatStrings[] = {"NativeFormat", "PdfFormat"};
    static const char *printerStateStrings[] = {"Idle", "Active", "Aborted", "Error"};
    qDebug() << "Printer" << name << "Info:";
    qDebug() << "\tColor Mode:" << colorModeStrings[printer.colorMode()];
    qDebug() << "\tDuplex Mode:" << duplexStrings[printer.duplex()];
    qDebug() << "\tFull Page:" << boolStrings[printer.fullPage()];
    qDebug() << "\tIs Vaild:" << boolStrings[printer.isValid()];
    qDebug() << "\tOutput Format:" << outputFormatStrings[printer.outputFormat()];
    qDebug() << "\tPrinter State:" << printerStateStrings[printer.printerState()];
    qDebug() << "\tResolution:" << printer.resolution();

    QPageLayout layout = printer.pageLayout();
    static const char *modeStrings[] = {"StandardMode", "FullPageMode"};
    static const char *orientationStrings[] = {"Portrait", "Landscape"};
    static const char *unitsStrings[] = {"Millimeter", "Point", "Inch", "Pica", "Didot", "Cicero"};
    int resolution = printer.resolution();
    qDebug() << "\tPage Layout:";
    qDebug() << "\t\tFull Rect:" << layout.fullRect();
    qDebug() << "\t\tFull Rect Pixels:" << layout.fullRectPixels(resolution);
    qDebug() << "\t\tMargins:" << layout.margins();
    qDebug() << "\t\tMargins Pixels:" << layout.marginsPixels(resolution);
    qDebug() << "\t\tMode:" << modeStrings[layout.mode()];
    qDebug() << "\t\tOrientation:" << orientationStrings[layout.orientation()];
    qDebug() << "\t\tPage Size:" << layout.pageSize();
    qDebug() << "\t\tPage Size Pixels:" << layout.paintRectPixels(resolution);
    qDebug() << "\t\tPaint Rect:" << layout.paintRect();
    qDebug() << "\t\tPaint Rect Pixels:" << layout.paintRectPixels(resolution);
    qDebug() << "\t\tUnits:" << unitsStrings[layout.units()];
}
#endif
