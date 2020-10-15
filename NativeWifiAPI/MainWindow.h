#pragma once

#include <QtWidgets/QDialog>
#include <QGroupBox>
#include <QPushButton>
#include <QLabel>
#include <QTimer>
#include "ui_MainWindow.h"
#include "nativeWifiConnect.h"
#include "QLineEditPassword.h"

class MainWindow : public QDialog
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = Q_NULLPTR);
	~MainWindow();	


public slots:
	void on_btnRefreshWIFI_clicked();
	void slot_btnWlanOpenControl(bool checked);
	void slot_btnConnectControl_clicked();
	void slot_itemClicked(QTreeWidgetItem *item, int column);
	void slot_refresh_timeout();
private:
	bool show_WLAN();
	void initWifiIcon();
	void createConnectWidget(QGroupBox* connectGroupB,bool isConnect);
	void createPasswordWidget(QGroupBox* passwordGroupB);
	void removeConnectItem();

    Ui::MainWindowClass ui;		
	NativeWifiConnect mNativeWifi;
	QTimer* mRefresh_timer = nullptr;
	std::map<std::string, int> mWifiMap;
	std::string mCurrentConnectedWifi="";
	QString mSelectWifiName = "";
	QStringList mWlan_slant_icon_files;
	QPushButton* btnConnectControl = NULL;
	QLabel* wifiState_Lab = NULL;
	QLineEditPassword* passwordLineEdit = NULL;
};
