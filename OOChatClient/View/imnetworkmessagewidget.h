#ifndef IMNETWORKMESSAGEWIDGET_H
#define IMNETWORKMESSAGEWIDGET_H

#include "IMMessageWidget.h"

class IMMainWidget;

class IMNetworkMessageWidget : public IMMessageWidget
{
    Q_OBJECT
public:
    IMNetworkMessageWidget(IMMainWidget *, QWidget *parent = 0);
    ~IMNetworkMessageWidget();

signals:

public slots:
    // 删除某日的信息记录
    void onClickDeleteDate();
    // 删除某好友信息记录
    void onClickDeleteOne();
    // 删除全部消息记录
    void onClickDeleteAll();
    // 导出某好友消息记录
    void onClickExportOne();
    // 导出所有消息记录
    void onClickExportAll();
    // 获取消息记录
    void onClickGetMessage();
private:
    IMMainWidget * m_mainWidget;
};

#endif // IMNETWORKMESSAGEWIDGET_H
