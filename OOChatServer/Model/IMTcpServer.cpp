/*********************************************
File Name： IMTcpServer.cpp
Author： hanyan
Date： 2016.10.17
Description： 服务器类
Changes：
********************************************/

#include "IMTcpServer.h"
//#include "../Model/IMTread.h"

IMTcpServer::IMTcpServer(QObject *parent)
  : QTcpServer(parent)
{
  //初始化一下数据库
}

IMTcpServer::~IMTcpServer()
{

}
// mark: public slots:------------------------------------------
/*************************************************
Function Name： clientDisconnected()
Description: 客户端断开连接
*************************************************/
void IMTcpServer::clientDisconnected(const QString &id)
{
  if(id.contains(MAIL_ADDRESS_FORMAT))
    {
      m_userMailMap.remove(id);
      emit haveLeftAddress(id);
    }
  else
    {
      m_userMap.remove(id);
      emit haveLeftID(id);
      if(m_database.updateStatus(OFFLINE, id))
        {
          tellFriendsStatusChanged(id, OFFLINE);
        }
    }
}
/*************************************************
Function Name： sendMessage()
Description: 处理请求，要求clientSocket发送信息
*************************************************/
void IMTcpServer::sendMessage(const SaveTmpInformation &save)
{
  m_save.m_requestKind = save.m_requestKind;
  //根据save.request来处理各种信息
  switch (m_save.m_requestKind) {
    case LOGIN:
      {
        m_save.m_loginInf = save.m_loginInf;
        m_save.m_clientSocket = save.m_clientSocket;
        //判断是否已登录
        m_save.m_myID = save.m_myID;
        if(m_userMap.contains(m_save.m_myID))
          {
            m_save.m_replyKind = HAVE_LOGINED;
          }
        else
          {
            //访问数据库 登录
            m_save.m_replyKind = m_database.searchUserInUserInformation(
                  m_save.m_loginInf, m_save.m_userInf);
           // qDebug() << m_save.m_replyKind << " " << LOGIN_SUCCESS;
          }
        //qDebug() << "IMTcpServer::sendMessage-login";
        save.m_clientSocket->sendMessage(m_save);
        break;
      }

     case REGISTER:
      {
        m_save.m_userInf = save.m_userInf;

        int sum = m_database.getUserTimes(QDate::currentDate());
        if(sum == 0)
          {
            m_save.m_replyKind = REGISTER_FAIL;
          }
        else
          {
            QString id = QDate::currentDate().toString("yyyyMMdd");
            id.append(QString::number(0));
            id.append(QString::number(sum));
            m_save.m_userInf.m_userID = id;
            m_save.m_userInf.m_regDateTime = QDateTime::currentDateTime();
           //访问数据库，注册
            m_save.m_replyKind = m_database.addUserInUserInformation(m_save.m_userInf);
            qDebug() << "register ";
          }
        // qDebug() << "register ";
        save.m_clientSocket->sendMessage(m_save);
        break;
      }
    case GET_QUESTION_ANSWER:
      {
        m_save.m_myID = save.m_myID;
        m_save.m_replyKind = m_database.searchQuestionAndAnswer(m_save.m_myID,
                                                                m_save.m_tempStr);
        save.m_clientSocket->sendMessage(m_save);
        break;
      }
    case CHECK_MESSAGE:
      break;
    case GET_ALL_FRIENDS:
      {
        m_save.m_myID = save.m_myID;
        m_save.m_status = save.m_status;
        m_save.m_clientSocket = save.m_clientSocket;
        //访问数据库 查询所有好友信息
        m_save.m_replyKind = m_database.searchFriendsInformation(m_save.m_myID,
                                                                 m_save.m_friendsVec);
        // qDebug() << "断点：" << GET_ALL_FRIENDS_SUCCESS << " " << m_save.m_replyKind;
        save.m_clientSocket->sendMessage(m_save);
        if(GET_ALL_FRIENDS_FAIL != m_save.m_replyKind)
          {
            if(!m_userMap.contains(m_save.m_myID))
              {
                m_userMap.insert(m_save.m_myID, m_save.m_clientSocket);
                emit haveNewID(m_save.m_myID);
              }

            tellFriendsStatusChanged(m_save.m_myID, m_save.m_status);
          }
        break;
      }
    case GET_ALL_FLOCKS:
      {
        m_save.m_myID = save.m_myID;
        //查询 群信息
        m_save.m_replyKind = m_database.searchFlocksInformation(
              m_save.m_myID, m_save.m_flocksVec);
       // qDebug() << "断点：" << m_save.m_replyKind << " " << GET_ALL_FLOCKS_SUCCESS;
        save.m_clientSocket->sendMessage(m_save);
        break;
      }
    case GET_ALL_DISCUSSIONS:
      {
        m_save.m_myID = save.m_myID;
        //获取 讨论组信息
        m_save.m_replyKind = m_database.searchDiscussionsInformation(
              m_save.m_myID, m_save.m_discussionsVec);
        // qDebug() << "断点：" << m_save.m_replyKind << " " << GET_ALL_DISCUSSIONS_SUCCESS;
        save.m_clientSocket->sendMessage(m_save);
        break;
      }
    case GET_FLOCK_MEMBERS:
      {
        m_save.m_flockID = save.m_flockID;
        //获取群成员
        m_save.m_replyKind = m_database.searchFlockMembers(
              m_save.m_flockID, m_save.m_flockMembersVec);
        save.m_clientSocket->sendMessage(m_save);
        break;
      }
    case GET_DISCUSSIONS_MEMBERS:
      {
        m_save.m_discussionID = save.m_discussionID;
        //获取群成员
        m_save.m_replyKind = m_database.searchDiscussionMembers(
              m_save.m_discussionID, m_save.m_discussionMembersVec);
        save.m_clientSocket->sendMessage(m_save);
        break;
      }
    case TALK:
      {
        talkRequest(save);
        break;
      }
    case CHANGE_INFORMATION:
      {
        m_save.m_userInf = save.m_userInf;
        m_save.m_replyKind = m_database.updateMyselfInformation(m_save.m_userInf);
        save.m_clientSocket->sendMessage(m_save);

        if(m_save.m_replyKind == CHANGE_INFORMATION_SUCCESS)
          {
            tellFriendsInformationChanged(m_save.m_userInf.m_userID);
          }
        break;
      }
    case GET_MYSELF_INFORMATION:
      {
        m_save.m_myID = save.m_myID;
        m_save.m_replyKind = m_database.searchMyselfInformationByID(
              m_save.m_myID,
              m_save.m_userInf);
        save.m_clientSocket->sendMessage(m_save);
        break;
      }
    case GET_FRIEND_INFORMATION:
      {
        m_save.m_peerID = save.m_peerID;
        m_save.m_replyKind = m_database.searchFriendInformationByID(
              m_save.m_peerID,
              m_save.m_userInf);
        save.m_clientSocket->sendMessage(m_save);
        break;
      }
    case DELETE_FRIEND:
      {
        m_save.m_myID = save.m_myID;
        m_save.m_peerID = save.m_peerID;
        m_save.m_replyKind = m_database.deleteFriend(
              m_save.m_myID,
              m_save.m_peerID,
              m_save.m_groupName,
              m_save.m_groupNameF
              );
        m_save.m_clientSocket->sendMessage(m_save);

        //如果好友在线，就发送被删除信息，如果不在线，就先存储
        QMap<QString, IMClientSocketCtrl *>::iterator iter;
        iter = m_userMap.find(m_save.m_peerID);
        if((iter == m_userMap.end()))
        {
            TalkMessage msg;
            msg.m_type = BE_DELETED;
            msg.m_senderID = save.m_myID;
            msg.m_receiverID = save.m_peerID;
            msg.m_text = save.m_groupNameF;
            //存储离线消息
            //m_database.messageRequest(msg);
         }
        else
          {
            m_save.m_myID = save.m_peerID;
            m_save.m_peerID = save.m_myID;
            m_save.m_replyKind = BE_DELETED;
            m_save.m_groupName = m_save.m_groupNameF;
            iter.value()->sendMessage(m_save);
          }
        break;

      }
    case CHANGE_REMARK:
      {
        m_save.m_tempStr = save.m_tempStr;
        m_save.m_replyKind = m_database.updateRemark(save.m_tempStr);
        save.m_clientSocket->sendMessage(m_save);
        break;
      }
    case ADD_FRIEND:
      {
        friendRequest(save);
      }
    case CHANGE_STATUE:
      {
        if(m_database.updateStatus(save.m_status, save.m_myID))
          {
            tellFriendsStatusChanged(save.m_myID, save.m_status);
          }
        break;
      }
    case ADD_FLOCK:
    {
        flockRequest(save);
        break;
    }
    case DROP_FLOCK:
      {
        m_save.m_myID = save.m_myID;
        m_save.m_flockID = save.m_flockID;

        tellMembersFlockDroped(m_save.m_flockID);

        m_database.deleteFlock(m_save.m_myID, m_save.m_flockID);
        break;
      }
    case LEAVE_FLOCK:
      {
        m_save.m_myID = save.m_myID;
        m_save.m_flockID = save.m_flockID;

        m_save.m_replyKind = m_database.deleteFlockMember(
              m_save.m_flockID, m_save.m_myID );
        //如果删除成功，通知群主，群成员退出群
        if(LEAVE_FLOCK_SUCCESS == m_save.m_replyKind)
          {
            //通过群号flockID获取到 群主号peerID
            if(m_database.searchFlockHost(m_save.m_flockID, m_save.m_peerID))
              {
                //如果群主在线，就发送群成员退出群消息，如果不在线，就先存储
                QMap<QString, IMClientSocketCtrl *>::iterator iter;
                iter = m_userMap.find(m_save.m_peerID);
                if(iter == m_userMap.end())
                  {
                      TalkMessage msg;
                      msg.m_type = LEAVE_FLOCK_SUCCESS;
                      msg.m_senderID = save.m_myID;
                      msg.m_receiverID = save.m_peerID;
                      //存储离线消息
                      //m_database.messageRequest(msg);
                  }
                else
                  {
                      iter.value()->sendMessage(m_save);
                  }

                //如果群成员在线，就发送群成员退出群消息，如果不在线，就先存储
                iter = m_userMap.find(m_save.m_myID);
                if(iter == m_userMap.end())
                  {
                    TalkMessage msg;
                    msg.m_type = LEAVE_FLOCK_SUCCESS;
                    msg.m_senderID = save.m_myID;
                    msg.m_receiverID = save.m_peerID;
                    //存储离线消息
                    //m_database.messageRequest(msg);
                  }
                else
                  {
                      iter.value()->sendMessage(m_save);

                  }
            }
            tellFlockMembersHaveLeaveMember(m_save.m_flockID);
          }
        break;
      }
      //修改群信息
    case CHANGE_FLOCK:
      {
        m_save.m_flockInf = save.m_flockInf;
        m_save.m_replyKind = m_database.updateFlockInformation(m_save.m_flockInf);
        if(CHANGE_FLOCK_SUCCESS == m_save.m_replyKind)
          {
            qDebug() << "CHANGE_FLOCK_SUCCESS";
            tellMembersFlockChanged(m_save.m_flockInf);
          }
        qDebug() << "CHANGE_FLOCK_Fail";
        break;
      }
    case CHANGE_FLOCK_REMARK:
      {
        m_save.m_tempStr = save.m_tempStr;
        m_save.m_replyKind = m_database.updateFlockMemberRemark(m_save.m_tempStr);
        save.m_clientSocket->sendMessage(m_save);
        break;
      }
    case CREATE_FLOCK:
      {
        m_save.m_tempStr = save.m_tempStr;
        QString flockID;
        m_save.m_replyKind = m_database.addFlock(m_save.m_tempStr, flockID);
        if(CREATE_FLOCK_SUCCESS == m_save.m_replyKind)
          {
            m_database.searchFlockInformation(flockID, m_save.m_flockInf);
          }
        save.m_clientSocket->sendMessage(m_save);
        break;
      }
    case ADD_DISCUSSION:
      {
        discussionRequest(save);
        break;
      }
    case DROP_DISCUSSION:
      {
        m_save.m_myID = save.m_myID;
        m_save.m_discussionID = save.m_discussionID;

        tellMembersDiscussionDrop(m_save.m_discussionID);

        m_database.deleteDiscussion(m_save.m_myID, m_save.m_discussionID);
        break;
      }
    case LEAVE_DISCUSSION:
      {
        m_save.m_myID = save.m_myID;
        m_save.m_discussionID = save.m_discussionID;

        m_save.m_replyKind = m_database.deleteDiscussionMember(m_save.m_discussionID,
                                                               m_save.m_myID);

        //如果删除成功
        if(LEAVE_DISCUSSION_SUCCESS == m_save.m_replyKind)
          {
            {
#if 0
//            // 通过组号flockID 获取到 组长号peerID
//            if (m_database.searchFlockHost(m_save.m_discussionID, m_save.m_peerID))
//            {
////                save.m_clientSocket->sendMessage(m_save);

//                // 如果组长在线，就发送成员退出信息， 如果不在线，就先存储
//                QMap<QString, IMClientSocketCtrl*>::iterator iter;
//                iter = m_userMap.find(m_save.m_peerID);
//                if(m_userMap.end() == iter)
//                {
//                    TalkMessage mes;
//                    mes.m_type = LEAVE_DISCUSSION_SUCCESS;
//                    mes.m_senderID = save.m_myID;
//                    mes.m_receiverID = save.m_peerID;
////                    mes.m_text = save.m_groupNameF;
//                    // 存储离线消息
//                    //m_database.messageRequest(save.message);
//                }
//                else
//                {
////                    TalkMessage mes;
////                    mes.m_type = LEAVE_FLOCK_SUCCESS;
////                    mes.m_senderID = save.m_myID;
////                    mes.m_receiverID = save.m_myID;
//                    iter.value()->sendMessage(m_save);
//                }

//                // 如果成员在线，就发送成员退出群信息， 如果不在线，就先存储
////                QMap<QString, IMClientSocketCtrl*>::iterator iter;
////                iter = m_userMap.find(m_save.m_myID);
////                if(m_userMap.end() == iter)
////                {
////                    TalkMessage mes;
////                    mes.m_type = LEAVE_DISCUSSION_SUCCESS;
////                    mes.m_senderID = save.m_myID;
////                    mes.m_receiverID = save.m_peerID;
//////                    mes.m_text = save.m_groupNameF;
////                    // 存储离线消息
////                    //m_database.messageRequest(save.message);
////                }
////                else
//                {
////                    TalkMessage mes;
////                    mes.m_type = LEAVE_FLOCK_SUCCESS;
////                    mes.m_senderID = save.m_myID;
////                    mes.m_receiverID = save.m_myID;
//                    iter.value()->sendMessage(m_save);
//                }
//            }
            #endif
          }
        m_save.m_replyKind = LEAVE_DISCUSSION_MEMBER;

        //通知讨论组成员， 有成员退出
        tellDiscussionMembersHaveLeaveMember(m_save.m_discussionID);
        }
        save.m_clientSocket->sendMessage(m_save);
        break;
      }
    case CHANGE_DISCUSSION:
      {
        m_save.m_discussionInf = save.m_discussionInf;
        m_save.m_replyKind = m_database.updateDiscussionInformation(m_save.m_discussionInf);
        if(CHANGE_DISCUSSION_SUCCESS == m_save.m_replyKind)
          {
            tellMembersDiscussionChanged(m_save.m_discussionInf);
          }
        break;
      }
    case CREATE_DISCUSSION:
      {
        m_save.m_tempStr = save.m_tempStr;
        QString discussionID;
        m_save.m_replyKind = m_database.addDiscussion(m_save.m_tempStr, discussionID);
        if(CREATE_DISCUSSION_SUCCESS == m_save.m_replyKind)
          {
            m_database.searchDiscussionInformation(discussionID, m_save.m_discussionInf);

          }
        save.m_clientSocket->sendMessage(m_save);
        break;
      }
    case GET_IN_MAILS:
      {
        m_save.m_myID = save.m_myID;
        m_save.m_replyKind = m_database.searchInMails(m_save.m_myID, m_save.m_inMailsVec);
        save.m_clientSocket->sendMessage(m_save);
        if(GET_IN_MAILS_SUCCESS == m_save.m_replyKind)
          {
            QString mailAddress = save.m_myID;
            mailAddress.append(MAIL_ADDRESS_FORMAT);
            if(m_userMailMap.contains(mailAddress))
              {
                qDebug() << "have connected:" << mailAddress;
              }
            else
              {
                m_userMailMap.insert(mailAddress, save.m_clientSocket);
                emit haveNewAddress(mailAddress);
              }
          }
        break;
      }
    case GET_OUT_MAILS:
      {
        m_save.m_myID = save.m_myID;
        m_save.m_replyKind = m_database.searchOutMails(m_save.m_myID, m_save.m_outMailsVec);
        save.m_clientSocket->sendMessage(m_save);
        break;
      }
    case DELETE_IN_MAIL:
      {
        m_save.m_myID = save.m_myID;
        m_save.m_mailID = save.m_mailID;
        m_save.m_replyKind = m_database.deleteOutMail(m_save.m_myID, m_save.m_mailID);
        save.m_clientSocket->sendMessage(m_save);
        break;
      }
    case SEND_MAIL:
      {
        m_save.m_mail = save.m_mail;
        m_save.m_replyKind = m_database.insertMail(m_save.m_mail);

        save.m_clientSocket->sendMessage(m_save);

        if(SEND_MAIL_SUCCESS == m_save.m_replyKind)
          {
            QString mailAddress = m_save.m_mail.m_receiverAddress;
            mailAddress.append(MAIL_ADDRESS_FORMAT);

            QMap<QString, IMClientSocketCtrl *>::iterator iter;
            iter = m_userMailMap.find(mailAddress);
            if(iter != m_userMailMap.end())
              {
                m_save.m_replyKind = RECEIVE_MAIL;
                iter.value()->sendMessage(m_save);
              }
          }
        break;
      }
    case SET_MAIL_READ:
      {
        m_save.m_myID = save.m_myID;
        m_save.m_mailID = save.m_mailID;
        m_save.m_replyKind = m_database.setMailRead(m_save.m_myID, m_save.m_mailID);
        break;
      }

    case REMOVE_BOX:
      {
        m_save.m_myID = save.m_myID;
        m_save.m_title = save.m_title;
        break;
      }
    case CREATE_BOX:
      {
        m_save.m_myID = save.m_myID;
        m_save.m_title = save.m_title;
        break;
      }
    case MOVE_FRIEND_BOX:
      {
        m_save.m_myID = save.m_myID;
        m_save.m_title = save.m_title;
        m_save.m_peerID = save.m_peerID;
        m_save.m_newTitle = save.m_newTitle;
        m_save.m_replyKind = m_database.moveFriendToGroup(
              m_save.m_myID, m_save.m_title,
              m_save.m_peerID, m_save.m_newTitle);
        save.m_clientSocket->sendMessage(m_save);
        break;
      }
    case RENAME_BOX:
      {
        m_save.m_myID = save.m_myID;
        m_save.m_title = save.m_title;
        m_save.m_newTitle = save.m_newTitle;
        m_save.m_replyKind = m_database.renameGroup(
              m_save.m_myID, m_save.m_title, m_save.m_newTitle);
        save.m_clientSocket->sendMessage(m_save);
        break;
      }
    case GET_HISTORY_MESSAGE:
      {
        m_save.m_myID = save.m_myID;
        m_save.m_peerID = save.m_peerID;
        m_save.m_date = save.m_date;
        m_save.m_replyKind = m_database.searchHistoryMessageOneFriendOneDate(
              m_save.m_myID, m_save.m_peerID,
              m_save.m_date, m_save.m_historyMessageVec
              );
        save.m_clientSocket->sendMessage(m_save);
        break;
      }
    case CHANGE_PASSWORD:
      {
        m_save.m_tempStr = save.m_tempStr;
        m_save.m_replyKind = m_database.updatePassword(m_save.m_tempStr);
        save.m_clientSocket->sendMessage(m_save);
        break;
      }
    default:
      break;
    }
}
// mark: private---------------------------------------------------
/*************************************************
Function Name： incomingConnection()
Description: 当有新的连接时，会调用此函数。用于创建新的线程去控制socket通信
Input： int socketDescriptor：socket标识，用于创建socket
Output：NULL
Changes： NULL
*************************************************/
void IMTcpServer::incomingConnection(int socketDescriptor)
{
  //建立新连接，并连接相应的信号/槽
  IMClientSocketCtrl *clientScoket = new IMClientSocketCtrl(this);
  clientScoket->setSocketDescriptor(socketDescriptor);
  connect(clientScoket, &IMClientSocketCtrl::sendSignal,
          this, &IMTcpServer::sendMessage);
  connect(clientScoket, &IMClientSocketCtrl::deleteSignal,
          this, &IMTcpServer::clientDisconnected);
}

