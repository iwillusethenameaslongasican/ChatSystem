/*********************************************
File Name： IMMainWidget.cpp
Author： hanyan
Date： 2016.11.6 - 2016.11.9
Description： 客户端主界面
Changes：
********************************************/

#include "IMMainWidget.h"
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QToolBox>
#include <QToolButton>
#include <QGroupBox>
#include <QMessageBox>
#include <QFrame>
#include <QMap>
#include <QFileDialog>
#include <QInputDialog>
#include <QMenu>
#include <QDir>
#include <QFile>
#include <QListWidget>
#include <QHBoxLayout>
#include <QDebug>
#include <QTime>
#include <QTimerEvent>
#include <QMouseEvent>

#include "IMFriendListWidget.h"
#include "IMFlockListWidget.h"
#include "IMDiscussionListWidget.h"
#include "IMLatestMessageListWidget.h"
#include "IMToolBox.h"
#include "IMChatWidget.h"
#include "IMFriendButton.h"
#include "IMFlockButton.h"
#include "IMDiscussionButton.h"
#include "IMInformationWidget.h"
#include "IMClickLabel.h"
#include "IMFlockChatWidget.h"
#include "IMFlockInformationWidget.h"
#include "IMDiscussionChatWidget.h"
#include "IMDiscussionInformationWidget.h"
#include "IMMailWidget.h"
#include "IMMessageManageWidget.h"
#include "IMLinkButton.h"
#include "IMLinkFriendButton.h"
#include "IMLinkFlockButton.h"
#include "IMLinkDiscussionButton.h"
#include "IMSearchListWidget.h"
#include "../Model/IMEncryption.h"
#include "../Control/IMMainCtrl.h"

static const QString TMP_LOCAL_HISTROY_PATH = "/tmp/local_history_message";

//mark: public:------------------------------------------------------------

//拷贝文件：
bool copyFileToPath(QString sourceDir, QString toDir, bool coverFileIfExist)
{
    toDir.replace("\\", "/");
    if(sourceDir == toDir)
        {
            return true;
        }
    if(!QFile::exists(sourceDir))
        {
            return false;
        }
    QDir *createfile = new QDir;
    bool exist = createfile->exists(toDir);
    if(exist)
        {
            if(coverFileIfExist)
                {
                    createfile->remove(toDir);
                }
        }
    if(!QFile::copy(sourceDir, toDir))
        {
            return false;
        }
    return true;
}

IMMainWidget::IMMainWidget(const UserInformation me, QWidget *parent)
    : QWidget(parent),
    m_database(me.m_userID)
{
    m_flag = 0;
    m_timerID = 0;
    m_mySelf = me;
    m_onLineCounter = 0;
    m_mailWidget = NULL;
    m_messageManWidget = NULL;
    initIMMainWidget();
    linkSignalWithSlot();
    createMenu();
    setWindowTitle(tr("OO"));

}

IMMainWidget::~IMMainWidget()
{
    //关闭定时器
    if(m_timerID != 0)
        {
            killTimer(m_timerID);
            m_timerID = 0;
        }

    if(m_database.isOpen())
        {
            m_database.close();
        }

    if(m_mailWidget != NULL)
        {
            m_mailWidget->close();
            delete m_mailWidget;
            m_mailWidget = NULL;
        }

    if(m_messageManWidget != NULL)
        {
            m_messageManWidget->close();
            delete m_messageManWidget;
            m_messageManWidget = NULL;
        }

    //清空容器
    clearContainers();
    //当容器里的元素是指针类型时，必须先调用qDeleteALL对容器里的所有指针执行delete操作
    //使它们变成野指针，再调用clear函数来避免这些指针的误用
    qDeleteAll(m_chatRoomMap.begin(), m_chatRoomMap.end());
    m_chatRoomMap.clear();

    qDeleteAll(m_flockRoomMap.begin(), m_flockRoomMap.end());
    m_flockRoomMap.clear();

    qDeleteAll(m_discussionRoomMap.begin(), m_discussionRoomMap.end());
    m_discussionRoomMap.clear();

    qDeleteAll(m_informationMap.begin(), m_informationMap.end());
    m_informationMap.clear();

    qDeleteAll(m_flockInformationMap.begin(), m_flockInformationMap.end());
    m_flockInformationMap.clear();

    qDeleteAll(m_discussionInformationMap.begin(), m_discussionInformationMap.end());
    m_discussionInformationMap.clear();

}

/*************************************************
Function Name： getLocalMyInformation()
Description: 获取本地个人信息
*************************************************/
UserInformation IMMainWidget::getLocalMyInformation() const
{
    return m_mySelf;
}

/*************************************************
Function Name： isMyFriend()
Description: 是否是我的好友
*************************************************/
bool IMMainWidget::isMyFriend(const QString &id) const
{
    if(m_friendMap.contains(id))
        {
            return true;
        }
    return false;
}

/*************************************************
Function Name： insertChatRoomMap()
Description: 插入聊天室
*************************************************/
void IMMainWidget::insertChatRoomMap(const QString &id, IMChatWidget *chatRoom)
{
    m_chatRoomMap.insert(id, chatRoom);
}

/*************************************************
Function Name： insertFlockRoomMap()
Description: 插入聊天室
*************************************************/
void IMMainWidget::insertFlockRoomMap(const QString &id, IMFlockChatWidget *chatRoom)
{
    m_flockRoomMap.insert(id, chatRoom);
}


/*************************************************
Function Name： insertDiscussionRoomMap()
Description: 插入聊天室
*************************************************/
void IMMainWidget::insertDiscussionRoomMap(const QString &id, IMDiscussionChatWidget *chatRoom)
{
    m_discussionRoomMap.insert(id, chatRoom);
}


/*************************************************
Function Name： getFriendInformation
Description: 获取好友信息（发送至服务器）
*************************************************/
void IMMainWidget::getFriendInformation(const QString &id)
{
    if(NULL != m_mainCtrl)
        {
            m_mainCtrl->getFriendInformation(id);
        }
}

/*************************************************
Function Name： changeMyInformation
Description: 改变自己信息（发送至服务器）
*************************************************/
void IMMainWidget::changeMyInformation(const UserInformation &me)
{
    if(NULL != m_mainCtrl)
        {
            m_mainCtrl->changeMyInformation(me);
        }
}

/*************************************************
Function Name： changeFriendRemark
Description: 改变好友备注（发送至服务器）
*************************************************/
void IMMainWidget::changeFriendRemark(const QString &friendID,
                                      const QString &remark)
{
    if(NULL != m_mainCtrl)
        {
            m_mainCtrl->changeFriendRemark(m_mySelf.m_userID, friendID, remark);
        }
}

/*************************************************
Function Name： deleteFriend
Description: 删除好友（发送至服务器
*************************************************/
void IMMainWidget::deleteFriend(const QString &friendID)
{
    if(NULL != m_mainCtrl)
        {
            m_mainCtrl->deleteFriend(m_mySelf.m_userID, friendID);
        }
}

/*************************************************
Function Name： removeInformationWidget
Description: 删除存储于m_informationMap中的信息窗口
*************************************************/
void IMMainWidget::removeInformationWidget(const QString &userID)
{
    m_informationMap[userID]->deleteLater();
    m_informationMap.remove(userID);
}

/*************************************************
Function Name： removeFlockInformationWidget
Description: 删除存储与m_flockInformationMap中的信息窗口
*************************************************/
void IMMainWidget::removeFlockInformationWidget(const QString &flockID)
{
    m_flockInformationMap[flockID]->deleteLater();
    m_flockInformationMap.remove(flockID);
}

/*************************************************
Function Name： removeDiscussionInformationWidget
Description: 删除存储与m_discussionInformationMap中的信息窗口
*************************************************/
void IMMainWidget::removeDiscussionInformationWidget(const QString &discussionID)
{
    m_discussionInformationMap[discussionID]->deleteLater();
    m_discussionInformationMap.remove(discussionID);
}


/*************************************************
Function Name： changeFlockInformation
Description: 修改群信息（发送至服务器）
*************************************************/
void IMMainWidget::changeFlockInformation(const FlockInformation &flock)
{
    if(NULL != m_mainCtrl)
        {
            m_mainCtrl->changeFlockInformation(flock);
        }
}


/*************************************************
Function Name： changeDiscussionInformation
Description: 修改讨论组信息 （发送至服务器）
*************************************************/
void IMMainWidget::changeDiscussionInformation(const DiscussionInformation &discussion)
{
    if(NULL != m_mainCtrl)
        {
            m_mainCtrl->changeDiscussionInformation(discussion);
        }
}


/*************************************************
Function Name： removeChatWidget
Description: 删除存储于m_chatRoomMap中的聊天窗口
*************************************************/
void IMMainWidget::removeChatWidget(const QString &friendID)
{
  m_chatRoomMap[friendID]->deleteLater();
  m_chatRoomMap.remove(friendID);
}

/*************************************************
Function Name： removeFlockChatWidget
Description: 删除存储于m_flockRoomMap中的群聊窗口
*************************************************/
void IMMainWidget::removeFlockChatWidget(const QString &flockID)
{
  m_flockRoomMap[flockID]->deleteLater();
  m_flockRoomMap.remove(flockID);
}

/*************************************************
Function Name： removeDiscussionChatWidget
Description: 删除存储于m_discussionRoomMap中的讨论组窗口
*************************************************/
void IMMainWidget::removeDiscussionChatWidget(const QString &discussionID)
{
  m_discussionRoomMap[discussionID]->deleteLater();
  m_discussionRoomMap.remove(discussionID);
}

/*************************************************
Function Name： dropFlock
Description: 解散群(发送至服务器)
*************************************************/
void IMMainWidget::dropFlock(const QString &flockID)
{
  if(NULL != m_mainCtrl)
    {
      m_mainCtrl->dropFlock(m_mySelf.m_userID, flockID);
    }
}


/*************************************************
Function Name： dropDiscussion
Description: 解散讨论组（发送至服务器）
*************************************************/
void IMMainWidget::dropDiscussion(const QString &discussionID)
{
  if(NULL != m_mainCtrl)
    {
      m_mainCtrl->dropDiscussion(m_mySelf.m_userID, discussionID);
    }
}

/*************************************************
Function Name： leaveFlock
Description: 退出群（发送至服务器）
*************************************************/
void IMMainWidget::leaveFlock(const QString &memberID, const QString &flockID)
{
  if(NULL != m_mainCtrl)
    {
      m_mainCtrl->leaveFlock(memberID, flockID);
    }
}


/*************************************************
Function Name： leaveFlock
Description: 退出讨论组（发送至服务器）
*************************************************/
void IMMainWidget::leaveDiscussion(const QString &memberID, const QString &discussionID)
{
  if(NULL != m_mainCtrl)
    {
      m_mainCtrl->leaveDiscussion(memberID, discussionID);
    }
}

/*************************************************
Function Name： addFriend
Description: 添加好友（发送至服务器）
*************************************************/
void IMMainWidget::addFriend(const QString friendID)
{
  if(0 == friendID.compare(m_mySelf.m_userID))
    {
      QMessageBox::critical(NULL, tr("添加失败"), tr("不能添加自己"));
      return;
    }
  if(m_friendMap.contains(friendID))
    {
      QMessageBox::critical(NULL, tr("添加失败"), tr("该用户已经是您的好友"));
      return;
    }

  bool isOK = false;
  QString checkMsg = QInputDialog::getText(NULL, "添加好友",
                                           "请输入验证信息",
                                           QLineEdit::Normal,
                                           QString(tr("您好，我是： %1"))
                                           .arg(m_mySelf.m_nickname),
                                           &isOK);

  if(!isOK)
    {
      return;
    }

  TalkMessage msg;
  msg.m_senderID = m_mySelf.m_userID;
  msg.m_receiverID = friendID;
  msg.m_text = checkMsg;
  msg.m_type = REQUEST_FRIEND;


  if(NULL != m_mainCtrl)
    {
      m_mainCtrl->addFriend(msg);
    }

}


