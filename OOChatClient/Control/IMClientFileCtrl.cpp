/*********************************************
File Name： IMClientFileCtrl.cpp
Author： hanyan
Date： 2016.11.4
Description： xml文件读写
Changes：
********************************************/

#include "IMClientFileCtrl.h"
#include <QDataStream>
#include <QDir>
#include <QFile>
#include <qdebug.h>
#include <QRegExp>
#include <QTextDocument>
#include <QTextBrowser>

IMClientFileCtrl::IMClientFileCtrl(QObject *parent)
  : QObject(parent)
{

}


/*************************************************
Function Name： writeAutoLogin()
Description: 写自动登录的文件
Input： 文件路径,帐号,密码,是否自动登录
*************************************************/
bool IMClientFileCtrl::writeAutoLogin(const QString &fileName,
                                      const QString &id,
                                      const QString &pwd,
                                      const bool &isAuto)
{
  QFile file(fileName);
  qDebug() << "fileName: " << fileName;
  if(!file.open(QIODevice::WriteOnly))
    {
      qDebug() << "Cannot open file for writing: "
               << qPrintable(file.errorString()) << endl;
      return false;
    }

  QDataStream out(&file);
  out.setVersion(QDataStream::Qt_4_8);
  out << id << pwd << isAuto;
  file.close();
  return true;
}


/*************************************************
Function Name： readAutoLogin()
Description: 读自动登录的xml文件
Input： 文件路径,帐号,密码
Output： bool
Changes： NULL
*************************************************/
bool IMClientFileCtrl::readAutoLogin(const QString &fileName, QString &id, QString &pwd, bool &isAuto)
{
  QFile file(fileName);
  if(!file.open(QIODevice::ReadOnly))
    {
      qDebug() << "Cannot open file for reading: "
               << qPrintable(file.errorString()) << endl;
      return false;
    }
  QDataStream in(&file);
  in.setVersion(QDataStream::Qt_4_8);
  in >> id >> pwd >> isAuto;
  file.close();
  return true;
}
/*************************************************
Function Name： remove()
Description: 删除文件
Input： 文件路径
Output： bool
Changes： NULL
*************************************************/
bool IMClientFileCtrl::remove(const QString &filename)
{
  QFile file(filename);
  return file.remove() ? true : false;
}

/*************************************************
Function Name： simplifyRichText()
Description: 将html格式的qstring转换
*************************************************/
QString IMClientFileCtrl::simplifyRichText(QString richText)
{
  QTextDocument doc;
  doc.setHtml(richText);
  qDebug() << "say: " << doc.toPlainText();
  return doc.toPlainText();
}

/*************************************************
Function Name： saveTalkHistory()
Description: 保存历史记录
*************************************************/
void IMClientFileCtrl::saveTalkHistory(const QString filePath, const QString fileName, const QTextBrowser *messageShow)
{
  QDir dir = QDir::current();
  //目录是否存在
  /*QDir的mkdir和mkpath区别
   * mkdir：上层目录不存在时，创建会失败。比如创建“c:\\test\test”，如果test不存在，那test也创建不了。目录已经存在时会返回false。
  * mkpath：上层目录不存在也没关系，自动一起创建。目录已经存在时会返回true。
  */
  if(!dir.exists(filePath))
    {
      dir.mkpath(filePath);
    }

  QString fileLoad = QString("%1/%2").arg(filePath).arg(fileName);

  QFile file(fileLoad);
  bool isOpen = file.open(QIODevice::WriteOnly | QIODevice::Append);
  if(!isOpen)
    {
      qDebug() << "fail to open file: " << fileLoad;
      return;
    }

  QDataStream out(&file);
  out.setVersion(QDataStream::Qt_4_8);
  //如果聊天记录是空的，就不用保存了
  //QTextBrowser是继承自QTextEdit，而且仅仅是只读的，对里面的内容并不能进行更改
  if(messageShow->toPlainText().isEmpty())
    {
      qDebug() << "messageShow->toPlainText().isEmpty()";
      return;
    }

  QString talkRecord = messageShow->toHtml();
  out << talkRecord;
  file.close();
}

/*************************************************
Function Name： readTalkHistory()
Description: 读取历史记录
*************************************************/
void IMClientFileCtrl::readTalkHistory(const QString filePath, const QString fileName, QTextBrowser *messageShow)
{
  QDir dir = QDir::current();
  if(!dir.exists(filePath))
    {
      dir.mkpath(filePath);
    }
  QString fileLoad = QString("%1/%2").arg(filePath).arg(fileName);

  QFile file(fileLoad);
  if(!file.open(QIODevice::ReadOnly))
    {
      return;
    }

  QString allRecord;
  QDataStream in(&file);
  in.setVersion(QDataStream::Qt_4_8);
  QString tempStr;
  //从文件中读取全部的字符串
  while(!file.atEnd())
    {
      in >> tempStr;
      allRecord += tempStr;
    }
  file.close();
  messageShow->setText(allRecord);
}
