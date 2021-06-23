#include "systemtray.h"
#include <QDebug>
#include <QMenu>

SystemTray::SystemTray(QObject *parent)
    : QObject(parent), tray_menu(new QMenu), tray(new QSystemTrayIcon) {

  tray_menu->addAction(new QAction("Show/Hide", this));
  tray_menu->addAction(new QAction("Exit", this));

  tray->setIcon(QIcon(":/sshout.png"));
  tray->setVisible(true);
  tray->setContextMenu(tray_menu);

  connect(this->tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
          this, SLOT(trayEvent(QSystemTrayIcon::ActivationReason)));
}

SystemTray::~SystemTray() { delete this->tray_menu; }

const QAction *SystemTray::exitAction() {
  foreach (QAction *action, this->tray_menu->actions()) {
    if (action->text() == "Exit")
      return action;
  }
  return nullptr;
}

const QAction *SystemTray::showAction() {
  foreach (QAction *action, this->tray_menu->actions()) {
    if (action->text() == "Show/Hide")
      return action;
  }
  return nullptr;
}

void SystemTray::trayEvent(QSystemTrayIcon::ActivationReason r) {
  qDebug() << "SystemTray::trayEvent";
  switch (r) {
  case QSystemTrayIcon::DoubleClick: {
    foreach (QAction *action, this->tray_menu->actions()) {
      if (action->text() == "Show/Hide") {
        action->trigger();
      }
      break;
    }
  case QSystemTrayIcon::Trigger: {
    this->tray->show();
    break;
  }
  default:
    break;
  }
  }
}

void SystemTray::show() { this->tray->show(); }
