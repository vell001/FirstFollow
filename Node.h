#ifndef NODE_H
#define NODE_H

#include <QObject>
#include <QList>
#include <QSet>

class Node : public QObject
{
    typedef QList<Node *> Nodes;
    typedef QList<QList<Node *> *> NodesList;
    Q_OBJECT
public:
    explicit Node(const QString &name = QString(), QObject *parent = 0);
    explicit Node(const Node &node) :
        QObject(node.parent())
    {
        this->mExplodedNodesList = node.mExplodedNodesList;
        this->mName = node.mName;
    }

    bool operator == (const Node &node){
        if(this->mName == node.mName) return true;
        else return false;
    }

    QString getName(){
        return mName;
    }
    NodesList getExplodedNodesList(){
        return mExplodedNodesList;
    }

    void addExplodedNodes(Nodes *explodedNodes);

    void setExplodedNodesList(NodesList *explodedNodesList){
        if(!explodedNodesList->isEmpty())
            this->mExplodedNodesList = *explodedNodesList;
    }

    bool isTerminal();
    bool isNonNode();
signals:

public slots:

private:
    QString mName;
    NodesList mExplodedNodesList;
};

typedef QList<Node *> Nodes;
typedef QSet<Node *> NodeSet;
typedef QList<QList<Node *> *> NodesList;

#endif // NODE_H
