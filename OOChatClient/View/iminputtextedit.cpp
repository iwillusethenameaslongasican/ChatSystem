/*********************************************
File Name： IMInputTextEdit.cpp
Author： hanyan
Date： 2016.11.14
Description： 输入编辑框
Changes：
********************************************/

#include "IMInputTextEdit.h"
#include <QKeyEvent>

IMInputTextEdit::IMInputTextEdit(QWidget *parent)
  : QTextEdit(parent)
{

}

IMInputTextEdit::~IMInputTextEdit()
{

}

void IMInputTextEdit::keyPressEvent(QKeyEvent *event)
{
  //如果按下Ctrl + Enter,就发生消息，否则就按默认的方式来处理
  //&&前判断ENTER是否按下 &&后判断ctrl是否按下
  bool isSend = (Qt::Key_Return == event->key() || Qt::Key_Enter == event->key())
                 && (event->modifiers() & Qt::ControlModifier);

  if(isSend)
    {
      emit sendMessage();
    }
  else
    {
      QTextEdit::keyPressEvent(event);
    }
}
