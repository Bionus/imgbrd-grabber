#include "blacklistfix.h"
#include "blacklistfix2.h"
#include "functions.h"
#include "ui_blacklistfix.h"

BlacklistFix::BlacklistFix(QMap<QString,QMap<QString,QString> > sites, QWidget *parent) : QDialog(parent), ui(new Ui::BlacklistFix), m_sites(sites)
{
	ui->setupUi(this);

	QSettings settings(savePath("settings.ini"), QSettings::IniFormat);
	ui->lineFolder->setText(settings.value("Save/path").toString());
	ui->lineFilename->setText(settings.value("Save/filename").toString());
	ui->lineBlacklist->setText(settings.value("blacklistedtags").toString());
	ui->comboSource->addItems(m_sites.keys());
}
BlacklistFix::~BlacklistFix()
{
	delete ui;
}

void BlacklistFix::on_buttonCancel_clicked()
{
	emit rejected();
	close();
}
void BlacklistFix::on_buttonContinue_clicked()
{
	BlacklistFix2 *bf2 = new BlacklistFix2(m_sites, ui->lineFolder->text(), ui->lineFilename->text(), ui->lineBlacklist->text().split(' '), m_sites.value(ui->comboSource->currentText()));
	close();
	bf2->show();
}
