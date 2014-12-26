#ifndef NODE_H
#define NODE_H

#include <QObject>
#include <QList>

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
        this->mFirstList = node.mFirstList;
        this->mFollowList = node.mFollowList;
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
    Nodes getFirstList(){
        return mFirstList;
    }
    Nodes getFollowList(){
        return mFollowList;
    }

    void addExplodedNodes(Nodes *explodedNodes);

    void setExplodedNodesList(NodesList *explodedNodesList){
        if(!explodedNodesList->isEmpty())
            this->mExplodedNodesList = *explodedNodesList;
    }

    bool isTerminal();
signals:

public slots:

private:
    QString mName;
    NodesList mExplodedNodesList;
    Nodes mFirstList;
    Nodes mFollowList;
};

typedef QList<Node *> Nodes;
typedef QList<QList<Node *> *> NodesList;

#endif // NODE_H
