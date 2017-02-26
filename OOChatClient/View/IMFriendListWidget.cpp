#include "IMFriendListWidget.h"
#include <QVBoxLayout>
#include <QScrollArea>
#include <QDebug>
#include <QMouseEvent>
#include <QMenu>
#include "IMMainWidget.h"

IMFriendListWidget::IMFriendListWidget(IMMainWidget *mainWidget, QWidget *parent)
  : QWidget(parent), m_layout(new QVBoxLayout),m_mainWidget(mainWidget)
{
  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->setContentsMargins(0,0,0,0);
  mainLayout->setSpacing(0);
  m_contentWidget = new QWidget;
  m_layout->setContentsMargins(0,0,0,0);

  m_layout->setSpacing(0);
  m_contentWidget->setLayout(m_layout);

  m_friendsScrollArea = new QScrollArea(this);
  m_friendsScrollArea->setWidgetResizable(true);
  m_friendsScrollArea->setAlignment(Qt::AlignLeft);
  m_friendsScrollArea->setWidget(m_contentWidget);

  mainLayout->addWidget(m_friendsScrollArea);
  setLayout(mainLayout);

  setStyleSheet("QWidget{border: 0;}");

  createMenu();
}

/***********************************************
 * function name: addItem
 * Descriptin: 添加
 * *********************************************/
void IMFriendListWidget::addItem(QWidget *item)
{
  //移除上一次加的间隔Stretch
  int count = m_layout->count();
  if(count > 1)
    {
      m_layout->removeItem(m_layout->itemAt(count - 1));
    }

  //添加一个item 和 一个间隔
  m_layout->addWidget(item);
  m_layout->addStretch();

}

/*************************************************
 * function name: createMenu
 * Description: 创建右键菜单
 * **********************************************/
void IMFriendListWidget::createMenu()
{
  m_menu = new QMenu(this);
  QAction *create = new QAction(tr("新建分组"), m_menu);
  QAction *refresh = new QAction(tr("刷新好友分组"), m_menu);
  QAction *addFriend = new QAction(tr("添加好友"), m_menu);

  connect(create, &QAction::triggered,
          m_mainWidget, &IMMainWidget::createBox);
  connect(refresh, &QAction::triggered,
         m_mainWidget, &IMMainWidget::getFriendsInformation);
  connect(addFriend, &QAction::triggered,
          m_mainWidget, &IMMainWidget::onClickAddFriend);

  m_menu->addAction(create);
  m_menu->addAction(refresh);
  m_menu->addAction(addFriend);
}

/**************************************************
 * function name: showMenu
 * Description: 显示右键菜单
 * *********************************************/
void IMFriendListWidget::showMenu()
{
  if(m_menu == NULL)
    {
      createMenu();
    }
  m_menu->exec(QCursor::pos());
}

//mark: protected------------------------------------------------
/************************************************************
 * function name: mousePressEvent
 * Dscription: 鼠标点击事件
 * *************************************************/
void IMFriendListWidget::mousePressEvent(QMouseEvent *event)
{
  if(event->button() == Qt::RightButton)
    {
      showMenu();
      return;
    }
  QWidget::mousePressEvent(event);
}
