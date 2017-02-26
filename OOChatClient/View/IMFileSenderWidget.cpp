#include "IMFileSenderWidget.h"
#include <QProgressBar>
#include <QPushButton>
#include <QLabel>
#include <QFileDialog>
#include <QTcpServer>
#include <QTcpSocket>
#include <QFile>
#include <QMessageBox>
#include <QVBoxLayout>

IMFileSenderWidget::IMFileSenderWidget(QWidget *parent)
    : QWidget(parent)
{
    m_isStopped = true;
    initIMFileSenderWidget();
    setWindowTitle(tr("发送文件"));
}

IMFileSenderWidget::~IMFileSenderWidget()
{
    onClickBtnClose();
}

/****************************************
 * function name: initSender
 * Description: 初始化
 * ************************************/
void IMFileSenderWidget::initSender()
{
    m_isStopped = true;
    m_loadSize = 4 * 1024;
    m_totalBytes = 0;
    m_bytesWritten = 0;
    m_bytesToWrite = 0;
    m_barFile->reset();
    m_fileSender->close();
}

/************************************
 * function name: showRefused
 * Description: 被拒绝时触发 聊天窗口调用
 * ***********************************/
void IMFileSenderWidget::showRefused()
{
    m_isStopped = true;
    m_fileSender->close();
    m_labelFileStatus->setText(tr("对方拒接接收！"));
}

/**********************************************
 * function name: startSending
 * Description: 初始化文件服务器，监听对方
 * ******************************************/
bool IMFileSenderWidget::startSending(const QString &file)
{
    m_isStopped = false;
    m_filePath = file;
    m_fileName = m_filePath.right(m_filePath.size() - m_filePath.lastIndexOf('/') - 1);
    m_labelFileName->setText(QString(tr("文件名：%1")).arg(m_fileName));

    //开始监听
    if(!m_fileSender->listen(QHostAddress::Any, m_tcpPort))
        {
            qDebug() << "file server error: " << m_fileSender->errorString();
            QMessageBox::critical(NULL, tr("OO文件传输"),
                                  tr("Unable to start file server: %1")
                                  .arg(m_fileSender->errorString()));
            close();
            return false;
        }
    m_labelFileStatus->setText(tr("等待对方接受"));
    return true;
}

/*******************************************
 * function name: isStopped
 * Description: 是否停止
 * *******************************************/
bool IMFileSenderWidget::isStopped()
{
    return m_isStopped;
}

//private:--------------------------------------------------------------

/********************************************
 * function name: initIMFileSenderWidget
 * Description: 初始化文件发送界面
 * ******************************************/
void IMFileSenderWidget::initIMFileSenderWidget()
{
    m_labelFileName = new QLabel(tr("文件名:"), this);
    m_labelFileStatus = new QLabel(tr("等待对方接受"), this);
    m_labelFileStatus->setFixedHeight(80);
    m_barFile = new QProgressBar(this);
    m_btnCancel = new QPushButton(tr("取消"), this);
    m_btnClose = new QPushButton(tr("关闭"), this);
    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->addWidget(m_btnCancel);
    hlayout->addWidget(m_btnClose);
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(m_labelFileName);
    layout->addWidget(m_barFile);
    layout->addWidget(m_labelFileStatus);
    layout->addLayout(hlayout);
    setLayout(layout);
    setFixedSize(400,200);
    m_tcpPort = 6666;

    m_fileSender = new QTcpServer(this);
    connect(m_fileSender, SIGNAL(newConnection()),
            this, SLOT(sendMessage()));
    connect(m_btnCancel, SIGNAL(clicked()),
            this, SLOT(onClickBtnCancel()));
    connect(m_btnClose, SIGNAL(clicked()),
            this, SLOT(onClickBtnClose()));
}

// private slots:-------------------------------------------------------

/*************************************************
Function Name： onClickBtnCancel()
Description: 取消发送
*************************************************/
void IMFileSenderWidget::onClickBtnCancel()
{
    if(m_fileSender != NULL && m_fileSender->isListening())
        {
            if(m_clientConnection != NULL)
                {
                    m_clientConnection->abort();
                }
            m_fileSender->close();
        }
    m_isStopped = true;
}

/*************************************************
 * function name: onClickBtnClose
 * Description: 点击退出
 * *************************************************/
void IMFileSenderWidget::onClickBtnClose()
{
    onClickBtnCancel();
    close();

}

/************************************************
 * funcrion name: sendMessage
 * Description: 当有新连接时触发，发送文件
 * ****************************************/
