#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mInputTextDialog(NULL),
    mFirstFollowDialog(NULL)
{
    ui->setupUi(this);
    newInputTextDialog();
}

void MainWindow::newInputTextDialog(){
    if(mInputTextDialog != NULL) {
        delete mInputTextDialog;
    }
    mInputTextDialog = new InputTextDialog(this);
    connect(mInputTextDialog, SIGNAL(InputEnd(QString)), this, SLOT(inputTextDialog_InputEnd()));
    connect(mInputTextDialog, SIGNAL(InputCancel(QString)), this, SLOT(inputTextDialog_InputCancel()));
}

void MainWindow::newFirstFollowDialog(const QString &text){
    if(mFirstFollowDialog != NULL) {
        delete mFirstFollowDialog;
    }
    mFirstFollowDialog = new FirstFollowDialog(this);
    connect(mFirstFollowDialog, SIGNAL(parseError(QMap<int,QString>)), this, SLOT(parseError(QMap<int,QString>)));
    connect(mFirstFollowDialog, SIGNAL(accepted()), this, SLOT(show()));
    mFirstFollowDialog->setSentenceText(text);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete mInputTextDialog;
    delete mFirstFollowDialog;
}

void MainWindow::on_BtnReadFromFile_clicked()
{
    /**
     * 获得生产式
     */
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    "",
                                                    tr("Text files (*.txt *.v)"));

    if(fileName == NULL) return;

    newInputTextDialog();
    mInputTextDialog->setTextFileName(fileName);
    this->hide();
    mInputTextDialog->show();
}

void MainWindow::on_BtnInputNow_clicked()
{
    this->hide();
    mInputTextDialog->show();
}

/**
 * @brief MainWindow::getConditionAcJtions
 * @param text
 * @return if success return ConditionActions's QList else return NULL
 */
QList<QStringList> *MainWindow::getConditionActions(const QString &text){
    QList<QStringList> *ConditionActions = new QList<QStringList>;
    QString pattern("\\s*(\\S*)\\s*->\\s*(.+))\\s*");
    QRegExp rx(pattern);

    QStringList strList = text.split("\n", QString::SkipEmptyParts);
    foreach (QString str, strList) {
        if (str.indexOf(rx) >= 0){ // read a AC
            ConditionActions->append(rx.capturedTexts());
        }
    }

    if(ConditionActions->empty()) {
        QMessageBox::warning(this,
                             "no ConditionActions read!!!",
                             "no ConditionActions read!!!\n please check your file");
        return NULL;
    }

    qDebug() << "ConditionActions's number: " << ConditionActions->size();
    foreach (QStringList CA, *ConditionActions) {
        qDebug() << CA;
    }

    return ConditionActions;
}

void MainWindow::inputTextDialog_InputEnd(){
    mInputTextDialog->hide();
    QString text = mInputTextDialog->getText();
    if(text != NULL && !text.isEmpty()){
        newFirstFollowDialog(text);
    } else {
        QMessageBox::warning(this,
                             "empty input!!!",
                             "empty input!!!\n please input again");
    }
}

void MainWindow::inputTextDialog_InputCancel(){
    mInputTextDialog->hide();
    this->show();
}

void MainWindow::parseError(const QMap<int, QString> &messageMap){
    if(!messageMap.isEmpty()) {
        QString errorMesStr = "";
        foreach (int lineNumber, messageMap.keys()) {
            QString mesStr = messageMap.value(lineNumber);
            errorMesStr += QString("line %1: %2").arg(lineNumber).arg(mesStr);
            if(lineNumber != messageMap.keys().at(messageMap.size()-1)) {
                errorMesStr += "\n";
            }
        }

        QMessageBox::warning(this,
                             "text parse error",
                             QString("text parse error!!! error message:\n%1").arg(errorMesStr));
        this->show();
        mFirstFollowDialog->deleteLater();
        mFirstFollowDialog = NULL;
    }
}