/*************************************************
Function Name： friendRequest()
Description:  处理好友请求
*************************************************/
void IMTcpServer::friendRequest(const SaveTmpInformation &save)
{
  m_save.m_replyKind = ADD_FRIEND;
  m_save.m_message = save.m_message;
  switch(save.m_message.m_type)
    {
     // 请求添加好友
    case REQUEST_FRIEND:
      {
        // 获得好友资料不成功
        if(GET_FRIEND_SUCCESS != m_database.searchFriendInformationByID(
             m_save.m_message.m_senderID, m_save.m_userInf))
          {
            return;
          }
        qDebug() << "Request add friend";
        break;
      }
    case REFUSE_FRIEND:
      {
          qDebug() << "refuse friend";
          break;
      }
    case AGREE_FRIEND:
      {
          if(!m_database.addFriend(m_save.m_message))
            {
              m_save.m_message.m_type = ADD_FRIEND_FAIL;
              break;
            }

          m_save.m_message.m_senderID = save.m_message.m_receiverID;
          m_save.m_message.m_receiverID = save.m_message.m_senderID;
          if(GET_FRIEND_SUCCESS != m_database.searchFriendInformationByID(
               m_save.m_message.m_senderID, m_save.m_userInf))
            {
              return;
            }
          save.m_clientSocket->sendMessage(m_save);

          m_save.m_message.m_senderID = save.m_message.m_senderID;
          m_save.m_message.m_receiverID = save.m_message.m_receiverID;
          if(GET_FRIEND_SUCCESS != m_database.searchFriendInformationByID(
               m_save.m_message.m_senderID, m_save.m_userInf))
            {
              m_save.m_message.m_type = ADD_FRIEND_FAIL;
              return;
            }
          qDebug() << "agree add friend";

          break;
      }
    default:
      return;
    }
  QMap<QString, IMClientSocketCtrl *>::iterator iter;
  iter = m_userMap.find(m_save.m_message.m_receiverID);
  if(iter == m_userMap.end())
    {
      //存储离线消息
      //m_database.messageRequest(save.m_message);
    }
  else
    {
      iter.value()->sendMessage(m_save);
    }
}

