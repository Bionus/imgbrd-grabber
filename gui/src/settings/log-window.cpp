#include "log-window.h"
#include "ui_log-window.h"
#include "models/profile.h"
#include "functions.h"


LogWindow::LogWindow(int index, Profile *profile, QWidget *parent)
	: QDialog(parent), ui(new Ui::LogWindow), m_profile(profile), m_index(index)
{
	ui->setupUi(this);

	if (index >= 0)
	{
		auto logFiles = getExternalLogFiles(m_profile->getSettings());
		auto dta = logFiles[index];

		ui->lineName->setText(dta["name"].toString());
		ui->comboLocationType->setCurrentIndex(dta["locationType"].toInt());
		ui->linePath->setText(dta["path"].toString());
		ui->lineFilename->setText(dta["filename"].toString());
		ui->lineUniquePath->setText(dta["uniquePath"].toString());
		ui->lineSuffix->setText(dta["suffix"].toString());
		ui->textEditContent->setPlainText(dta["content"].toString());
	}

	connect(this, SIGNAL(accepted()), this, SLOT(save()));
}

LogWindow::~LogWindow()
{
	delete ui;
}

void LogWindow::save()
{
	QMap<QString, QVariant> dta;

	dta["name"] = ui->lineName->text();
	dta["locationType"] = ui->comboLocationType->currentIndex();
	dta["path"] = ui->linePath->text();
	dta["filename"] = ui->lineFilename->text();
	dta["uniquePath"] = ui->lineUniquePath->text();
	dta["suffix"] = ui->lineSuffix->text();
	dta["content"] = ui->textEditContent->toPlainText();

	emit validated(m_index, dta);
}