/*************************************************
Function Name： changeFlockMemberRemark
Description: 修改群成员名片（发送至服务器）
*************************************************/
void IMMainWidget::changeFlockMemberRemark(const TempStrings &tmpStr)
{
  if(NULL != m_mainCtrl)
    {
      m_mainCtrl->changeFlockMemberRemark(tmpStr);
    }
}


/*************************************************
Function Name： moveFriendToBox
Description: 移动好友到其他分组（发送至服务器）
*************************************************/
void IMMainWidget::moveFriendToBox(const QString &friendID, const QString &groupName,
                                   const QString &title)
{
  if(NULL != m_mainCtrl)
    {
      m_mainCtrl->moveFriendToBox(friendID, groupName, title);
    }
}

/*************************************************
Function Name： setOnlineCounter
Description: 设置在线人数
*************************************************/
void IMMainWidget::setOnlineCounter(int num)
{
  m_onLineCounter = num;
  m_labelOnlineNum->setText(QString(tr("在线人数： %1"))
                            .arg(QString::number(num)));
}

/*************************************************
Function Name： getGroupFriendList()
Description: 获取给定分组的所有好友
*************************************************/
QStringList IMMainWidget::getGroupFriendList(const QString &groupName)
{
  m_friendsRemarkList.clear();
  m_friendsIDList.clear();
  if(!m_indexFriendsGroupMap.contains(groupName))
    {
      //返回空的列表
      return m_friendsRemarkList;
    }
  int groupIndex = m_indexFriendsGroupMap[groupName];
  //m_toolItemsFriendsVec是存储每个分组的向量
  IMToolItem *group = m_toolItemsFriendsVec[groupIndex];
  const QList<QWidget *> *friendList = group->getWidgetList();

  QString mark;
  FriendInformation friendInformation;

  for(int i = 0; i < friendList->size(); ++ i)
    {
      IMFriendButton *friendButton = (IMFriendButton *)friendList->value(i);
      friendInformation = friendButton->getInformation();

      if(friendInformation.m_remarkName.isEmpty())
        {
          mark = friendInformation.m_nickname;
        }
      else
        {
          mark = friendInformation.m_remarkName;
        }
      m_friendsRemarkList.append(mark);
      m_friendsIDList.append(friendInformation.m_userID);
    }
  return m_friendsRemarkList;
}

/*************************************************
Function Name： getLocalHistoryMessage()
Description: 获取本地某日的消息记录
*************************************************/
void IMMainWidget::getLocalHistoryMessage(const int index, const QDate &date)
{
  m_localHistoryMessageVec.clear();
  if(index < 0 || index >= m_friendsIDList.size())
    {
      return;
    }
  bool isOK = m_database.searchHistoryMessage(m_mySelf.m_userID,
                                               m_friendsIDList[index],
                                               date,
                                               m_localHistoryMessageVec);
  qDebug() << "m_localHistoryMessageVec: " << isOK << m_localHistoryMessageVec.size();
  m_messageManWidget->setLocalMessageWidget(m_localHistoryMessageVec);
}


/*************************************************
Function Name： exportLocalMessage()
Description: 导出消息记录
*************************************************/
void IMMainWidget::exportLocalMessage(const int index)
{
  QString csv = tr("CSV文件 (*.csv)");
  QString excel = tr("EXCEL文件 (*.xls)");
  QString txt = tr("TXT文件 (*.txt)");
  QString history = "resource/history";
  QString id = m_friendsIDList[index];
  QString selectFilter = "ALL files (*.*)";
  QDir dir = QDir::current();
  if(!dir.exists(history))
    {
      dir.mkpath(history);
    }
  QString path = QString("%1/%2_%3").arg(history, m_mySelf.m_userID, id);
  QString filter = QString("%1;;%2;;%3").arg(csv, excel, txt);
  QString name = QFileDialog::getSaveFileName(this, tr("保存文件"), path,
                                              filter, &selectFilter);

  if(name.isEmpty())
    {
      return;
    }
   qDebug() << "export-path" << name << selectFilter;
  if(selectFilter == csv)
    {
      if(!name.endsWith(".csv", Qt::CaseInsensitive))
        {
          name.append(".csv");
        }
    }
  else if(selectFilter == excel)
    {
      if(!name.endsWith(".xls", Qt::CaseInsensitive))
        {
          name.append(".xls");
        }
    }
  else if(selectFilter == txt)
    {
      if(!name.endsWith(".txt", Qt::CaseInsensitive))
        {
          name.append(".txt");
        }
    }

  QString tmpPath = TMP_LOCAL_HISTROY_PATH;
  QString newPath = tmpPath;
  static int i = 0;
  while(1)
    {
      QFile file(newPath.append(".csv"));
      if(file.exists())
        {
          newPath = tmpPath.append("_").append(QString::number( ++ i));
          tmpPath = TMP_LOCAL_HISTROY_PATH;
        }
      else
        {
          break;
        }
    }
  qDebug() << "new Path:" << newPath;

  int type = m_database.exportHistoryMessageOneFriend(m_mySelf.m_userID,
                                                      id, newPath);

  QString temp;
  if(type < 0)
    {
      temp = QString(tr("聊天记录导出失败！"));
    }
  else if(type == 0)
    {
      temp = QString(tr("聊天记录为空"));
    }
  else
    {
      temp = QString(tr("聊天记录导出成功:%1.")).arg(name);
      copyFileToPath(newPath, name, true);
      emit exportHistoryMessageOneFriendSuccess(m_mySelf.m_userID, id, newPath);
    }
  QMessageBox::information(NULL, tr("导出聊天记录"), temp);
  //raise函数把窗口显示到最顶部
  m_messageManWidget->raise();
}

/*************************************************
Function Name： exportLocalMessageAll()
Description: 导出所有消息记录
*************************************************/
void IMMainWidget::exportLocalMessageAll()
{
    // 代码有点冗余
    QString csv = tr("CSV文件 (*.csv)");
    QString excel = tr("EXCEL文件 (*.xls)");
    QString txt = tr("TXT文件 (*.txt)");
    QString history = "resource/history";
    QString selFilter="All files (*.*)";
    QDir dir = QDir::current();
    if (!dir.exists(history))
        dir.mkpath(history);
    QString path = QString("%1/%2").arg(history, m_mySelf.m_userID);
    QString filter = QString(tr("%1;;%2;;%3").arg(csv, excel, txt));
    QString name = QFileDialog::getSaveFileName(this,tr("保存文件"), path,
                                                filter, &selFilter);
    if(name.isEmpty())
    {
        return;
    }
    qDebug() << "export-path" << name << selFilter;
    if (selFilter == csv)
    {
        if (!name.endsWith(".csv", Qt::CaseInsensitive))
            name.append(".csv");
    }
    else if (selFilter == excel)
    {
        if (!name.endsWith(".xls", Qt::CaseInsensitive))
            name.append(".xls");
    }
    else if (selFilter == txt)
    {
        if (!name.endsWith(".txt", Qt::CaseInsensitive))
            name.append(".txt");
    }

//    TMP_LOCAL_HISTORY_PARH.append(QTime::currentTime().toString("hh-mm-ss"));
    QString tmpPath = TMP_LOCAL_HISTROY_PATH;
    QString newPath = tmpPath;
    static int i = 0;
    while(1)
    {

        QFile file(newPath.append(".csv"));
        if (file.exists())
        {
            newPath = tmpPath.append("_").append(QString::number(++i));
            tmpPath = TMP_LOCAL_HISTROY_PATH;
        }
        else
            break;
    }

    qDebug() << "newpath:" << newPath;

    int type =  m_database.exportHistoryMessageAll(m_mySelf.m_userID, newPath);

    QString temp;
    if (type < 0)
    {
        temp = QString(tr("聊天记录导出失败！"));

    }
    else if (type == 0)
    {
        temp = QString(tr("聊天记录为空！"));
    }
    else
    {
        temp = QString(tr("聊天记录导出成功:%1.")).arg(name);
        copyFileToPath(newPath, name, true);
         emit exportHistoryMessageAllSuccess(m_mySelf.m_userID, newPath);
    }

    QMessageBox::information(NULL, tr("导出聊天记录"), temp);
    m_messageManWidget->raise();
}

/*************************************************
Function Name： deleteLocalMessage()
Description: 删除单个好友消息记录
*************************************************/
void IMMainWidget::deleteLocalMessage(const int index)
{
  QString id = m_friendsIDList[index];
  int type = m_database.deleteHistoryMessageOneFriend(m_mySelf.m_userID,id);
  QString temp;

  if(type < 0)
    {
      temp = QString(tr("删除聊天记录失败!"));
    }
  else if(type == 0)
    {
      temp = QString(tr("聊天记录为空！"));
    }
  else
    {
      temp = QString(tr("删除聊天记录成功！"));
    }
  QMessageBox::information(NULL, tr("删除聊天记录"), temp);
  m_messageManWidget->raise();
}

/*************************************************
Function Name： deleteLocalMessageAll()
Description: 删除所有消息记录
*************************************************/
void IMMainWidget::deleteLocalMessageAll()
{
    // 代码有点冗余
    int type =  m_database.deleteHistoryMessageAll(m_mySelf.m_userID);
    QString temp;
    if (type < 0)
    {
        temp = QString(tr("删除聊天记录失败！"));
    }
    else if (type == 0)
    {
        temp = QString(tr("聊天记录为空！"));
    }
    else
    {
        temp = QString(tr("删除聊天记录成功！"));
    }
    QMessageBox::information(NULL, tr("删除聊天记录"), temp);
    m_messageManWidget->raise();
    return;
}

/*************************************************
Function Name： getNetWorkHistoryMessage()
Description: 获取网络某日某好友的消息记录
*************************************************/
void IMMainWidget::getNetworkHistoryMessage(const int index, const QDate &date)
{
  if(index < 0 || index >= m_friendsIDList.size())
    {
      return;
    }
  if(NULL != m_mainCtrl)
    {
      m_mainCtrl->requestGetNetWorkHistoryMessageOneFriendOneDate(
            m_friendsIDList[index], date);
    }
}

// mark: public slots:-------------------------------------------

/*************************************************
Function Name： getFriendsInformation()
Description: 获取好友信息列表（发送至服务器）
*************************************************/
void IMMainWidget::getFriendsInformation()
{
  if(NULL != m_mainCtrl)
    {
      m_mainCtrl->getFriendsInformation(m_mySelf.m_userID, m_mySelf.m_status);
    }
}

/*************************************************
Function Name： getFlocksInformation()
Description: 获取群信息列表（发送至服务器）
*************************************************/
void IMMainWidget::getFlocksInformation()
{
  if(NULL != m_mainCtrl)
    {
      m_mainCtrl->getFlocksInformation(m_mySelf.m_userID);
    }
}

/*************************************************
Function Name： getDiscussionsInformation()
Description: 获取讨论组信息列表（发送至服务器）
*************************************************/
void IMMainWidget::getDiscussionsInformation()
{
  if(NULL != m_mainCtrl)
    {
      m_mainCtrl->getDiscussionsInformation(m_mySelf.m_userID);
    }
}

/*************************************************
Function Name： getLatestMessage()
Description: 获取本地最近消息（访问本地数据库）
*************************************************/
void IMMainWidget::getLatestMessage()
{
  if(NULL != m_mainCtrl)
    {
      m_mainCtrl->getLatestMessage(m_mySelf.m_userID);
    }
}


