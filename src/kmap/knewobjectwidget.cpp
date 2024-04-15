#include "knewobjectwidget.h"
#include <QPushButton>
#include <QIcon>

KNewObjectWidget::KNewObjectWidget()
{
  setLayout(&root_layout);
  root_layout.setAlignment(Qt::AlignCenter);
  root_layout.addSpacerItem(new QSpacerItem(
      0, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));

  QVBoxLayout* options_layout = new QVBoxLayout(&options_group_box);
  send_check_box = new QCheckBox("Send", &options_group_box);
  options_layout->addWidget(send_check_box);
  options_group_box.setLayout(options_layout);
  options_group_box.setAlignment(Qt::AlignLeft);
  options_group_box.setSizePolicy(QSizePolicy::Expanding,
                                  QSizePolicy::Maximum);
  root_layout.addWidget(&options_group_box);
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
  if (send_check_box->isChecked())
    emit sendObject();
  else
    emit doNotSendObject();
  hide();
}

void KNewObjectWidget::show()
{
  auto list = getUserShapeImageList();
  int  posy = 0;
  for (auto l: list)
  {
    QIcon icon = QPixmap::fromImage(l.image);
    addItem(posy, l.image, l.id);
  }
  QWidget::show();
}
