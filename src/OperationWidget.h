#ifndef OPERATIONWIDGET_H
#define OPERATIONWIDGET_H

#include <QPlainTextEdit>
#include <QPushButton>
#include <QWidget>

class OperationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit OperationWidget(QWidget *parent = nullptr);
    ~OperationWidget();

    QString data() const;

signals:
    void print();
    void generate();

private:
    QPlainTextEdit *mDataEdit;
    QPushButton *mPrintButton;
    QPushButton *mGenerateButton;
};

#endif // OPERATIONWIDGET_H