/*************************************************
Function Name： setNetworkMessageWidget()
Description: 设置消息界面
*************************************************/
void IMMainWidget::setNetworkMessageWidget(const QString & friendID, const QDate & date,
                                           const QVector<TalkMessage> & msgVec)
{
  m_messageManWidget->setNetworkMessageWidget(msgVec);
}

/*************************************************
Function Name： setFriendListWidget
Description: 设置好友列表
*************************************************/
void IMMainWidget::setFriendListWidget(const QVector<FriendInformation> &friendVec)
{
  int num = 0;
  int len = friendVec.size();
  for(int i = 0; i < len; ++ i)
    {
      //qDebug() << "断点1：";
      addFriendButton(friendVec[i]);
      if(OFFLINE != friendVec[i].m_status && INVISIBLE != friendVec[i].m_status)
        {
          num ++;
        }
    }
  setOnlineCounter(num);
  getFlocksInformation();
}

/*************************************************
Function Name： setFlockListWidget
Description: 设置群列表
*************************************************/
void IMMainWidget::setFlockListWidget(const QVector<FlockInformation> & flocksVec)
{
//    return;

    qDebug() << "flock-size:" << flocksVec.size();

    int len = flocksVec.size();
    for(int i=0; i<len; i++)
    {
        addFlockButton(flocksVec[i]);
    }
    // qDebug() << "断点2：";
    getDiscussionsInformation();
}

/*************************************************
Function Name： setDiscussionListWidget
Description: 设置讨论组列表
*************************************************/
void IMMainWidget::setDiscussionListWidget(const QVector<DiscussionInformation> & disVec)
{
//    return;
    qDebug() << "dis-seize:" << disVec.size();

    int len = disVec.size();
    for(int i=0; i<len; i++)
    {
        addDiscussionButton(disVec[i]);
    }
   //  qDebug() << "断点3：";
}


/*************************************************
Function Name： addFriendButton
Description: 添加好友按钮
*************************************************/
bool IMMainWidget::addFriendButton(const FriendInformation &friendInformation)
{
  if(m_friendMap.contains(friendInformation.m_userID))
    {
      return false;
    }
  QString group = friendInformation.m_groupName;
  if(!m_friendsGroupList.contains(group))
    {
      //存储 新的分组号
      m_friendsGroupList.append(group);

      //新建并存储分组
      IMToolItem *toolItem = new IMToolItem(group);

      connect(toolItem, &IMToolItem::removeBoxSignal,
              this, &IMMainWidget::removeBox);
      connect(toolItem, &IMToolItem::renameBoxSignal,
              this, &IMMainWidget::renameBox);

      m_toolItemsFriendsVec.push_back(toolItem);
      m_friendListWidget->addItem(toolItem);

      //存储映射关系
      m_indexFriendsGroupMap.insert(group, m_toolItemsFriendsVec.size() - 1);
    }

  IMFriendButton *button = new IMFriendButton(friendInformation,
                                              &m_friendsGroupList, this, this);
  m_toolItemsFriendsVec[m_indexFriendsGroupMap.value(group)]->addItem(button);
  m_friendMap.insert(friendInformation.m_userID, button);
  return true;
}


/*************************************************
Function Name： addFlockButton
Description: 添加群按钮
*************************************************/
bool IMMainWidget::addFlockButton(const FlockInformation &flockInfo)
{
  if(m_flockMap.contains(flockInfo.m_flockID))
    {
      return false;
    }
  IMFlockButton *button = new IMFlockButton(flockInfo, this, this);

  m_flockMap.insert(flockInfo.m_flockID, button);
  m_flockListWidget->addItem(button);
  qDebug() << "addFlockButton success";
  return true;
}

/*************************************************
Function Name： addDiscussionButton
Description: 添加讨论组按钮
*************************************************/

bool IMMainWidget::addDiscussionButton(const DiscussionInformation & disInfo)
{
    if (m_discussionMap.contains(disInfo.m_discussionID))
        return false;

    IMDiscussionButton *button = new IMDiscussionButton(disInfo, this, this);
//    button->setAutoRaise(true);

    m_discussionMap.insert(disInfo.m_discussionID, button);
    m_discussionListWidget->addItem(button);

    return true;
}

//--------------------------------------------------------------
/*************************************************
Function Name： receiveFriendTalkMessage
Description: 获取好友发来的消息
*************************************************/
void IMMainWidget::receiveFriendTalkMessages(const TalkMessage &msg)
{
  if(TALK_FLOCK == msg.m_type)
    {
      if(!m_flockMap.contains(msg.m_receiverID))
        {
          return;
        }
      if(0 == msg.m_senderID.compare(m_mySelf.m_userID))
        {
          return;
        }
      IMFlockChatWidget *flockRoom = NULL;
      if(m_flockRoomMap.contains(msg.m_receiverID))
        {
          flockRoom = m_flockRoomMap[msg.m_receiverID];
        }
      else
        {
          m_flockMap[msg.m_receiverID]->openChatRoom();
          flockRoom = m_flockMap[msg.m_receiverID]->getRoomPoint();
        }
      flockRoom->appendMessageShow(msg);

      if(!m_flockMap[msg.m_receiverID]->isRoomShow())
        {
          //创建 IMLinkFlockButton
          if(!m_linkMap.contains(msg.m_receiverID))
            {
              IMLinkFlockButton *button = new IMLinkFlockButton(
                    m_flockMap[msg.m_receiverID], this);
              connect(button, &IMLinkFlockButton::deleteLinkButton,
                      this, &IMMainWidget::removeLinkButton);
              m_linkMap.insert(msg.m_receiverID, button);
              m_messageListWidget->addItem(button);
              m_tabWidget->setTabText(3, QString(tr("新消息%1"))
                                      .arg(m_messageListWidget->size()));
            }
          m_linkMap[msg.m_receiverID]->setLatestMessage(msg);
        }
      return;
    }
  if(TALK_DISCUSSION == msg.m_type)
    {
      if(!m_discussionMap.contains(msg.m_receiverID))
        {
          return;
        }
      if(0 == msg.m_receiverID.compare(m_mySelf.m_userID))
        {
          return;
        }

      IMDiscussionChatWidget *discussionRoom = NULL;
      if(m_discussionRoomMap.contains(msg.m_receiverID))
        {
          discussionRoom = m_discussionRoomMap[msg.m_receiverID];
        }
      else
        {
          m_discussionMap[msg.m_receiverID]->openChatRoom();
          discussionRoom = m_discussionMap[msg.m_receiverID]->getRoomPoint();
        }
      discussionRoom->appendMessageShow(msg);

      if(!m_discussionMap[msg.m_receiverID]->isRoomShow())
        {
          //创建 IMLinkDiscussionButton
          if(!m_linkMap.contains(msg.m_receiverID))
            {
              IMLinkDiscussionButton *button = new IMLinkDiscussionButton(
                    m_discussionMap[msg.m_receiverID], this);
              connect(button, &IMLinkDiscussionButton::deleteLinkButton,
                      this, &IMMainWidget::removeLinkButton);
              m_linkMap.insert(msg.m_receiverID, button);
              m_messageListWidget->addItem(button);
              m_tabWidget->setTabText(3, QString(tr("新消息:%1"))
                                      .arg(m_messageListWidget->size()));
            }
          m_linkMap[msg.m_receiverID]->setLatestMessage(msg);
        }
      return;
    }

  IMChatWidget *chatRoom = NULL;

  if(!m_friendMap.contains(msg.m_senderID))
    {
      return;
    }
  if(m_chatRoomMap.contains(msg.m_senderID))
    {
      chatRoom = m_chatRoomMap[msg.m_senderID];
    }
  else
    {
      m_friendMap[msg.m_senderID]->openChatRoom();
      chatRoom = m_friendMap[msg.m_senderID]->getRoomPoint();
    }

  switch(msg.m_type)
    {
    case TALK_MESSAGE:
      {
        chatRoom->appendMessageShow(msg);
        //存储消息记录 至 数据库
        m_database.addHistoryMessage(msg);
        if(!m_friendMap[msg.m_senderID]->isRoomShow())
          {
            if(!m_linkMap.contains(msg.m_senderID))
              {
              //创建 IMLinkFriendButton
               IMLinkFriendButton *button = new IMLinkFriendButton(
                   m_friendMap[msg.m_senderID], this);
               connect(button, &IMLinkFriendButton::deleteLinkButton,
                       this, &IMMainWidget::removeLinkButton);
               m_linkMap.insert(msg.m_senderID, button);
               m_messageListWidget->addItem(button);
               m_tabWidget->setTabText(3, QString(tr("新消息：%1"))
                                       .arg(m_messageListWidget->size()));
              }
            m_linkMap[msg.m_senderID]->setLatestMessage(msg);
          }
        break;
      }
    case REQUEST_VIDEO:
      {
        chatRoom->pendingVideo(msg);
        break;
      }
    case AGREE_VOICE:
      {
        //
        break;
      }
    case REFUSE_VOICE:
      {
        chatRoom->closeVideoWidget();
        break;
      }
    case REQUEST_FILE:
      {
        qDebug() << "REQUEST_FILE";
        chatRoom->pendingFile(msg);
        break;
      }
    case AGREE_FILE:
      {
        break;
      }
    case REFUSE_FILE:
      {
        chatRoom->refusedSendFile();
        break;
      }
    }

}


/*************************************************
Function Name： setFriendStatus
Description: 设置好友状态
*************************************************/
void IMMainWidget::setFriendStatus(const QString &id, int status)
{
  QMap<QString, IMFriendButton *>::iterator iter;
  iter = m_friendMap.find(id);
  if(m_friendMap.end() == iter)
    {
      return;
    }
  iter.value()->getInformation().m_status = status;
  iter.value()->setButtonStatus();
}

/*************************************************
Function Name： setFriendInformation
Description: 设置好友信息
*************************************************/
void IMMainWidget::setFriendInformation(const FriendInformation &friendInformation)
{
  QMap<QString, IMFriendButton *>::iterator iter;
  iter = m_friendMap.find(friendInformation.m_userID);
  if(m_friendMap.end() == iter)
    {
      return;
    }
  iter.value()->setChangedInformation(friendInformation);
  iter.value()->setButtonStatus();
}

/*************************************************
Function Name： showFriendInformation
Description: 显示好友信息
*************************************************/
void IMMainWidget::showFriendInformation(const UserInformation &user)
{
  if(m_informationMap.contains(user.m_userID))
    {
      m_informationMap[user.m_userID]->setUserInformation(user);
      m_informationMap[user.m_userID]->raise();
      m_informationMap[user.m_userID]->activateWindow();
      m_informationMap[user.m_userID]->show();
      return;
    }
  IMInformationWidget *infWidget = new IMInformationWidget(this);
  m_informationMap.insert(user.m_userID, infWidget);
  infWidget->setReadOnly(true, false);
  infWidget->setUserInformation(user);
  infWidget->show();
}

/*************************************************
Function Name： showMyInformation
Description: 显示自己信息
*************************************************/
void IMMainWidget::showMyInformation(const UserInformation &user)
{
  m_mySelf = user;
  if(m_informationMap.contains(m_mySelf.m_userID))
    {
      m_informationMap[m_mySelf.m_userID]->raise();
      m_informationMap[m_mySelf.m_userID]->activateWindow();
      m_informationMap[m_mySelf.m_userID]->show();
      return;
    }

  IMInformationWidget *infWidget = new IMInformationWidget(this);
  m_informationMap.insert(m_mySelf.m_userID, infWidget);
  infWidget->setReadOnly(true, true);
  infWidget->setUserInformation(user);
  infWidget->show();
}

