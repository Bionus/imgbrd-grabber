#include "advancedWindow.h"



advancedWindow::advancedWindow(QList<bool> selected, QWidget *parent) : QWidget(parent), selected(selected)
{
	this->setWindowIcon(QIcon(":/images/icon.ico"));
	this->setWindowTitle(tr("Grabber")+" - "+tr("Sources"));
	this->setWindowFlags(Qt::Window);
	this->setWindowModality(Qt::WindowModal);

	QVBoxLayout *form = new QVBoxLayout;
	QStringList dir = QStringList() << QDir("sites/xml").entryList(QDir::Files) << QDir("sites/json").entryList(QDir::Files) << QDir("sites/regex").entryList(QDir::Files);
		dir.removeDuplicates();
		dir.sort();
		for (int i = 0; i < dir.count(); i++)
		{
			QCheckBox *check = new QCheckBox();
				check->setChecked(this->selected[i]);
				this->checks << check;
			QLabel *label = new QLabel(dir.at(i).section('.', 0, -2));
			QHBoxLayout *l = new QHBoxLayout;
				l->addWidget(check);
				l->addWidget(label);
					l->setStretchFactor(label, 2);
			form->addLayout(l);
		}
	
	QPushButton *backButton = new QPushButton(tr("Annuler"));
		connect(backButton, SIGNAL(clicked()), this, SLOT(close()));
	QPushButton *okButton = new QPushButton(tr("Ok"));
		connect(okButton, SIGNAL(clicked()), this, SLOT(valid()));
	QGridLayout *l = new QGridLayout();
		l->addLayout(form, 0, 0, 1, 2);
		l->addWidget(backButton, 1, 0, 1, 1);
		l->addWidget(okButton, 1, 1, 1, 1);
	this->setLayout(l);
}



void advancedWindow::closeEvent(QCloseEvent *event)
{
	emit closed(this);
	event->accept();
}



void advancedWindow::valid()
{
	QLayout *form = this->layout()->itemAt(0)->layout();
	for (int i = 0; i < form->count(); i++)
	{ this->selected[i] = this->checks.at(i)->isChecked(); }
	this->close();
}

QList<bool> advancedWindow::getSelected()
{ return this->selected; }
