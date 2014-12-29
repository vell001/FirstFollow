#include "FirstFollowDialog.h"
#include "ui_FirstFollowDialog.h"

FirstFollowDialog::FirstFollowDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FirstFollowDialog),
    mFirstModel(new QStandardItemModel),
    mFollowModel(new QStandardItemModel)
{
    ui->setupUi(this);
    loadTemplate();
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
    foreach (Node *node, mGrammarParser.getNodesHash()->values()) {
        mFirstSetHash[node] = getFirstSet(node);
    }
}

NodeSet FirstFollowDialog::getFirstSet(Node *node){
    NodeSet firstNodeSet;
    if(node->isTerminal()) {
        firstNodeSet.insert(node);
    } else {
        foreach (Nodes * explodedNodes, node->getExplodedNodesList()) {
            foreach (Node *explodedNode, *explodedNodes) {
                NodeSet tempNodeSet = getFirstSet(explodedNode);

                Node* nonNode = findNonNode(tempNodeSet);

                if(nonNode != NULL){ // found nonNode
                    if(!explodedNodes->endsWith(explodedNode)){
                        tempNodeSet.remove(nonNode);
                    }
                    firstNodeSet.unite(tempNodeSet);
                } else {
                    firstNodeSet.unite(tempNodeSet);
                    break;
                }
            }
        }
    }
    return firstNodeSet;
}

void FirstFollowDialog::processFollowSet(){
    bool isSetHashChanged = true;
    while(isSetHashChanged) {
        getFollowSet(mGrammarParser.getHeadNode()); // 从起始符号开始获取
        foreach (Node *node, mNonTerminalNodes) {
            isSetHashChanged = getFollowSet(node);
        }
    }
}

bool FirstFollowDialog::getFollowSet(Node *node){
    bool isSetHashChanged = false;
    QHash<Node *, int> setHashSize;
    foreach (Node * tempNode, mFollowSetHash.keys()) {
        setHashSize[tempNode] = mFollowSetHash.value(tempNode).size();
    }

    if(!node->isTerminal()) {
        if(node == mGrammarParser.getHeadNode() && findNode(mFollowSetHash[node], "#") == NULL) {
            mFollowSetHash[node].insert(new Node("#"));
        }
        foreach (Nodes *explodedNodes, node->getExplodedNodesList()) {
            // node -> preNode explodedNode
            // A    -> B       b
            Node *preNode = NULL;
            foreach (Node *explodedNode, *explodedNodes) {
                if(!explodedNode->isTerminal() && explodedNode != node) getFollowSet(explodedNode); // 保证顺序遍历

                if(preNode != NULL && !preNode->isTerminal()) { // 前字符为非终结符， 将当前字符的First集加入前字符的Follow集
                    NodeSet firstSet = mFirstSetHash[explodedNode];
                    Node *nonNode = findNonNode(firstSet);
                    if(nonNode != NULL) { // 除去当前字符First集中的空字符
                        firstSet.remove(nonNode);
                    }
                    mFollowSetHash[preNode].unite(firstSet);

                    if(nonNode != NULL) { // 如果当前字符的Frist集中存在空字符， 将左部的Follow集加入前字符的Follow集
                        mFollowSetHash[preNode].unite(mFollowSetHash[node]);
                        mFollowSetHash[preNode].unite(mFollowSetHash[explodedNode]);
                    }
                }

                if(!explodedNode->isTerminal() && explodedNodes->endsWith(explodedNode)) {
                    // *反复传送：对形如U－>…P的产生式（其中P是非终结符），应把Follow(U)中的全部内容传送到Follow(P)中。
                    mFollowSetHash[explodedNode].unite(mFollowSetHash[node]);
                    // 最后字符的Follow集添加空字符
                    if(findNode(mFollowSetHash[explodedNode], "#") == NULL){
                        Node *nonNode = findNode(mFollowSetHash[node], "#");
                        if(nonNode == NULL)
                            mFollowSetHash[explodedNode].insert(new Node("#"));
                        else
                            mFollowSetHash[explodedNode].insert(nonNode);
                    }
                }
                preNode = explodedNode;
            }
        }
    }

    foreach (Node * tempNode, setHashSize.keys()) {
        int sizeValue = setHashSize.value(tempNode);
        if(mFollowSetHash.value(tempNode).size() != sizeValue){
            isSetHashChanged = true;
            break;
        }
    }
    return isSetHashChanged;
}

