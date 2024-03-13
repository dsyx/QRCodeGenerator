#include "PrinterSetupDialog.h"
#include <QDoubleValidator>
#include <QFormLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QList>
#include <QMessageBox>
#include <QPrinterInfo>
#include <QSpacerItem>
#include <QVBoxLayout>

PrinterSetupDialog::PrinterSetupDialog(QWidget *parent)
    : QDialog{parent}
    , mPrinter{new QPrinter(QPrinter::HighResolution)}
    , mScaling{false}
    , mPrinterNameLabel{new QLabel(QStringLiteral("Printer Name:"))}
    , mPrinterNameComboBox{new QComboBox()}
    , mPageSizeLabel{new QLabel(QStringLiteral("Page Size:"))}
    , mPageSizeComboBox{new QComboBox()}
    , mOrientationLabel{new QLabel(QStringLiteral("Orientation:"))}
    , mOrientationComboBox{new QComboBox()}
    , mScalingLabel{new QLabel(QStringLiteral("Scaling:"))}
    , mScalingComboBox{new QComboBox()}
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

    // PageSize
    mPageSizeComboBox->addItem(QStringLiteral("A3 (297 x 420 mm)"), QPageSize::A3);
    mPageSizeComboBox->addItem(QStringLiteral("A4 (210 x 297 mm)"), QPageSize::A4);
    mPageSizeComboBox->addItem(QStringLiteral("A5 (148 x 210 mm)"), QPageSize::A5);
    mPageSizeComboBox->addItem(QStringLiteral("A6 (105 x 148 mm)"), QPageSize::A6);
    mPageSizeComboBox->addItem(QStringLiteral("B4 (250 x 353 mm)"), QPageSize::B4);
    mPageSizeComboBox->addItem(QStringLiteral("B5 (176 x 250 mm)"), QPageSize::B5);
    mPageSizeComboBox->addItem(QStringLiteral("Executive (190.5 x 254 mm)"), QPageSize::Executive);
    mPageSizeComboBox->addItem(QStringLiteral("Folio (210 x 330 mm)"), QPageSize::Folio);
    mPageSizeComboBox->addItem(QStringLiteral("Ledger (431.8 x 279.4 mm)"), QPageSize::Ledger);
    mPageSizeComboBox->addItem(QStringLiteral("Legal (215.9 x 355.6 mm)"), QPageSize::Legal);
    mPageSizeComboBox->addItem(QStringLiteral("Letter (215.9 x 279.4 mm)"), QPageSize::Letter);
    mPageSizeComboBox->insertItem(0, QString(""));
    mPageSizeComboBox->setCurrentIndex(0);

    // Orientation
    mOrientationComboBox->insertItem(0, QStringLiteral("Portrait"), QPageLayout::Portrait);
    mOrientationComboBox->insertItem(1, QStringLiteral("Landscape"), QPageLayout::Landscape);

    // Scaling
    mScalingComboBox->insertItem(0, QStringLiteral("No"), false);
    mScalingComboBox->insertItem(1, QStringLiteral("Yes"), true);

    QFormLayout *formLayout = new QFormLayout();
    formLayout->addRow(mPrinterNameLabel, mPrinterNameComboBox);
    formLayout->addRow(mPageSizeLabel, mPageSizeComboBox);
    formLayout->addRow(mOrientationLabel, mOrientationComboBox);
    formLayout->addRow(mScalingLabel, mScalingComboBox);

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
    connect(mOkButton, &QPushButton::clicked, this, &PrinterSetupDialog::updatePrinterSetup);
    connect(mCancelButton, &QPushButton::clicked, this, &PrinterSetupDialog::reject);
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

    updatePrinterSetup();

    setWindowTitle(QStringLiteral("Printer Setup"));
    resize(sizeHint());
}

PrinterSetupDialog::~PrinterSetupDialog() {
    delete mPrinter;
}

void PrinterSetupDialog::updatePrinterSetup()
{
    mPrinter->setPrinterName(mPrinterNameComboBox->currentText());

    if (!mPageSizeComboBox->currentText().isEmpty()) {
        if (!mPrinter->setPageSize(mPageSizeComboBox->currentData().value<QPageSize>())) {
            QMessageBox::critical(this,
                                  QStringLiteral("Error"),
                                  QStringLiteral("Failed to set page size."));
            return;
        }
    }

    if (!mPrinter->setPageOrientation(
            mOrientationComboBox->currentData().value<QPageLayout::Orientation>())) {
        QMessageBox::critical(this,
                              QStringLiteral("Error"),
                              QStringLiteral("Failed to set page orientation."));
        return;
    }

    mScaling = mScalingComboBox->currentData().value<bool>();

    if (!mPrinter->setPageMargins(QMarginsF(mLeftMarginEdit->text().toDouble(),
                                           mTopMarginEdit->text().toDouble(),
                                           mRightMarginEdit->text().toDouble(),
                                           mBottomMarginEdit->text().toDouble()),
                                 QPageLayout::Millimeter)) {
        QMessageBox::critical(this,
                              QStringLiteral("Error"),
                              QStringLiteral("Failed to set page orientation."));
        return;
    }

    accept();
}
