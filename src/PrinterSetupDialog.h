#ifndef PRINTERSETUPDIALOG_H
#define PRINTERSETUPDIALOG_H

#include <QComboBox>
#include <QDialog>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QMarginsF>
#include <QPageLayout>
#include <QPrinter>
#include <QPushButton>

class PrinterSetupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PrinterSetupDialog(QWidget *parent = nullptr);
    ~PrinterSetupDialog();

    QPrinter *printer() { return mPrinter; }
    bool isScaling() const { return mScaling; }

private slots:
    void updatePrinterSetup();

private:
    QPrinter *mPrinter;
    bool mScaling;

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

#endif // PRINTERSETUPDIALOG_H
