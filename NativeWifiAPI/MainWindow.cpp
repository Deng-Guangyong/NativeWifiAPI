#include "MainWindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QDebug>
#include <string>
#include <thread>
#include <iostream>
#include <sstream>

//清理指针所指向的内容，并置0
#define Delete_And_Null(any_pointer)\
		if(any_pointer){			\
			delete any_pointer;		\
			any_pointer = 0;		\
		};

#define print_info(str) std::cout<<"[INFO] "<<str<<std::endl 
#define print_error(str) std::cout<<"[ERROR] "<<str<<std::endl 

MainWindow::MainWindow(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	setFixedSize(QSize(461, 529));
	initWifiIcon();
	ui.wifi_treeWidget->setRootIsDecorated(false);
	mRefresh_timer = new QTimer(this);
	connect(mRefresh_timer, &QTimer::timeout, this, &MainWindow::slot_refresh_timeout);
	connect(ui.wifi_treeWidget, SIGNAL(itemClicked(QTreeWidgetItem *, int)),
		this, SLOT(slot_itemClicked(QTreeWidgetItem *, int)));
	connect(ui.btnWlanOpenControl, SIGNAL(toggled(bool)), this, SLOT(slot_btnWlanOpenControl(bool)));
	ui.btnWlanOpenControl->setChecked(true);
}

MainWindow::~MainWindow()
{
}

void MainWindow::closeEvent(QCloseEvent * event)
{
	myclose();
	QDialog::closeEvent(event);
}

void MainWindow::myclose()
{
	Delete_And_Null(mRefresh_timer);
	Delete_And_Null(btnConnectControl);
	Delete_And_Null(wifiState_Lab);
	Delete_And_Null(passwordLineEdit);
}

void MainWindow::initWifiIcon()
{
	mWlan_slant_icon_files.push_back(":/icon/WLAN0.png");
	mWlan_slant_icon_files.push_back(":/icon/WLAN1.png");
	mWlan_slant_icon_files.push_back(":/icon/WLAN2.png");
	mWlan_slant_icon_files.push_back(":/icon/WLAN3.png");
	mWlan_slant_icon_files.push_back(":/icon/WLAN4.png");
	ui.wifi_treeWidget->setIconSize(QSize(40, 40));
}

void MainWindow::slot_itemClicked(QTreeWidgetItem *item, int column)
{
	Q_UNUSED(column);
	if (!item)
		return;
	if (!item->data(0, Qt::DisplayRole).isNull() && item->childCount() == 0)
	{
		mSelectWifiName = item->data(0, Qt::DisplayRole).toString();
		removeConnectItem();
		QTreeWidgetItem* connectControlItem = new QTreeWidgetItem(item);
		QGroupBox* connectGrpB = new QGroupBox(this);
		bool isconnected = mSelectWifiName == QString::fromStdString(mCurrentConnectedWifi);
		qDebug() << "selected: " << mSelectWifiName;
		createConnectWidget(connectGrpB, isconnected);
		ui.wifi_treeWidget->setItemWidget(connectControlItem, 0, connectGrpB);
	}
	ui.wifi_treeWidget->expandAll();
}

void MainWindow::on_btnRefreshWIFI_clicked()
{
	removeConnectItem();
	ui.wifi_treeWidget->clear();
	mNativeWifi.refreshWLAN(mWifiMap);
	show_WLAN();
}

void MainWindow::on_btnClose_clicked()
{
	mRefresh_timer->stop();
	repaint();
	close();
}

bool MainWindow::show_WLAN()
{
	if (mWifiMap.empty())
		return false;
	int quality;
	if (mNativeWifi.queryInterface(mCurrentConnectedWifi, quality))
	{
		std::stringstream ss;
		ss << quality;
		print_info("current connected: " + mCurrentConnectedWifi);
		print_info("quality: " + ss.str());
		ui.ActiveWifiName_Lab->setText(QString::fromStdString(mCurrentConnectedWifi));
	}
	int signalQuality = 0;
	int signalLevel = 0;
	std::map<std::string, int>::iterator iter;
	for (iter = mWifiMap.begin(); iter != mWifiMap.end(); ++iter)
	{
		QTreeWidgetItem* item = new QTreeWidgetItem(ui.wifi_treeWidget);
		item->setData(0, Qt::DisplayRole, QString::fromStdString(iter->first));
		QFont font = item->font(1);
		if (mCurrentConnectedWifi == iter->first)
		{
			font.setBold(true);
			ui.wifi_treeWidget->setCurrentItem(item);
		}
		item->setFont(0, font);
		signalQuality = iter->second;
		if (signalQuality == 0)
		{
			signalLevel = 0;
		}
		else if (signalQuality == 100)
		{
			signalLevel = 4;
		}
		else
		{
			signalLevel = signalQuality / 25 + 1;
		}
		QIcon wlanIcon;
		wlanIcon.addFile(mWlan_slant_icon_files[signalLevel]);
		item->setIcon(0, wlanIcon);
		ui.wifi_treeWidget->addTopLevelItem(item);
	}
	return true;
}

