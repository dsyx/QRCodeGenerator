#ifndef QRCODEGENERATOR_H
#define QRCODEGENERATOR_H

#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QMap>
#include <QPushButton>
#include <QTextEdit>
#include <QWidget>
#include "QZXing.h"

class QRCodeGenerator : public QMainWindow
{
    Q_OBJECT

public:
    QRCodeGenerator(QWidget *parent = nullptr);
    ~QRCodeGenerator();

private:
    void initPreviewPart();
    void initSettingPart();
    void initOperationPart();

private slots:
    void generate();

private:
    QLabel *mPreviewLabel;
    QWidget *mPreviewWidget;

    QLabel *mErrorCorrectionLabel;
    QComboBox *mErrorCorrectionComboBox;
    QMap<QString, QZXing::EncodeErrorCorrectionLevel> mErrorCorrectionMap;
    QLabel *mSizeLabel;
    QLineEdit *mSizeWidthEdit;
    QLineEdit *mSizeHeightEdit;
    QWidget *mSettingWidget;

    QTextEdit *mDataEdit;
    QPushButton *mGenerateButton;
    QWidget *mOperationWidget;
};
#endif // QRCODEGENERATOR_H
