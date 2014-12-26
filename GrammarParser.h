#ifndef GRAMMARPARSER_H
#define GRAMMARPARSER_H

#include <QObject>
#include <QList>
#include <QHash>
#include <QMap>
#include <QStringList>
#include <QColor>
#include <QDebug>
#include "Node.h"

typedef QHash<Node *, NodesList *> NodeParseHash;
typedef QHash<QString,  NodeParseHash *> SentenceParseHash;
typedef QHash<int, SentenceParseHash *> SentenceHash;

/**
 * @brief The GrammarParser class
 *
 */
class GrammarParser : public QObject
{
    Q_OBJECT
public:
    explicit GrammarParser(QObject *parent = 0);
    virtual ~GrammarParser();
    void updateSentencesText(const QString &text);

    QString getSentencesRichText(){
        return mSentencesRichText;
    }
    SentenceHash *getSentenceHash(){
        return mSentenceHash;
    }
    QHash<QString, Node *> *getNodes(){
        return mNodesHash;
    }
    Node *getHeadNode(){
        return mHeadNode;
    }
signals:
    void parseSuccess(const QString &richText);
    void parseError(const QMap<int, QString> &messageMap);
public slots:

private:
    QString mSentencesText;
    QString mSentencesRichText;

    Node *mHeadNode;
    SentenceHash *mSentenceHash; // 产生式
    QHash<QString, Node *> *mNodesHash; // 符集
    QMap<int, QString> mErrorMessageMap; // 错误集

    void parseSentence();
    NodesList *parseNodesList(const QString &text);
    Nodes *parseNodes(const QString &text);
    void clear();
    void processSentenceRichText();

    const QString mSentencePattern;
    const QString mNodePattern;
};

#endif // GRAMMARPARSER_H
