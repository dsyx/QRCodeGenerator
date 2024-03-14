#include "PrinterSetupDialog.h"
#include <QFormLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QList>
#include <QMessageBox>
#include <QPrinterInfo>
#include <QSpacerItem>
#include <QVBoxLayout>
#include <QMarginsF>

PrinterSetupDialog::PrinterSetupDialog(QSharedPointer<QPrinter> p, QWidget *parent)
    : QDialog{parent}
    , mPrinter{p}
    , mPrinterNameLabel{new QLabel{QStringLiteral("Printer Name:")}}
    , mPrinterNameComboBox{new QComboBox{}}
    , mPageSizeLabel{new QLabel{QStringLiteral("Page Size:")}}
    , mPageSizeComboBox{new QComboBox{}}
    , mOrientationLabel{new QLabel{QStringLiteral("Orientation:")}}
    , mOrientationComboBox{new QComboBox{}}
    , mScalingLabel{new QLabel{QStringLiteral("Scaling:")}}
    , mScalingComboBox{new QComboBox{}}
    , mMarginsGroupBox{new QGroupBox{QStringLiteral("Margins")}}
    , mTopMarginLabel{new QLabel{QStringLiteral("Top:")}}
    , mTopMarginSpinBox{new QDoubleSpinBox{}}
    , mBottomMarginLabel{new QLabel{QStringLiteral("Bottom:")}}
    , mBottomMarginSpinBox{new QDoubleSpinBox{}}
    , mLeftMarginLabel{new QLabel{QStringLiteral("Left:")}}
    , mLeftMarginSpinBox{new QDoubleSpinBox{}}
    , mRightMarginLabel{new QLabel{QStringLiteral("Right:")}}
    , mRightMarginSpinBox{new QDoubleSpinBox{}}
    , mOkButton{new QPushButton{QStringLiteral("OK")}}
    , mCancelButton{new QPushButton{QStringLiteral("Cancel")}}
{
    // PrinterName
    {
        const QList<QPrinterInfo> infos{QPrinterInfo::availablePrinters()};
        for (const auto &info : infos) {
            mPrinterNameComboBox->addItem(info.printerName());
        }
        const QString name{mPrinter->printerName()};
        name.isEmpty() ? mPrinterNameComboBox->setCurrentIndex(-1)
                       : mPrinterNameComboBox->setCurrentText(name);
    }

    const QPageLayout pageLayout{mPrinter->pageLayout()};

    // PageSize
    {
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
        mPageSizeComboBox->setCurrentIndex(mPageSizeComboBox->findData(pageLayout.pageSize().id()));
    }

    // Orientation
    {
        mOrientationComboBox->insertItem(0, QStringLiteral("Portrait"), QPageLayout::Portrait);
        mOrientationComboBox->insertItem(1, QStringLiteral("Landscape"), QPageLayout::Landscape);
        mOrientationComboBox->setCurrentIndex(mOrientationComboBox->findData(pageLayout.orientation()));
    }

    // Scaling
    {
        mScalingComboBox->insertItem(0, QStringLiteral("No"), false);
        mScalingComboBox->insertItem(1, QStringLiteral("Yes"), true);
        mScalingComboBox->setCurrentIndex(0);
    }

    // Margins
    {
        const QMarginsF margins = pageLayout.margins(QPageLayout::Millimeter);

        mTopMarginSpinBox->setRange(MARGINS_MIN_VALUE, MARGINS_MAX_VALUE);
        mTopMarginSpinBox->setDecimals(MARGINS_DECIMALS);
        mTopMarginSpinBox->setSingleStep(MARGINS_SINGLE_STEP);
        mTopMarginSpinBox->setSuffix(QString{MARGINS_SUFFIX});
        mTopMarginSpinBox->setAlignment(Qt::AlignRight);
        mTopMarginSpinBox->setValue(margins.top());

        mBottomMarginSpinBox->setRange(MARGINS_MIN_VALUE, MARGINS_MAX_VALUE);
        mBottomMarginSpinBox->setDecimals(MARGINS_DECIMALS);
        mBottomMarginSpinBox->setSingleStep(MARGINS_SINGLE_STEP);
        mBottomMarginSpinBox->setSuffix(QString{MARGINS_SUFFIX});
        mBottomMarginSpinBox->setAlignment(Qt::AlignRight);
        mBottomMarginSpinBox->setValue(margins.top());

        mLeftMarginSpinBox->setRange(MARGINS_MIN_VALUE, MARGINS_MAX_VALUE);
        mLeftMarginSpinBox->setDecimals(MARGINS_DECIMALS);
        mLeftMarginSpinBox->setSingleStep(MARGINS_SINGLE_STEP);
        mLeftMarginSpinBox->setSuffix(QString{MARGINS_SUFFIX});
        mLeftMarginSpinBox->setAlignment(Qt::AlignRight);
        mLeftMarginSpinBox->setValue(margins.top());

        mRightMarginSpinBox->setRange(MARGINS_MIN_VALUE, MARGINS_MAX_VALUE);
        mRightMarginSpinBox->setDecimals(MARGINS_DECIMALS);
        mRightMarginSpinBox->setSingleStep(MARGINS_SINGLE_STEP);
        mRightMarginSpinBox->setSuffix(QString{MARGINS_SUFFIX});
        mRightMarginSpinBox->setAlignment(Qt::AlignRight);
        mRightMarginSpinBox->setValue(margins.top());
    }

    // Buttons
    {
        connect(mOkButton, &QPushButton::clicked, this, &PrinterSetupDialog::updateSetup);
        connect(mCancelButton, &QPushButton::clicked, this, &PrinterSetupDialog::restoreSetup);
    }

    // Layout
    {
        QFormLayout *formLayout{new QFormLayout{}};
        formLayout->addRow(mPrinterNameLabel, mPrinterNameComboBox);
        formLayout->addRow(mPageSizeLabel, mPageSizeComboBox);
        formLayout->addRow(mOrientationLabel, mOrientationComboBox);
        formLayout->addRow(mScalingLabel, mScalingComboBox);

        QGridLayout *marginsLayout{new QGridLayout{}};
        marginsLayout->addWidget(mTopMarginLabel, 0, 0);
        marginsLayout->addWidget(mTopMarginSpinBox, 0, 1);
        marginsLayout->addItem(new QSpacerItem{20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum}, 0, 2);
        marginsLayout->addWidget(mBottomMarginLabel, 0, 3);
        marginsLayout->addWidget(mBottomMarginSpinBox, 0, 4);
        marginsLayout->addWidget(mLeftMarginLabel, 1, 0);
        marginsLayout->addWidget(mLeftMarginSpinBox, 1, 1);
        marginsLayout->addItem(new QSpacerItem{20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum}, 1, 2);
        marginsLayout->addWidget(mRightMarginLabel, 1, 3);
        marginsLayout->addWidget(mRightMarginSpinBox, 1, 4);
        mMarginsGroupBox->setLayout(marginsLayout);

        QHBoxLayout *buttonLayout{new QHBoxLayout{}};
        buttonLayout->addSpacerItem(new QSpacerItem{40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum});
        buttonLayout->addWidget(mOkButton);
        buttonLayout->addWidget(mCancelButton);

        QVBoxLayout *layout{new QVBoxLayout{}};
        layout->addLayout(formLayout);
        layout->addWidget(mMarginsGroupBox);
        layout->addLayout(buttonLayout);
        setLayout(layout);
    }

    setWindowTitle(QStringLiteral("Printer Setup"));
    resize(sizeHint());
}

