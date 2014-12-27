#include "InputTextDialog.h"
#include "ui_InputTextDialog.h"

InputTextDialog::InputTextDialog(QWidget *parent, const QString &textFileName) :
    QDialog(parent),
    ui(new Ui::InputTextDialog),
    mIsSettingHtml(false)
{
    ui->setupUi(this);
    loadTemplate();
    TitleOfSaved = this->windowTitle();
    TitleOfUnsaved = this->windowTitle() + " (*unsaved*)";

    setTextFileName(textFileName);
    mInputTextTemp = ui->textEdit->toPlainText();

    ui->ErrorMessageTextBrowser->setVisible(false);

    connect(&mGrammarParser, SIGNAL(parseSuccess(QString)), this, SLOT(parseSuccess(QString)));
    connect(&mGrammarParser, SIGNAL(parseError(QMap<int,QString>)), this, SLOT(parseError(QMap<int,QString>)));
}

InputTextDialog::~InputTextDialog()
{
    delete ui;
}

void InputTextDialog::on_BtnOk_clicked()
{
    emit InputEnd(this->ui->textEdit->toPlainText());

    if(!mTextFileName.isEmpty() && this->windowTitle() == TitleOfUnsaved){
        on_BtnSave_clicked(); // save to file
    }
}

void InputTextDialog::on_BtnCancel_clicked()
{
    emit InputCancel(this->ui->textEdit->toPlainText());
}

void InputTextDialog::on_BtnSave_clicked()
{
    if(this->windowTitle() == TitleOfUnsaved){
        if(mTextFileName.isEmpty()){
            mTextFileName = QFileDialog::getSaveFileName(this, "save file", "", "Text (*.txt *.v)");
        }
        if(!TextFIleUtil::writeFile(this, this->ui->textEdit->toPlainText(), mTextFileName)){
            mTextFileName = QString();
            return;
        }
        this->setWindowTitle(InputTextDialog::TitleOfSaved);
    }
}

QString InputTextDialog::getText(){
    return this->ui->textEdit->toPlainText();
}

void InputTextDialog::setTextFileName(const QString &textFileName){
    if(!textFileName.isEmpty()){
        QString text = TextFIleUtil::readFile(this, textFileName);
        if(text != NULL && !text.isEmpty()){
            mGrammarParser.updateSentencesText(text);
            mTextFileName = textFileName; // change temp filename
        }
    }
}

void InputTextDialog::on_textEdit_textChanged()
{
    if(this->windowTitle() == TitleOfSaved)
        this->setWindowTitle(TitleOfUnsaved);

    if(!mIsSettingHtml) {
        QString tempText = ui->textEdit->toPlainText();

        if(tempText.indexOf(mInputTextTemp) != 0 ||
                tempText.lastIndexOf("\n") == tempText.size()-1) {

            mInputTextTemp = tempText.left(tempText.lastIndexOf("\n") + 1);

            mGrammarParser.updateSentencesText(mInputTextTemp);
        }
    }
}

void InputTextDialog::parseSuccess(const QString &richText){
    qDebug() << richText;
    mIsSettingHtml = true;
    ui->textEdit->setHtml(QString(mInputEditHtmlTemplate).arg(richText));
    ui->textEdit->moveCursor(QTextCursor::End);
    mIsSettingHtml = false;
}

void InputTextDialog::parseError(const QMap<int, QString> &messageMap){
    if(messageMap.isEmpty()) {
        ui->ErrorMessageTextBrowser->setVisible(false);
        ui->BtnOk->setEnabled(true);
        return;
    }
    QString richText = "";
    foreach (int lineNumber, messageMap.keys()) {
        QString errorMessage = messageMap.value(lineNumber);
        richText += QString("<p><span class=\"LineNumber\">line %1</span> : <span class=\"Message\">%2</span></p>")
                .arg(lineNumber)
                .arg(errorMessage);
    }
    ui->ErrorMessageTextBrowser->setVisible(true);
    ui->BtnOk->setEnabled(false);
    ui->ErrorMessageTextBrowser->setHtml(mErrorMessageHtmlTemplate.arg(richText));
    ui->ErrorMessageTextBrowser->moveCursor(QTextCursor::End);
}

void InputTextDialog::loadTemplate(){
    QFile inputEditHtmlTemplateFile("InputEditHtmlTemplate.html");
    if(inputEditHtmlTemplateFile.exists()) {
        mInputEditHtmlTemplate = TextFIleUtil::readFile(this, "InputEditHtmlTemplate.html");
    } else {
        mInputEditHtmlTemplate = TextFIleUtil::readFile(this, ":/Template/InputEditHtmlTemplate.html");
    }

    QFile errorMessageHtmlTemplateFile("ErrorMessageHtmlTemplate.html");
    if(errorMessageHtmlTemplateFile.exists()) {
        mErrorMessageHtmlTemplate = TextFIleUtil::readFile(this, "ErrorMessageHtmlTemplate.html");
    } else {
        mErrorMessageHtmlTemplate = TextFIleUtil::readFile(this, ":/Template/ErrorMessageHtmlTemplate.html");
    }
}
