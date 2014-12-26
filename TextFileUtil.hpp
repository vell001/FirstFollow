#ifndef TEXTFILEUTIL_HPP
#define TEXTFILEUTIL_HPP

#include <QTextStream>
#include <QFile>
#include <QString>
#include <QMessageBox>

class TextFIleUtil{
public:
    static QString readFile(QWidget *parent, const QString &fileName){
        QFile textFile(fileName);
        if(!textFile.open(QFile::ReadOnly | QFile::Text)){
            QMessageBox::warning(parent,
                                 "can't open file!!!",
                                 "can't open file!!!\n please check your file");
            return NULL;
        }

        QTextStream in(&textFile);
        QString text = in.readAll();
        textFile.close();
        return text;
    }

    static bool writeFile(QWidget *parent, const QString &text, const QString &fileName){
        QFile textFile(fileName);
        if(!textFile.open(QFile::WriteOnly | QFile::Text)){
            QMessageBox::warning(parent,
                                 "can't open file!!!",
                                 "can't open file!!!\n please check your file");
            return false;
        }

        QTextStream out(&textFile);
        out << text;
        out.flush();
        textFile.close();
        return true;
    }
};

#endif // TEXTFILEUTIL_HPP
