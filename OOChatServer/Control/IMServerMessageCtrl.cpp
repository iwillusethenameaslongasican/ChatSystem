/*********************************************
File Name： IMServerMessageCtrl.cpp
Author： hanyan
Date： 2016.10.15
Description： 控制服务器类
Changes：
********************************************/

#include "IMServerMessageCtrl.h"
#include "../Model/IMTcpServer.h"
#include "IMClientSocketCtrl.h" //暂时没用到
#include <QMessageBox>
#include <QDateTime>
#include <QStringListModel>
#include <QListView>
#include <QStringList>

IMServerMessageCtrl::IMServerMessageCtrl(QObject *parent)
  :QObject(parent)
{
  m_modelID = new QStringListModel(this);
  m_modelID->setStringList(m_listID);
  m_listViewID = new QListView;
  m_listViewID->setModel(m_modelID);
  //不可编辑
  m_listViewID->setEditTriggers(QAbstractItemView::NoEditTriggers);


  m_modelAddress = new QStringListModel(this);
  m_modelAddress->setStringList(m_listAddress);
  m_listViewAddress = new QListView;
  m_listViewAddress->setModel(m_modelAddress);
  //不可编辑
  m_listViewAddress->setEditTriggers(QAbstractItemView::NoEditTriggers);

}

IMServerMessageCtrl::~IMServerMessageCtrl()
{
  if(m_listViewID != NULL)
    {
      delete m_listViewID;
      m_listViewID = NULL;
    }
  if(m_listViewAddress != NULL)
    {
      delete m_listViewAddress;
      m_listViewAddress = NULL;
    }
}

/*************************************************
Function Name： runServer()
Description: 用于启动服务器
Input： NULL
Output：NULL
Changes： NULL
*************************************************/

void IMServerMessageCtrl::runServer()
{
  m_tcpServer = new IMTcpServer(this);

  connect(m_tcpServer, &IMTcpServer::haveNewID,
          this, &IMServerMessageCtrl::insertIDData);
  connect(m_tcpServer, &IMTcpServer::haveLeftID,
          this, &IMServerMessageCtrl::deleteIDData);

  connect(m_tcpServer, &IMTcpServer::haveNewAddress,
          this, &IMServerMessageCtrl::insertAddressData);
  connect(m_tcpServer, &IMTcpServer::haveLeftAddress,
          this, &IMServerMessageCtrl::deleteAddressData);

  if(!m_tcpServer->listen(QHostAddress::Any, 1234))
    {
      qDebug("run fail");
      QMessageBox::critical(NULL, tr("IM Server"),
                            tr("Unable to start the server: %1")
                            .arg(m_tcpServer->errorString()));
      exit(EXIT_FAILURE);
    }

  m_databaseCtrl.initDatabase();
}

/*************************************************
Function Name： getIDListView()
Description: 获取listiew
*************************************************/
QListView * IMServerMessageCtrl::getIDListView()
{
    return m_listViewID;
}

/*************************************************
Function Name： getAddressListView()
Description: 获取listiew
*************************************************/
QListView * IMServerMessageCtrl::getAddressListView()
{
    return m_listViewAddress;
}

/*************************************************
Function Name： insertIDData()
Description: 插入数据
*************************************************/
void IMServerMessageCtrl::insertIDData(const QString &id)
{
  m_listID.append(id);
  m_modelID->setStringList(m_listID);
}

/*************************************************
Function Name： deleteIDData()
Description: 删除数据
*************************************************/
void IMServerMessageCtrl::deleteIDData(const QString &id)
{
  int index = m_listID.indexOf(id);
  m_listID.removeAt(index);
  m_modelID->removeRow(index);
}

/*************************************************
Function Name： insertIDData()
Description: 插入数据
*************************************************/
void IMServerMessageCtrl::insertAddressData(const QString &address)
{
  m_listAddress.append(address);
  m_modelAddress->setStringList(m_listAddress);
}

/*************************************************
Function Name： deleteIDData()
Description: 删除数据
*************************************************/
void IMServerMessageCtrl::deleteAddressData(const QString &address)
{
  int index = m_listAddress.indexOf(address);
  m_listAddress.removeAt(index);
  m_modelAddress->removeRow(index);
}
