/*********************************************
File Name： IMClientSocketCtrl.cpp
Author： hanyan
Date： 2016.10.10
Description： 用于控制客户端socket的类
Changes：
********************************************/

#include "IMClientSocketCtrl.h"
//#include "../Model/IMUser.h"
#include <QMultiMap>
#include <QDateTime>

// mark: public--------------------------------------------
IMClientSocketCtrl::IMClientSocketCtrl(QObject *parent)
    :QTcpSocket(parent)
{
    connect(this, &IMClientSocketCtrl::readyRead,
            this, &IMClientSocketCtrl::receiveMessage);
    connect(this, &IMClientSocketCtrl::disconnected,
            this, &IMClientSocketCtrl::deleteScoket);

    m_blockSize = 0;
    m_save.m_clientSocket = this;
}

IMClientSocketCtrl::~IMClientSocketCtrl()
{
    if(m_database.isOpen())
        {
            m_database.close();
        }
}

// mark: public slots-----------------------------------------

/*************************************************
Function Name： receiveMessage()
Description: 接收用户发送的信息
*************************************************/
void IMClientSocketCtrl::receiveMessage()
{
    QDataStream in(this);
    in.setVersion(QDataStream::Qt_4_8);
    if(m_blockSize == 0)
        {
            if(bytesAvailable() < (int)sizeof(quint16))
                {
                    return;
                }
            in >> m_blockSize;
        }

    if(bytesAvailable() < m_blockSize)
        {
            return;
        }
    in >> m_save.m_requestKind;

   // qDebug() << "receive message: " << m_save.m_requestKind;

    switch (m_save.m_requestKind) {
        case CHECK_CONNECTION: // 检查是否能与服务器连接
            {
                m_save.m_replyKind = CHECK_CONNECTION;
                sendMessage(m_save);
                m_blockSize = 0;
                break;
            }
        case LOGIN:
            {
                in >> m_save.m_loginInf;
                m_save.m_myID = m_save.m_loginInf.m_userID;
                m_save.m_socketID = m_save.m_myID;
                break;
            }
        case CHECK_MESSAGE: // 检查有无给自己的信息（登录时检查）
            {
                in >> m_save.m_myID;
                break;
            }
        case REGISTER:
            {
                in >> m_save.m_userInf;
                break;
            }
        case GET_QUESTION_ANSWER:// 获取密保问题以及答案
            {
                in >> m_save.m_myID;
                break;
            }
        case TALK:// 发送对话
            {
                qDebug() << "receive message(TALK)";
                in >> m_save.m_message;// 对话信息
                break;
            }
        case GET_ALL_FRIENDS://获取所有好友信息
            {
                in >> m_save.m_myID >> m_save.m_status;
                m_save.m_socketID = m_save.m_myID;
                break;
            }
        case GET_ALL_FLOCKS:// 获取所有群信息
            {
                in >> m_save.m_myID;
                break;
            }
        case GET_ALL_DISCUSSIONS:
            {
                in >> m_save.m_myID;
                break;
            }
        case GET_FLOCK_MEMBERS:
            {
                in >> m_save.m_flockID;
                break;
            }
        case GET_DISCUSSIONS_MEMBERS:
            {
                in >> m_save.m_discussionID;
                break;
            }
        case GET_FRIEND_INFORMATION:
            {
                in >> m_save.m_peerID;
                break;
            }
        case GET_MYSELF_INFORMATION:
            {
                in >> m_save.m_myID;
                break;
            }
        case CHANGE_INFORMATION:// 改变自己的信息
            {
                in >> m_save.m_userInf;
                break;
            }
        case CHANGE_REMARK:// 改变（好友）备注
            {
                in >> m_save.m_tempStr;// 储存在各种小窗口中填写的信息
                break;
            }
        case CHANGE_STATUE:// 改变自身状态
            {
                in >> m_save.m_myID >> m_save.m_status;
                break;
            }
        case ADD_FRIEND:// 添加好友
        {
            in >> m_save.m_message;
            break;
        }
        case REFUSE_FRIEND:// 不同意好友请求
        {
            //in >> m_save.m_peerID;
            break;
        }
        case DELETE_FRIEND:
        {
            in >> m_save.m_myID >> m_save.m_peerID;
            break;
        }

        case ADD_FLOCK:
        {
            in >> m_save.m_message;
            break;
        }
        case DROP_FLOCK:// 解散群
        {
            in >> m_save.m_myID >> m_save.m_flockID;
            break;
        }
        case LEAVE_FLOCK:
        {
            in >> m_save.m_myID >> m_save.m_flockID;
            break;
        }
        case CHANGE_FLOCK:
        {
            in >> m_save.m_flockInf;
            break;
        }
        case CHANGE_FLOCK_REMARK:
        {
            in >> m_save.m_tempStr;
            break;
        }
        case CREATE_FLOCK:
        {
            in >> m_save.m_tempStr;
            break;
        }

        case ADD_DISCUSSION:
        {
            in >> m_save.m_message;
            break;
        }
        case DROP_DISCUSSION:
        {
            in >> m_save.m_myID >> m_save.m_discussionID;
            break;
        }
        case LEAVE_DISCUSSION:
        {
            in >> m_save.m_myID >> m_save.m_discussionID;
            break;
        }
        case CHANGE_DISCUSSION:
        {
            in >> m_save.m_discussionInf;
            break;
        }
        case CREATE_DISCUSSION:
        {
            in >> m_save.m_tempStr;
            break;
        }
        case GET_IN_MAILS:
        case GET_OUT_MAILS:
        {
            in >> m_save.m_myID;
            m_save.m_socketID = m_save.m_myID;
            m_save.m_socketID.append(MAIL_ADDRESS_FORMAT);
            break;
        }
        case DELETE_IN_MAIL:
        case DELETE_OUT_MAIL:
        case SET_MAIL_READ:
        {
            in >> m_save.m_myID >> m_save.m_mailID;
            break;
        }
       case SEND_MAIL:
        {
            in >> m_save.m_mail;
            break;
        }

        case REMOVE_BOX:// 删除分组
        case CREATE_BOX:// 创建分组
        {
            in >> m_save.m_myID >> m_save.m_title;
            break;
        }
        case MOVE_FRIEND_BOX:// 移动好友至其他分组
        {
            in >> m_save.m_myID >> m_save.m_peerID
               >> m_save.m_title >> m_save.m_newTitle;
            break;
        }
        case RENAME_BOX:
        {
            in >> m_save.m_myID >> m_save.m_title >> m_save.m_newTitle;
            break;
        }
        case GET_HISTORY_MESSAGE:
        {
            in >> m_save.m_myID >> m_save.m_peerID >> m_save.m_date;
            break;
        }
        case CHANGE_PASSWORD:
        {
            in >> m_save.m_tempStr;
            break;
        }

        default:
            break;
        }

    QByteArray data = this->readAll();
   // qDebug() << "leaved in sicket: " << data.size();

    m_blockSize = 0;

    emit sendSignal(m_save);
}
/*************************************************
Function Name： sendMessage()
Description: 发送信息
*************************************************/
void IMClientSocketCtrl::sendMessage(const SaveTmpInformation &temp)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);
    out << quint16(0) << (int)temp.m_replyKind;

    switch (temp.m_replyKind)
    {
    case LOGIN_SUCCESS:
    {
        out << temp.m_userInf;
        break;
    }
    case REGISTER_SUCCESS:
    {
        out << temp.m_userInf.m_userID << temp.m_userInf.m_regDateTime;
        break;
    }
    case GET_QUESTION_ANSWER_SUCCESS:
    {
        out << temp.m_tempStr;
        break;
    }
    case GET_ALL_FRIENDS_SUCCESS:
    {
        int len = temp.m_friendsVec.size();
        out << len;
        for (int i=0; i<len; ++i)
            out << temp.m_friendsVec[i];
        break;
    }
    case GET_ALL_FLOCKS_SUCCESS:
    {
        int len = temp.m_flocksVec.size();
        out << len;
        for (int i=0; i<len; ++i)
            out << temp.m_flocksVec[i];
        break;
    }
    case GET_ALL_DISCUSSIONS_SUCCESS:
    {
        int len = temp.m_discussionsVec.size();
        out << len;
        for (int i=0; i<len; ++i)
            out << temp.m_discussionsVec[i];
        break;
    }
    case GET_FLOCK_MEMBERS_SUCCESS:
    {
        qDebug()<< "GET_FLOCK_MEMBERS_SUCCESS";
        int len = temp.m_flockMembersVec.size();
        out << len;
        for (int i=0; i<len; ++i)
            out << temp.m_flockMembersVec[i];
        break;
    }
    case GET_DISCUSSION_MEMBERS_SUCCESS:
    {
        int len = temp.m_discussionMembersVec.size();
        out << len;
        for (int i=0; i<len; ++i)
            out << temp.m_discussionMembersVec[i];
        break;
    }


    case HAVE_MESSAGE:
        break;
    case CHANGE_STATUE:
        out << temp.m_peerID << temp.m_status;
        break;
    case GET_MYSELF_SUCCESS:
    {
        out << temp.m_userInf;
        break;
    }
    case DELETE_FRIEND_SUCCESS: // 获得自己详细资料成功
    {
        out << temp.m_myID << temp.m_peerID << temp.m_groupName;
        break;
    }
    case CHANGE_INFORMATION_SUCCESS:
    {
        out << temp.m_userInf;
        break;
    }
    case CHANGE_INFORMATION:// 改变自己的信息
    {
        out << temp.m_friendInf;
        break;
    }
    case ADD_FRIEND:
    {
        out << temp.m_message << temp.m_userInf;
        break;
    }
    case GET_FRIEND_SUCCESS:
    {
        out << temp.m_userInf;
        break;
    }
    case TALK:
        qDebug() << "out << temp.m_message";
        qDebug() << temp.m_message.m_type << " " << REQUEST_FILE;
        out << temp.m_message;
        break;
    case CHANGE_REMARK_SUCCESS:
    {
        out << temp.m_tempStr;
        break;
    }
    case BE_DELETED:// 自己被好友删除了
    {
        out << temp.m_myID << temp.m_peerID << temp.m_groupNameF;
        break;
    }
    case REQUEST_FLOCK:
    {
        out << temp.m_message;
        break;
    }
    case ADD_FLOCK:
    {
        out << temp.m_message << temp.m_flockMember << temp.m_flockInf;
        break;
    }

    case DROP_FLOCK_SUCCESS:
    {
        out << temp.m_flockID;
        break;
    }
     case LEAVE_FLOCK_SUCCESS:
    {
        out << temp.m_myID << temp.m_flockID << temp.m_peerID;
        break;
    }
    case LEAVE_FLOCK_MEMBER:
    {
        out << temp.m_myID << temp.m_flockID << temp.m_peerID;
        break;
    }
    case CHANGE_FLOCK_SUCCESS:
    {
        out << temp.m_flockInf;
        break;
    }
    case CHANGE_FLOCK_REMARK_SUCCESS:
    {
        out << temp.m_tempStr;
        break;
    }
    case CREATE_FLOCK_SUCCESS:
    {
        out << temp.m_flockInf;
        break;
    }



    case ADD_DISCUSSION:
    {
        out << temp.m_message;
        if (ADD_DISCUSSION_SUCCESS == temp.m_message.m_type)
            out << temp.m_discussionInf;
        else if (NEW_DISCUSSION_MEMBER == temp.m_message.m_type)
            out << temp.m_discussionMember;
        break;
    }

    case DROP_DISCUSSION_SUCCESS:
    {
        out << temp.m_discussionID;
        break;
    }
     case LEAVE_DISCUSSION_MEMBER:
    {
        out << temp.m_myID << temp.m_discussionID;// << temp.m_peerID;
        break;
    }
    case CHANGE_DISCUSSION_SUCCESS:
    {
        out << temp.m_discussionInf;
        break;
    }

    case CREATE_DISCUSSION_SUCCESS:
    {
        out << temp.m_discussionInf;
        break;
    }
    /*****************mail*****************/
    case GET_IN_MAILS_SUCCESS:
    {
        int len = temp.m_inMailsVec.size();
        out << len;
        for (int i=0; i<len; ++i)
            out << temp.m_inMailsVec[i];
        break;
    }
    case GET_OUT_MAILS_SUCCESS:
    {
        int len = temp.m_outMailsVec.size();
        out << len;
        for (int i=0; i<len; ++i)
            out << temp.m_outMailsVec[i];
        break;
    }

    case DELETE_IN_MAIL_SUCCESS:
    case DELETE_OUT_MAIL_SUCCESS:
    {
        out << temp.m_myID << temp.m_mailID;
        break;
    }

    case SEND_MAIL_SUCCESS:
    case RECEIVE_MAIL:
    {
        out << temp.m_mail;
        break;
    }

    case RENAME_BOX_SUCCESS:
    {
        out << temp.m_myID << temp.m_title << temp.m_newTitle;
        break;
    }
    case MOVE_FRIEND_BOX_SUCCESS:
    {
        out << temp.m_myID << temp.m_peerID << temp.m_title << temp.m_newTitle;
        break;
    }
    case GET_HISTORY_MESSAGE_SUCCESS:
    {
        out << temp.m_myID << temp.m_peerID << temp.m_date;
        int len = temp.m_historyMessageVec.size();
        out << len;
        for (int i=0; i<len; ++i)
            out << temp.m_historyMessageVec[i];
        break;
    }
    default:
        break;
    }

    out.device()->seek(0);
    out << quint16(block.size() - sizeof(quint16));
    write(block);
}

/*************************************************
Function Name： deleteScoket()
Description: 删除socket
*************************************************/
void IMClientSocketCtrl::deleteScoket()
{
  if(!m_save.m_socketID.isEmpty())//m_socketID是一个QString
  {
      if(LOGIN != m_save.m_requestKind)
        {
          emit deleteSignal(m_save.m_socketID);
        }
  }
  deleteLater();
}
