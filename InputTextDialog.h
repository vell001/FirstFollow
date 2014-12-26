#ifndef INPUTTEXTDIALOG_H
#define INPUTTEXTDIALOG_H

#include <QDialog>
#include <QString>
#include <QFileDialog>
#include <QDateTime>
#include <QMap>
#include "TextFileUtil.hpp"
#include "GrammarParser.h"

namespace Ui {
class InputTextDialog;
}

class InputTextDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InputTextDialog(QWidget *parent = 0, const QString &textFileName = QString());
    ~InputTextDialog();
    QString getText();
    void setTextFileName(const QString &textFileName);

    QString TitleOfSaved;
    QString TitleOfUnsaved;

signals:
    void InputEnd(const QString &text);
    void InputCancel(const QString &text);

private slots:
    void on_BtnOk_clicked();

    void on_BtnCancel_clicked();

    void on_BtnSave_clicked();

    void on_textEdit_textChanged();

    void parseSuccess(const QString &richText);
    void parseError(const QMap<int, QString> &messageMap);
private:
    Ui::InputTextDialog *ui;
    QString mTextFileName;
    GrammarParser mGrammarParser;
    QString mInputTextTemp;
    QString mInputEditHtmlTemplate;
    QString mErrorMessageHtmlTemplate;

    void loadTemplate();
};

#endif // INPUTTEXTDIALOG_H
