#include "FirstFollowDialog.h"
#include "ui_FirstFollowDialog.h"

FirstFollowDialog::FirstFollowDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FirstFollowDialog),
    mHeadNode(NULL)
{
    ui->setupUi(this);

    connect(&mGrammarParser, SIGNAL(parseSuccess(QString)), this, SLOT(parseSuccess(QString)));
    connect(&mGrammarParser, SIGNAL(parseError(QMap<int,QString>)), this, SIGNAL(parseError(QMap<int,QString>)));
}

void FirstFollowDialog::setSentenceText(const QString &text){
    mSentenceText = text;
    mGrammarParser.updateSentencesText(mSentenceText);
}

FirstFollowDialog::~FirstFollowDialog()
{
    delete ui;
}

void FirstFollowDialog::parseSuccess(const QString &richText){
    this->show();
    mHeadNode = mGrammarParser.getHeadNode();
    processFirst();
    processFollow();
    updateView();
}

void FirstFollowDialog::separateNodes(){

}

void FirstFollowDialog::processFirst(){

}
void FirstFollowDialog::processFollow(){

}

void FirstFollowDialog::updateView(){

}
