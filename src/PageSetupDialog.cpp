#include "PageSetupDialog.h"
#include <QDoubleValidator>
#include <QFormLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QList>
#include <QPrinterInfo>
#include <QSpacerItem>
#include <QVBoxLayout>

PageSetupDialog::PageSetupDialog(QWidget *parent)
    : QDialog{parent}
    , mPrinterNameLabel{new QLabel(QStringLiteral("Printer Name:"))}
    , mPrinterNameComboBox{new QComboBox()}
    , mPrintingMethodLabel{new QLabel(QStringLiteral("Printing Method:"))}
    , mPrintingMethodComboBox{new QComboBox()}
    , mOrientationLabel{new QLabel(QStringLiteral("Orientation:"))}
    , mOrientationComboBox{new QComboBox()}
    , mMarginsGroupBox{new QGroupBox(QStringLiteral("Margins (mm)"))}
    , mTopMarginLabel{new QLabel(QStringLiteral("Top:"))}
    , mTopMarginEdit{new QLineEdit(QString::number(DEFAULT_MARGIN_LENGTH))}
    , mBottomMarginLabel{new QLabel(QStringLiteral("Bottom:"))}
    , mBottomMarginEdit{new QLineEdit(QString::number(DEFAULT_MARGIN_LENGTH))}
    , mLeftMarginLabel{new QLabel(QStringLiteral("Left:"))}
    , mLeftMarginEdit{new QLineEdit(QString::number(DEFAULT_MARGIN_LENGTH))}
    , mRightMarginLabel{new QLabel(QStringLiteral("Right:"))}
    , mRightMarginEdit{new QLineEdit(QString::number(DEFAULT_MARGIN_LENGTH))}
    , mOkButton{new QPushButton(QStringLiteral("OK"))}
    , mCancelButton{new QPushButton(QStringLiteral("Cancel"))}
{
    // PrinterName
    QList<QPrinterInfo> printerInfoList = QPrinterInfo::availablePrinters();
    for (const auto &printerInfo : printerInfoList) {
        mPrinterNameComboBox->addItem(printerInfo.printerName());
    }
    mPrinterNameComboBox->insertItem(0, QString(""));
    mPrinterNameComboBox->setCurrentIndex(0);

    // PrintingMethod
    mPrintingMethodComboBox->insertItem(0, QStringLiteral("Normal"), Normal);
    mPrintingMethodComboBox->insertItem(1, QStringLiteral("Scale"), Scale);

    // Orientation
    mOrientationComboBox->insertItem(0, QStringLiteral("Portrait"), QPageLayout::Portrait);
    mOrientationComboBox->insertItem(1, QStringLiteral("Landscape"), QPageLayout::Landscape);

    QFormLayout *formLayout = new QFormLayout();
    formLayout->addRow(mPrinterNameLabel, mPrinterNameComboBox);
    formLayout->addRow(mPrintingMethodLabel, mPrintingMethodComboBox);
    formLayout->addRow(mOrientationLabel, mOrientationComboBox);

    // Margins
    mTopMarginEdit->setValidator(new QDoubleValidator(mTopMarginEdit));
    mBottomMarginEdit->setValidator(new QDoubleValidator(mBottomMarginEdit));
    mLeftMarginEdit->setValidator(new QDoubleValidator(mLeftMarginEdit));
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
    mMarginsGroupBox->setLayout(marginsLayout);

    // Buttons
    connect(mOkButton, &QPushButton::clicked, this, &PageSetupDialog::accept);
    connect(mOkButton, &QPushButton::clicked, this, &PageSetupDialog::updatePageSetup);
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

    setWindowTitle(QStringLiteral("Page Setup"));
    resize(sizeHint());
}

PageSetupDialog::~PageSetupDialog() {}

void PageSetupDialog::updatePageSetup()
{
    mPrinterName = mPrinterNameComboBox->currentText();
    mPrintingMethod = mPrintingMethodComboBox->currentData().value<PrintingMethod>();
    mOrientation = mOrientationComboBox->currentData().value<QPageLayout::Orientation>();
    mMargins = QMarginsF(mLeftMarginEdit->text().toDouble(),
                         mTopMarginEdit->text().toDouble(),
                         mRightMarginEdit->text().toDouble(),
                         mBottomMarginEdit->text().toDouble());
}