void IMFileSenderWidget::sendMessage()
{
    qDebug() << "sendMessage";
    m_isStopped = false;
    //nextPendingConnection获取已经建立的连接的Tcp套接字，
    //使用它来完成数据的发送和其它操作
    m_clientConnection = m_fileSender->nextPendingConnection();
      //当有数据发送成功时，我们更新进度条
    connect(m_clientConnection, &QTcpSocket::bytesWritten,
            this, &IMFileSenderWidget::updateClientProgress);
    connect(m_clientConnection, &QTcpSocket::disconnected,
            this, &IMFileSenderWidget::showClosedConnect);

    m_labelFileStatus->setText(tr("开始传送文件！"));

    m_localFile = new QFile(m_filePath);
    if(!m_localFile->open(QFile::ReadOnly))
        {
            //以只读方式打开
            QMessageBox::warning(this, tr("应用程序"), tr("无法读取文件：%1\n%2")
                                 .arg(m_filePath, m_localFile->errorString() ));
            return;
        }
    //文件总大小
    m_totalBytes = m_localFile->size();
    QDataStream sendOut(&m_outBlock, QIODevice::WriteOnly);
     sendOut.setVersion(QDataStream::Qt_4_8);
    //开始计时
    m_time.start();
    QString currentFile = m_filePath.right(m_filePath.size() -
                                           m_filePath.lastIndexOf('/') - 1);
     //依次写入总大小信息空间，文件名大小信息空间，文件名
    sendOut << qint64(0) << qint64(0) << currentFile;
    //这里的总大小是文件名大小等信息和实际文件大小的总和
    m_totalBytes += m_outBlock.size();
    sendOut.device()->seek(0);
    //返回outBolock的开始，用实际的大小信息代替两个qint64(0)空间
    sendOut << m_totalBytes << qint64((m_outBlock.size() - sizeof(qint64) *2));
     //发送完头数据后剩余数据的大小
    m_bytesToWrite = m_totalBytes - m_clientConnection->write(m_outBlock);
    qDebug() << currentFile << m_totalBytes;
    m_outBlock.resize(0);
}

/*************************************************
Function Name： updateClientProgress()
Description: 根据发送情况实时更新进度条
*************************************************/
//当发送成功时就会发出bytesWritten(qint64)信号，
//这时我们执行updateClientProgress(qint64 numBytes)进行文件数据的传输
//和进度条的更新
void IMFileSenderWidget::updateClientProgress(qint64 numBytes)
{
    //已经发送数据的大小
    m_bytesWritten += (int)numBytes;
    //如果已经发送了数据
    if(m_bytesToWrite > 0)
        { //每次发送loadSize大小的数据，这里设置为4KB，如果剩余的数据不足4KB，
            //就发送剩余数据的大小
            m_outBlock = m_localFile->read(qMin(m_bytesToWrite,m_loadSize));
            //发送完一次数据后还剩余数据的大小
            m_bytesToWrite -= (int)m_clientConnection->write(m_outBlock);
            //清空缓冲区
            m_outBlock.resize(0);
        }
    else
        {
            //如果没有发送任何数据，则关闭文件
            m_localFile->close();
        }
    m_barFile->setMaximum(m_totalBytes);
    m_barFile->setValue(m_bytesWritten);

    float userTime = m_time.elapsed();
    double speed = m_bytesWritten / userTime;
    m_labelFileStatus->setText(tr("已发送 %1MB (%2MB/S) \n共%3MB 已用时：%4秒\n估计剩余时间：%5秒")
                               .arg(m_bytesWritten / (1024 * 1024))     //已发送
                               .arg(speed * 1000 / (1024 * 1024), 0, 'f', 2)  //速度
                               .arg(m_totalBytes / (1024 * 1024))  //总大小
                               .arg(userTime / 1000, 0, 'f', 0)     //同时
                               .arg(m_totalBytes / speed / 1000 - userTime / 1000, 0, 'f', 0));//剩余时间
    if(m_bytesWritten == m_totalBytes)
        {
            m_isStopped = true;
            m_labelFileStatus->setText(tr("传送文件成功"));
        }
}

/************************************************
 * function name: showClosedConnect
 * Description: 连接断开
 * **********************************************/
void IMFileSenderWidget::showClosedConnect()
{
    m_isStopped = true;
    m_labelFileStatus->setText(tr("文件传送被中断！"));

    if(m_bytesWritten == m_totalBytes)
        m_labelFileStatus->setText(tr("传送文件成功"));
}
