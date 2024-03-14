#ifndef PRINTERSETUPDIALOG_H
#define PRINTERSETUPDIALOG_H

#include <QComboBox>
#include <QDialog>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QLabel>
#include <QPageLayout>
#include <QPrinter>
#include <QPushButton>
#include <QSharedPointer>

class PrinterSetupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PrinterSetupDialog(QSharedPointer<QPrinter> p, QWidget *parent = nullptr);
    ~PrinterSetupDialog();

    QSharedPointer<QPrinter> printer() const;
    bool scaling() const;

private slots:
    void updateSetup();
    void restoreSetup();

private:
    QSharedPointer<QPrinter> mPrinter;

private:
    QLabel *mPrinterNameLabel;
    QComboBox *mPrinterNameComboBox;
    QLabel *mPageSizeLabel;
    QComboBox *mPageSizeComboBox;
    QLabel *mOrientationLabel;
    QComboBox *mOrientationComboBox;
    QLabel *mScalingLabel;
    QComboBox *mScalingComboBox;

    QGroupBox *mMarginsGroupBox;
    QLabel *mTopMarginLabel;
    QDoubleSpinBox *mTopMarginSpinBox;
    QLabel *mBottomMarginLabel;
    QDoubleSpinBox *mBottomMarginSpinBox;
    QLabel *mLeftMarginLabel;
    QDoubleSpinBox *mLeftMarginSpinBox;
    QLabel *mRightMarginLabel;
    QDoubleSpinBox *mRightMarginSpinBox;

    QPushButton *mOkButton;
    QPushButton *mCancelButton;

private:
    static constexpr double MARGINS_MIN_VALUE = 0.0;
    static constexpr double MARGINS_MAX_VALUE = 9999.9;
    static constexpr int MARGINS_DECIMALS = 1;
    static constexpr double MARGINS_SINGLE_STEP = 0.1;
    static constexpr char MARGINS_SUFFIX[] = "  mm";
};

#endif // PRINTERSETUPDIALOG_H
