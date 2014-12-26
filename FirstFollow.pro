#-------------------------------------------------
#
# Project created by QtCreator 2014-12-23T14:46:47
# First&Follow
# 基本要求:
#   模拟算法的基本功能实现：
#      1. 输入一个文法G;
#      2. 输出由文法G构造的FIRST集合;
#      3. 输出由文法G构造的FOLLOW集合;
#      4. 构造预测分析表，并判断该文法是否为LL(1)文法�?(选作);
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FirstFollow
TEMPLATE = app


SOURCES += main.cpp\
        MainWindow.cpp \
    InputTextDialog.cpp \
    GrammarParser.cpp \
    Node.cpp \
    FirstFollowDialog.cpp

HEADERS  += MainWindow.h \
    InputTextDialog.h \
    TextFileUtil.hpp \
    GrammarParser.h \
    Node.h \
    FirstFollowDialog.h

FORMS    += MainWindow.ui \
    InputTextDialog.ui \
    FirstFollowDialog.ui

OTHER_FILES +=

RESOURCES += \
    Template.qrc
