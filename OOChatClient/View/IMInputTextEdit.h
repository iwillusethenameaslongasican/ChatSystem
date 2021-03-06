/*********************************************
File Name：IMInputTextEdit.h
Author： hanyan
Date： 2016.11.16
Description： 输入编辑框
Changes：
********************************************/

#ifndef IMINPUTTEXTEDIT_H
#define IMINPUTTEXTEDIT_H

#include <QTextEdit>

class IMInputTextEdit: public QTextEdit
{
  Q_OBJECT
public:
  IMInputTextEdit(QWidget *parent = 0);
  ~IMInputTextEdit();

protected:
  //重载了keyPressEvent
  void keyPressEvent(QKeyEvent *event);

signals:
  void sendMessage();
};


#endif // IMINPUTTEXTEDIT_H
