#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QWidget>
#include <QtGui/QPainter>

class CircleDrawer : public QMainWindow {
public:
    CircleDrawer() {
        setGeometry(100, 100, 400, 400);
        setWindowTitle("Hollow Circle Drawer");

        centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);
    }

    void paintEvent(QPaintEvent *) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);

        int radius = 100;
        int x = width() / 2 - radius;
        int y = height() / 2 - radius;

        painter.setPen(Qt::black);
        painter.setBrush(Qt::NoBrush);  // Set brush to NoBrush for hollow circle
        painter.drawEllipse(x, y, 2 * radius, 2 * radius);
    }

private:
    QWidget* centralWidget;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    CircleDrawer window;
    window.show();

    return app.exec();
}
