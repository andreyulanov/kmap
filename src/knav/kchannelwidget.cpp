#include "kchannelwidget.h"
#include "kchannelmanager.h"
#include "kswitch.h"
#include <QPushButton>
#include <QScrollArea>

KChannelWidget::KChannelWidget(QSize s)
{
  if (s.isEmpty())
    s = QSize(480, 800);
  setFixedSize(s);
  setLayout(&root_layout);
  root_layout.setAlignment(Qt::AlignCenter);
}

void KChannelWidget::addItem(int& posy, QString text, bool enabled)
{
  QPushButton* b = new QPushButton(this);
  b->setFixedSize(width() - 100, 100);
  b->move(0, posy);
  b->setText(text);
  KSwitch* sw = new KSwitch(this);
  sw->setFixedSize(100, 30);
  sw->move(width() - 100, posy);
  sw->setEnabled(enabled);
  posy += b->height();
}

void KChannelWidget::show(KChannel::Direction dir)
{
  auto list = getChannels();

  int posy = 0;
  if (dir == KChannel::Out)
    addItem(posy, "ВСЕ", true);

  for (auto ch: list)
  {
    if (ch.dir != dir)
      continue;
    addItem(posy, getUserNameById(ch.id), ch.enabled);
  }
  QWidget::show();
}

void KChannelWidget::showIn()
{
  show(KChannel::In);
}

void KChannelWidget::showOut()
{
  show(KChannel::Out);
}