/*************************************************
Function Name： setMyInformation
Description: 设置个人信息
*************************************************/
void IMMainWidget::setMyInformation(const UserInformation &user)
{
  QString str = QString(":/resource/image/head/%1.bmp")
      .arg(QString::number(user.m_headPortrait));
  m_labelHead->setPixmap(QPixmap(str));
  m_labelNickName->setText(QString("%1\n%2")
                           .arg(user.m_nickname, user.m_userID));
}

/*************************************************
Function Name： showFlockInformation
Description: 显示群信息
*************************************************/
void IMMainWidget::showFlockInformation(const FlockInformation &flock)
{
  if(m_flockInformationMap.contains(flock.m_flockID))
    {
      m_flockInformationMap[flock.m_flockID]->raise();
      m_flockInformationMap[flock.m_flockID]->activateWindow();
      m_flockInformationMap[flock.m_flockID]->show();
      return;
  }

  IMFlockInformationWidget *infWidget = new IMFlockInformationWidget(this);
  m_flockInformationMap.insert(flock.m_flockID, infWidget);
  if (flock.m_creatorID.compare(m_mySelf.m_userID) == 0)
      infWidget->setReadOnly(true, true);
  else
      infWidget->setReadOnly(true, false);

  infWidget->setFlockInformation(flock);
  infWidget->show();

}

/*************************************************
Function Name： showDiscussionInformation
Description: 显示讨论组信息
*************************************************/
void IMMainWidget::showDiscussionInformation(const DiscussionInformation & discussion)
{
    if (m_discussionInformationMap.contains(discussion.m_discussionID))
    {
        m_discussionInformationMap[discussion.m_discussionID]->raise();
        m_discussionInformationMap[discussion.m_discussionID]->activateWindow();
        m_discussionInformationMap[discussion.m_discussionID]->show();
        return;
    }

    IMDiscussionInformationWidget *infWidget = new IMDiscussionInformationWidget(this);
    m_discussionInformationMap.insert(discussion.m_discussionID, infWidget);
//    if (discussion.m_creatorID.compare(m_myself.m_userID) == 0)
        infWidget->setReadOnly(true, true);
//    else
//        infWidget->setReadOnly(true, false);

    infWidget->setDiscussionInformation(discussion);
    infWidget->show();
}


/*************************************************
Function Name： setFlockInformation
Description: 设置群信息
*************************************************/
void IMMainWidget::setFlockInformation(const FlockInformation &flock)
{
  if(m_flockMap.contains(flock.m_flockID))
    {
      m_flockMap[flock.m_flockID]->setChangedInformation(flock);
    }
  if(m_flockRoomMap.contains(flock.m_flockID))
    {
      m_flockRoomMap[flock.m_flockID]->setChatInformation(flock);
    }
}


/*************************************************
Function Name： setDiscussionInformation
Description: 设置讨论组信息
*************************************************/
void IMMainWidget::setDiscussionInformation(const DiscussionInformation & discussion)
{
    if (m_discussionMap.contains(discussion.m_discussionID))
    {
        m_discussionMap[discussion.m_discussionID]->setChangedInformation(discussion);
    }
    if (m_discussionRoomMap.contains(discussion.m_discussionID))
    {
        m_discussionRoomMap[discussion.m_discussionID]->setChatInformation(discussion);
    }
}


/*************************************************
Function Name： setFlockMemberRemark
Description: 设置群成员名片
*************************************************/
void IMMainWidget::setFlockMemberRemark(const TempStrings &temp)
{
  if(m_flockRoomMap.contains(temp.m_one))
    {
      m_flockRoomMap[temp.m_one]->setFlockMemberRemark(temp);
    }
}


/*************************************************
Function Name： setFriendRemark
Description: 设置好友备注
*************************************************/
void IMMainWidget::setFriendRemark(const TempStrings &temp)
{
  if(temp.m_one != m_mySelf.m_userID)
    {
      return;
    }
  QMap<QString, IMFriendButton *>::iterator iter;
  iter = m_friendMap.find(temp.m_two);
  if(m_friendMap.end() == iter)
    {
      return;
    }

  iter.value()->getInformation().m_remarkName = temp.m_three;
  iter.value()->setButtonStatus();
}

/*************************************************
Function Name： deleteFriend
Description: 已删除好友
*************************************************/
void IMMainWidget::deletedFriend(const QString &myID, const QString &friendID, const QString &groupName)
{
  if(myID != m_mySelf.m_userID)
    {
      return;
    }

  if(!m_friendMap.contains(friendID))
    {
      return;
    }

  m_toolItemsFriendsVec[m_indexFriendsGroupMap.value(groupName)]->removeItem(m_friendMap[friendID]);
  m_friendMap[friendID]->deleteLater();
  m_friendMap.remove(friendID);

  if(m_chatRoomMap.contains(friendID))
    {
      m_chatRoomMap[friendID]->close();
      m_chatRoomMap[friendID]->deleteLater();
      m_chatRoomMap.remove(friendID);
    }

  qDebug() << "Friend delete";
  setOnlineCounter(m_onLineCounter - 1);
}

/*************************************************
Function Name： dropedFlock
Description: 已删除群
*************************************************/
void IMMainWidget::dropedFlock(const QString &flockID)
{
  if(!m_flockMap.contains(flockID))
    {
      return;
    }
  QMessageBox::information(NULL, tr("群消息"),
                           QString(tr("群%1已经被群主解散")).arg(flockID));

  if(m_flockRoomMap.contains(flockID))
    {
      m_flockRoomMap[flockID]->close();
      m_flockRoomMap[flockID]->deleteLater();
      m_flockRoomMap.remove(flockID);
    }

  m_flockListWidget->removeItem(m_flockMap[flockID]);
  m_flockMap[flockID]->deleteLater();
  m_flockMap.remove(flockID);

  qDebug() << "flock droped";
}


/*************************************************
Function Name： dropedDiscussion
Description: 已删除讨论组
*************************************************/
void IMMainWidget::dropedDiscussion(const QString & discussionID)

{
    if (!m_discussionMap.contains(discussionID))
        return;

    QMessageBox::information(NULL, tr("讨论组消息"),
                             QString(tr("讨论组%1已经被组长解散")).arg(discussionID));

    if (m_discussionRoomMap.contains(discussionID))
    {
        m_discussionRoomMap[discussionID]->close();
        m_discussionRoomMap[discussionID]->deleteLater();
        m_discussionRoomMap[discussionID] = NULL;
        m_discussionRoomMap.remove(discussionID);
    }


    m_discussionListWidget->removeItem( m_discussionMap[discussionID] );
    m_discussionMap[discussionID]->deleteLater();
    m_discussionMap.remove(discussionID);




    qDebug() << "discussion drop";
}

/*************************************************
Function Name： leavedFlock
Description: 已退出群
*************************************************/
void IMMainWidget::leavedFlock(const QString &memberID,
                               const QString &flockID,
                               const QString &creatorID)
{
  if(creatorID == m_mySelf.m_userID)
    {
      QMessageBox::information(NULL, tr("群消息"),
                               QString(tr("用户%1已退出群%2"))
                                       .arg(memberID, flockID));
      //如果群是打开的，最好及时刷新，将成员在群中删除
      //不然要等到下次打开群的时候，才能看到成员已被删除
      //关系也不大
      if(m_flockRoomMap.contains(flockID))
        {
          m_flockRoomMap[flockID]->removeFlockMemberButton(memberID);
        }
      if(m_flockMap.contains(flockID))
        {
          m_flockMap[flockID]->deleteFlockMember(memberID);
        }
    }
  if(memberID == m_mySelf.m_userID)
    {
      QMessageBox::information(NULL, tr("群消息"),
                               QString(tr("您好，%1，您已经退出群%2"))
                                       .arg(memberID, flockID));
      if(m_flockRoomMap.contains(flockID))
      {
        m_flockRoomMap[flockID]->close();
        m_flockRoomMap[flockID]->deleteLater();
        m_flockRoomMap.remove(flockID);
      }

      if(m_flockMap.contains(flockID))
        {
          m_flockListWidget->removeItem(m_flockMap[flockID]);
          m_flockMap[flockID]->deleteLater();
          m_flockMap.remove(flockID);
        }
      qDebug() << "Leaved flock";
    }
  else
    {
      if(m_flockRoomMap.contains(flockID))
        {
          m_flockRoomMap[flockID]->removeFlockMemberButton(memberID);
        }
      if(m_flockMap.contains(flockID))
        {
          m_flockMap[flockID]->deleteFlockMember(memberID);
        }
    }
}


/*************************************************
Function Name： leavedDiscussion
Description: 已退出讨论组
*************************************************/
void IMMainWidget::leavedDiscussion(const QString & memberID,
                                    const QString & discussionID)
{
    if (memberID == m_mySelf.m_userID)
    {
//        QMessageBox::information(NULL, tr("讨论组消息"),
//                                 QString(tr("您好,%1,您已经退出讨论组%2")).
//                                 arg(memberID, discussionID));

        if (m_discussionRoomMap.contains(discussionID))
        {
            m_discussionRoomMap[discussionID]->close();
            m_discussionRoomMap[discussionID]->deleteLater();
            m_discussionRoomMap.remove(discussionID);
        }

        if (m_discussionMap.contains(discussionID))
        {
            m_discussionListWidget->removeItem( m_discussionMap[discussionID] );
            m_discussionMap[discussionID]->deleteLater();
            m_discussionMap.remove(discussionID);
        }
        qDebug() << "leaved discussion";
    }

//         if (creatorID == m_myself.m_userID)
    else
    {
//             QMessageBox::information(NULL, tr("群消息"),
//                                      QString(tr("用户%1已经退出讨论组%2")).
//                                      arg(memberID, discussionID));

//             // 如果讨论组是打开的，最好及时刷新，将成员在讨论组中删除
//             // 不然要等到下次打开群的时候，才能看到成员已被删除
//             // 关系也不大
             if (m_discussionRoomMap.contains(discussionID))
             {
                 m_discussionRoomMap[discussionID]->removeDiscussionMemberButton(memberID);
             }
             if (m_discussionMap.contains(discussionID))
             {
                 m_discussionMap[discussionID]->deleteDiscussionMember(memberID);
             }
    }


}

/*************************************************
Function Name： createFlockSuccess
Description: 创建群成功
*************************************************/
void IMMainWidget::createFlockSuccess(const FlockInformation &flock)
{
  if(flock.m_creatorID.compare(m_mySelf.m_userID) != 0)
    {
      return;
    }
  if(m_flockMap.contains(flock.m_flockID))
    {
      return;
    }
  if(!addFlockButton(flock))
    {
      return;
    }
  QString temp = QString(tr("您好，%1(%2).\n您已经成功创建群%3(%4).\n创建日期： %5."))
      .arg(m_mySelf.m_userID, m_mySelf.m_nickname,
           flock.m_flockID, flock.m_flockName,
           flock.m_createDate.toString("yyyy-MM-dd"));
  QMessageBox::information(NULL, tr("群消息"), temp);
}


/*************************************************
Function Name： createDiscussionSuccess
Description: 创建讨论组成功
*************************************************/
void IMMainWidget::createDiscussionSuccess(const DiscussionInformation & discussion)
{
    if (discussion.m_creatorID.compare(m_mySelf.m_userID) != 0)
        return;

    if (m_discussionMap.contains(discussion.m_discussionID))

        return;
    if (!addDiscussionButton(discussion))
        return;
    QString temp =  QString(tr("您好,%1(%2).\n您已经成功创建讨论组%3(%4).\n创建日期: %5.")).
                    arg(m_mySelf.m_userID, m_mySelf.m_nickname,
                        discussion.m_discussionID, discussion.m_discussionName,
                        discussion.m_createDate.toString("yyyy-MM-dd"));

    QMessageBox::information(NULL, tr("讨论组消息"), temp);
}

