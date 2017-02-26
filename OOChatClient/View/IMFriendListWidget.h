#ifndef IMFRIENDLISTWIDGET_H
#define IMFRIENDLISTWIDGET_H


#include <QWidget>

class QVBoxLayout;
class QScrollArea;
class QMenu;
class IMMainWidget;

class IMFriendListWidget: public QWidget
{
  Q_OBJECT
public:
  IMFriendListWidget(IMMainWidget *mainWidget, QWidget *parent = 0);
  void addItem(QWidget *item);

  //创建右键菜单
  void createMenu();

signals:

public slots:
  void showMenu();
protected:
  void mousePressEvent(QMouseEvent *event);

private:
  QVBoxLayout *m_layout;
  QWidget *m_contentWidget;
  QScrollArea *m_friendsScrollArea;

  //右键菜单
  QMenu *m_menu;

  IMMainWidget *m_mainWidget;
};


#endif // IMFRIENDLISTWIDGET_H
