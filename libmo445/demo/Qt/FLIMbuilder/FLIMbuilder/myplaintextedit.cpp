#include "myplaintextedit.h"
#include <QFocusEvent>

MyPlainTextEdit::MyPlainTextEdit(QWidget *parent):
    QPlainTextEdit(parent)
{

}

MyPlainTextEdit::~MyPlainTextEdit()
{

}

void MyPlainTextEdit::focusOutEvent(QFocusEvent *event)
{
    if (event->lostFocus())
        emit focusChanged();
}