/*************************************************
Function Name： searchFriend
Description: 搜索好友
*************************************************/
void IMMainWidget::searchFriend(const QString &text)
{
  m_searchListWidget->setVisible(true);
  m_tabWidget->setVisible(false);
  if(text.isEmpty())
    {
      qDebug() << "empty";
      onClickSearchFriend();
      return;
    }
  m_toolBtnSearch->setIcon(QIcon(":/resource/image/cancel.png"));
  m_toolBtnSearch->setEnabled(true);

  m_searchListWidget->clearAll();
  //搜索匹配的好友
  QMap<QString, IMFriendButton *>::iterator iter = m_friendMap.begin();
  while(iter != m_friendMap.end())
    {
      if(iter.key().contains(text)
         || iter.value()->getInformation().m_nickname.contains(text)
         || iter.value()->getInformation().m_remarkName.contains(text))
        {
          IMLinkFriendButton *button = new IMLinkFriendButton(
                m_friendMap[iter.key()], this);
          connect(button, &IMLinkFriendButton::deleteLinkButton,
                  this, &IMMainWidget::removeLinkButton);
          m_searchListWidget->addItem(button);
        }
      ++ iter;
    }
}

/*************************************************
Function Name： onClickSearchFriend
Description: 点击“搜索”按钮
*************************************************/
void IMMainWidget::onClickSearchFriend()
{
  qDebug() << "click search friend.";

  m_toolBtnSearch->setIcon(QIcon(":/resource/image/search.png"));
  m_toolBtnSearch->setEnabled(false);
  m_searchListWidget->setVisible(false);
  m_tabWidget->setVisible(true);
}


/*************************************************
Function Name： onClickAddFriend
Description: 点击添加好友按钮
*************************************************/
void IMMainWidget::onClickAddFriend()
{
  bool isOK = false;
  QString friendID = QInputDialog::getText(NULL, "添加好友",
                                          "请输入对方帐号",
                                           QLineEdit::Normal,
                                           "",
                                           &isOK);
  if(!isOK)
    {
      return;
    }
  addFriend(friendID);
}


/*************************************************
Function Name： isDigitStr
Description: 判断一个字符串是否为纯数字
*************************************************/

bool isDigitStr(QString src)
{
    QByteArray ba = src.toLatin1();//QString 转换为 char*
     const char *s = ba.data();

    while(*s && *s>='0' && *s<='9') s++;

    if (*s)
    { //不是纯数字
        return false;
    }
    else
    { //纯数字
        return true;
    }
}

/*************************************************
Function Name： onclickAddFlock
Description: 点击添加群按钮
*************************************************/
void IMMainWidget::onClickAddFlock()
{
  bool isOK = false;
  QString FlockID = QInputDialog::getText(NULL, "添加群",
                                          "请输入群帐号",
                                          QLineEdit::Normal,
                                          "",
                                          &isOK);
  //这里可以检测FlockID是否为纯数字
  if(!isDigitStr(FlockID))
    {
      QMessageBox::critical(NULL, tr("添加失败"), tr("群帐号必须为纯数字"));
      return;
    }
  if(!isOK)
    {
      return;
    }

  if(m_flockMap.contains(FlockID))
    {
      QMessageBox::critical(NULL, tr("添加失败"),tr("你已经加入该群"));
      return;
    }


  bool isOKMsg = false;
  QString checkMsg = QInputDialog::getText(NULL, "添加群",
                                           "请输入验证信息",
                                           QLineEdit::Normal,
                                           QString(tr("您好,我是：%1"))
                                           .arg(m_mySelf.m_nickname),
                                           &isOKMsg);
  if(!isOKMsg)
    {
      return;
    }

  TalkMessage msg;
  msg.m_senderID = m_mySelf.m_userID;
  msg.m_receiverID = FlockID;
  msg.m_text = checkMsg;
  msg.m_type = REQUEST_FLOCK;

  if(NULL != m_mainCtrl)
    {
      m_mainCtrl->addFlock(msg);
    }

}



/*************************************************
Function Name： onClickChangePwd
Description: 修改密码
*************************************************/
void IMMainWidget::onClickChangePwd()
{
  bool isOKMsg = false;
  QString oldPwd = QInputDialog::getText(NULL, "修改密码",
                                         "请输入原密码:",
                                         QLineEdit::Normal,
                                         NULL,
                                         &isOKMsg);
  if(!isOKMsg)
    {
      return;
    }
  while(1)
    {
      QString newPwdOne = QInputDialog::getText(NULL, "修改密码",
                                                "请输入新密码：",
                                                QLineEdit::Normal,
                                                NULL,
                                                &isOKMsg);
      if(!isOKMsg)
        {
          return;
        }
      if(newPwdOne.length() < 6)
        {
          QMessageBox::critical(NULL, tr("修改密码"),
                                tr("新密码长度不能少于六个字符!"));
          continue;
        }
      if(newPwdOne.length() > 14)
        {
          QMessageBox::critical(NULL, tr("修改密码"),
                                tr("新密码长度不能超过14个字符！"));
          continue;
        }

      QString newPwdTwo = QInputDialog::getText(NULL, "修改密码",
                                                "请再次输入新密码",
                                                QLineEdit::Normal,
                                                NULL,
                                                &isOKMsg);
      if(!isOKMsg)
        {
          return;
        }

      if(newPwdOne != newPwdTwo)
        {
          QMessageBox::critical(NULL, tr("修改密码"), tr("两次新密码不一致"));
          continue;
        }
      else
        {
          if(NULL != m_mainCtrl)
            {
              QString oldPassword = IMEncryption::getInstace()
                  .getXorEncryptDecrypt(oldPwd, 10);
              QString newPassword = IMEncryption::getInstace()
                  .getXorEncryptDecrypt(newPwdOne, 10);
              m_mainCtrl->requestChangePwd(oldPassword, newPassword);
              break;
            }
        }
    }
}


/*************************************************
Function Name： createFlock
Description: 创建群
*************************************************/
void IMMainWidget::createFlock()
{
  bool isOK = false;
  QString flockName = QInputDialog::getText(NULL, "创建群",
                                   "请输入群名",
                                   QLineEdit::Normal,
                                   QString("%1的群")
                                   .arg(m_mySelf.m_nickname),
                                   &isOK);
  if(!isOK)
    {
      return;
    }
  isOK = false;
  QString flockTheme = QInputDialog::getText(NULL, "创建群",
                                             "请输入群主题",
                                             QLineEdit::Normal,
                                             tr("没有主题"),
                                             &isOK);
  if(!isOK)
    {
      return;
    }
  TempStrings temp;
  temp.m_one = m_mySelf.m_userID;
  temp.m_two = flockName;
  temp.m_three = flockTheme;
  if(NULL != m_mainCtrl)
    {
      m_mainCtrl->createFlock(temp);
    }
}

/*************************************************
Function Name： createDiscussion
Description: 创建讨论组
*************************************************/
void IMMainWidget::createDiscussion()
{
    bool isOk = false;
    QString name = QInputDialog::getText(NULL, "创建讨论组",
                                           "请输入组名",
                                           QLineEdit::Normal,
                                           QString("%1的IM讨论组").
                                              arg(m_mySelf.m_nickname),
                                           &isOk);
    if (!isOk)
        return;

    isOk = false;
    QString theme = QInputDialog::getText(NULL, "创建讨论组",
                                           "请输入主题",
                                           QLineEdit::Normal,
                                           tr("没有主题"),
                                           &isOk);
    if (!isOk)
        return;

    TempStrings tmpStr;
    tmpStr.m_one = m_mySelf.m_userID;
    tmpStr.m_two = name;
    tmpStr.m_three = theme;

    if (NULL != m_mainCtrl)
        m_mainCtrl->createDiscussion(tmpStr);
}


/*************************************************
Function Name： showMessageWidget
Description: 显示消息管理窗口
*************************************************/
void IMMainWidget::showMessageWidget()
{
  if(m_messageManWidget == NULL)
    {
      m_messageManWidget = new IMMessageManageWidget(this);
    }
  m_messageManWidget->setListModelGroup();
  m_messageManWidget->raise();
  //激活窗体
  m_messageManWidget->activateWindow();
  m_messageManWidget->show();
}

/*************************************************
Function Name： showMailWidget
Description: 显示邮箱
*************************************************/
void IMMainWidget::showMailWidget()
{
    if (m_mailWidget == NULL)
    {
        m_mailWidget = new IMMailWidget(m_mySelf.m_userID);
    }
    m_mailWidget->raise();
    m_mailWidget->activateWindow();
    m_mailWidget->show();
}

/*************************************************
Function Name： showMailWriteWidget
Description: 显示邮箱中写信窗口
*************************************************/
void IMMainWidget::showMailWriteWidget(const QString &address)
{
  showMailWidget();
  m_mailWidget->showMailWriteWidget(address);
}


/*************************************************
Function Name： showLatestMessageWidget
Description: 显示最新消息窗口
*************************************************/
void IMMainWidget::showLatestMessageWidget()
{
  if(m_messageListWidget == NULL)
    {
      return;
    }
  m_tabWidget->setCurrentWidget(m_messageListWidget);
}

/*************************************************
Function Name： setTimerStatus
Description: 设置显示有新消息的定时器
*************************************************/
void IMMainWidget::setTimerStatus(bool isOpen)
{
  if(isOpen)
    {
      if(0 == m_timerID)
        {
          m_toolBtnNewMes->setHidden(false);
          m_timerID = startTimer(400);//400ms
          m_toolBtnNewMes->setStatusTip(tr("查阅最新消息"));
          m_toolBtnNewMes->setToolTip(QString(tr("有新消息")));
        }
    }
  else if(!isOpen && m_timerID != 0)
    {
      killTimer(m_timerID);
      m_timerID = 0;
      m_toolBtnNewMes->setToolTip(tr("无新消息"));
      m_toolBtnNewMes->setStatusTip(tr("无最新消息"));
      m_toolBtnNewMes->setHidden(true);
    }
}

/*************************************************
Function Name： renameBoxSuccess
Description: 重命名分组成功
*************************************************/
void IMMainWidget::renameBoxSuccess(const QString &title, const QString &newTitle)
{
  //先修改：组名 于 下标 映射表
  int index = m_indexFriendsGroupMap.value(title);
  m_indexFriendsGroupMap.remove(title);
  m_indexFriendsGroupMap.insert(newTitle, index);

  //通过 存储每个分组 的向量 获取分组 item， 再修改组名
  m_toolItemsFriendsVec[index]->setTitle(newTitle);

  m_friendsGroupList[m_friendsGroupList.indexOf(title)] = newTitle;

  if(m_messageManWidget != NULL)
    {
      m_messageManWidget->setListModelGroup();
    }
  refreshFriendButtonMenu();
}

/*************************************************
Function Name： moveFriendToBoxSuccess
Description: 移动好友至其他分组成功
*************************************************/
void IMMainWidget::moveFriendToBoxSuccess(const QString &friendID,
                                          const QString &title,
                                          const QString &newTitle)
{
  if(!m_friendMap.contains(friendID))
    {
      return;
    }
  m_toolItemsFriendsVec[m_indexFriendsGroupMap.value(title)]->removeItem(m_friendMap[friendID]);
  m_friendMap[friendID]->getInformation().m_groupName = newTitle;
  m_friendMap[friendID]->refreshMoveMenu();
  m_toolItemsFriendsVec[m_indexFriendsGroupMap.value(newTitle)]->addItem(m_friendMap[friendID]);
}


