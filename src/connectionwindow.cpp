/*
 * Copyright 2015-2018 Rivoreo
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 */

#include "connectionwindow.h"
#include "mainwindow.h"
#include "serverinformation.h"
#include "settingsdialog.h"
#include "systemtray.h"
#include "ui_connectionwindow.h"
#include <qobject.h>
#if QT_VERSION < 0x050000
#include <QtGui/QCompleter>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#else
#include <QtWidgets/QCompleter>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
#endif
#include <QAction>
#include <QCloseEvent>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QList>
#include <QtCore/QSettings>

ConnectionWindow::ConnectionWindow(QWidget *parent, QSettings *config)
    : QDialog(parent), ui(new Ui::ConnectionWindow), close_(0) {
  ui->setupUi(this);
  QCompleter *completer = new QCompleter(this);
  completer->setCompletionMode(QCompleter::PopupCompletion);
  ui->remote_host_comboBox->setCompleter(completer);
  this->config = config;
#if 0
	QStringList server_list = config->value("ServerList").toStringList();
	if(!server_list.isEmpty()) {
		int index = config->value("LastServerIndex", 0).toInt();
		ui->remote_host_comboBox->addItems(server_list);
		ui->remote_host_comboBox->setCurrentIndex(index);
		QList<QVariant> port_list = config->value("PortList").toList();
		if(port_list.count() > index) {
			bool ok;
			int port = port_list[index].toInt(&ok);
			if(ok) ui->remote_port_lineEdit->setText(QString(ok));
		}
		QStringList id_list = config->value("IdentifyList").toStringList();
		if(id_list.count() > index) {
			ui->identify_file_lineEdit->setText(id_list[index]);
		}
	}
#else
  server_list = config->value("ServerList").toList();
  if (!server_list.isEmpty()) {
    foreach (const QVariant &i, server_list) {
      const QString &host = i.value<ServerInformation>().host;
      ui->remote_host_comboBox->addItem(host);
    }
    int index = config->value("LastServerIndex", 0).toInt();
    if (index < 0 || index >= server_list.count())
      index = 0;
    const ServerInformation &info =
        server_list[index].value<ServerInformation>();
    ui->remote_host_comboBox->setCurrentIndex(index);
    ui->remote_port_lineEdit->setText(QString::number(info.port));
    ui->identify_file_lineEdit->setText(info.identify_file);
  }
#endif
  remote_host_name_change_event(ui->remote_host_comboBox->currentText());
  ui->checkBox_auto_connect->setChecked(
      config->value("AutoConnect", false).toBool());
}

ConnectionWindow::~ConnectionWindow() { delete ui; }

static QString ssh_config_dir() {
#ifdef Q_OS_WINCE
  return QApplication::applicationDirPath();
#else
  return QDir::homePath() + "/.ssh";
#endif
}

void ConnectionWindow::browse_identity_file() {
  QFileDialog d(this, tr("Choose the identity"), ssh_config_dir());
  d.setAcceptMode(QFileDialog::AcceptOpen);
  d.setFileMode(QFileDialog::ExistingFile);
  d.setOption(QFileDialog::DontUseNativeDialog);
  if (d.exec()) {
    ui->identify_file_lineEdit->setText(d.selectedFiles()[0]);
  }
}

void ConnectionWindow::change_settings() {
  SettingsDialog d(this, config);
  d.set_current_tab(0);
  d.exec();
}

void ConnectionWindow::closeEvent(QCloseEvent *e) {
  if (!this->close_) {
    qDebug("function: ConnectionWindow::closeEvent(%p)", e);
    this->hide();
    e->ignore();
  }
}

void ConnectionWindow::start_main_window() {
  qDebug("slot: ConnectionWindow::start_main_window()");
  QString comm = ui->remote_host_comboBox->currentText();
  QUrl url("ssh://" + comm);
  QString host = url.host();
  QString user = url.userName();
  if (host.isEmpty()) {
    QMessageBox::critical(this, tr("Check Server Information"),
                          tr("Host name cannot be empty"));
    return;
  }
  quint16 port;
  const QString &port_str = ui->remote_port_lineEdit->text();
  if (port_str.isEmpty()) {
    port = 22;
    ui->remote_port_lineEdit->setText("22");
  } else {
    bool ok;
    port = port_str.toUInt(&ok);
    if (!ok) {
      QMessageBox::critical(this, tr("Check Server Information"),
                            tr("Invalid port number"));
      return;
    }
  }
  const QString &identify_file = ui->identify_file_lineEdit->text();

  bool found = false;
  foreach (const QVariant &i, server_list) {
    ServerInformation info = i.value<ServerInformation>();
    if (info.host == comm) {
      if (info.port == port && info.identify_file == identify_file)
        found = 1;
      else
        server_list.removeOne(i);
      break;
    }
  }
  if (!found) {
    ServerInformation info;
    info.host = comm;
    info.port = port;
    info.identify_file = identify_file;
    QVariant v = QVariant::fromValue<ServerInformation>(info);
    server_list << v;
    config->setValue("ServerList", server_list);
  }
  int index = ui->remote_host_comboBox->currentIndex();
  if (index >= 0)
    config->setValue("LastServerIndex", index);
  config->setValue("AutoConnect", ui->checkBox_auto_connect->isChecked());
  MainWindow *w = new MainWindow(NULL, config, host, port, identify_file);
  w->setAttribute(Qt::WA_DeleteOnClose);
  w->setSystemTray(SystemTray::instance());
  if (user != "")
    w->set_ssh_user(user);
  w->connect_ssh();
  w->show();
  accept();
  this->exit();
}

void ConnectionWindow::remote_host_name_change_event(int index) {
  if (index < 0 || index >= server_list.count())
    return;
  ServerInformation info = server_list[index].value<ServerInformation>();
  if (ui->remote_host_comboBox->currentText() != info.host)
    return;
  ui->remote_port_lineEdit->setText(QString::number(info.port));
  ui->identify_file_lineEdit->setText(info.identify_file);
}

void ConnectionWindow::remote_host_name_change_event(QString host_name) {
  ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!host_name.isEmpty());
}

void ConnectionWindow::exit() {
  this->close_ = true;
  this->close();
}

void ConnectionWindow::showOrHide() {
  if (this->isHidden())
    this->show();
  else
    this->hide();
}

void ConnectionWindow::setSystemTray(SystemTray *tray) {
  connect(tray->exitAction(), SIGNAL(triggered()), this, SLOT(exit()));
  connect(tray->showAction(), SIGNAL(triggered()), this, SLOT(showOrHide()));
}
