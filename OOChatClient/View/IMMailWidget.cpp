#include "IMMailWidget.h"

#include <QTabWidget>
#include <QPushButton>
#include <QLabel>
#include <QTableWidget>
#include <QVBoxLayout>

#include "IMMailButton.h"
#include "IMMailInWidget.h"
#include "IMMailOutWidget.h"
#include "IMMailInformationWidget.h"
#include "IMMailWriteWidget.h"

#include "../Control/IMMailCtrl.h"

IMMailWidget::IMMailWidget(const QString &myID, QWidget *parent)
  : QWidget(parent),m_myID(myID)
{
  m_mailCtrl = new IMMailCtrl(myID, this);
  initIMMailWidget();
  linkSignalWithSlot();
  setWindowTitle("圈圈邮件");
}


IMMailWidget::~IMMailWidget()
{
  clearContainers();
}

/********************************************
 * function name: getMailCtrl
 * Description: 获取mail控制对象
 * ***************************************/
IMMailCtrl *IMMailWidget::getMailCtrl()
{
  return m_mailCtrl;
}

/******************************************
 * function name: getMyID
 * Description: 获取用户自己的ID
 * ************************************/
QString IMMailWidget::getMyID()
{
  return m_myID;
}


/******************************************
 * function name: showMailInformationWidget
 * Description: 显示邮件信息面板
 * ************************************/
void IMMailWidget::showMailInformationWidget(const MailInformation &mail)
{
  int count = m_tabWidget->count();

  if(count < 4)
    {
      m_tabWidget->addTab(m_informationWidget, tr("信件详情"));
    }
  m_informationWidget->setMailInformation(mail);
  m_tabWidget->setCurrentWidget(m_informationWidget);
}

/*******************************************
 * function name: showMailInWidget
 * Description: 显示收信箱
 * *******************************************/
void IMMailWidget::showMailInWidget()
{
  if(m_inWidget == NULL || m_tabWidget == NULL)
    {
      return;
    }
  m_tabWidget->setCurrentWidget(m_inWidget);
}

/*****************************************
 * function name: showMailWriteWidget
 * Description: 显示写信窗口
 * **************************************/
void IMMailWidget::showMailWriteWidget(const QString &address)
{
  if(m_writeWidget == NULL || m_tabWidget == NULL)
    {
      return;
    }
  m_tabWidget->setCurrentWidget(m_writeWidget);
  m_writeWidget->setReceiverAddress(address);
}

// private slots:-------------------------------------------------

/***********************************************
 * function name: setInMailsWidget
 * Description: 设置收信箱
 * ********************************************/
void IMMailWidget::setInMailsWidget(const QVector<MailInformation> &inMailsVec)
{
  int len = inMailsVec.size();
  for(int i = 0; i < len; i ++)
    {
      addInMailButton(inMailsVec[i]);
    }
  m_mailCtrl->requestGetOutMails();
}

/**********************************************
 * function name: setOutMailsWidget
 * Description: 设置发信箱
 * ********************************************/
void IMMailWidget::setOutMailsWidget(const QVector<MailInformation> &outMailsVec)
{
  int len = outMailsVec.size();
  for(int i = 0; i < len; i ++)
    {
      addOutMailButton(outMailsVec[i]);
    }
}

/*********************************************
 * function name: addInMailButton
 * Description: 添加收信箱按钮
 * *******************************************/
bool IMMailWidget::addInMailButton(const MailInformation &mail)
{
  if(m_inMailMap.contains(mail.m_mailID))
    {
      return false;
    }
  IMMailButton *button = new IMMailButton(mail, this, this);

  m_inMailMap.insert(mail.m_mailID, button);
  m_inWidget->addItem(button);

  return true;
}

/**************************************************
 * function name: addOutMailButton
 * Description: 添加发信箱按钮
 * **********************************************/
bool IMMailWidget::addOutMailButton(const MailInformation &mail)
{
  if(m_outMailMap.contains(mail.m_mailID))
    {
      return false;
    }
  IMMailButton *button = new IMMailButton(mail, this, this);
  m_outMailMap.insert(mail.m_mailID, button);
  m_outWidget->addItem(button);

  return true;
}

