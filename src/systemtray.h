#include <QSystemTrayIcon>
#include <qobjectdefs.h>

class QAction;

class SystemTray : public QObject {
  Q_OBJECT
public:
  static SystemTray *instance() noexcept {
    static SystemTray ret;
    return &ret;
  }
  SystemTray(QObject *parent = nullptr);
  ~SystemTray();
  const QAction *exitAction();
  const QAction *showAction();

public slots:
  void exit();

private slots:
  void trayEvent(QSystemTrayIcon::ActivationReason r);

public slots:
  void show();

private:
  QMenu *tray_menu;
  QSystemTrayIcon *tray;
};