/*************************************************
Function Name： flockRequest()
Description:  处理群请求
*************************************************/
void IMTcpServer::flockRequest(const SaveTmpInformation &save)
{
  m_save.m_replyKind = ADD_FLOCK;
  m_save.m_message = save.m_message;
  switch(save.m_message.m_type)
    {
    // 用户请求加入群
    case REQUEST_FLOCK:
      {
        //查询到群主的ID
        if(!m_database.searchFlockHost(m_save.m_message.m_receiverID,
                                       m_save.m_peerID))
          {
            return;
          }
        qDebug() << "request user add flock";
        break;
      }
      // 群主请求用户加入群
    case FLOCK_REQUEST:
      {
        m_save.m_peerID = m_save.m_message.m_receiverID;
        break;
      }
      // 群主拒绝用户加入
    case FLOCK_REFUSE:
      {
        m_save.m_peerID = m_save.m_message.m_receiverID;
        qDebug() << "refuse user add flock";
        break;
      }
      // 群主同意用户加入
    case FLOCK_AGREE:
      {
        if(!m_database.addFlockMember(m_save.m_message.m_receiverID,
                                      m_save.m_message.m_senderID))
          {
            qDebug() << "database addFlockMember fail";
            m_save.m_message.m_type = ADD_FLOCK_FAIL;
            break;
          }
        //新成员
        m_save.m_message.m_senderID = save.m_message.m_receiverID;
        //群号
        m_save.m_message.m_receiverID = save.m_message.m_senderID;

        //获取新成员信息.发送给群主
        if(!m_database.searchFlockMember(m_save.m_message.m_receiverID,
                                         m_save.m_message.m_senderID,
                                         m_save.m_flockMember))
          {
            qDebug() << "get new member information fail";
            return;
          }

        //获取群信息，发送给请求入群者 以及 群主
        if(!m_database.searchFlockInformation(m_save.m_message.m_receiverID,
                                              m_save.m_flockInf))
          {
            qDebug() << "add flock information fail";
            m_save.m_message.m_type = ADD_FLOCK_FAIL;
            return;
          }

        m_save.m_message.m_type = AGREE_FLOCK;
        qDebug() << "socket send AGREE_FLOCK";
        save.m_clientSocket->sendMessage(m_save);

        tellFlockMembersHaveNewMember(m_save.m_flockInf.m_flockID);

//-----------------------------------------------
        m_save.m_message.m_type = FLOCK_AGREE;
        m_save.m_message.m_senderID = save.m_message.m_senderID;     // 群号
        m_save.m_message.m_receiverID = save.m_message.m_receiverID; // 新成员
        m_save.m_peerID = m_save.m_message.m_receiverID;
        qDebug() << "agree user add flock";

        break;
      }
      //用户同意加入群
    case AGREE_FLOCK:
      {
        //通过群ID（receiverID）查询到群主的ID（peerID）
        if(!m_database.searchFlockHost(m_save.m_message.m_receiverID,
                                       m_save.m_peerID))
          {
            return;
          }
        //在数据库中添加群成员信息
        if(!m_database.addFlockMember(m_save.m_message.m_receiverID,
                                      m_save.m_message.m_senderID))
          {
            m_save.m_message.m_type = ADD_FLOCK_FAIL;
            return;
          }
        //获取新成员信息，发送给群主
        if(!m_database.searchFlockMember(
             m_save.m_message.m_receiverID,
             m_save.m_message.m_senderID,
             m_save.m_flockMember))
          {
            return;
          }
        //获取群信息，发送给入群者 以及 群主
        if(!m_database.searchFlockInformation(
             m_save.m_message.m_receiverID,
             m_save.m_flockInf))
          {
            m_save.m_message.m_type = ADD_FLOCK_FAIL;
            return;
          }

        m_save.m_message.m_type = FLOCK_AGREE;
        //群号
        m_save.m_message.m_senderID = save.m_message.m_receiverID;
        //新成员
        m_save.m_message.m_receiverID = save.m_message.m_senderID;
        save.m_clientSocket->sendMessage(m_save);

        tellFlockMembersHaveNewMember(m_save.m_flockInf.m_flockID);

        m_save.m_message.m_type = AGREE_FLOCK;
        //新成员
        m_save.m_message.m_senderID = save.m_message.m_senderID;
        //群号
        m_save.m_message.m_receiverID = save.m_message.m_receiverID;

        break;
      }
    case REFUSE_FLOCK:
      {
        break;
      }
    default:
      {
        return;
      }
    }

  QMap<QString, IMClientSocketCtrl*>::iterator iter;
  iter = m_userMap.find(m_save.m_peerID);
  if(iter == m_userMap.end())
    {
      //存储离线消息
      //m_database.messageRequest(save.m_message);
    }
  else
    {
      qDebug() << "send Message from flock";
      iter.value()->sendMessage(m_save);
    }
}

