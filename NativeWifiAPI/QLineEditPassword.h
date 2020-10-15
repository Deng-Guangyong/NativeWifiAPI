#pragma once

#include <QLineEdit>

class QLineEditPassword : public QLineEdit
{
	Q_OBJECT

public:
	QLineEditPassword(QWidget *parent=nullptr);
	~QLineEditPassword();
};
