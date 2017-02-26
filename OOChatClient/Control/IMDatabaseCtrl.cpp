#include "IMDatabaseCtrl.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>
#include <QDebug>
#include <QSqlError>
#include "../Control/IMClientFileCtrl.h"

/****************************message*****************************/
static const QString INSERT_MESSAGE_IN_HISTORYMESSAGE =
    "insert into ##HISTORY##MESSAGE## (senderID, receiverID, content, dateTime) "
    "values(?, ?, ?, ?);";
static const QString SEARCH_MESSAGE_IN_HISTORYMESSAGE =
        "select * from ##HISTORY##MESSAGE## where ( (senderID = ? and receiverID = ?) "
        "or (senderID = ? and receiverID = ?) )and dateTime >=? and dateTime < ?;";
static const QString EXPORT_ONE_MESSAGE_IN_HISTORY =
        "select * from ##HISTORY##MESSAGE## where (senderID = ? and receiverID = ?) or "
        "(senderID = ? and receiverID = ?) into outfile ? "
        "fields terminated by ',' optionally enclosed by '\"' escaped by '\"'"
        "lines terminated by '\\r\\n';";

static const QString EXPORT_ALL_MESSAGE_IN_HISTORY =
        "select * from ##HISTORY##MESSAGE## where senderID = ? or receiverID = ? "
        "into outfile ? "
        "fields terminated by ',' optionally enclosed by '\"' escaped by '\"'"
        "lines terminated by '\\r\\n';";

static const QString DELETE_ONE_MESSAGE_IN_HISTORY =
        "delete from ##HISTORY##MESSAGE## where (senderID = ? and receiverID = ?) "
        "or (senderID = ? and receiverID = ?)";
static const QString DELETE_ALL_MESSAGE_IN_HISTORY =
        "delete from ##HISTORY##MESSAGE## where senderID = ? or receiverID = ? ";
/********************create tables********************/
static const QString CREATE_TABLE_HISTORYMESSAGE_SQL =
"create table if not exists ##HISTORY##MESSAGE## "
"("
"messageID INTEGER PRIMARY KEY  AUTOINCREMENT, "
"senderID varchar(15) NOT NULL, "
"receiverID varchar(15) NOT NULL, "
"content varchar(1000) NOT NULL DEFAULT '', "
"dateTime datetime NOT NULL "
")ENGINE=InnoDB DEFAULT CHARSET=utf8;";

IMDatabaseCtrl::IMDatabaseCtrl(const QString myID, QObject *parent)
  : QObject(parent),m_myID(myID)
{
  m_localHistoryTableName = "HistoryMessage";
  m_localHistoryTableName.append(m_myID);

  m_db = new QSqlDatabase;
  createConnection();
  createTable();
  close();
}

IMDatabaseCtrl::~IMDatabaseCtrl()
{
  if(m_db != NULL)
  {
      if(m_db->isOpen())
        {
          m_db->close();
        }
      delete m_db;
      m_db = NULL;
  }

}

/*************************************************
Function Name： createConnection()
Description: 创建连接
*************************************************/
bool IMDatabaseCtrl::createConnection()
{
  if(m_db == NULL)
    {
      m_db = new QSqlDatabase;
    }
  if(m_db->isOpen())
    {
      return true;
    }
  //如果已经打开
  if(QSqlDatabase::contains(("clientIMDB.db")))
    {
      *m_db = QSqlDatabase::database("clientIMDB.db");
    }
  else
    {
      //如果没有，就添加
      *m_db = QSqlDatabase::addDatabase("SQLITE", "clientIMDB.db");
      m_db->setDatabaseName("clientIMDB.db");
    }
  if(!m_db->isOpen())
    {
      return false;
    }
  return true;
}

/*************************************************
Function Name： close()
Description: 关闭数据库
*************************************************/
void IMDatabaseCtrl::close()
{
  if(m_db != NULL && m_db->isOpen())
    {
      m_db->close();
    }
}

/*************************************************
Function Name： isOpen()
Description: 是否连接着
*************************************************/
bool IMDatabaseCtrl::isOpen()
{
  return (m_db->isOpen());
}


