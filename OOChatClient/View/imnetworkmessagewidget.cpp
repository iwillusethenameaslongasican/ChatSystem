#include "IMNetworkMessageWidget.h"
#include "IMMainWidget.h"
#include "CustomLineEdit.h"
#include <QMessageBox>
#include <QDebug>
#include <QModelIndex>
#include <QListView>
#include <QTextBrowser>
#include <QFileDialog>

IMNetworkMessageWidget::IMNetworkMessageWidget(IMMainWidget *mainWidget, QWidget *parent)
  : IMMessageWidget(mainWidget, parent), m_mainWidget(mainWidget)
{
  if(m_mainWidget == NULL)
    {
      return;
    }
  setShowMode(NETWORK);
}

IMNetworkMessageWidget::~IMNetworkMessageWidget()
{

}

// public slots : ------------------------------------------------

/****************************************************
 * function name: onClickDeleteDate
 * Description: 删除某日消息记录
 * **********************************************/
void IMNetworkMessageWidget::onClickDeleteDate()
{

}

/**********************************************
 * function name: onClickDeleteOne
 * Description: 删除某好友信息记录
 * *****************************************/
void IMNetworkMessageWidget::onClickDeleteOne()
{

}

/**********************************************
 * function name: onclickDeleteAll
 * Description: 删除全部消息记录
 * ******************************************/
void IMNetworkMessageWidget::onClickDeleteAll()
{

}

/********************************************
 * function name: onClickExportOne
 * Description: 导出当前好友消息记录
 * ***************************************/
void IMNetworkMessageWidget::onClickExportOne()
{
  if(m_indexOfFriendList < 0)
    {
      QString temp = QString(tr("请先获取消息记录！"));
      QMessageBox::information(NULL, tr("导出聊天记录"), temp);
      return;
    }
  if(m_tbMessageShow->toPlainText().isEmpty())
    {
      QString temp = QString(tr("消息记录为空！"));
      QMessageBox::information(NULL, tr("导出消息记录"), temp);
      return;
    }

  QString history = NETWORK_HISTORY_MESSAGE_PATH;
  QDir dir = QDir::current();
  if(!dir.exists(history))
    {
      dir.mkpath(history);
    }

  QString filePath = history.append(QDateTime::currentDateTime().toString("yyyy-MM-dd_hh_mm_ss"));

  QString filter = QString(tr("TXT文件 (*.txt)"));
  QString name = QFileDialog::getSaveFileName(this, tr("保存文件"), filePath,
                                              filter);
  if(name.isEmpty())
    {
      return;
    }
  QFile file(name);
  bool isOpen = file.open(QFile::WriteOnly | QFile::Text);

  if(!isOpen)
    {
      QMessageBox::warning(this, tr("保存文件"),
                           tr("无法保存文件：\n%1").arg(file.errorString()));
    }
  QTextStream out(&file);

  QString talkRecord = m_tbMessageShow->toPlainText();
  out << talkRecord;
  file.close();
}

/********************************************
 * function name: onClickExportAll
 * Description: 导出所有消息记录
 * ***************************************/
void IMNetworkMessageWidget::onClickExportAll()
{

}

/**********************************************
 * function name: onClickGetMessage
 * Description: 获取消息记录
 * *****************************************/
void IMNetworkMessageWidget::onClickGetMessage()
{
  int row = m_listViewFriend->currentIndex().row();
  if(row < 0)
    {
      QString temp = QString(tr("请先选择好友！"));
      QMessageBox::information(NULL, tr("显示聊天记录"), temp);
      return;
    }
  m_date = m_leBirthday->getDate();
  qDebug() << "get date" << m_date;
  m_mainWidget->getNetworkHistoryMessage(m_indexOfFriendList, m_date);
}