Node *FirstFollowDialog::findNode(const NodeSet& nodeSet, QString nodeName){
    Node* nonNode = NULL;
    foreach (Node *node, nodeSet) { // finding node
        if(node->getName() == nodeName){
            nonNode = node;
            break;
        }
    }
    return nonNode;
}

Node *FirstFollowDialog::findNonNode(const NodeSet& nodeSet){
    return findNode(nodeSet, "ε");
}

void FirstFollowDialog::updateView(){
    mFirstModel->insertColumns(0, mTerminalNodes.size());
    mFollowModel->insertColumns(0, mTerminalNodes.size());

    int i=0;
    foreach (Node * terminalNode, mTerminalNodes) {
        mFirstModel->setHeaderData(i, Qt::Horizontal, terminalNode->getName());
        if(terminalNode->getName() == "ε")
            mFollowModel->setHeaderData(i, Qt::Horizontal, "#");
        else
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
            NodeSet firstNodeSet = mFirstSetHash.value(nonTerminalNode);
            foreach (Node *node, firstNodeSet) {
                if(node->getName() == mFirstModel->headerData(j, Qt::Horizontal)){
                    mFirstModel->setData(mFirstModel->index(i, j), "*");
                    break;
                }
            }

            NodeSet followNodeSet = mFollowSetHash.value(nonTerminalNode);
            foreach (Node *node, followNodeSet) {
                if(node->getName() == mFollowModel->headerData(j, Qt::Horizontal)){
                    mFollowModel->setData(mFollowModel->index(i, j), "*");
                    break;
                }
            }
        }
        i++;
    }

//    mFollowModel->insertColumns(0, mFollowSetHash.values().size());
//    mFollowModel->insertRows(0, mFollowSetHash.keys().size());

//    i=0;
//    foreach (Node * terminalNode, mFollowSetHash.values()) {
//        mFirstModel->setHeaderData(i, Qt::Horizontal, terminalNode->getName());
//        i++;
//    }
//    i=0;
//    foreach (Node * nonTerminalNode, mFollowSetHash.keys()) {
//        mFirstModel->setHeaderData(i, Qt::Vertical, nonTerminalNode->getName());
//        i++;
//    }
//    i=0;
//    foreach (Node * nonTerminalNode, mFollowSetHash.keys()) {
//        for(int j=0; j<mFirstModel->columnCount(); j++){
//            NodeSet firstNodeSet = mFirstSetHash.value(nonTerminalNode);
//            foreach (Node *node, firstNodeSet) {
//                if(node->getName() == mFirstModel->headerData(j, Qt::Horizontal)){
//                    mFirstModel->setData(mFirstModel->index(i, j), "*");
//                    break;
//                }
//            }
//        }
//        i++;
//    }

    QString firstRichText = setHashToRichText(mFirstSetHash);
    ui->FirstTextBrowser->setHtml(firstRichText);

    QString followRichText = setHashToRichText(mFollowSetHash);
    ui->FollowTextBrowser->setHtml(followRichText);
}

QString FirstFollowDialog::setHashToRichText(const QHash<Node *, NodeSet> &setHash){
    QString richText = "";
    foreach (Node *nonTerminalNode, setHash.keys()) {
        if(nonTerminalNode->isTerminal()) continue;
        richText += QString("<p><span class=\"NonTerminalNode\">%1</span><span class=\"Symbol\">={</span>")
                .arg(nonTerminalNode->getName());
        Nodes nodes = setHash.value(nonTerminalNode).toList();
        foreach (Node *node, nodes) {
            richText += QString("<span class=\"%1\">%2</span>")
                    .arg(node->isTerminal() ? "TerminalNode" : "NonTerminalNode")
                    .arg(node->getName());
            if(!nodes.endsWith(node)) {
                richText += QString("<span class=\"Symbol\">,</span>");
            }
        }
        richText += QString("<span class=\"Symbol\">}</span></p>");
    }
    return QString(mSetHashHtmlTemplate).arg(richText);
}

void FirstFollowDialog::loadTemplate(){
    QFile setHashHtmlTemplateFile("SetHashHtmlTemplate.html");
    if(setHashHtmlTemplateFile.exists()) {
        mSetHashHtmlTemplate = TextFIleUtil::readFile(this, "SetHashHtmlTemplate.html");
    } else {
        mSetHashHtmlTemplate = TextFIleUtil::readFile(this, ":/Template/SetHashHtmlTemplate.html");
    }
}
