#include "IMFlockListWidget.h"
#include "IMMainWidget.h"
#include <QVBoxLayout>
#include <QScrollArea>
#include <QMenu>
#include <QDebug>
#include <QMouseEvent>

IMFlockListWidget::IMFlockListWidget(IMMainWidget *mainWidget, QWidget *parent)
  :QWidget(parent),m_layout(new QVBoxLayout), m_mainWidget(mainWidget)
{
  if(NULL == m_mainWidget)
    {
      return;
    }
  QVBoxLayout *mainLayout = new QVBoxLayout();
  mainLayout->setContentsMargins(0,0,0,0);
  mainLayout->setSpacing(0);

  m_contentsWidget = new QWidget;
  m_layout->setContentsMargins(0,0,0,0);
  m_layout->setSpacing(0);
  m_contentsWidget->setLayout(m_layout);
  m_contentsWidget->setStyleSheet("QWidget{border: 0;}");

  m_flocksScrollArea = new QScrollArea(this);
  m_flocksScrollArea->setWidgetResizable(true);
  m_flocksScrollArea->setStyleSheet("QWidget{border: 0;}");

  m_flocksScrollArea->setWidget(m_contentsWidget);

  mainLayout->addWidget(m_flocksScrollArea);
  setLayout(mainLayout);

  setStyleSheet("QWidget{border: 0;}");

  //创建右键菜单
  createMenu();
}

/*********************************************
 * function name: addItem
 * Description: 添加
 * *******************************************/
void IMFlockListWidget::addItem(QWidget *item)
{
 int count = m_layout->count();

 if(count > 1)
   {
     m_layout->removeItem(m_layout->itemAt(count - 1));
   }

  m_layout->addWidget(item);
  m_layout->addStretch();
}

/*******************************************
 * function name: removeItem
 * Description: 删除
 * ****************************************/
void IMFlockListWidget::removeItem(QWidget *item)
{
  m_layout->removeWidget(item);
  if(m_layout->count() == 1)
    {
      m_layout->removeItem(m_layout->itemAt(0));
    }
}

/********************************************
 * function name: createMenu
 * Description: 创建右键菜单
 * ******************************************/
void IMFlockListWidget::createMenu()
{
    m_menu = new QMenu(this);
    QAction *actCreateFlock = new QAction(tr("新建群"), this);
    QAction *actAddFlock = new QAction(tr("添加群"), this);
    QAction *actRefresh = new QAction(tr("刷新群列表"), this);

    connect(actCreateFlock, SIGNAL(triggered()),
        m_mainWidget, SLOT(createFlock()));
    connect(actAddFlock, SIGNAL(triggered()),
        m_mainWidget, SLOT(onClickAddFlock()));
    connect(actRefresh, SIGNAL(triggered()),
        m_mainWidget, SLOT(getFlocksInformation()));

    m_menu->addAction(actCreateFlock);
    m_menu->addAction(actAddFlock);
    m_menu->addAction(actRefresh);
}


/*************************************************
Function Name： showMenu()
Description:  显示右键菜单
*************************************************/
void IMFlockListWidget::showMenu()
 {
    if (m_menu == NULL)
    {
        //创建右键菜单
        createMenu();
    }
    m_menu->exec(QCursor::pos());
 }


/*************************************************
Function Name： mousePressEvent
Description: 鼠标单击事件
*************************************************/
void IMFlockListWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
    {
        showMenu();
        return;
    }
    QWidget::mousePressEvent(event);
}
