#include <QSettings>
#include "md5fix.h"
#include "functions.h"
#include "ui_md5fix.h"

md5Fix::md5Fix(QMap<QString,QMap<QString,QString> > sites, QWidget *parent) : QDialog(parent), ui(new Ui::md5Fix), m_sites(sites)
{
	ui->setupUi(this);

	QSettings settings(savePath("settings.ini"), QSettings::IniFormat);
	ui->lineFolder->setText(settings.value("Save/path").toString());
	ui->lineFilename->setText(settings.value("Save/filename").toString());
	ui->comboSource->addItems(m_sites.keys());
}
md5Fix::~md5Fix()
{
	delete ui;
}

void md5Fix::on_buttonCancel_clicked()
{
	emit rejected();
	close();
}
void md5Fix::on_buttonContinue_clicked()
{
	//md5Fix *bf2 = new BlacklistFix2(m_sites, ui->lineFolder->text(), ui->lineFilename->text(), ui->lineBlacklist->text().split(' '), m_sites.value(ui->comboSource->currentText()));
	close();
	//bf2->show();
}