/*************************************************
 * function name: deleteMail
 * Description: 删除邮件按钮
 * *********************************************/
void IMMailWidget::deleteMail(const int &mailID)
{
  if(m_inMailMap.contains(mailID))
    {
      m_inWidget->removeItem(m_inMailMap[mailID]);
      m_inMailMap[mailID]->deleteLater();
      m_inMailMap.remove(mailID);
    }
  if(m_outMailMap.contains(mailID))
    {
      m_outWidget->removeItem(m_outMailMap[mailID]);
      m_outMailMap[mailID]->deleteLater();
      m_outMailMap.remove(mailID);
    }
  if(m_tabWidget->currentIndex() == 3)
    {
      m_tabWidget->setCurrentIndex(0);
    }
  if(m_tabWidget->count() >= 4)
    {
      m_tabWidget->removeTab(3);
    }
}

/*************************************************
 * function name: addNewOutMailButton
 * Description: 添加新邮件到发信箱
 * *********************************************/
bool IMMailWidget::addNewOutMailButton(const MailInformation &mail)
{
  if(m_outMailMap.contains(mail.m_mailID))
    {
      return false;
    }
  IMMailButton *button = new IMMailButton(mail, this, this);

  m_outMailMap.insert(mail.m_mailID, button);
  m_outWidget->insertItem(0, button);
  return true;
}

/************************************************
 * function name: addNewInMailButton
 * Description: 添加新邮件到收件箱
 * **********************************************/
bool IMMailWidget::addNewInMailButton(const MailInformation &mail)
{
  if(m_inMailMap.contains(mail.m_mailID))
    {
      return false;
    }
  IMMailButton *button = new IMMailButton(mail, this, this);

  m_inMailMap.insert(mail.m_mailID, button);
  m_inWidget->insertItem(0, button);
  return true;
}


//private:--------------------------------------------------------------

/***************************************************
 * function name: initIMMailWidget
 * Description: 初始化
 * **********************************************/
void IMMailWidget::initIMMailWidget()
{
  m_tabWidget = new QTabWidget(this);
  m_outWidget = new IMMailOutWidget(m_mailCtrl, this);
  m_inWidget = new IMMailInWidget(m_mailCtrl, this);
  m_writeWidget = new IMMailWriteWidget(m_myID, m_mailCtrl, this);
  m_informationWidget = new IMMailInformationWidget(this, this);

  m_tabWidget->addTab(m_inWidget, tr("收信箱"));
  m_tabWidget->addTab(m_outWidget, tr("发信箱"));
  m_tabWidget->addTab(m_writeWidget, tr("写信"));
  m_labelInformation = new QLabel(QString(tr("%1, 你好")).arg(m_myID));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(m_labelInformation);
  mainLayout->addWidget(m_tabWidget);

  setLayout(mainLayout);
}

/***************************************************
 * function name: linkSignalWithSlot
 * Description: 连接信号于槽
 * *************************************************/
void IMMailWidget::linkSignalWithSlot()
{
  connect(m_mailCtrl, &IMMailCtrl::getInMailsSuccess,
          this, &IMMailWidget::setInMailsWidget);
  connect(m_mailCtrl, &IMMailCtrl::getOutMailsSuccess,
          this, &IMMailWidget::setOutMailsWidget);

  connect(m_mailCtrl, &IMMailCtrl::deleteMailSuccess,
          this, &IMMailWidget::deleteMail);
  connect(m_mailCtrl, &IMMailCtrl::getInMail,
          this, &IMMailWidget::addNewInMailButton);
  connect(m_mailCtrl, &IMMailCtrl::getOutMail,
          this, &IMMailWidget::addNewOutMailButton);
}


/***************************************
 * function name: clearContainers
 * Description: 清理容器
 * ************************************/
void IMMailWidget::clearContainers()
{
  qDeleteAll(m_inMailMap.begin(), m_inMailMap.end());
  m_inMailMap.clear();

  qDeleteAll(m_outMailMap.begin(), m_outMailMap.end());
  m_outMailMap.clear();
}
