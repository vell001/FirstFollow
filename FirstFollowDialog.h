#ifndef FIRSTFOLLOWDIALOG_H
#define FIRSTFOLLOWDIALOG_H

#include <QDialog>
#include <QHash>
#include <QMessageBox>
#include <QTableView>
#include <QStandardItemModel>
#include "Node.h"
#include "GrammarParser.h"
#include "TextFileUtil.hpp"

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
    void parseSuccess();
signals:
    void parseError(const QMap<int, QString> &messageMap);
private:
    Ui::FirstFollowDialog *ui;

    QString mSentenceText;
    GrammarParser mGrammarParser;
    Nodes mTerminalNodes;
    Nodes mNonTerminalNodes;
    QHash<Node *, NodeSet> mFirstSetHash;
    QHash<Node *, NodeSet> mFollowSetHash;
    QString mSetHashHtmlTemplate;

    QAbstractItemModel *mFirstModel;
    QAbstractItemModel *mFollowModel;

    void separateNodes(); // separate terminal and nonterminal nodes
    void processFirstSet();
    void processFollowSet();
    void updateView();
    NodeSet getFirstSet(Node *node);
    bool getFollowSet(Node *node);
    Node *findNode(const NodeSet& nodeSet, QString nodeName);
    Node *findNonNode(const NodeSet& nodeSet);
    QString setHashToRichText(const QHash<Node *, NodeSet> &setHash);
    void loadTemplate();
};

#endif // FIRSTFOLLOWDIALOG_H
