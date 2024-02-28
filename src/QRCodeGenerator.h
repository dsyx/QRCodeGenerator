#ifndef QRCODEGENERATOR_H
#define QRCODEGENERATOR_H

#include <QComboBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QMap>
#include <QPushButton>
#include <QSpacerItem>
#include <QTextEdit>
#include <QVBoxLayout>
#include "QZXing.h"

class QRCodeGenerator : public QMainWindow
{
    Q_OBJECT

public:
    QRCodeGenerator(QWidget *parent = nullptr);
    ~QRCodeGenerator();

private slots:
    void generate();

private:
    QLabel *mPreviewLabel = new QLabel();

    QLabel *mErrorCorrectionLabel;
    QComboBox *mErrorCorrectionComboBox;
    QMap<QString, QZXing::EncodeErrorCorrectionLevel> mErrorCorrectionMap;

    QLabel *mSizeLabel;
    QLabel *mSizeXLabel;
    QLineEdit *mSizeWidthEdit;
    QLineEdit *mSizeHeightEdit;

    QTextEdit *mDataEdit;

    QSpacerItem *mSpacer;
    QPushButton *mGenerateButton;

    QHBoxLayout *mSizeEditLayout;
    QFormLayout *mSettingLayout;
    QHBoxLayout *mPreviewAndSettingLayout;
    QHBoxLayout *mButtonLayout;
    QVBoxLayout *mMainLayout;
};
#endif // QRCODEGENERATOR_H