/*************************************************
Function Name： onClickRightButton()
Description: 右键点击按键
*************************************************/
void IMMainWidget::onClickRightButton()
{
  if(m_menu == NULL)
    {
      //创建右键菜单
      createMenu();
    }
  m_menu->exec(QCursor::pos() - QPoint(105, 125));
}

/*************************************************
Function Name： createBox
Description: 新建组
*************************************************/
void IMMainWidget::createBox()
{
  bool isOK = false;
  QString groupName = QInputDialog::getText(NULL, "新建组",
                                            "请输入分组名",
                                            QLineEdit::Normal,
                                            tr("new group"),
                                            &isOK);
  if(!isOK)
    {
      return;
    }
  if(m_indexFriendsGroupMap.contains(groupName))
    {
      QMessageBox::critical(NULL, tr("新建组"), tr("无法创建，因为已存在此分组"));
      return;
    }

  //存储新的分组号
  m_friendsGroupList.append(groupName);

  //新建并存储分组
  IMToolItem *toolItem = new IMToolItem(groupName);

  connect(toolItem, &IMToolItem::removeBoxSignal,
          this, &IMMainWidget::removeBox);
  connect(toolItem, &IMToolItem::renameBoxSignal,
          this, &IMMainWidget::renameBox);

  m_toolItemsFriendsVec.push_back(toolItem);
  m_friendListWidget->addItem(toolItem);

  //存储映射关系
  m_indexFriendsGroupMap.insert(groupName, m_toolItemsFriendsVec.size() - 1);

  if(m_messageManWidget != NULL)
    {
      m_messageManWidget->setListModelGroup();
    }
  refreshFriendButtonMenu();
}


/*************************************************
Function Name： removeLinkButton
Description: 删除新消息连接按钮
*************************************************/
void IMMainWidget::removeLinkButton(const QString &id)
{
  if(m_linkMap.contains(id))
    {
      m_messageListWidget->removeItem(m_linkMap[id]);
      m_linkMap[id]->deleteLater();
      m_linkMap.remove(id);
      m_tabWidget->setTabText(3, QString(tr("新消息(%1)"))
                              .arg(m_messageListWidget->size()));
    }
  else
    {
      //qDebug() << "there is some error in linkButton delete.";
    }
}


/*************************************************
Function Name： removeAllLinkButton
Description: 删除所有消息连接按钮
*************************************************/
void IMMainWidget::removeAllLinkButton()
{
  QMapIterator<QString, IMLinkButton*> i(m_linkMap);
  while (i.hasNext())
  {
      i.next();
      m_messageListWidget->removeItem(i.value());
      m_linkMap[i.key()]->deleteLater();
      m_linkMap.remove(i.key());
  }
  m_tabWidget->setTabText(3, QString(tr("新消息(%1)")).arg(0));
}

/*************************************************
Function Name： showAllChatRoom
Description: 显示所有聊天窗口
*************************************************/
void IMMainWidget::showAllChatRoom()
{
  QMap<QString, IMLinkButton *>::iterator iter = m_linkMap.begin();
  if(iter != m_linkMap.end())
    {
      iter.value()->onDoubleClickButton();
      iter ++;
    }
}



// mark protected:------------------------------------------------------
/*************************************************
Function Name： closeEvent
Description: 关闭事件
*************************************************/
void IMMainWidget::closeEvent(QCloseEvent *event)
{
  if(continueToClose())
    {
      //关闭主窗口之前，先关闭所有聊天窗口
      closeAllWindows();
      /*只有一种情况下，必须使用调用accept（）和ignore（），那就是closeEvent（），
       * 在closeEvent（）的事件处理函数中，必须调用accept（）和ignore（）。
       * 即如果想窗口被关闭，那么必须显示调用event->accept（）；
       * 如果不想关闭窗口，必须显示调用ignore（），否则窗口默认会关闭
       * */
      event->accept();
    }
  else
    {
      event->ignore();
    }
}


/*************************************************
Function Name： mousePressEvent
Description: 鼠标点击事件
*************************************************/
void IMMainWidget::mousePressEvent(QMouseEvent *event)
{
  if(event->button() == Qt::RightButton)
    {
      return;
    }
  QWidget::mousePressEvent(event);
}


/*************************************************
Function Name： timerEvent
Description: 定时器事件
*************************************************/
void IMMainWidget::timerEvent(QTimerEvent *event)
{
  static bool show = true;

  if(event->timerId() == m_timerID)
    {
      if(show)
        {
          m_toolBtnNewMes->setIcon(QIcon(":/resource/image/newMessage.png"));
        }
      else
        {
          m_toolBtnNewMes->setIcon(QIcon(":/resource/image/noMessage.png"));
        }
      show = !show;
    }
}

// mark: private slots:-------------------------------------------

/*************************************************
Function Name： closeWindow
Description: 关闭主窗口
*************************************************/
void IMMainWidget::closeWindow()
{
  m_flag = 1;
  closeAllWindows();
  m_mainCtrl->closeConnect();
  close();
}

/*************************************************
Function Name： closeAllWindows
Description: 关闭所有别的窗口
*************************************************/
void IMMainWidget::closeAllWindows()
{
  QMap<QString, IMChatWidget *>::iterator iterFriendChat = m_chatRoomMap.begin();
  while(iterFriendChat != m_chatRoomMap.end())
    {
      iterFriendChat.value()->deleteLater();
      ++ iterFriendChat;
    }

  QMap<QString, IMFlockChatWidget *>::iterator iterFlockChat = m_flockRoomMap.begin();
  while(iterFlockChat != m_flockRoomMap.end())
    {
      iterFlockChat.value()->deleteLater();
      ++ iterFlockChat;
    }

  QMap<QString, IMDiscussionChatWidget *>::iterator iterDiscussionChat = m_discussionRoomMap.begin();
  while(iterDiscussionChat != m_discussionRoomMap.end())
    {
      iterDiscussionChat.value()->deleteLater();
      ++ iterDiscussionChat;
    }

  QMap<QString, IMInformationWidget *>::iterator iterInformation = m_informationMap.begin();
  while(iterInformation != m_informationMap.end())
    {
      iterInformation.value()->deleteLater();
      ++ iterInformation;
    }

  QMap<QString, IMFlockInformationWidget*>::iterator iterFlockInformation;
  for(iterFlockInformation=m_flockInformationMap.begin();
      iterFlockInformation!=m_flockInformationMap.end();
      ++iterFlockInformation)
  {

      iterFlockInformation.value()->deleteLater();
  }

  QMap<QString, IMDiscussionInformationWidget*>::iterator iterDiscussionInformation;
  for(iterDiscussionInformation=m_discussionInformationMap.begin();
      iterDiscussionInformation!=m_discussionInformationMap.end();
      ++iterDiscussionInformation)
  {
      iterDiscussionInformation.value()->deleteLater();
  }

  if( m_messageManWidget != NULL)
    {
      m_messageManWidget->deleteLater();
    }
  if(m_mailWidget != NULL)
    {
      m_mailWidget->deleteLater();
    }

}

/*************************************************
Function Name： getMyInformation
Description: 取得自己的信息
*************************************************/
void IMMainWidget::getMyInformation()
{
  if(m_mainCtrl != NULL)
    {
      m_mainCtrl->getMyInformation(m_mySelf.m_userID);
    }
}

/*************************************************
Function Name： receiveMessagesFromChat
Description: 获取好友聊天室发出的消息
*************************************************/
void IMMainWidget::receiveMessagesFromChat(TalkMessage &msg)
{
  msg.m_senderID = m_mySelf.m_userID;
  if(TALK_MESSAGE == msg.m_type)
    {
      m_database.addHistoryMessage(msg);
    }
  if(NULL != m_mainCtrl)
    {
      qDebug() << "m_mainCtrl->sendTalkMessage(msg)";
      m_mainCtrl->sendTalkMessage(msg);
    }
}

/*************************************************
Function Name： changeStatus
Description: 改变个人状态（发送至服务器）
*************************************************/
void IMMainWidget::changeMyStatus(const int index)
{
  int status = ONLINE;
  switch(index)
    {
    case 0:
      {
        status = ONLINE;
        break;
      }
    case 1:
      {
        status = INVISIBLE;
        break;
      }
    case 2:
      {
        status = BUSY;
        break;
      }
    case 3:
      {
        status = LEAVE;
        break;
      }
    case 4:
      {
        status = NOTDISTURB;
        break;
      }
    default:
      {
        status = ONLINE;
        break;
      }
    }

  if(NULL != m_mainCtrl)
    {
      m_mainCtrl->changeMyStatus(m_mySelf.m_userID, status);
    }
}


