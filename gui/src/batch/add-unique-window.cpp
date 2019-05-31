#include "batch/add-unique-window.h"
#include <QFileDialog>
#include <QPlainTextEdit>
#include <QScrollBar>
#include <QSettings>
#include <ui_add-unique-window.h>
#include "downloader/download-query-image.h"
#include "helpers.h"
#include "logger.h"
#include "models/api/api.h"
#include "models/filename.h"
#include "models/image.h"
#include "models/page.h"
#include "models/profile.h"
#include "models/site.h"


AddUniqueWindow::AddUniqueWindow(Site *selected, Profile *profile, QWidget *parent)
	: QDialog(parent), ui(new Ui::AddUniqueWindow), m_page(nullptr), m_sites(profile->getSites()), m_close(false), m_profile(profile)
{
	ui->setupUi(this);

	auto items = m_sites.keys();
	ui->comboSites->addItems(items);
	ui->comboSites->setCurrentIndex(items.indexOf(selected->url()));

	QSettings *settings = profile->getSettings();
	ui->lineFolder->setText(settings->value("Save/path").toString());
	ui->lineFilename->setText(settings->value("Save/filename").toString());

	ui->lineId->setContentsMargins(0, 0, 0, 0);
	ui->lineId->document()->setDocumentMargin(3);
	ui->lineMd5->setContentsMargins(0, 0, 0, 0);
	ui->lineMd5->document()->setDocumentMargin(3);
	toggleMultiLineId(false);
	toggleMultiLineMd5(false);
	ui->progressBar->hide();
}

void setTextEditRows(QPlainTextEdit *plainTextEdit, int nRows)
{
	const QTextDocument *plainDoc = plainTextEdit->document();
	const QFontMetrics fm(plainDoc->defaultFont());
	const QMargins margins = plainTextEdit->contentsMargins();

	const int nHeight = fm.lineSpacing() * nRows
			+ qRound((plainDoc->documentMargin() + plainTextEdit->frameWidth()) * 2)
			+ margins.top()
			+ margins.bottom();
	plainTextEdit->setFixedHeight(nHeight);
}
void AddUniqueWindow::toggleMultiLine(bool toggle, QPlainTextEdit *plainTextEdit, QLabel *label)
{
	if (toggle) {
		setTextEditRows(plainTextEdit, 6);
	} else {
		setTextEditRows(plainTextEdit, 1);
	}

	plainTextEdit->verticalScrollBar()->setVisible(toggle);
	label->setVisible(toggle);

	update();
	resize(width(), 0);
}
void AddUniqueWindow::toggleMultiLineId(bool toggle)
{
	toggleMultiLine(toggle, ui->lineId, ui->labelLineId);
}
void AddUniqueWindow::toggleMultiLineMd5(bool toggle)
{
	toggleMultiLine(toggle, ui->lineMd5, ui->labelLineMd5);
}

/**
 * Ui events
 */
void AddUniqueWindow::on_buttonFolder_clicked()
{
	QString folder = QFileDialog::getExistingDirectory(this, tr("Choose a save folder"), ui->lineFolder->text());
	if (!folder.isEmpty()) {
		ui->lineFolder->setText(folder);
	}
}
void AddUniqueWindow::on_lineFilename_textChanged(const QString &text)
{
	QString message;
	Filename fn(text);
	fn.isValid(m_profile, &message);
	ui->labelFilename->setText(message);
}

/**
 * Search for image in available websites.
 */
void AddUniqueWindow::add()
{ ok(false); }
void AddUniqueWindow::ok(bool close)
{
	Site *site = m_sites[ui->comboSites->currentText()];

	m_close = close;
	Api *api = site->detailsApi();

	const QStringList ids = ui->lineId->toPlainText().split('\n', QString::SkipEmptyParts);
	for (const QString &id : ids) {
		UniqueQuery q;
		q.site = site;
		q.api = api;
		q.id = id.trimmed();
		m_queue.enqueue(q);
	}

	const QStringList md5s = ui->lineMd5->toPlainText().split('\n', QString::SkipEmptyParts);
	for (const QString &md5 : md5s) {
		UniqueQuery q;
		q.site = site;
		q.api = api;
		q.md5 = md5.trimmed();
		m_queue.enqueue(q);
	}

	if (m_queue.count() > 1) {
		ui->progressBar->setMaximum(m_queue.count());
		ui->progressBar->setValue(0);
		ui->progressBar->show();
	}

	loadNext();
}
void AddUniqueWindow::loadNext()
{
	if (m_queue.isEmpty()) {
		if (m_close) {
			close();
		} else {
			ui->progressBar->hide();
		}
		return;
	}

	const UniqueQuery q = m_queue.dequeue();

	if (q.api != nullptr && false) {
		const QString url = q.api->detailsUrl(q.id.toULongLong(), q.md5, q.site).url;

		auto details = QMap<QString, QString>();
		details.insert("page_url", url);
		details.insert("id", q.id);
		details.insert("md5", q.md5);

		m_image = QSharedPointer<Image>(new Image(q.site, details, m_profile));
		connect(m_image.data(), &Image::finishedLoadingTags, this, &AddUniqueWindow::addLoadedImage);
		m_image->loadDetails();
	} else {
		const QString query = (q.id.isEmpty() ? "md5:" + q.md5 : "id:" + q.id);
		const QStringList search = QStringList() << query << "status:any";
		m_page = new Page(m_profile, q.site, m_sites.values(), search, 1, 1);
		connect(m_page, &Page::finishedLoading, this, &AddUniqueWindow::replyFinished);
		m_page->load();
	}
}

void AddUniqueWindow::replyFinished(Page *p)
{
	if (p->images().isEmpty()) {
		log(QString("No image found for search '%1'").arg(p->search().join(' ')), Logger::Warning);
		p->deleteLater();
		next();
		return;
	}

	addImage(p->images().first());
	p->deleteLater();
}

void AddUniqueWindow::addLoadedImage()
{
	addImage(m_image);
	// m_image->deleteLater();
}
void AddUniqueWindow::addImage(const QSharedPointer<Image> &img)
{
	emit sendData(DownloadQueryImage(img, m_sites[ui->comboSites->currentText()], ui->lineFilename->text(), ui->lineFolder->text()));
	next();
}

void AddUniqueWindow::next()
{
	ui->progressBar->setValue(ui->progressBar->value() + 1);
	loadNext();
}
