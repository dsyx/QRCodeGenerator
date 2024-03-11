#include "MenuBar.h"

MenuBar::MenuBar(QWidget *parent)
    : QMenuBar{parent}
    , mFileMenu{new QMenu(QStringLiteral("File"))}
    , mSaveAsAction{new QAction(QStringLiteral("Save as"))}
    , mPrintPreviewAction{new QAction(QStringLiteral("Print Preview"))}
    , mPrinterSetupAction{new QAction(QStringLiteral("Printer Setup"))}
    , mPrintAction{new QAction(QStringLiteral("Print"))}
    , mExitAction{new QAction(QStringLiteral("Exit"))}
{
    mSaveAsAction->setShortcut(QKeySequence::SaveAs);
    connect(mSaveAsAction, &QAction::triggered, this, &MenuBar::saveAs);

    connect(mPrintPreviewAction, &QAction::triggered, this, &MenuBar::printPreview);

    connect(mPrinterSetupAction, &QAction::triggered, this, &MenuBar::printerSetup);

    mPrintAction->setShortcut(QKeySequence::Print);
    connect(mPrintAction, &QAction::triggered, this, &MenuBar::print);

    connect(mExitAction, &QAction::triggered, this, &MenuBar::exit);

    mFileMenu->addAction(mSaveAsAction);
    mFileMenu->addSeparator();
    mFileMenu->addAction(mPrintPreviewAction);
    mFileMenu->addAction(mPrinterSetupAction);
    mFileMenu->addAction(mPrintAction);
    mFileMenu->addSeparator();
    mFileMenu->addAction(mExitAction);

    addMenu(mFileMenu);
}

MenuBar::~MenuBar() {}