PrinterSetupDialog::~PrinterSetupDialog() {}

QSharedPointer<QPrinter> PrinterSetupDialog::printer() const
{
    return mPrinter;
}

bool PrinterSetupDialog::scaling() const
{
    return mScalingComboBox->currentData().value<bool>();
}

#define VERIFY(job, msg) \
    do { \
        if (!(job)) { \
            QMessageBox::critical(this, QStringLiteral("Error"), msg); \
            return; \
        } \
    } while (0)

void PrinterSetupDialog::updateSetup()
{
    const QString name{mPrinterNameComboBox->currentText()};
    const QPageSize::PageSizeId pageSizeId{mPageSizeComboBox->currentData().value<QPageSize::PageSizeId>()};
    const QPageLayout::Orientation orientation{mOrientationComboBox->currentData().value<QPageLayout::Orientation>()};
    const QMarginsF margins{mLeftMarginSpinBox->value(),
                            mTopMarginSpinBox->value(),
                            mRightMarginSpinBox->value(),
                            mBottomMarginSpinBox->value()};

    mPrinter->setPrinterName(name);
    if (mPageSizeComboBox->currentIndex() != -1) {
        VERIFY(mPrinter->setPageSize(QPageSize{pageSizeId}), QStringLiteral("Failed to set page size."));
    }
    VERIFY(mPrinter->setPageOrientation(orientation), QStringLiteral("Failed to set page orientation."));
    VERIFY(mPrinter->setPageMargins(margins, QPageLayout::Millimeter), QStringLiteral("Failed to set page orientation."));

    accept();
}

void PrinterSetupDialog::restoreSetup()
{
    const QString name{mPrinter->printerName()};
    const QPageLayout pageLayout{mPrinter->pageLayout()};
    const QMarginsF margins = pageLayout.margins(QPageLayout::Millimeter);

    name.isEmpty() ? mPrinterNameComboBox->setCurrentIndex(-1)
                   : mPrinterNameComboBox->setCurrentText(name);
    mPageSizeComboBox->setCurrentIndex(mPageSizeComboBox->findData(pageLayout.pageSize().id()));
    mOrientationComboBox->setCurrentIndex(mOrientationComboBox->findData(pageLayout.orientation()));
    mTopMarginSpinBox->setValue(margins.top());
    mBottomMarginSpinBox->setValue(margins.top());
    mLeftMarginSpinBox->setValue(margins.top());
    mRightMarginSpinBox->setValue(margins.top());

    reject();
}
// TODO
