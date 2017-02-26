/******************************************
 * hanyan
 * date: 2016.11.15
 * *****************************************/

#ifndef IMLINKBUTTON_H
#define IMLINKBUTTON_H


#include <QPushButton>
#include "../Model/IMContant.h"
class QLabel;

class IMLinkButton: public QPushButton
{
  Q_OBJECT
public:
  IMLinkButton(QWidget *parent = 0);
  virtual ~IMLinkButton();

  void setLatestMessage(const TalkMessage &msg);

  //设置button显示信息
  virtual void setButtonStatus() = 0;

signals:
  void deleteLinkButton(const QString &id);

public slots:
  //左键 双击
  virtual void onDoubleClickButton() = 0;
  //右键点击按钮
  virtual void onClickRightButton();
  //忽略消息
  virtual void ignoreMessage() = 0;

protected:
  void mousePressEvent(QMouseEvent *event);
  void mouseDoubleClickEvent(QMouseEvent *event);

protected:
  QLabel *m_labelHead;
  QLabel *m_labelInfo;
  QLabel *m_labelMsg;

  //右键菜单
  QMenu *m_menu;

private:
  //创建右键菜单
  void createMenu();
};


#endif // IMLINKBUTTON_H
