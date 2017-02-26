#include "IMLinkButton.h"
#include <QMenu>
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QDebug>

IMLinkButton::IMLinkButton(QWidget *parent)
  :QPushButton(parent)
{
  QHBoxLayout *mainLayout = new QHBoxLayout;
  m_labelHead = new QLabel;
  m_labelHead->setFixedSize(HEAD_SMALL_SIZE, HEAD_SMALL_SIZE);
  QVBoxLayout *rightLayout = new QVBoxLayout;
  rightLayout->setContentsMargins(0,0,0,0);
  m_labelInfo = new QLabel;
  m_labelMsg = new QLabel;
  rightLayout->addWidget(m_labelInfo);
  rightLayout->addWidget(m_labelMsg);

  mainLayout->addWidget(m_labelHead);
  mainLayout->addLayout(rightLayout);
  mainLayout->addStretch();
  mainLayout->setContentsMargins(0,0,0,0);
  setLayout(mainLayout);
  setMinimumSize(100, 40);

  m_menu = NULL;

  setStyleSheet("QPushButton{border:0};");
}

IMLinkButton::~IMLinkButton()
{

}

/*********************************************
 * function name: setLaststMessage
 * ***********************************/
void IMLinkButton::setLatestMessage(const TalkMessage &msg)
{
  m_labelMsg->setText(msg.m_text);
}

/*****************************************
 * function name: onClickRightButton
 * Description: 右键单击
 * ***************************************/
void IMLinkButton::onClickRightButton()
{
  if(m_menu == NULL)
    {
      //创建右键菜单
      createMenu();
    }
  m_menu->exec(QCursor::pos());
}

//remark protectd------------------------------------------------
/**********************************************
 * function name: mousePressEvent
 * Description: 鼠标点击事件
 * *******************************************/
void IMLinkButton::mousePressEvent(QMouseEvent *event)
{
  if(event->button() == Qt::RightButton)
    {
      onClickRightButton();
      return;
    }
  QPushButton::mousePressEvent(event);
}

/**************************************************
 * function name: mouseDoubleClickevent
 * Description: 鼠标双击事件
 * *******************************************/
void IMLinkButton::mouseDoubleClickEvent(QMouseEvent *event)
{
  if(event->button() == Qt::LeftButton)
    {
      onDoubleClickButton();
      return;
    }
  QPushButton::mouseDoubleClickEvent(event);
}

//remark: private------------------------------------------------
/********************************************
 *
 * function name: createMenu
 * Description: 创建右键菜单
 * ******************************************/
void IMLinkButton::createMenu()
{
  m_menu = new QMenu(this);
  QAction  *showMessage = new QAction(tr("查看消息"), m_menu);
  QAction *ignoreMessage = new QAction(tr("忽略消息"), m_menu);
  connect(showMessage, SIGNAL(triggered()),
          this, SLOT(onDoubleClickButton()));
  connect(ignoreMessage, SIGNAL(triggered()),
          this, SLOT(ignoreMessage()));

  m_menu->addAction(showMessage);
  m_menu->addAction(ignoreMessage);
}


