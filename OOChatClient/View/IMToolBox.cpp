/*****************************************************************
 * date: 2016.11.15
 *
 * *************************************************************/
#include "IMToolBox.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QMenu>
#include <QDebug>

//mark :public------------------------------------------------------
IMToolItem::IMToolItem(const QString &title, QWidget *parent)
  : QWidget(parent)
{
  m_isVisable = false;
  m_layout = new QVBoxLayout;
  m_label = new QLabel(title);
  m_layout->addWidget(m_label);

  m_layout->setContentsMargins(0,0,0,0);
  m_layout->setSpacing(0);
  setLayout(m_layout);

  createMenu();
}

/**********************************************
 * function name: addItem
 * Description:  添加
 * ****************************************/
void IMToolItem::addItem(QWidget *item)
{
  m_itemList.append(item);
  m_layout->addWidget(item);

  if(0 == m_itemList.size())
    {
      item->setVisible(false);
    }
  else
    {
      item->setVisible(m_isVisable);
    }
}

/*******************************************
 * function name: removeItem
 * Description: 删除
 * ***************************************/
void IMToolItem::removeItem(QWidget *item)
{
  m_layout->removeWidget(item);
  m_itemList.removeAt(m_itemList.indexOf(item));
}

/************************************************
 * function name: setTitle
 * Description: 设置标题
 * ********************************************/
void IMToolItem::setTitle(const QString &title)
{
  m_label->setText(title);
}

/*******************************************
 * function name: getSize
 * Description: 获取成员数量
 * **************************************/
int IMToolItem::getSize()
{
  return m_itemList.size();
}

// mark: protected-------------------------------------------------

/********************************************
 * function name: mousePressEvent
 * Description: 鼠标点击事件
 * *************************************/
void IMToolItem::mousePressEvent(QMouseEvent *event)
{
  if(event->button() == Qt::LeftButton)
    {
      for(int i = 0; i < m_itemList.size();  ++ i)
        {
          m_itemList[i]->setVisible(!m_itemList[i]->isVisible());
        }
      if(m_itemList.size() > 0)
        {
          m_isVisable = m_itemList[0]->isVisible();
        }
    }
  else if(event->button() == Qt::RightButton)
    {
      onClickRightButton();
      return;
    }
  QWidget::mousePressEvent(event);
}


//mark: private slots:--------------------------------------------

/*******************************************
 * function name: renameBox
 * Description: 重命名
 * ***************************************/
void IMToolItem::renameBox()
{
  qDebug() << "chose rename";
  emit renameBoxSignal(m_label->text());
}

/**************************************
 * function name: removeBox
 * Description: 删除整个分组
 * *********************************/
void IMToolItem::removeBox()
{
  qDebug() << "chose remove";
  emit removeBoxSignal(m_label->text());
}

/***********************************************
 * function name: onClickRightButton
 * Description: 右键点击案件
 * *******************************************/
void IMToolItem::onClickRightButton()
{
  if(m_menu == NULL)
    {
      //新建右键菜单
      createMenu();
    }
  m_menu->exec(QCursor::pos());
}

/**********************************************
 * function name: createMenu
 * Description: 创建右键菜单
 * ********************************************/
void IMToolItem::createMenu()
{
  m_menu = new QMenu(this);
  QAction *rename = new QAction(tr("重命名"), m_menu);
  QAction *remove = new QAction(tr("删除"), m_menu);

  connect(rename, &QAction::triggered,
          this, &IMToolItem::renameBox);
  connect(remove, &QAction::triggered,
          this, &IMToolItem::removeBox);

  m_menu->addAction(rename);
  m_menu->addAction(remove);
}

IMToolBox::IMToolBox(QWidget *parent)
  :QWidget(parent)
{
  setLayout(m_layout);
  m_layout->setContentsMargins(0,0,0,0);
  m_layout->setSpacing(0);
}


/**********************************************
 * function name: addItem
 * Description: 添加
 * ******************************************/
void IMToolBox::addItem(IMToolItem *item)
{
  m_layout->addWidget(item);
}
