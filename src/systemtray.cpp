#include "systemtray.h"
#include <QApplication>
#include <QDebug>
#include <QMenu>

SystemTray::SystemTray(QObject *parent)
    : QObject(parent), tray_menu(new QMenu), tray(new QSystemTrayIcon) {

  QAction *show = new QAction("Show/Hide", this);
  QAction *exit = new QAction("Exit", this);

  tray_menu->addAction(show);
  tray_menu->addAction(exit);

  tray->setIcon(QIcon(":/sshout.png"));
  tray->setVisible(true);
  tray->setContextMenu(tray_menu);

  connect(this->tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
          this, SLOT(trayEvent(QSystemTrayIcon::ActivationReason)));
  connect(exit, SIGNAL(triggered()), this, SLOT(exit()));
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

void SystemTray::exit() { QApplication::exit(); }
