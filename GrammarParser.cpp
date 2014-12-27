#include "GrammarParser.h"

GrammarParser::GrammarParser(QObject *parent) :
    QObject(parent),
    mSentencePattern("\\s*(\\S*)\\s*->\\s*(.+)\\s*"), // 语句正则
    mNodePattern("([^|'\\s]{1}'*)"), // 符号正则
    mNodesHash(new QHash<QString, Node *>),
    mSentenceHash(new SentenceHash)
{
    mHeadNode = NULL;
}

GrammarParser::~GrammarParser(){
    clear();
    delete mNodesHash;
    delete mSentenceHash;
}

void GrammarParser::parseSentence(){
    QRegExp sentenceRE(mSentencePattern);

    Nodes explodedNodes; // collect exploded nodes

    QStringList strList = mSentencesText.split("\n");
    qDebug() << strList;

    for(int i=0; i<strList.size(); i++) {
        QString str = strList.at(i);

        NodeParseHash *nodeParseHash = new NodeParseHash;
        SentenceParseHash *sentenceParseHash = new SentenceParseHash;
        sentenceParseHash->insert(str, nodeParseHash);
        mSentenceHash->insert(i+1, sentenceParseHash);

        if(str.trimmed().isEmpty()) { // empty line
            continue;
        } else if (str.indexOf(sentenceRE) >= 0){ // read a sentence
            QStringList sentence = sentenceRE.capturedTexts(); // QStringList("S->Ab", "S", "Ab")

            Nodes *headList = parseNodes(sentence.at(1)); // head node
            NodesList *nodesList = parseNodesList(sentence.at(2)); // exploded nodes list

            // processing error message
            if(headList->size() != 1) {
                mErrorMessageMap[i+1] = QString("needed one and only one head node for every sentence. head node's number is %1")
                                 .arg(headList->size());
                continue;
            } else if(nodesList->size() <= 0) {
                mErrorMessageMap[i+1] = QString("needed more than one exploded node for every sentence. exploded node's number is %1")
                                 .arg(nodesList->size());
                continue;
            }

            // processing node connection
            Node *headNode = headList->at(0);
            if(mHeadNode == NULL){ // first headNode
                mHeadNode = headNode;
            }
            headNode->setExplodedNodesList(nodesList);

            // processing node context
            if(headNode != mHeadNode && !explodedNodes.contains(headNode)) { // head node must exist before except first node
                mErrorMessageMap[i+1] = QString("head node must exist before, except first node")
                                 .arg(nodesList->size());
            }
            foreach (Nodes *nodes, *nodesList) {
                explodedNodes.append(*nodes);
            }

            (*nodeParseHash)[headNode] = nodesList;

        } else { // error line
            mErrorMessageMap[i+1] = QString("syntax error, sentence: %2").arg(str);
        }
    }
    emit parseError(mErrorMessageMap);
}

void GrammarParser::updateSentencesText(const QString &text){
    clear();

    mSentencesText = text;

    parseSentence();

    processSentenceRichText();
}

NodesList *GrammarParser::parseNodesList(const QString &text){
    NodesList *nodesList = new NodesList;
    QStringList nodesStr = text.split("|");
    foreach (QString nodeStr, nodesStr) {
        Nodes *nodes = parseNodes(nodeStr);
        if(!nodes->isEmpty()) nodesList->append(nodes);
    }
    return nodesList;
}

Nodes *GrammarParser::parseNodes(const QString &text){
    Nodes *nodeList = new Nodes;
    QRegExp nodeRE(mNodePattern);
    for(int i=0; i<text.length(); ){
        int pos = text.indexOf(nodeRE, i);
        if(pos != -1){
            QString nodeName = nodeRE.capturedTexts().at(1).trimmed();
            if(!nodeName.isEmpty()){
                if(!mNodesHash->keys().contains(nodeName)){ // the node is not exist;
                    Node *node = new Node(nodeName);
                    (*mNodesHash)[nodeName] = node; // add to terminal node list
                    nodeList->append(node);
                } else {
                    nodeList->append((*mNodesHash)[nodeName]);
                }
            }
            i = pos + 1;
        } else {
            break;
        }
    }

    return nodeList;
}

void GrammarParser::processSentenceRichText(){
    mSentencesRichText += "<pre>";

    Nodes explodedNodes; // collect exploded nodes
    for(int i=1; i <= mSentenceHash->size(); i++){
        // mSentencesRichText += QString("<span class=\"LineNumber\">%1 </span>").arg(i);
        bool isLineError = false;

        SentenceParseHash *sentenceParseHash = (*mSentenceHash)[i];
        QString sentenceText = "";

        QList<QString> sentenceTextList = sentenceParseHash->keys();
        if(sentenceTextList.size() != 1){ // error line
            isLineError = true;
            foreach (QString text, sentenceTextList) {
                sentenceText += text;
            }
        } else {
            sentenceText = sentenceTextList.at(0);
            if(sentenceText.trimmed().isEmpty()) { // 空行
                mSentencesRichText += sentenceText;
            } else {
                NodeParseHash *nodeParseHash = sentenceParseHash->value(sentenceText);
                QList<Node *> headNodeList = nodeParseHash->keys();
                if(headNodeList.size() != 1) { // error line
                    isLineError = true;
                } else {
                    Node *headNode = headNodeList.at(0);
                    if(headNode != mHeadNode && !explodedNodes.contains(headNode)) { // head node must exist before except first node
                        isLineError = true;
                    } else {
                        NodesList *nodesList = nodeParseHash->value(headNode);
                        if(nodesList->isEmpty()) { // error line
                            isLineError = true;
                        } else {
                            mSentencesRichText += QString("<span class=\"%1\">%2</span>")
                                    .arg(headNode->isTerminal() ? "TerminalNode" : "NonTerminalNode")
                                    .arg(headNode->getName());
                            mSentencesRichText += "<span class=\"Symbol\"> -> </span>";
                            foreach (Nodes *nodes, *nodesList) {
                                explodedNodes.append(*nodes);
                                foreach (Node *node, *nodes) {
                                    mSentencesRichText += QString("<span class=\"%1\">%2</span>")
                                            .arg(node->isTerminal() ? "TerminalNode" : "NonTerminalNode")
                                            .arg(node->getName());
                                }
                                if(nodes != nodesList->at(nodesList->size()-1)){
                                    mSentencesRichText += "<span class=\"Symbol\"> | </span>";
                                }
                            }
                        }
                    }
                }
            }
        }

        if(isLineError) {
            mSentencesRichText += QString("<span class=\"ErrorLine\">%1</span>")
                    .arg(sentenceText);
        }

        mSentencesRichText += "\n";
    }

    mSentencesRichText += "</pre>";
    emit parseSuccess(mSentencesRichText);
}

void GrammarParser::clear(){
    mHeadNode = NULL;

    mSentencesText.clear();
    mSentencesRichText.clear();

    mSentenceHash->clear();

    foreach (Node *node, mNodesHash->values()) {
        delete node;
    }
    mNodesHash->clear();
    mErrorMessageMap.clear();
}
