#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QDebug>
#include <QWidget>
#include "InputTextDialog.h"
#include "GrammarParser.h"
#include "FirstFollowDialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_BtnReadFromFile_clicked();

    void on_BtnInputNow_clicked();

    void inputTextDialog_InputEnd();
    void inputTextDialog_InputCancel();

    void parseError(const QMap<int, QString> &messageMap);

    void closeFirstFollowDialog();
private:
    Ui::MainWindow *ui;

    QList<QStringList> * getConditionActions(const QString &text);

    InputTextDialog *mInputTextDialog;
    FirstFollowDialog *mFirstFollowDialog;

    void newInputTextDialog();
    void newFirstFollowDialog(const QString &text);
    void showWidget(QWidget *widget);
};

#endif // MAINWINDOW_H
