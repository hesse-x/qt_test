#include <memory>

#include <QtCore/QPoint>
#include <QtCore/QTimer>
#include <QtGui/QKeyEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>
#include <QtWidgets/QWidget>

#include "Game/Engine/Engine.h"
#include "Game/Entry/MainWindow.h"

class QKeyEvent;

MainWindow::~MainWindow() = default;
MainWindow::MainWindow(Engine *engine) : engine(engine) {
  setGeometry(100, 100, 400, 400);
  setWindowTitle("Game");

  centralWidget = std::make_unique<QWidget>(this);
  setCentralWidget(centralWidget.get());

  // Initialize circle position.
  engine->init(width(), height(), timeInterval, &pressedKeys, &mousePos);

  timer = std::make_unique<QTimer>(this);
  connect(timer.get(), &QTimer::timeout, this, &MainWindow::update);
  timer->start(timeInterval);

  setMouseTracking(true);
  grabKeyboard();
}

void MainWindow::paintEvent(QPaintEvent *event) {
  QMainWindow::paintEvent(event);
  QPainter painter(this);
  engine->paint(painter);
}

void MainWindow::resizeEvent(QResizeEvent *event) {
  // 在窗口缩放时执行的代码
  // QSize newSize = event->size();

  // 继续调用基类的事件处理函数，确保其他事件处理逻辑正常执行
  QMainWindow::resizeEvent(event);
  engine->resize(width(), height());
}

void MainWindow::mouseMoveEvent(QMouseEvent *event) {
  // 获取鼠标的全局位置
  // QPoint globalPos = QCursor::pos();

  // 获取鼠标相对于窗口的位置
  QPoint localPos = event->pos();
  mousePos = localPos;
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
  // 记录按下的键
  pressedKeys.insert(event->key());

  // 继续调用基类的事件处理函数，确保其他事件处理逻辑正常执行
  QMainWindow::keyPressEvent(event);
}

void MainWindow::keyReleaseEvent(QKeyEvent *event) {
  // 移除释放的键
  pressedKeys.remove(event->key());

  // 继续调用基类的事件处理函数，确保其他事件处理逻辑正常执行
  QMainWindow::keyReleaseEvent(event);
}

// slot interface
void MainWindow::update() {
  engine->update();
  // Trigger a repaint to update the window
  repaint();
}
