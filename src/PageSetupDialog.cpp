#include "PageSetupDialog.h"
#include <QDoubleValidator>
#include <QFormLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QList>
#include <QPrinterInfo>
#include <QSpacerItem>
#include <QVBoxLayout>
#ifndef NDEBUG
#include <QDebug>
#endif

PageSetupDialog::PageSetupDialog(QWidget *parent)
    : QDialog(parent)
{
    mPrintingMethodHash.insert(QStringLiteral("Normal"), Normal);
    mPrintingMethodHash.insert(QStringLiteral("Scale"), Scale);

    mOrientationHash.insert(QStringLiteral("Portrait"), QPageLayout::Portrait);
    mOrientationHash.insert(QStringLiteral("Landscape"), QPageLayout::Landscape);

    mPrinterNameLabel = new QLabel(QStringLiteral("Printer Name:"));
    mPrinterNameComboBox = new QComboBox();
    QList<QPrinterInfo> printerInfoList = QPrinterInfo::availablePrinters();
    for (const auto &printerInfo : printerInfoList) {
        mPrinterNameComboBox->addItem(printerInfo.printerName());
    }
    mPrinterNameComboBox->insertItem(0, QString(""));
    mPrinterNameComboBox->setCurrentIndex(0);

    mPrintingMethodLabel = new QLabel(QStringLiteral("Printing Method:"));
    mPrintingMethodComboBox = new QComboBox();
    mPrintingMethodComboBox->insertItem(0, QStringLiteral("Normal"));
    mPrintingMethodComboBox->insertItem(1, QStringLiteral("Scale"));

    mOrientationLabel = new QLabel(QStringLiteral("Orientation:"));
    mOrientationComboBox = new QComboBox();
    mOrientationComboBox->insertItem(0, QStringLiteral("Portrait"));
    mOrientationComboBox->insertItem(1, QStringLiteral("Landscape"));

    QFormLayout *formLayout = new QFormLayout();
    formLayout->addRow(mPrinterNameLabel, mPrinterNameComboBox);
    formLayout->addRow(mPrintingMethodLabel, mPrintingMethodComboBox);
    formLayout->addRow(mOrientationLabel, mOrientationComboBox);

    mTopMarginLabel = new QLabel(QStringLiteral("Top:"));
    mTopMarginEdit = new QLineEdit(QString::number(DEFAULT_MARGIN_LENGTH));
    mTopMarginEdit->setValidator(new QDoubleValidator(mTopMarginEdit));

    mBottomMarginLabel = new QLabel(QStringLiteral("Bottom:"));
    mBottomMarginEdit = new QLineEdit(QString::number(DEFAULT_MARGIN_LENGTH));
    mBottomMarginEdit->setValidator(new QDoubleValidator(mBottomMarginEdit));

    mLeftMarginLabel = new QLabel(QStringLiteral("Left:"));
    mLeftMarginEdit = new QLineEdit(QString::number(DEFAULT_MARGIN_LENGTH));
    mLeftMarginEdit->setValidator(new QDoubleValidator(mLeftMarginEdit));

    mRightMarginLabel = new QLabel(QStringLiteral("Right:"));
    mRightMarginEdit = new QLineEdit(QString::number(DEFAULT_MARGIN_LENGTH));
    mRightMarginEdit->setValidator(new QDoubleValidator(mRightMarginEdit));

    QGridLayout *marginsLayout = new QGridLayout();
    marginsLayout->addWidget(mTopMarginLabel, 0, 0);
    marginsLayout->addWidget(mTopMarginEdit, 0, 1);
    marginsLayout->addWidget(mBottomMarginLabel, 0, 2);
    marginsLayout->addWidget(mBottomMarginEdit, 0, 3);
    marginsLayout->addWidget(mLeftMarginLabel, 1, 0);
    marginsLayout->addWidget(mLeftMarginEdit, 1, 1);
    marginsLayout->addWidget(mRightMarginLabel, 1, 2);
    marginsLayout->addWidget(mRightMarginEdit, 1, 3);

    mMarginsGroupBox = new QGroupBox(QStringLiteral("Margins (mm)"));
    mMarginsGroupBox->setLayout(marginsLayout);

    mOkButton = new QPushButton(QStringLiteral("OK"));
    connect(mOkButton, &QPushButton::clicked, this, &PageSetupDialog::accept);
    connect(mOkButton, &QPushButton::clicked, this, &PageSetupDialog::updatePageSetup);

    mCancelButton = new QPushButton(QStringLiteral("Cancel"));
    connect(mCancelButton, &QPushButton::clicked, this, &PageSetupDialog::reject);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addSpacerItem(
        new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
    buttonLayout->addWidget(mOkButton);
    buttonLayout->addWidget(mCancelButton);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addLayout(formLayout);
    layout->addWidget(mMarginsGroupBox);
    layout->addLayout(buttonLayout);
    setLayout(layout);

    updatePageSetup();

    resize(sizeHint());
}

PageSetupDialog::~PageSetupDialog() {}

void PageSetupDialog::updatePrinterName()
{
    mPrinterName = mPrinterNameComboBox->currentText();

#ifndef NDEBUG
    if (mPrinterName.isEmpty()) {
        return;
    }

    QPrinter printer;
    printer.setPrinterName(mPrinterName);
    static const char *boolStrings[] = {"False", "True"};
    static const char *colorModeStrings[] = {"GrayScale", "Color"};
    static const char *duplexStrings[] = {"DuplexNone",
                                          "DuplexAuto",
                                          "DuplexLongSide",
                                          "DuplexShortSide"};
    static const char *outputFormatStrings[] = {"NativeFormat", "PdfFormat"};
    static const char *printerStateStrings[] = {"Idle", "Active", "Aborted", "Error"};
    qDebug() << "Printer" << mPrinterName << "Info:";
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
#endif
}

void PageSetupDialog::updatePrintingMethod()
{
    mPrintingMethod = mPrintingMethodHash[mPrintingMethodComboBox->currentText()];
}

void PageSetupDialog::updateOrientation()
{
    mOrientation = mOrientationHash[mOrientationComboBox->currentText()];
}

void PageSetupDialog::updateMargins()
{
    mMargins = QMarginsF(mLeftMarginEdit->text().toDouble(),
                         mTopMarginEdit->text().toDouble(),
                         mRightMarginEdit->text().toDouble(),
                         mBottomMarginEdit->text().toDouble());
}

void PageSetupDialog::updatePageSetup()
{
    updatePrinterName();
    updatePrintingMethod();
    updateOrientation();
    updateMargins();
}
