#include <QColor>
#include <QQmlPropertyMap>

namespace paddock
{
namespace ui
{
class Styling : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QQmlPropertyMap* colors MEMBER _colors NOTIFY changed)
    Q_PROPERTY(QQmlPropertyMap* sizes MEMBER _sizes NOTIFY changed)

public:
    Styling();

    Q_INVOKABLE static QColor hovered(QColor color);

signals:
    void changed(); // Not used, to avoid warning

private:
    qreal _scaling{1};
    QQmlPropertyMap* _colors{nullptr};
    QQmlPropertyMap* _sizes{nullptr};
};
}
}
