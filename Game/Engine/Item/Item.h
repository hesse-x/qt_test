#include <QtCore/QPoint>
#include <QtCore/QRect>

class QPainter;

class ItemBase : public QRect {
public:
  using QRect::QRect;
  void paint(QPainter &painter) const;
};

class DynamicItemBase : public ItemBase {
public:
  DynamicItemBase() = default;
  DynamicItemBase(int x, int y, int w, int h, int ux = 0, int uy = 0)
      : ItemBase(x, y, w, h), dx(ux, uy) {}
  void update() { moveTo(dx + topLeft()); }
  int ux() const { return dx.x(); }
  int uy() const { return dx.y(); }
  int &rux() { return dx.rx(); }
  int &ruy() { return dx.ry(); }

protected:
  QPoint dx;
};

class Board : public DynamicItemBase {
public:
  using DynamicItemBase::DynamicItemBase;
  static void setPaintParam(QPainter &painter);
};

class Brick : public ItemBase {
public:
  using ItemBase::ItemBase;
  static void setPaintParam(QPainter &painter);
};

class Ball : public DynamicItemBase {
public:
  Ball() = default;
  Ball(int x, int y, int r, int ux = 0, int uy = 0)
      : DynamicItemBase(x, y, 2 * r, 2 * r, ux, uy) {}
  static void setPaintParam(QPainter &painter);
  void paint(QPainter &painter) const;
};
