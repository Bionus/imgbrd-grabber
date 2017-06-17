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
		auto data = logFiles[index];

		ui->lineName->setText(data["name"].toString());
		ui->comboLocationType->setCurrentIndex(data["locationType"].toInt());
		ui->linePath->setText(data["path"].toString());
		ui->lineFilename->setText(data["filename"].toString());
		ui->lineUniquePath->setText(data["uniquePath"].toString());
		ui->lineSuffix->setText(data["suffix"].toString());
		ui->textEditContent->setPlainText(data["content"].toString());
	}

	connect(this, SIGNAL(accepted()), this, SLOT(save()));
}

LogWindow::~LogWindow()
{
	delete ui;
}

void LogWindow::save()
{
	QMap<QString, QVariant> data;

	data["name"] = ui->lineName->text();
	data["locationType"] = ui->comboLocationType->currentIndex();
	data["path"] = ui->linePath->text();
	data["filename"] = ui->lineFilename->text();
	data["uniquePath"] = ui->lineUniquePath->text();
	data["suffix"] = ui->lineSuffix->text();
	data["content"] = ui->textEditContent->toPlainText();

	emit validated(m_index, data);
}