/*************************************************
Function Name： addHistoryMessage()
Description: 存储历史记录
*************************************************/
bool IMDatabaseCtrl::addHistoryMessage(const TalkMessage &msg)
{
  if(!createConnection())
    {
      return false;
    }
  //SQL事务
  QSqlDatabase::database().transaction();

  QString queryStr = INSERT_MESSAGE_IN_HISTORYMESSAGE;
  queryStr.replace("##HISTORY##MESSAGE##", m_localHistoryTableName);

  QSqlQuery query(*m_db);
  query.prepare(queryStr);

  query.addBindValue(msg.m_senderID);
  query.addBindValue(msg.m_receiverID);
  query.addBindValue(IMClientFileCtrl::simplifyRichText(msg.m_text));
  query.addBindValue(msg.m_dateTime);

  query.exec();

  errorSQLOrder(query, "insert-msg-to-HistoryMessage");
  if(!query.isActive())
    {
      qDebug() << "QUERY_FAIL: " << query.lastError().text();
      m_db->close();
      return false;
    }

  if(0 >= query.numRowsAffected())
    {
      return false;
    }
  QSqlDatabase::database().commit();

  return true;
}

/*************************************************
Function Name： searchHistoryMessage()
Description: 获取某日的消息记录
*************************************************/
bool IMDatabaseCtrl::searchHistoryMessage(const QString &myID,
                                          const QString &friendID,
                                          const QDate &date,
                                          QVector<TalkMessage> &msgVec)
{
  if(!createConnection())
    {
      return false;
    }
  //SQL事务
  QSqlDatabase::database().transaction();
  QString queryStr = SEARCH_MESSAGE_IN_HISTORYMESSAGE;
  queryStr.replace("##HISTORY##MESSAGE##", m_localHistoryTableName);
  QSqlQuery query(*m_db);
  query.prepare(queryStr);
  query.exec();
  query.addBindValue(myID);
  query.addBindValue(friendID);
  query.addBindValue(friendID);
  query.addBindValue(myID);
  query.addBindValue(date);
  query.addBindValue(date.addDays(1));

  query.exec();
  errorSQLOrder(query, "search-msg-in-HistoryMessage");
  if(!query.isActive())
    {
      qDebug() << "QUERY_FAIL: " << query.lastError().text();
      m_db->close();
      return false;
    }

  TalkMessage msg;
  while(query.next())
    {
      msg.m_senderID = query.value(1).toString();
      msg.m_receiverID = query.value(2).toString();
      msg.m_text = query.value(3).toString();
      msg.m_dateTime = query.value(4).toDateTime();
      msgVec.push_back(msg);
    }

  QSqlDatabase::database().commit();
  return true;
}

/*************************************************
Function Name： exportHistoryMessageOneFriend
Description: 导出好友消息记录
*************************************************/
int IMDatabaseCtrl::exportHistoryMessageOneFriend(const QString &myID,
                                                  const QString &friendID,
                                                  const QString &fileName)
{
  if(!createConnection())
    {
      return -1;
    }
  //sql事务
  QSqlDatabase::database().transaction();

  QString queryStr = EXPORT_ONE_MESSAGE_IN_HISTORY;
  queryStr.replace("##HISTORY##MESSAGE##", m_localHistoryTableName);

  QSqlQuery query(*m_db);
  query.prepare(queryStr);
  query.addBindValue(myID);
  query.addBindValue(friendID);
  query.addBindValue(friendID);
  query.addBindValue(myID);
  query.addBindValue(fileName);

  query.exec();

  errorSQLOrder(query, "export-one-message");
  if(!query.isActive())
    {
      qDebug() << "QUERY_FAIL: " << query.lastError().text();
      m_db->close();
      return -1;
    }
  if(0 >= query.numRowsAffected())
    {
      return 0;
    }

  QSqlDatabase::database().commit();
  return 1;

}