void MainWindow::slot_btnWlanOpenControl(bool checked)
{
	if (checked)
	{
		if (!mNativeWifi.openWLAN(mWifiMap))
		{
			print_error("failed to open WLAN");
			return;
		}
		print_info("open WLAN success");
		if (!show_WLAN())
			return;
		ui.btnWlanOpenControl->setText("WLAN ON");
		ui.btnWlanOpenControl->setIcon(QIcon(":/icon/wifiOn.png"));
		mRefresh_timer->start(60000);
	}
	else
	{
		if (!mNativeWifi.closeWLAN())
			return;
		ui.wifi_treeWidget->clear();
		ui.btnWlanOpenControl->setText("WLAN OFF");
		ui.btnWlanOpenControl->setIcon(QIcon(":/icon/wifiOFF.png"));
		mRefresh_timer->stop();
		print_info("close WLAN success");
		ui.ActiveWifiName_Lab->clear();
	}
}

void MainWindow::slot_btnConnectControl_clicked()
{
	if (btnConnectControl->text() == QString::fromLocal8Bit("连接"))
	{
		if (mNativeWifi.connectWLAN(mSelectWifiName.toStdString()))
		{
			print_info("connect WLAN success");
			wifiState_Lab->setText(QString::fromLocal8Bit("已连接"));
			btnConnectControl->setText(QString::fromLocal8Bit("断开连接"));
			std::this_thread::sleep_for(std::chrono::milliseconds(300));
			on_btnRefreshWIFI_clicked();
		}
		else
		{
			std::cout << "密码连接 " << std::endl;
			QTreeWidgetItem* currentItem = ui.wifi_treeWidget->currentItem();
			if (currentItem->text(0).isEmpty())
			{
				currentItem = currentItem->parent();
			}
			QTreeWidgetItem* passwordItem = new QTreeWidgetItem(currentItem);
			QGroupBox* passwordGroupB = new QGroupBox(this);
			createPasswordWidget(passwordGroupB);
			ui.wifi_treeWidget->setItemWidget(passwordItem, 0, passwordGroupB);
			btnConnectControl->setText(QString::fromLocal8Bit("密码连接"));
		}
	}
	else if (btnConnectControl->text() == QString::fromLocal8Bit("断开连接"))
	{
		if (mNativeWifi.disConnect())
		{
			print_info("disconnect WLAN success");
			wifiState_Lab->setText(QString::fromLocal8Bit("安全"));
			btnConnectControl->setText(QString::fromLocal8Bit("连接"));
			std::this_thread::sleep_for(std::chrono::milliseconds(300));
			on_btnRefreshWIFI_clicked();
		}
		else
		{
			print_error("failed to disconnect wlan");
		}
	}
	else if (btnConnectControl->text() == QString::fromLocal8Bit("密码连接"))
	{
		if (mNativeWifi.passwordToConnectWLAN(mSelectWifiName.toStdString(), passwordLineEdit->text().toStdString()))
		{
			print_info("connect WLAN success");
			wifiState_Lab->setText(QString::fromLocal8Bit("已连接"));
			btnConnectControl->setText(QString::fromLocal8Bit("断开连接"));
			std::this_thread::sleep_for(std::chrono::milliseconds(300));
			on_btnRefreshWIFI_clicked();
		}
		else
		{
			QMessageBox::critical(this, "error", QString::fromLocal8Bit("密码错误"));
		}
	}
}

void MainWindow::createConnectWidget(QGroupBox* connectGroupB, bool isConnect)
{
	connectGroupB->setObjectName(QStringLiteral("connectGroupB"));
	connectGroupB->setGeometry(QRect(610, 260, 291, 51));
	connectGroupB->setStyleSheet(QLatin1String("QGroupBox{\n"
		"border-radius:10px;\n"
		"background-color: rgba(170, 170, 127, 0);\n"
		"margin-top:0px;\n"
		"}\n"
		"\n"
		""));
	QVBoxLayout *verticalLayout = new QVBoxLayout(connectGroupB);
	verticalLayout->setSpacing(6);
	verticalLayout->setContentsMargins(11, 11, 11, 11);
	verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
	btnConnectControl = new QPushButton(connectGroupB);
	btnConnectControl->setCheckable(true);
	btnConnectControl->setChecked(false);
	btnConnectControl->setFixedSize(QSize(93, 25));
	connect(btnConnectControl, SIGNAL(clicked()), this, SLOT(slot_btnConnectControl_clicked()));
	wifiState_Lab = new QLabel(connectGroupB);
	wifiState_Lab->setFixedHeight(25);
	wifiState_Lab->setAlignment(Qt::AlignLeft);
	if (isConnect)
	{
		wifiState_Lab->setText(QString::fromLocal8Bit("已连接"));
		btnConnectControl->setText(QString::fromLocal8Bit("断开连接"));
	}
	else
	{
		wifiState_Lab->setText(QString::fromLocal8Bit("安全"));
		btnConnectControl->setText(QString::fromLocal8Bit("连接"));
	}
	QHBoxLayout* horizontalLayout = new QHBoxLayout();
	horizontalLayout->setSpacing(6);
	horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
	horizontalLayout->addWidget(wifiState_Lab);
	horizontalLayout->addWidget(btnConnectControl);
	verticalLayout->addLayout(horizontalLayout);
}

