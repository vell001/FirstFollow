#ifndef FIRSTFOLLOWDIALOG_H
#define FIRSTFOLLOWDIALOG_H

#include <QDialog>
#include <QHash>
#include <QMessageBox>
#include "Node.h"
#include "GrammarParser.h"

namespace Ui {
class FirstFollowDialog;
}

class FirstFollowDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FirstFollowDialog(QWidget *parent = 0);
    ~FirstFollowDialog();
    void setSentenceText(const QString &text);

public slots:
    void parseSuccess(const QString &richText);
signals:
    void parseError(const QMap<int, QString> &messageMap);
private:
    Ui::FirstFollowDialog *ui;

    QString mSentenceText;
    GrammarParser mGrammarParser;
    Nodes mTerminalNodes;
    Nodes mNonTerminalNodes;
    Node *mHeadNode;

    void separateNodes(); // separate terminal and nonterminal nodes
    void processFirst();
    void processFollow();
    void updateView();
};

#endif // FIRSTFOLLOWDIALOG_H
