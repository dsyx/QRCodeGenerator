#include "OperationWidget.h"
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QVBoxLayout>

OperationWidget::OperationWidget(QWidget *parent)
    : QWidget{parent}
    , mDataEdit{new QPlainTextEdit(QStringLiteral("Hello World"))}
    , mPrintButton{new QPushButton(QStringLiteral("Print"))}
    , mGenerateButton{new QPushButton(QStringLiteral("Generate"))}
{
    connect(mPrintButton, &QPushButton::clicked, this, &OperationWidget::print);
    connect(mGenerateButton, &QPushButton::clicked, this, &OperationWidget::generate);

    QVBoxLayout *buttonLayout = new QVBoxLayout();
    buttonLayout->addSpacerItem(
        new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));
    buttonLayout->addWidget(mPrintButton);
    buttonLayout->addWidget(mGenerateButton);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(mDataEdit, 8);
    layout->addLayout(buttonLayout, 2);

    setLayout(layout);
}

OperationWidget::~OperationWidget() {}

QString OperationWidget::data() const
{
    return mDataEdit->toPlainText();
}
