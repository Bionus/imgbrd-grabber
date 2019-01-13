#include "utils/blacklist-fix/blacklist-fix-1.h"
#include <QCryptographicHash>
#include <QDir>
#include <QDirIterator>
#include <QMessageBox>
#include <QSettings>
#include <ui_blacklist-fix-1.h>
#include "helpers.h"
#include "functions.h"
#include "logger.h"
#include "models/image.h"
#include "models/page.h"
#include "models/profile.h"
#include "utils/blacklist-fix/blacklist-fix-2.h"


BlacklistFix1::BlacklistFix1(Profile *profile, QWidget *parent)
	: QDialog(parent), ui(new Ui::BlacklistFix1), m_profile(profile), m_sites(profile->getSites())
{
	ui->setupUi(this);

	QSettings *settings = profile->getSettings();
	ui->lineFolder->setText(settings->value("Save/path").toString());
	ui->lineFilename->setText(settings->value("Save/filename").toString());
	ui->comboSource->addItems(m_sites.keys());
	ui->progressBar->hide();

	ui->textBlacklist->setPlainText(profile->getBlacklist().toString());

	resize(size().width(), 0);
}

BlacklistFix1::~BlacklistFix1()
{
	delete ui;
}

void BlacklistFix1::on_buttonCancel_clicked()
{
	emit rejected();
	close();
}

void BlacklistFix1::on_buttonContinue_clicked()
{
	ui->buttonContinue->setEnabled(false);
	m_details.clear();

	// Check that directory exists
	QDir dir(ui->lineFolder->text());
	if (!dir.exists()) {
		error(this, tr("This directory does not exist."));
		ui->buttonContinue->setEnabled(true);
		return;
	}

	// Make sure the input is valid
	if (!ui->radioForce->isChecked() && !ui->lineFilename->text().contains("%md5%")) {
		error(this, tr("If you want to get the MD5 from the filename, you have to include the %md5% token in it."));
		ui->buttonContinue->setEnabled(true);
		return;
	}

	// Get all files from the destination directory
	QVector<QPair<QString, QString>> files;
	QDirIterator it(dir, QDirIterator::Subdirectories);
	while (it.hasNext()) {
		it.next();
		if (!it.fileInfo().isDir()) {
			int len = it.filePath().length() - dir.absolutePath().length() - 1;
			files.append(QPair<QString, QString>(it.filePath().right(len), it.filePath()));
		}
	}

	// Parse all files
	for (const QPair<QString, QString> &file : files) {
		QString md5 = ui->radioForce->isChecked()
			? getFileMd5(file.second)
			: getFilenameMd5(file.first, ui->lineFilename->text());

		if (!md5.isEmpty()) {
			QMap<QString, QString> det;
			det.insert("md5", md5);
			det.insert("path", file.first);
			det.insert("path_full", file.second);
			m_details.append(det);
		}
	}

	int response = QMessageBox::question(this, tr("Blacklist fixer"), tr("You are about to download information from %n image(s). Are you sure you want to continue?", "", m_details.size()), QMessageBox::Yes | QMessageBox::No);
	if (response == QMessageBox::Yes) {
		// Show progress bar
		ui->progressBar->setValue(0);
		ui->progressBar->setMaximum(files.size());
		ui->progressBar->show();

		getAll();
	}
}

void BlacklistFix1::getAll(Page *p)
{
	if (p != nullptr && !p->images().empty()) {
		QSharedPointer<Image> img = p->images().at(0);
		m_getAll[img->md5()].insert("tags", img->tagsString().join(" "));
		ui->progressBar->setValue(ui->progressBar->value() + 1);
		p->deleteLater();
	}

	if (!m_details.empty()) {
		QMap<QString, QString> det = m_details.takeFirst();
		m_getAll.insert(det.value("md5"), det);

		Page *page = new Page(m_profile, m_sites.value(ui->comboSource->currentText()), m_sites.values(), QStringList("md5:" + det.value("md5")), 1, 1);
		connect(page, &Page::finishedLoading, this, &BlacklistFix1::getAll);
		page->load();
	} else {
		Blacklist blacklist;
		for (const QString &tags : ui->textBlacklist->toPlainText().split("\n", QString::SkipEmptyParts)) {
			blacklist.add(tags.trimmed().split(' ', QString::SkipEmptyParts));
		}

		BlacklistFix2 *bf2 = new BlacklistFix2(m_getAll.values(), blacklist);
		close();
		bf2->show();
	}
}
