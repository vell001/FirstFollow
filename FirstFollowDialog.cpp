#include "FirstFollowDialog.h"
#include "ui_FirstFollowDialog.h"

FirstFollowDialog::FirstFollowDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FirstFollowDialog),
    mFirstModel(new QStandardItemModel),
    mFollowModel(new QStandardItemModel)
{
    ui->setupUi(this);
    ui->FirstTableView->setModel(mFirstModel);
    ui->FollowTableView->setModel(mFollowModel);
    ui->FirstTableView->setSizeAdjustPolicy(QTableView::AdjustToContents);

    connect(&mGrammarParser, SIGNAL(parseSuccess(QString)), this, SLOT(parseSuccess()));
    connect(&mGrammarParser, SIGNAL(parseError(QMap<int,QString>)), this, SIGNAL(parseError(QMap<int,QString>)));
}

void FirstFollowDialog::setSentenceText(const QString &text){
    mSentenceText = text;
    mGrammarParser.updateSentencesText(mSentenceText);
}

FirstFollowDialog::~FirstFollowDialog()
{
    delete ui;
    delete mFirstModel;
    delete mFollowModel;
}

void FirstFollowDialog::parseSuccess(){
    this->show();

    separateNodes();
    processFirstSet();
    processFollowSet();
    updateView();
}

void FirstFollowDialog::separateNodes(){
    foreach (Node *node, mGrammarParser.getNodesHash()->values()) {
        if(node->isTerminal()){
            mTerminalNodes.append(node);
        } else {
            mNonTerminalNodes.append(node);
        }
    }
}

void FirstFollowDialog::processFirstSet(){
    foreach (Node *node, mNonTerminalNodes) {
        mFirstSetHash[node] = getFirstSet(node);
    }
}
void FirstFollowDialog::processFollowSet(){

}

NodeSet FirstFollowDialog::getFirstSet(Node *node){
    NodeSet firstNodeSet;
    if(node->isTerminal()) {
        firstNodeSet.insert(node);
    } else {
        foreach (Nodes * explodedNodes, node->getExplodedNodesList()) {
            foreach (Node *explodedNode, *explodedNodes) {
                firstNodeSet.unite(getFirstSet(explodedNode));
                if(explodedNode->isNonNode()){
                    continue;
                }
            }
        }
    }
    return firstNodeSet;
}



void FirstFollowDialog::updateView(){
    mFirstModel->insertColumns(0, mTerminalNodes.size());
    mFollowModel->insertColumns(0, mTerminalNodes.size());
    int i=0;
    foreach (Node * terminalNode, mTerminalNodes) {
        mFirstModel->setHeaderData(i, Qt::Horizontal, terminalNode->getName());
        mFollowModel->setHeaderData(i, Qt::Horizontal, terminalNode->getName());
        i++;
    }

    mFirstModel->insertRows(0, mNonTerminalNodes.size());
    mFollowModel->insertRows(0, mNonTerminalNodes.size());
    i=0;
    foreach (Node * nonTerminalNode, mNonTerminalNodes) {
        mFirstModel->setHeaderData(i, Qt::Vertical, nonTerminalNode->getName());
        mFollowModel->setHeaderData(i, Qt::Vertical, nonTerminalNode->getName());
        i++;
    }

    i=0;
    foreach (Node * nonTerminalNode, mNonTerminalNodes) {
        for(int j=0; j<mFirstModel->columnCount(); j++){
            NodeSet nodeSet = mFirstSetHash.value(nonTerminalNode);
            foreach (Node *node, nodeSet) {
                if(node->getName() == mFirstModel->headerData(j, Qt::Horizontal)){
                    mFirstModel->setData(mFirstModel->index(i, j), "*");
                    break;
                }
            }
        }
        i++;
    }
}

QString FirstFollowDialog::setHashToRichText(const QHash<Node *, NodeSet> &setHash){

}