/*************************************************
Function Name： discussionRequest()
Description:  处理讨论组请求
*************************************************/
void IMTcpServer::discussionRequest(const SaveTmpInformation &save)
{
  m_save.m_replyKind = ADD_DISCUSSION;
  m_save.m_message = save.m_message;

  if(ADD_DISCUSSION != m_save.m_message.m_type)
    {
      return;
    }

  //在数据库中添加讨论组成员信息
  if(!m_database.addDiscussionMember(m_save.m_message.m_senderID,
                                     m_save.m_message.m_receiverID))
    {
      m_save.m_message.m_type = ADD_DISCUSSION_FAIL;
      save.m_clientSocket->sendMessage(m_save);
      return;
    }

  //获取讨论组信息，发给新加入的成员
  if(m_database.searchDiscussionInformation(m_save.m_message.m_senderID,
                                            m_save.m_discussionInf))
    {
      m_save.m_message.m_type = ADD_DISCUSSION_SUCCESS;
      QMap<QString, IMClientSocketCtrl *>::iterator iter;
      iter = m_userMap.find(m_save.m_message.m_receiverID);
      if(iter != m_userMap.end())
        {
          iter.value()->sendMessage(m_save);
        }
    }

  //获取新成员信息， 发送给之前的成员
  if(m_database.searchDiscussionMember(m_save.m_message.m_senderID,
                                       m_save.m_message.m_receiverID,
                                       m_save.m_discussionMember))
    {
      int replyKind = m_database.searchDiscussionMembers(m_save.m_message.m_senderID,
                                                         m_save.m_discussionMembersVec);
      if(GET_DISCUSSION_MEMBERS_SUCCESS == replyKind)
        {
          m_save.m_message.m_type = NEW_DISCUSSION_MEMBER;
          QMap<QString, IMClientSocketCtrl *>::iterator iter;
          for(int i = 0; i < m_save.m_discussionMembersVec.size(); ++ i)
            {
              if(m_save.m_discussionMembersVec[i].m_userID.
                 compare(m_save.m_message.m_receiverID) == 0)
                  continue;
              iter = m_userMap.find(m_save.m_discussionMembersVec[i].m_userID);
              if(iter != m_userMap.end())
                {
                  iter.value()->sendMessage(m_save);
                }
            }
        }
    }
}