void MainWindow::createPasswordWidget(QGroupBox* passwordGroupB)
{
	passwordGroupB->setObjectName(QStringLiteral("passwordGroupBox"));
	passwordGroupB->setGeometry(QRect(470, 430, 371, 61));
	passwordGroupB->setStyleSheet(QLatin1String("QGroupBox{\n"
		"border-radius:10px;\n"
		"background-color: rgba(170, 170, 127, 0);\n"
		"margin-top:0px;\n"
		"}\n"
		"\n"
		""));
	QVBoxLayout* verticalLayout = new QVBoxLayout(passwordGroupB);
	verticalLayout->setSpacing(6);
	verticalLayout->setContentsMargins(11, 11, 11, 11);
	verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
	QHBoxLayout* horizontalLayout = new QHBoxLayout();
	horizontalLayout->setSpacing(6);
	horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
	QLabel* label = new QLabel(passwordGroupB);
	label->setObjectName(QStringLiteral("label"));
	label->setText(QString::fromLocal8Bit("请输入密码"));
	horizontalLayout->addWidget(label);
	passwordLineEdit = new QLineEditPassword(passwordGroupB);
	passwordLineEdit->setObjectName(QStringLiteral("passwordLineEdit"));
	passwordLineEdit->setStyleSheet(QString::fromUtf8("QLineEdit {\n"
		"    font-family: \"Microsoft YaHei\";\n"
		"    font-size: 12px;\n"
		"    color: rgb(50, 50, 50);\n"
		"    background-color: #f2f2f2;\n"
		"    border: 1px solid #e5e5e5;\n"
		"    border-radius: 4px;\n"
		"    padding: 2px 2px;\n"
		"    min-height: 18px;   /* \345\257\271\345\272\224\351\253\230\345\272\246\345\244\247\346\246\202\346\230\25724px\357\274\214\350\277\231\351\207\214\344\270\215\347\237\245\344\270\272\344\275\225\344\270\215\350\203\275\347\255\211\344\273\267 */\n"
		"    padding-right: 18px;\n"
		"} \n"
		"QLineEdit:hover{\n"
		"    border: 1px solid #014099;\n"
		"} \n"
		"QLineEdit:focus{\n"
		"    border: 1px solid #014099;\n"
		"} \n"
		"QLineEdit:hover{\n"
		"    border: 1px solid #014099;\n"
		"} \n"
		"QLineEdit QPushButton {\n"
		"    width:  16px;\n"
		"    height: 16px;\n"
		"    qproperty-flat: true;\n"
		"    margin-right: 4px;\n"
		"    border: none;\n"
		"    border-width: 0;\n"
		"    border-image: url(:/icon/password_hide.png) 0 0 0 0 stretch stretch;\n"
		"    background: transparent;\n"
		"} \n"
		"QLineEdit QPu"
		"shButton::checked {\n"
		"    border-image: url(:/icon/password_show.png) 0 0 0 0 stretch stretch;\n"
		"}\n"
		" \n"
		" \n"
		" "));
	passwordLineEdit->setObjectName(QStringLiteral("passwordLineEdit"));
	horizontalLayout->addWidget(passwordLineEdit);
	verticalLayout->addLayout(horizontalLayout);
}

void MainWindow::removeConnectItem()
{
	QTreeWidgetItemIterator it(ui.wifi_treeWidget, QTreeWidgetItemIterator::HasChildren);
	while (*it)
	{
		int childCount = (*it)->childCount();
		while (childCount--)
		{
			QTreeWidgetItem* child = (*it)->child(childCount);
			ui.wifi_treeWidget->removeItemWidget(child, 0);
			(*it)->removeChild(child);
			delete child;
			child = nullptr;
		}
		++it;
	}
}

void MainWindow::slot_refresh_timeout()
{
	on_btnRefreshWIFI_clicked();
}