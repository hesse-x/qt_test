#include "Game/Engine/Engine.h"
#include "Game/Entry/MainWindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  auto *game0 = Engine::create(Engine::Type::game0);
  MainWindow window(game0);
  window.show();
  return app.exec();
}