/*************************************************
Function Name： talkRequest()
Description:  处理对话请求
*************************************************/
void IMTcpServer::talkRequest(const SaveTmpInformation &save)
{
  m_save.m_message = save.m_message;

  switch(save.m_message.m_type)
    {
    //聊天信息
    case TALK_MESSAGE:
      {
        m_save.m_replyKind = TALK;
        QMap<QString, IMClientSocketCtrl *>::iterator iter;
        iter = m_userMap.find(m_save.m_message.m_receiverID);

        //聊天记录存储到数据库
        m_database.addHistoryMessage(m_save.m_message);

        if(m_userMap.end() == iter)
          {
            //存储离线消息到数据库
            m_database.addOfflineMessage(m_save.m_message);
          }
        else
          {
            iter.value()->sendMessage(m_save);
          }
        break;
      }
    case REQUEST_FILE:
    case REFUSE_FILE:
    case REQUEST_VIDEO:
      {
        qDebug() << "IMTcpServer REQUEST_FILE";
        m_save.m_replyKind = TALK;
        QMap<QString, IMClientSocketCtrl *>::iterator iter;
        iter = m_userMap.find(m_save.m_message.m_receiverID);
        if(iter != m_userMap.end())
          {
            qDebug() << "iter.value()->sendMessage(m_save)";
            iter.value()->sendMessage(m_save);
          }
        break;
      }
      // 群对话
    case TALK_FLOCK:
      {
          m_save.m_replyKind = m_database.searchFlockMembers(m_save.m_message.m_receiverID,
                                                             m_save.m_flockMembersVec);
          if(GET_FLOCK_MEMBERS_SUCCESS == m_save.m_replyKind)
            {
              m_save.m_replyKind = TALK;
              QMap<QString, IMClientSocketCtrl *>::iterator iter;
              for(int i = 0; i < m_save.m_flockMembersVec.size(); ++ i)
                {
                  iter = m_userMap.find(m_save.m_flockMembersVec[i].m_userID);
                  if(iter == m_userMap.end())
                    {
                      //存储离线信息
                      //m_database.messageRequest(save.m_message);
                    }
                  else
                    {
                      iter.value()->sendMessage(m_save);
                    }
                }

            }
          break;
      }
    case TALK_DISCUSSION:
      {
        m_save.m_replyKind = m_database.searchDiscussionMembers(m_save.m_message.m_receiverID,
                                                         m_save.m_discussionMembersVec);
        if(GET_DISCUSSION_MEMBERS_SUCCESS == m_save.m_replyKind)
          {
            m_save.m_replyKind = TALK;
            QMap<QString, IMClientSocketCtrl *>::iterator iter;
            for(int i = 0; i < m_save.m_discussionMembersVec.size(); ++i)
              {
                iter = m_userMap.find(m_save.m_discussionMembersVec[i].m_userID);

                if(iter == m_userMap.end())
                  {
                    //存储离线信息
                    //m_database.messageRequest(save.m_message);
                  }
                else
                  {
                    iter.value()->sendMessage(m_save);
                  }
              }
          }
        break;
      }
    default:
      {
        break;
      }
    }
}

