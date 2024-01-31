#include "Game/Engine/Item/Item.h"
#include <QtGui/QPainter>
void ItemBase::paint(QPainter &painter) const { painter.drawRect(*this); }

void Brick::setPaintParam(QPainter &painter) {
  painter.setBrush(Qt::BDiagPattern);
}

void Ball::paint(QPainter &painter) const { painter.drawEllipse(*this); }

void Ball::setPaintParam(QPainter &painter) {
  painter.setBrush(Qt::Dense4Pattern);
}
