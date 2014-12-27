#include "Node.h"

Node::Node(const QString &name, QObject *parent) :
    QObject(parent),
    mName(name)
{
}

void Node::addExplodedNodes(Nodes *explodedNodes){
    if(!explodedNodes->isEmpty()){
        mExplodedNodesList.append(explodedNodes);
    }
}

bool Node::isTerminal(){
    if(mExplodedNodesList.isEmpty()) return true;
    else return false;
}

bool Node::isNonNode(){
    if(mName == "ε")  return true;
    else return false;
}