/*************************************************
Function Name： tellFriendStatusCHanged()
Description:  通知好友，改变用户状态
*************************************************/
void IMTcpServer::tellFriendsStatusChanged(const QString &id, int status)
{
  m_save.m_replyKind = CHANGE_STATUE;
  m_save.m_status = status;
  m_save.m_peerID = id;
  m_database.searchFriendsID(id, m_save.m_IDVec);

  QMap<QString, IMClientSocketCtrl *>::iterator iter;

  for(int i = 0; i < m_save.m_IDVec.size(); ++ i)
    {
      iter = m_userMap.find(m_save.m_IDVec[i]);
      if(iter != m_userMap.end())
        {
          iter.value()->sendMessage(m_save);
        }
    }
}

/*************************************************
Function Name： tellFriendsInformationChanged()
Description:  通知好友，改变用户信息
*************************************************/
void IMTcpServer::tellFriendsInformationChanged(const QString &id)
{
  m_save.m_replyKind = CHANGE_INFORMATION;
  if(!m_database.searchFriendsID(id, m_save.m_IDVec))
    {
      return;
    }
  m_save.m_friendInf.m_headPortrait = m_save.m_userInf.m_headPortrait;
  m_save.m_friendInf.m_userID = m_save.m_userInf.m_userID;
  m_save.m_friendInf.m_nickname = m_save.m_userInf.m_nickname;
  m_save.m_friendInf.m_status = m_save.m_userInf.m_status;

  QMap<QString, IMClientSocketCtrl *>::iterator iter;
  for(int i = 0; i < m_save.m_IDVec.size(); ++ i)
    {
      iter = m_userMap.find(m_save.m_IDVec[i]);
      if(iter != m_userMap.end())
        {
          iter.value()->sendMessage(m_save);
        }
    }
}

