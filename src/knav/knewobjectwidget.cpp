#include "knewobjectwidget.h"
#include <QPushButton>
#include <QIcon>

KNewObjectWidget::KNewObjectWidget(QSize s)
{
  setLayout(&root_layout);
  root_layout.setAlignment(Qt::AlignCenter);
  setFixedSize(s);
}

void KNewObjectWidget::addItem(int& posy, QImage image, QString id)
{
  QPushButton* icon_button = new QPushButton(this);
  icon_button->setFixedSize(100, 100);
  icon_button->move(0, posy);
  icon_button->setIcon(QPixmap::fromImage(image));
  QPushButton* b = new QPushButton(this);
  b->setFixedSize(width() - 100, 100);
  b->move(100, posy);
  b->setText(id);
  posy += b->height();
  connect(b, &QPushButton::pressed, this,
          &KNewObjectWidget::onSelected);
  button_id_map.insert(b, id);
}

void KNewObjectWidget::onSelected()
{
  auto b  = dynamic_cast<QPushButton*>(sender());
  auto id = button_id_map.value(b);
  selectedShape(getShapeById(id));
  hide();
}

void KNewObjectWidget::show()
{
  auto list = getUserShapeList();
  int  posy = 0;
  for (auto l: list)
  {
    QIcon icon = QPixmap::fromImage(l.image);
    addItem(posy, l.image, l.id);
  }
  QWidget::show();
}