/*************************************************
Function Name： exportHistoryMessageAll()
Description: 导出所有消息记录
*************************************************/
int IMDatabaseCtrl::exportHistoryMessageAll(const QString &myID,
                                            const QString &fileName)
{
  if(!createConnection())
    {
      return -1;
    }
  //sql事务
  QSqlDatabase::database().transaction();

  QString queryStr = EXPORT_ALL_MESSAGE_IN_HISTORY;
  queryStr.replace("##HISTORY##MESSAGE##", m_localHistoryTableName);

  QSqlQuery query(*m_db);
  query.prepare(queryStr);

  query.addBindValue(myID);
  query.addBindValue(myID);
  query.addBindValue(fileName);

  query.exec();

  errorSQLOrder(query, "export-all-message");
  if(!query.isActive())
    {
      qDebug() << "QUERY-FAIL: " << query.lastError().text();
      m_db->close();
      return -1;
    }

  if(0 >= query.numRowsAffected())
    {
      return 0;
    }

  QSqlDatabase::database().commit();
  return 1;
}

/*************************************************
Function Name： deleteHistoryMessageOneFriend()
Description: 删除单个好友消息记录
*************************************************/
int IMDatabaseCtrl::deleteHistoryMessageOneFriend(const QString myID,
                                                  const QString friendID)
{
  if(!createConnection())
    {
      return -1;
    }
  //sql事务
  QSqlDatabase::database().transaction();

  QString queryStr = DELETE_ONE_MESSAGE_IN_HISTORY;
  queryStr.replace("##HISTORY##MESSAGE##", m_localHistoryTableName);

  QSqlQuery query(*m_db);
  query.prepare(queryStr);

  query.addBindValue(myID);
  query.addBindValue(friendID);
  query.addBindValue(friendID);
  query.addBindValue(myID);

  query.exec();

  errorSQLOrder(query, "delete-one-message");

  if(!query.isActive())
    {
      qDebug() << "QUERY_FAIL: " << query.lastError().text();
      m_db->close();
      return -1;
    }
  if(0 >= query.numRowsAffected())
    {
      return 0;
    }
  QSqlDatabase::database().commit();

  return 1;
}

/*************************************************
Function Name： deleteHistoryMessageAll()
Description: 删除所有好友消息记录
*************************************************/
int IMDatabaseCtrl::deleteHistoryMessageAll(const QString myID)
{
    if (!createConnection())
    {
        return -1;
    }
    //sql 事务
    QSqlDatabase::database().transaction();

    QString queryStr = DELETE_ALL_MESSAGE_IN_HISTORY;
    queryStr.replace("##HISTORY##MESSAGE##", m_localHistoryTableName);
    QSqlQuery query(*m_db);
    query.prepare(queryStr);
    query.addBindValue(myID);
    query.addBindValue(myID);
    query.exec();

    errorSQLOrder(query, "delete-all-message");
    if (!query.isActive())
    {
        qDebug() << "QUERY_FAIL::" << query.lastError().text();
        m_db->close();
        return -1;
    }

    if (0 >= query.numRowsAffected())
    {
        return 0;
    }

    QSqlDatabase::database().commit();


    return 1;
}


// mark: private----------------------------------------------------

/*************************************************
Function Name： errorSQLOrder()
Description: 检查SQL语句的正确性
*************************************************/
void IMDatabaseCtrl::errorSQLOrder(QSqlQuery query, QString mark)
{
  //如果SQL语句错误，就弹出警告信息
  if(!query.isActive())
    {
      QString str = query.lastError().text() + "\n" + mark;
      qDebug() << "Mysql query error: " << str;
      qDebug() << "query text: " << query.lastQuery();
    }
}

/*************************************************
Function Name： createTable()
Description: 创建数据库表格
*************************************************/
void IMDatabaseCtrl::createTable()
{
  QString queryStr = CREATE_TABLE_HISTORYMESSAGE_SQL;
  queryStr.replace("##HISTORY##MESSAGE##", m_localHistoryTableName);
  QSqlQuery query(*m_db);
  query.exec();
  errorSQLOrder(query, "createTable-historyMessage");

}