/*************************************************
Function Name： tellMembersFlockDroped()
Description:  通知群成员，群被解散
*************************************************/
void IMTcpServer::tellMembersFlockDroped(const QString &flockID)
{
  m_save.m_replyKind = m_database.searchFlockMembers(flockID,
                                                     m_save.m_flockMembersVec);
  if(GET_DISCUSSION_MEMBERS_SUCCESS == m_save.m_replyKind)
    {
        m_save.m_replyKind = DROP_FLOCK_SUCCESS;
        m_save.m_flockID = flockID;
        QMap<QString, IMClientSocketCtrl *>::iterator iter;
        for(int i = 0; i  < m_save.m_discussionMembersVec.size(); ++ i )
          {
            iter = m_userMap.find(m_save.m_discussionMembersVec[i].m_userID);
            if(iter == m_userMap.end())
              {
                //存储离线信息
              }
            else
              {
                iter.value()->sendMessage(m_save);
              }
          }
    }
}

/*************************************************
Function Name： tellMembersDiscussionDroped()
Description:  通知讨论组成员， 讨论组被解散了
*************************************************/
void IMTcpServer::tellMembersDiscussionDrop(const QString &discussionID)
{
  m_save.m_replyKind = m_database.searchDiscussionMembers(discussionID,
                                                          m_save.m_discussionMembersVec);
  if(GET_DISCUSSION_MEMBERS_SUCCESS == m_save.m_replyKind)
    {
      m_save.m_replyKind = DROP_DISCUSSION_SUCCESS;
      m_save.m_discussionID = discussionID;
      QMap<QString, IMClientSocketCtrl *>::iterator iter;
      for(int i = 0; i < m_save.m_discussionMembersVec.size(); ++ i)
        {
          iter = m_userMap.find(m_save.m_discussionMembersVec[i].m_userID);

          if(iter == m_userMap.end())
            {
              //存储离线信息
            }
          else
            {
              iter.value()->sendMessage(m_save);
            }
        }
    }
}

