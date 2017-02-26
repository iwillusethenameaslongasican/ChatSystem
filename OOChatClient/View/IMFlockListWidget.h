/********************************
 * 主面板上的群列表面板
 * hanyan
 * 2016.11.19
 * ****************************/

#ifndef IMFLOCKLISTWIDGET_H
#define IMFLOCKLISTWIDGET_H

#include <QWidget>

class QVBoxLayout;
class QScrollArea;
class QMenu;
class IMMainWidget;


class IMFlockListWidget : public QWidget
{
  Q_OBJECT
public:
  IMFlockListWidget(IMMainWidget *mainWidget, QWidget *parent = 0);
  void addItem(QWidget *item);
  void removeItem(QWidget *item);

  void createMenu();

signals:

public slots:
  void showMenu();

protected:
  void mousePressEvent(QMouseEvent *event);

private:
  QVBoxLayout *m_layout;
  QWidget *m_contentsWidget;
  QScrollArea *m_flocksScrollArea;

  //右键菜单
  QMenu *m_menu;

  IMMainWidget *m_mainWidget;
};


#endif // IMFLOCKLISTWIDGET_H
