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
    void saveAs();
    void printPreview();
    void pageSetup();
    void print();
    void exit();

private:
    QMenu *mFileMenu;

    QAction *mSaveAsAction;
    QAction *mPrintPreviewAction;
    QAction *mPageSetupAction;
    QAction *mPrintAction;
    QAction *mExitAction;
};

#endif // MENUBAR_H
