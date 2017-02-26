/*********************************************
File Name： IMClientFileCtrl.h
Author： hanyan
Date： 2016.11.4
Description： xml文件读写
Changes：
********************************************/

#ifndef IMCLIENTFILECTRL_H
#define IMCLIENTFILECTRL_H

#include <QObject>

class QTextBrowser;

class IMClientFileCtrl : public QObject
{
  Q_OBJECT
public:
  explicit IMClientFileCtrl(QObject *parent = 0);
  //写自动登录的文件
  static bool writeAutoLogin(const QString &fileName, const QString &id,
                             const QString &pwd, const bool &isAuto = false);
  //读自动登录的文件
  static bool readAutoLogin(const QString &fileName, QString &id,
                            QString &pwd, bool &isAuto);

  //删除文件
  static bool remove(const QString &filename);

  static QString simplifyRichText(QString richText);

  static void saveTalkHistory(const QString filePath, const QString fileName,
                              const QTextBrowser *messageShow);
  static void readTalkHistory(const QString filePath, const QString fileName,
                              QTextBrowser *messageShow);
signals:

public slots:
private:

};


#endif // IMCLIENTFILECTRL_H
