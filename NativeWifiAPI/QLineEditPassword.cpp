#include "QLineEditPassword.h"
#include <QHBoxLayout>
#include <QPushButton>

QLineEditPassword::QLineEditPassword(QWidget *parent)
	: QLineEdit(parent)
{
	setEchoMode(QLineEdit::Password);
	QPushButton *button = new QPushButton();
	button->setCursor(Qt::PointingHandCursor);
	button->setCheckable(true);
	connect(button, &QPushButton::toggled, [this](bool checked){
		if(checked)
			setEchoMode(QLineEdit::Normal);
		else
			setEchoMode(QLineEdit::Password);
	});
	QHBoxLayout *layout = new QHBoxLayout();
	layout->addStretch();
	layout->addWidget(button);
	layout->setContentsMargins(0, 0, 0, 0);
	setLayout(layout);
}

QLineEditPassword::~QLineEditPassword()
{
}
