#ifndef MENUBAR_H
#define MENUBAR_H

#include <QAction>
#include <QMenu>
#include <QMenuBar>

class MenuBar : public QMenuBar
{
    Q_OBJECT

public:
    explicit MenuBar(QWidget *parent = nullptr);
    ~MenuBar();

signals:
    void saveAsRequested();
    void printPreviewRequested();
    void printerSetupRequested();
    void printRequested();
    void exitRequested();

private:
    QMenu *mFileMenu;

    QAction *mSaveAsAction;
    QAction *mPrintPreviewAction;
    QAction *mPrinterSetupAction;
    QAction *mPrintAction;
    QAction *mExitAction;
};

#endif // MENUBAR_H