/*************************************************
Function Name： receiveFriendRequest
Description: 接收并且处理好友的请求
*************************************************/
void IMMainWidget::receiveFriendRequest(const TalkMessage &msg, const UserInformation &friendInf)
{
  if(0 != msg.m_receiverID.compare(m_mySelf.m_userID))
    {
      return;
    }
  TalkMessage returnMsg;

  switch(msg.m_type)
    {
    case REQUEST_FRIEND:
      {
        QMessageBox::StandardButton rb = QMessageBox::question(
              NULL, tr("好友请求"),
              QString(tr("是否同意用户%1(%2)添加你为好友？\n验证消息:\n%3"))
              .arg(msg.m_senderID, friendInf.m_nickname, msg.m_text),
              QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        if(rb == QMessageBox::Yes)
          {
            returnMsg.m_type = AGREE_FRIEND;
          }
        else
          {
            returnMsg.m_type = REFUSE_FRIEND;
          }
        returnMsg.m_senderID = msg.m_receiverID;
        returnMsg.m_receiverID = msg.m_senderID;
        if(NULL != m_mainCtrl)
          {
            m_mainCtrl->resultOfFriendRequest(returnMsg);
          }
        break;
      }
    case AGREE_FRIEND:
      {
        FriendInformation fri;
        fri.m_headPortrait = friendInf.m_headPortrait;
        fri.m_userID = friendInf.m_userID;
        fri.m_nickname = friendInf.m_nickname;
        fri.m_status = friendInf.m_status;

        if(!addFriendButton(fri))
          {
            return;
          }
        QString temp = QString(tr("成功添加好友%1(%2)"))
            .arg(fri.m_userID, fri.m_nickname);
        QMessageBox::information(NULL, tr("消息"), temp);
        break;
      }
    case REFUSE_FRIEND:
      {
        QString temp = QString(tr("用户%1(%2)\n拒绝您的好友请求"))
            .arg(friendInf.m_userID, friendInf.m_nickname);
        QMessageBox::information(NULL, tr("消息"), temp);
        break;
      }
    case DELETE_FRIEND:
      {
        break;
      }
    }
}

/*************************************************
Function Name： receiveFlockRequest
Description: 接收并且处理群的请求
*************************************************/
void IMMainWidget::receiveFlockRequest(const TalkMessage &msg,
                                       const FlockMember &member,
                                       const FlockInformation &flock)
{
  qDebug() << "reveiveFlockRequest";
  TalkMessage returnMsg;
  switch(msg.m_type)
    {
    case REQUEST_FLOCK:
      {
        QMessageBox::StandardButton sb = QMessageBox::question(
              NULL, tr("用户请求"),
              QString(tr("是否同意用户%1加入群%2？\n验证消息：\n%3"))
              .arg(msg.m_senderID, msg.m_receiverID, msg.m_text),
              QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        if(sb == QMessageBox::Yes)
          {
            returnMsg.m_type = FLOCK_AGREE;
          }
        else
          {
            returnMsg.m_type = FLOCK_REFUSE;
          }
        returnMsg.m_senderID = msg.m_receiverID;
        returnMsg.m_receiverID = msg.m_senderID;

        if(NULL != m_mainCtrl)
          {
            qDebug() << "flock request type: " << returnMsg.m_type <<" " << FLOCK_AGREE;
            m_mainCtrl->resultOfFlockRequest(returnMsg);
          }
        break;
      }
    case AGREE_FLOCK:
      {
        qDebug() << "AGREE FLOCK";
        if(!m_flockMap.contains(msg.m_receiverID))
          {
            return;
          }
        if(!m_flockRoomMap.contains(msg.m_receiverID))
          {
            return;
          }
        if(!m_flockRoomMap[msg.m_receiverID]->addFlockMemberButton(member))
          {
            return;
          }
        m_flockMap[msg.m_receiverID]->addFlockMember(member.m_userID);

        QString temp = QString(tr("用户%1，成功加入群%2(%3)."))
            .arg(msg.m_senderID, flock.m_flockID, flock.m_flockName);
        QMessageBox::information(NULL, tr("群消息"), temp);
        break;
      }
    case REFUSE_FLOCK:
      {
        QString temp = QString(tr("您好，%1,用户%2拒绝加入群%3."))
            .arg(m_mySelf.m_userID, msg.m_senderID, msg.m_receiverID);
        QMessageBox::information(NULL, tr("群消息"), temp);
        return;
      }
    case FLOCK_AGREE:
      {
        if(m_flockMap.contains(flock.m_flockID))
          {
            return;
          }
        if(!addFlockButton(flock))
          {
            qDebug() << "addFlockButton fail";
            return;
          }
        QString temp = QString(tr("成功加入群%1(%2)."))
            .arg(flock.m_flockID, flock.m_flockName);
        QMessageBox::information(NULL, tr("群消息"), temp);
        break;
      }
    case FLOCK_REFUSE:
      {
        QString temp = QString(tr("您好，%1，群%2\n的群主拒绝您的加入请求."))
            .arg(m_mySelf.m_userID, msg.m_senderID);
        QMessageBox::information(NULL, tr("群消息"), temp);
      }
    case FLOCK_REQUEST:
      {
        QMessageBox::StandardButton sb = QMessageBox::question(
              NULL, tr("群请求"), QString(tr("是否同意加入群%1？\n验证消息：\n%2"))
              .arg(msg.m_senderID,msg.m_text),
              QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        if(sb == QMessageBox::Yes)
          {
            returnMsg.m_type = AGREE_FLOCK;
          }
        else
          {
            returnMsg.m_type = REFUSE_FLOCK;
          }
        returnMsg.m_senderID = msg.m_receiverID;
        returnMsg.m_receiverID = msg.m_senderID;
        if(NULL != m_mainCtrl)
          {
            m_mainCtrl->resultOfFlockRequest(returnMsg);
          }
        break;
      }
    case NEW_FLOCK_MEMBER:
      {
        if(!m_flockMap.contains(flock.m_flockID))
          {
            return;
          }
        if(!m_flockRoomMap.contains(flock.m_flockID))
          {
            return;
          }
        if(!m_flockRoomMap[flock.m_flockID]->addFlockMemberButton(member))
          {
            return;
          }
        m_flockMap[flock.m_flockID]->addFlockMember(member.m_userID);
        break;
      }
    default:
      {
        break;
      }
    }
}


/*************************************************
Function Name： receiveDiscussionRequest
Description: 接收并处理讨论组请求
*************************************************/
void IMMainWidget::receiveDiscussionRequest(const TalkMessage &msg,
                                            const DiscussionMember &member,
                                            const DiscussionInformation &discussion)
{
  switch(msg.m_type)
    {
    case ADD_DISCUSSION_FAIL:
      {
        QString temp = QString(tr("您好，%1，用户%2加入讨论组%3失败"))
            .arg(m_mySelf.m_userID, msg.m_receiverID, msg.m_senderID);
        QMessageBox::information(NULL, tr("讨论组消息"), temp);
        break;
      }

    case NEW_DISCUSSION_MEMBER:
      {
        if(!m_discussionMap.contains(msg.m_senderID))
          {
            return;
          }
        if(!m_discussionRoomMap.contains(msg.m_senderID))
          {
            return;
          }
        if(!m_discussionRoomMap[msg.m_senderID]->addDiscussionMemberButton(member))
          {
            return;
          }

        m_discussionMap[msg.m_senderID]->addDiscussionMember(member.m_userID);
        break;
      }

    case ADD_DISCUSSION_SUCCESS:
      {
        if(m_discussionMap.contains(msg.m_senderID))
          {
            return;
          }
        if(!addDiscussionButton(discussion))
          {
            return;
          }
        break;
      }
    default:
      {
        break;
      }
    }
}


// 删除组
/*************************************************
Function Name： removeBox
Description: 删除组
*************************************************/
void IMMainWidget::removeBox(const QString &title)
{
  QMessageBox::StandardButton returnBtn = QMessageBox::question(
        NULL, tr("删除分组"),
        QString(tr("你确定要删除分组(%1)吗？"))
        .arg(title),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

  if(returnBtn == QMessageBox::No)
    {
      return;
    }
  int index = m_indexFriendsGroupMap[title];

  if(m_toolItemsFriendsVec[index]->getSize() > 0)
    {
      QMessageBox::information(NULL, tr("删除组"), tr("无法删除，因为该组内还有组员！"));
      return;
    }

  m_toolItemsFriendsVec[index]->deleteLater();
  m_toolItemsFriendsVec.remove(index);
  m_friendsGroupList.removeAt(m_friendsGroupList.indexOf(title));
  //删除映射
  m_indexFriendsGroupMap.remove(title);
  if(m_messageManWidget != NULL)
    {
      m_messageManWidget->setListModelGroup();
    }
  refreshFriendButtonMenu();
}


/*************************************************
Function Name： renameBox
Description: 重命名组
*************************************************/
void IMMainWidget::renameBox(const QString &title)
{
  bool isOK = false;
  QString newTitle = QInputDialog::getText(NULL, "重命名分组",
                                           "请输入新的分组名",
                                           QLineEdit::Normal,
                                           title,
                                           &isOK);
  if(!isOK)
    {
      return;
    }

  if(m_indexFriendsGroupMap.contains(newTitle))
    {
      QMessageBox::information(NULL, tr("重命名分组"), tr("失败，因为存在同名的组"));
      return;
    }
  int index = m_indexFriendsGroupMap.value(title);
  if(m_toolItemsFriendsVec[index]->getSize() <= 0)
    {
      renameBoxSuccess(title, newTitle);
      refreshFriendButtonMenu();
      return;
    }

  if(0 != title.compare(newTitle))
    {
      m_mainCtrl->renameBox(title, newTitle);
    }
}


// mark: private:--------------------------------------------------
/*************************************************
Function Name： initIMMainWidget
Description: 初始化
*************************************************/
void IMMainWidget::initIMMainWidget()
{
  m_mainCtrl = new IMMainCtrl(m_mySelf.m_userID);

  m_labelHead = new IMClickLabel(this);
  qDebug() << "m_headPortrait: " << m_mySelf.m_headPortrait;
  m_labelHead->setFixedSize(HEAD_BIG_SIZE,HEAD_BIG_SIZE);
  QString str = QString(tr(":/resource/image/head/%1.bmp"))
      .arg(QString::number(m_mySelf.m_headPortrait));
  m_labelHead->setPixmap(QPixmap(str));

  m_cbStatus = new QComboBox(this);
  m_cbStatus->addItem("在线");
  m_cbStatus->addItem("隐身");
  m_cbStatus->addItem("忙碌");
  m_cbStatus->addItem("离开");
  m_cbStatus->addItem("请勿打扰");
  m_cbStatus->setCurrentIndex(m_mySelf.m_status);

  m_labelNickName = new QLabel(QString("%1\n%2")
                               .arg(m_mySelf.m_nickname, m_mySelf.m_userID));

  m_labelMark = new QLabel(tr("我的签名")); //说说 签名


  m_toolBtnSpace = new QToolButton(this); //空间
  m_toolBtnSpace->setIcon(QIcon(":/resource/image/space.png"));
  m_toolBtnSpace->setAutoRaise(true);//设置为自动浮起
  m_toolBtnSpace->setToolTip(tr("空间"));
  m_toolBtnSpace->setStatusTip(tr("打开空间"));

  m_toolBtnMail = new QToolButton(this); //邮箱
  m_toolBtnMail->setIcon(QIcon(":/resource/image/mail.png"));
  m_toolBtnMail->setAutoRaise(true);
  m_toolBtnMail->setToolTip(tr("邮箱"));
  m_toolBtnMail->setStatusTip(tr("打开邮箱"));

  m_toolBtnMesBox = new QToolButton(this); // 消息盒子
  m_toolBtnMesBox->setIcon(QIcon(":/resource/image/messageBox.png"));
  m_toolBtnMesBox->setAutoRaise(true);
  m_toolBtnMesBox->setToolTip(tr("消息管理"));
  m_toolBtnMesBox->setStatusTip(tr("打开消息管理器"));

  m_toolBtnNewMes = new QToolButton(this);
  m_toolBtnNewMes->setIcon(QIcon(":/resource/image/noMessage.png"));//:/resource/image/noMessage.png"));
  m_toolBtnNewMes->setAutoRaise(true);
  m_toolBtnNewMes->setToolTip(tr("无新消息"));
  m_toolBtnNewMes->setStatusTip(tr("无最新消息"));
  m_toolBtnNewMes->setHidden(true);

  QLabel *searchLabel = new QLabel(this);
  searchLabel->setStyleSheet("border: 1px;\
                             border-radius:3px;\
                             border-style: solid;\
                             border-color: gray;\
                             background: white;");
  searchLabel->setFixedHeight(25);

  m_leSearch = new QLineEdit();
  m_leSearch->setStyleSheet("border: 0px;");
  m_leSearch->setPlaceholderText(tr("Search friends"));

  m_toolBtnSearch = new QToolButton();
  m_toolBtnSearch->setIcon(QIcon(":/resource/image/search.png"));
  m_toolBtnSearch->setStyleSheet("border: 0px;");
  m_toolBtnSearch->setEnabled(false);

  QHBoxLayout *layoutTopSearch = new QHBoxLayout;
  layoutTopSearch->setContentsMargins(0, 0, 0, 0);
  layoutTopSearch->addWidget(m_leSearch);
  layoutTopSearch->addWidget(m_toolBtnSearch);
  searchLabel->setLayout(layoutTopSearch);

  m_searchListWidget = new IMSearchListWidget;
  m_searchListWidget->setVisible(false);

  m_tabWidget = new QTabWidget;
  m_friendListWidget = new IMFriendListWidget(this, this);
  m_flockListWidget = new IMFlockListWidget(this, this);
  m_discussionListWidget = new IMDiscussionListWidget(this, this); 
  m_messageListWidget = new IMLatestMessageListWidget(this, this);
  m_messageListWidget = new IMLatestMessageListWidget(this, this);
  m_tabWidget->addTab(m_friendListWidget, tr("联系人"));
  m_tabWidget->addTab(m_flockListWidget,tr("群"));
  m_tabWidget->addTab(m_discussionListWidget, tr("讨论组"));
  m_tabWidget->addTab(m_messageListWidget, tr("新消息(0)"));

  m_labelOnlineNum = new QLabel("在线人数： 0");
  m_labelOnlineNum->setHidden(true);

  m_toolBtnAddFlock = new QToolButton;
  m_toolBtnAddFlock->setIcon(QIcon(":/resource/image/addFlock.png"));
  m_toolBtnAddFlock->setAutoRaise(true);
  m_toolBtnAddFlock->setToolTip(tr("加群"));
  m_toolBtnAddFlock->setStatusTip(tr("添加群"));

  m_toolBtnConfig = new QToolButton;
  m_toolBtnConfig->setIcon(QIcon(":/resource/image/config.png"));
  m_toolBtnConfig->setAutoRaise(true);
  m_toolBtnConfig->setToolTip(tr("系统管理"));
  m_toolBtnConfig->setStatusTip(tr("打开系统管理"));

  m_toolBtnAddFriend = new QToolButton;
  m_toolBtnAddFriend->setIcon(QIcon(":/resource/image/addFriend.png"));
  m_toolBtnAddFriend->setAutoRaise(true);
  m_toolBtnAddFriend->setToolTip(tr("加好友"));
  m_toolBtnAddFriend->setStatusTip(tr("添加好友"));

  /****************头部*********/
  QVBoxLayout *layoutTopRight = new QVBoxLayout;
  layoutTopRight->addWidget(m_labelNickName);

  QHBoxLayout *layoutTop = new QHBoxLayout;
  layoutTop->addWidget(m_labelHead);
  layoutTop->addLayout(layoutTopRight);

  QHBoxLayout *layoutTopTool = new QHBoxLayout;
  layoutTopTool->addWidget(m_cbStatus);
  layoutTopTool->addWidget(m_toolBtnSpace);
  layoutTopTool->addWidget(m_toolBtnMail);
  layoutTopTool->addWidget(m_toolBtnMesBox);
  layoutTopTool->addWidget(m_toolBtnNewMes);
  //平均分配Layout
  layoutTopTool->addStretch();

  QVBoxLayout *layoutHead = new QVBoxLayout;
  layoutHead->addLayout(layoutTop);
  layoutHead->addLayout(layoutTopTool);
  layoutHead->addWidget(searchLabel);

  /***********ennd 头部************/

  QHBoxLayout *layoutBtm = new QHBoxLayout;
  layoutBtm->addWidget(m_labelOnlineNum);
  layoutBtm->addStretch();
  layoutBtm->addWidget(m_toolBtnAddFlock);
  layoutBtm->addWidget(m_toolBtnAddFriend);
  layoutBtm->addWidget(m_toolBtnConfig);

  QVBoxLayout *layout = new QVBoxLayout;
  layout->addLayout(layoutHead);
  layout->addWidget(m_tabWidget);
  layout->addWidget(m_searchListWidget);
  layout->addLayout(layoutBtm);

  setLayout(layout);
  setMinimumSize(278, 300);
  resize(278,500);
}

/*************************************************
Function Name： linkSignalWithSlot
Description:  连接信号与槽
*************************************************/
void IMMainWidget::linkSignalWithSlot()
{
  connect(m_mainCtrl, &IMMainCtrl::getFriendSuccess,
          this, &IMMainWidget::setFriendListWidget);
  connect(m_mainCtrl, &IMMainCtrl::getFlockSuccess,
          this, &IMMainWidget::setFlockListWidget);
  connect(m_mainCtrl, &IMMainCtrl::getDiscussionSuccess,
          this, &IMMainWidget::setDiscussionListWidget);

  connect(m_mainCtrl, &IMMainCtrl::getFriendTalkMessage,
          this, &IMMainWidget::receiveFriendTalkMessages);

  connect(m_mainCtrl, &IMMainCtrl::getFriendChangeStatus,
          this, &IMMainWidget::setFriendStatus);
  connect(m_mainCtrl, &IMMainCtrl::getFriendChangeInformation,
          this, &IMMainWidget::setFriendInformation);
  connect(m_mainCtrl, &IMMainCtrl::getFriendInformationSuccess,
          this, &IMMainWidget::showFriendInformation);

  connect(m_labelHead, &IMClickLabel::clicked,
          this, &IMMainWidget::getMyInformation);

  connect(m_mainCtrl, &IMMainCtrl::getMyInformationSuccess,
          this, &IMMainWidget::showMyInformation);

  connect(m_mainCtrl, &IMMainCtrl::changeMyInformationSuccess,
          this, &IMMainWidget::setMyInformation);


  connect(m_cbStatus, SIGNAL(currentIndexChanged(int)),
          this, SLOT(changeMyStatus(int)));
  connect(m_mainCtrl, &IMMainCtrl::changeFriendRemarkSuccess,
          this, &IMMainWidget::setFriendRemark);
  connect(m_mainCtrl, &IMMainCtrl::deleteFriendSuccess,
          this, &IMMainWidget::deletedFriend);

  connect(m_mainCtrl, &IMMainCtrl::getFriendRequest,
          this, &IMMainWidget::receiveFriendRequest);

  connect(m_mainCtrl, &IMMainCtrl::getFlockRequest,
          this, &IMMainWidget::receiveFlockRequest);
  connect(m_mainCtrl, &IMMainCtrl::dropFlockSuccess,
          this, &IMMainWidget::dropedFlock);
  connect(m_mainCtrl, &IMMainCtrl::leaveFlockSuccess,
          this, &IMMainWidget::leavedFlock);
  connect(m_mainCtrl, &IMMainCtrl::haveMemberLeaveFlock,
          this, &IMMainWidget::leavedFlock);
  connect(m_mainCtrl, &IMMainCtrl::changeFlockSuccess,
          this, &IMMainWidget::setFlockInformation);
  connect(m_mainCtrl, &IMMainCtrl::changeFlockRemarkSuccess,
          this, &IMMainWidget::setFlockMemberRemark);
  connect(m_mainCtrl, &IMMainCtrl::createFlockSuccess,
          this, &IMMainWidget::createFlockSuccess);


  connect(m_mainCtrl, SIGNAL(getDiscussionRequest(TalkMessage,DiscussionMember,DiscussionInformation)),
          this, SLOT(receiveDiscussionRequest(TalkMessage,DiscussionMember,DiscussionInformation)));
  connect(m_mainCtrl, SIGNAL(dropDiscussionSuccess(QString)),
          this, SLOT(dropedDiscussion(QString)));
  connect(m_mainCtrl, SIGNAL(leaveDiscussionSuccess(QString,QString)),
          this, SLOT(leavedDiscussion(QString,QString)));
  connect(m_mainCtrl, SIGNAL(changeDiscussionSuccess(DiscussionInformation)),
          this, SLOT(setDiscussionInformation(DiscussionInformation)));
  connect(m_mainCtrl, SIGNAL(createDiscussionSuccess(DiscussionInformation)),
          this, SLOT(createDiscussionSuccess(DiscussionInformation)));

  connect(m_mainCtrl, &IMMainCtrl::closeWindowSignal,
          this, &IMMainWidget::closeWindow);

  connect(m_leSearch, &QLineEdit::textChanged,
          this, &IMMainWidget::searchFriend);
  connect(m_toolBtnSearch, &QToolButton::clicked,
          this, &IMMainWidget::onClickSearchFriend);

  connect(m_toolBtnAddFriend, &QToolButton::clicked,
          this, &IMMainWidget::onClickAddFriend);
  connect(m_toolBtnAddFlock, &QToolButton::clicked,
          this, &IMMainWidget::onClickAddFlock);
  connect(m_toolBtnMail, &QToolButton::clicked,
          this, &IMMainWidget::showMailWidget);
  connect(m_toolBtnMesBox, &QToolButton::clicked,
          this, &IMMainWidget::showMessageWidget);
  connect(m_toolBtnNewMes, &QToolButton::clicked,
          this, &IMMainWidget::showLatestMessageWidget);

  connect(m_mainCtrl, &IMMainCtrl::renameBoxSuccess,
          this, &IMMainWidget::renameBoxSuccess);
  connect(m_mainCtrl, &IMMainCtrl::moveFriendToBoxSuccess,
          this, &IMMainWidget::moveFriendToBoxSuccess);

  connect(m_mainCtrl, &IMMainCtrl::getNetworkHistoryMessageSuccess,
          this, &IMMainWidget::setNetworkMessageWidget);

  connect(m_messageListWidget, &IMLatestMessageListWidget::timerStatus,
          this, &IMMainWidget::setTimerStatus);

}

/*************************************************
Function Name： clearContainers
Description:  清理容器
*************************************************/
void IMMainWidget::clearContainers()
{
  m_friendsGroupList.clear();

  m_indexFriendsGroupMap.clear();

  qDeleteAll(m_friendMap.begin(), m_friendMap.end());
  m_friendMap.clear();

  qDeleteAll(m_toolItemsFriendsVec.begin(), m_toolItemsFriendsVec.end());
  m_toolItemsFriendsVec.clear();

  qDeleteAll(m_flockMap.begin(), m_flockMap.end());
  m_flockMap.clear();

  qDeleteAll(m_discussionMap.begin(), m_discussionMap.end());
  m_discussionMap.clear();

  qDeleteAll(m_linkMap.begin(), m_linkMap.end());
  m_linkMap.clear();
}

/*************************************************
Function Name： creatMenu
Description: 创建右键菜单
*************************************************/
void IMMainWidget::createMenu()
{
  m_menu = new QMenu(this);
  QAction *addFriend = new QAction(tr("添加好友"), m_menu);
  QAction *addFlock = new QAction(tr("添加群"), m_menu);
  QAction *changePwd = new QAction(tr("修改密码"), m_menu);
  QAction *information = new QAction(tr("个人信息"), m_menu);
  QAction *manMes = new QAction(tr("消息管理"), m_menu);
  QAction *mail = new QAction(tr("邮件管理"), m_menu);

  connect(addFriend, SIGNAL(triggered()),
          this, SLOT(onClickAddFriend()));
  connect(addFlock, SIGNAL(triggered()),
          this, SLOT(onClickAddFlock()));
  connect(changePwd, SIGNAL(triggered()),
          this, SLOT(onClickChangePwd()));
  connect(information, SIGNAL(triggered()),
          this, SLOT(getMyInformation()));
  connect(manMes, SIGNAL(triggered()),
          this, SLOT(showMessageWidget()));
  connect(mail, SIGNAL(triggered()),
          this, SLOT(showMailWidget()));


  m_menu->addAction(addFlock);
  m_menu->addAction(addFriend);
  m_menu->addAction(changePwd);
  m_menu->addAction(information);
  m_menu->addAction(manMes);
  m_menu->addAction(mail);

}



/*************************************************
Function Name： refreshFriendButtonMenu
Description: 刷新好友按钮右键菜单
*************************************************/
void IMMainWidget::refreshFriendButtonMenu()
{
  QMap<QString, IMFriendButton *>::iterator iter = m_friendMap.begin();
  while(iter != m_friendMap.end())
    {
      iter.value()->refreshMoveMenu();
      ++ iter;
    }
}



/*************************************************
Function Name： continueToClose
Description: 是否继续关闭
*************************************************/
bool IMMainWidget::continueToClose()
{
  if(m_flag == 1)
    {
      return true;
    }
  //如果还有聊天窗口没有关闭，就弹出信息窗口
  if(m_chatRoomMap.size() != 0 ||
     m_flockRoomMap.size() != 0 ||
     m_discussionRoomMap.size() != 0)
    {
      QString temp = QString(tr("还有聊天窗口没有关闭，您确定想要退出程序?"));
      QMessageBox::StandardButton sb = QMessageBox::question(
            this, tr("退出程序"), temp,
            QMessageBox::Yes | QMessageBox::No,
            QMessageBox::Yes);

      if(sb == QMessageBox::Yes)
        {
          return true;
        }
      else
        {
          return false;
        }

    }
  return true;
}
