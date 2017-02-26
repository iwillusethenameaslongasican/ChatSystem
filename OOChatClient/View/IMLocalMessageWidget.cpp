#include "IMLocalMessageWidget.h"
#include "IMMainWidget.h"
#include "CustomLineEdit.h"
#include <QDebug>
#include <QStringListModel>
#include <QModelIndex>
#include <QListView>
#include <QMessageBox>

IMLocalMessageWidget::IMLocalMessageWidget(IMMainWidget *mainWidget, QWidget *parent)
  :  IMMessageWidget(mainWidget,parent), m_mainWidget(mainWidget)
{
  if(m_mainWidget == NULL)
    {
      return;
    }
  setShowMode(LOCAL);
}

IMLocalMessageWidget::~IMLocalMessageWidget()
{

}

//mark public slots:-----------------------------------------------

/***********************************************
 * function name: onClickDeleteDate
 * Description: 删除某日的消息记录
 * ********************************************/
void IMLocalMessageWidget::onClickDeleteDate()
{

}

/***********************************************
 * function name: onClickDeleteOne
 * Description: 删除好友消息记录
 * ******************************************/
void IMLocalMessageWidget::onClickDeleteOne()
{
  int row = m_listViewFriend->currentIndex().row();
  if(row < 0)
    {
      QString temp = QString(tr("请先选择好友！"));
      QMessageBox::information(NULL, tr("删除聊天记录"), temp);
      return;
    }
  if(NULL == m_mainWidget)
    {
      return;
    }
  m_mainWidget->deleteLocalMessage(m_indexOfFriendList);
}

/******************************************************
 * function name: onClickDeleteAll
 * Description: 删除全部聊天记录
 * ****************************************************/
void IMLocalMessageWidget::onClickDeleteAll()
{
  if(NULL == m_mainWidget)
    {
      return;
    }
  m_mainWidget->deleteLocalMessageAll();
}



/**************************************************
 * function name: onClickExportOne
 * Description: 导出当前好友消息记录
 * ***********************************************/
void IMLocalMessageWidget::onClickExportOne()
{
  int row = m_listViewFriend->currentIndex().row();
  if(row < 0)
    {
      QString temp = QString(tr("请先选择好友！"));
      QMessageBox::information(NULL, tr("导出聊天记录"), temp);
      return;
    }
  if(NULL == m_mainWidget)
    {
      return;
    }
  m_mainWidget->exportLocalMessage(m_indexOfFriendList);
}

/******************************************************
 * function name: onClickExportAll
 * Description: 导出所有消息记录
 * ************************************************/
void IMLocalMessageWidget::onClickExportAll()
{
  if(NULL == m_mainWidget)
    {
      return;
    }
  m_mainWidget->exportLocalMessageAll();
}

/*****************************************************
 * function name: onClickGetMessage
 * Description: 获取消息记录
 * ***********************************************/
void IMLocalMessageWidget::onClickGetMessage()
{
  int row = m_listViewFriend->currentIndex().row();
  if(row < 0)
    {
      QString temp = QString(tr("请选择好友"));
      QMessageBox::information(NULL, tr("显示聊天记录"), temp);
      return;
    }
  m_date = m_leBirthday->getDate();
  qDebug() << "get date: " << m_date;
  m_mainWidget->getLocalHistoryMessage(m_indexOfFriendList, m_date);
}