/*************************************************
Function Name： tellMembersFlockChanged()
Description:  通知群成员，群信息被改变
*************************************************/
void IMTcpServer::tellMembersFlockChanged(const FlockInformation &flock)
{
  int kind = m_database.searchFlockMembers(flock.m_flockID,
                                           m_save.m_flockMembersVec);
  if(kind == GET_FLOCK_MEMBERS_SUCCESS)
    {
      qDebug() << "GET_FLOCK_MEMBERS_SUCCESS";
      QMap<QString, IMClientSocketCtrl *>::iterator iter;
      for(int i = 0; i < m_save.m_flockMembersVec.size(); ++ i)
        {
          iter = m_userMap.find(m_save.m_flockMembersVec[i].m_userID);

          if(iter == m_userMap.end())
            {
              //存储离线信息
            }
          else
            {
              iter.value()->sendMessage(m_save);
            }
        }
    }
  qDebug() << "GET_FLOCK_MEMBERS_Fail";
}

/*************************************************
Function Name： tellMembersDiscussionChanged()
Description:  通知讨论组成员，讨论组信息被修改了
*************************************************/
void IMTcpServer::tellMembersDiscussionChanged(const DiscussionInformation &discussion)
{
  int kind = m_database.searchDiscussionMembers(discussion.m_discussionID,
                                                m_save.m_discussionMembersVec);
  if(kind == GET_DISCUSSION_MEMBERS_SUCCESS)
    {
      QMap<QString, IMClientSocketCtrl *>::iterator iter;
      for(int i = 0; i < m_save.m_discussionMembersVec.size(); ++ i)
        {
          iter = m_userMap.find(m_save.m_discussionMembersVec[i].m_userID);

          if(iter == m_userMap.end())
            {
              //存储离线信息
            }
          else
            {
              iter.value()->sendMessage(m_save);
            }
        }
    }
}

/*************************************************
Function Name： tellFlockMemberHaveNewMember()
Description:  通知群成员， 有新的成员
*************************************************/
void IMTcpServer::tellFlockMembersHaveNewMember(const QString &flockID)
{
  int replyKind = m_database.searchFlockMembers(flockID,
                                                m_save.m_flockMembersVec);
  if(GET_FLOCK_MEMBERS_SUCCESS == replyKind)
    {
      m_save.m_message.m_type = NEW_FLOCK_MEMBER;

      QMap<QString, IMClientSocketCtrl *>::iterator iter;
      for(int i = 0; i < m_save.m_flockMembersVec.size(); ++ i)
      {
        if(m_save.m_flockMembersVec[i].m_userID.compare(m_save.m_flockMember.m_userID) == 0)
          {
            continue;
          }
        if(m_save.m_flockMembersVec[i].m_userID.compare(m_save.m_flockInf.m_creatorID) == 0)
          {
            continue;
          }
        iter = m_userMap.find(m_save.m_flockMembersVec[i].m_userID);
        if(m_userMap.end() == iter)
          {
            //存储离线消息
          }
        else
          {
            iter.value()->sendMessage(m_save);
          }

      }
    }
}

/*************************************************
Function Name： tellFlockMemberHaveLeavedMember()
Description:  通知群成员， 有成员离开
*************************************************/
void IMTcpServer::tellFlockMembersHaveLeaveMember(const QString &flockID)
{
  int replyKind = m_database.searchFlockMembers(flockID,
                                                m_save.m_flockMembersVec);
  if(GET_FLOCK_MEMBERS_SUCCESS == replyKind)
    {
      m_save.m_replyKind = LEAVE_FLOCK_MEMBER;
      QMap<QString, IMClientSocketCtrl *>::iterator iter;
      for(int i = 0; i < m_save.m_flockMembersVec.size(); ++ i)
        {
          if(m_save.m_flockMembersVec[i].m_userID.compare(m_save.m_peerID) == 0)
            {
              continue;
            }
          iter = m_userMap.find(m_save.m_flockMembersVec[i].m_userID);
          if(iter == m_userMap.end())
            {
              //存储离线消息
            }
          else
            {
              iter.value()->sendMessage(m_save);
            }

        }
    }
}

/*************************************************
Function Name： tellDiscussionMemberHaveNewMember()
Description:  通知讨论组群成员， 有新的成员
*************************************************/
/*void IMTcpServer::tellDiscussionMemberHaveNewMember(const QString & discussionID)
{
}
*/
/*************************************************
Function Name： tellDiscussionMemberHaveLeavedMember()
Description:  通知讨论组成员， 有成员退出
*************************************************/

void IMTcpServer::tellDiscussionMembersHaveLeaveMember(const QString &discussionID)
{
  int replyKind = m_database.searchDiscussionMembers(discussionID,
                                                     m_save.m_discussionMembersVec);
  if(replyKind == GET_DISCUSSION_MEMBERS_SUCCESS)
    {
      m_save.m_replyKind = LEAVE_DISCUSSION_MEMBER;

      QMap<QString, IMClientSocketCtrl *>::iterator iter;

      for(int i = 0; i < m_save.m_flockMembersVec.size(); ++ i)
        {
          iter = m_userMap.find(m_save.m_flockMembersVec[i].m_userID);
          if(m_userMap.end() == iter)
            {
              //存储离线消息
            }
          else
            {
              iter.value()->sendMessage(m_save);
            }
        }
    }
}

