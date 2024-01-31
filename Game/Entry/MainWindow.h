#include <cstdint>
#include <memory>
#include <optional>

#include <QtCore/QPoint>
#include <QtCore/QSet>
#include <QtWidgets/QMainWindow>

class Engine;
class QWidget;
class QTimer;
class QPaintEvent;
class QMouseEvent;
class QResizeEvent;
class QKeyEvent;

class MainWindow : public QMainWindow {
public:
  MainWindow(Engine *engine);
  ~MainWindow() override;
  void paintEvent(QPaintEvent *) override;

protected:
  void keyPressEvent(QKeyEvent *event) override;
  void keyReleaseEvent(QKeyEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;

private slots:
  void update();

private:
  Engine *engine;
  std::unique_ptr<QWidget> centralWidget;
  std::unique_ptr<QTimer> timer;
  std::optional<QPoint> mousePos;
  QSet<int> pressedKeys;
  uint64_t timeInterval = 16; // 16 milliseconds interval (60 FPS)
};
