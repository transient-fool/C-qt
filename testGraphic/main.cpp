#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QTimer>
#include <QPainter>

class MovingItem : public QGraphicsRectItem {
public:
    MovingItem() : QGraphicsRectItem(-50, -30, 100, 60) {
        setBrush(Qt::blue);
        setFlag(ItemIsMovable);  // 使项可移动
        setFlag(ItemSendsGeometryChanges);  // 使项在位置变化时发送通知
    }

    QRectF boundingRect() const override {
        // 定义图形项的边界矩形
        return QRectF(-50, -30, 100, 60);
    }

    void advance(int phase) override {
        // 如果 phase 为 1，更新图形项的状态
        if (phase == 1) {
            // 简单的移动逻辑：向右移动 1 个单位
            moveBy(1, 0);

            // 检测与场景边界的碰撞
            if (x() > 300) {
                setX(-300);  // 如果超出场景右边界，重新定位到左边
            }
        }
    }

    bool collidesWithItem(const QGraphicsItem *other, Qt::ItemSelectionMode mode) const override {
        // 检测与其他图形项的碰撞
        return QGraphicsRectItem::collidesWithItem(other, mode);
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override {
        // 绘制图形项
        painter->setBrush(brush());
        painter->drawRect(rect());
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QGraphicsScene scene;
    MovingItem *movingItem = new MovingItem();
    scene.addItem(movingItem);

    QGraphicsView view(&scene);
    view.setScene(&scene);
    view.setRenderHint(QPainter::Antialiasing);
    view.setRenderHint(QPainter::SmoothPixmapTransform);
    view.show();

    QTimer timer;
    QObject::connect(&timer, &QTimer::timeout, [&]() {
        scene.advance();  // 更新场景中所有图形项的状态
    });
    timer.start(16);  // 大约每 16 毫秒更新一次（约 60 FPS）

    return app.exec();
}
