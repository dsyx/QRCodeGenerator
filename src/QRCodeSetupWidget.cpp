#include "QRCodeSetupWidget.h"
#include <QFormLayout>
#include <QHBoxLayout>
#include <QIntValidator>

QRCodeSetupWidget::QRCodeSetupWidget(QWidget *parent)
    : QWidget{parent}
    , mErrorCorrectionLabel{new QLabel(QStringLiteral("Error Correction:"))}
    , mErrorCorrectionComboBox{new QComboBox()}
    , mSizeLabel{new QLabel(QStringLiteral("Size (Width x Height):"))}
    , mSizeWidthEdit{new QLineEdit(QString::number(DEFAULT_LENGTH_OR_WIDTH))}
    , mSizeHeightEdit{new QLineEdit(QString::number(DEFAULT_LENGTH_OR_WIDTH))}
{
    mErrorCorrectionComboBox->insertItem(0,
                                         QStringLiteral("Low"),
                                         QZXing::EncodeErrorCorrectionLevel_L);
    mErrorCorrectionComboBox->insertItem(1,
                                         QStringLiteral("Medium"),
                                         QZXing::EncodeErrorCorrectionLevel_M);
    mErrorCorrectionComboBox->insertItem(2,
                                         QStringLiteral("Quartile"),
                                         QZXing::EncodeErrorCorrectionLevel_Q);
    mErrorCorrectionComboBox->insertItem(3,
                                         QStringLiteral("High"),
                                         QZXing::EncodeErrorCorrectionLevel_H);
    mErrorCorrectionComboBox->setCurrentIndex(2);

    mSizeWidthEdit->setValidator(new QIntValidator(mSizeWidthEdit));
    mSizeHeightEdit->setValidator(new QIntValidator(mSizeHeightEdit));

    QHBoxLayout *sizeLayout = new QHBoxLayout();
    sizeLayout->addWidget(mSizeWidthEdit);
    sizeLayout->addWidget(new QLabel(QStringLiteral("x")));
    sizeLayout->addWidget(mSizeHeightEdit);

    QFormLayout *layout = new QFormLayout();
    layout->addRow(mErrorCorrectionLabel, mErrorCorrectionComboBox);
    layout->addRow(mSizeLabel, sizeLayout);

    setLayout(layout);
}

QRCodeSetupWidget::~QRCodeSetupWidget() {}

QZXing::EncodeErrorCorrectionLevel QRCodeSetupWidget::errorCorrection() const
{
    return mErrorCorrectionComboBox->currentData().value<QZXing::EncodeErrorCorrectionLevel>();
}

QSize QRCodeSetupWidget::size() const
{
    return QSize(mSizeWidthEdit->text().toInt(), mSizeHeightEdit->text().toInt());
}
