#ifndef PAGESETUPDIALOG_H
#define PAGESETUPDIALOG_H

#include <QComboBox>
#include <QDialog>
#include <QGroupBox>
#include <QHash>
#include <QLabel>
#include <QLineEdit>
#include <QMarginsF>
#include <QPageLayout>
#include <QPushButton>

class PageSetupDialog : public QDialog
{
    Q_OBJECT

public:
    enum PrintingMethod {
        Normal = 0,
        Scale = 1,
    };

public:
    PageSetupDialog(QWidget *parent = nullptr);
    ~PageSetupDialog();

    QString printerName() const { return mPrinterName; }
    PrintingMethod printingMethod() const { return mPrintingMethod; }
    QPageLayout::Orientation orientation() const { return mOrientation; }
    QMarginsF margins() const { return mMargins; }

private:
    void updatePrinterName();
    void updatePrintingMethod();
    void updateOrientation();
    void updateMargins();

private slots:
    void updatePageSetup();

private:
    QString mPrinterName;
    PrintingMethod mPrintingMethod;
    QPageLayout::Orientation mOrientation;
    QMarginsF mMargins;

    QHash<QString, PrintingMethod> mPrintingMethodHash;
    QHash<QString, QPageLayout::Orientation> mOrientationHash;

private:
    QLabel *mPrinterNameLabel;
    QComboBox *mPrinterNameComboBox;

    QLabel *mPrintingMethodLabel;
    QComboBox *mPrintingMethodComboBox;

    QLabel *mOrientationLabel;
    QComboBox *mOrientationComboBox;

    QGroupBox *mMarginsGroupBox;
    QLabel *mTopMarginLabel;
    QLineEdit *mTopMarginEdit;
    QLabel *mBottomMarginLabel;
    QLineEdit *mBottomMarginEdit;
    QLabel *mLeftMarginLabel;
    QLineEdit *mLeftMarginEdit;
    QLabel *mRightMarginLabel;
    QLineEdit *mRightMarginEdit;

    QPushButton *mOkButton;
    QPushButton *mCancelButton;

private:
    static constexpr qreal DEFAULT_MARGIN_LENGTH = 4.23;
};

#endif // PAGESETUPDIALOG_H
