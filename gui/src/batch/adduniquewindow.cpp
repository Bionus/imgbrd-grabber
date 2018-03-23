#include "batch/adduniquewindow.h"
#include <QFileDialog>
#include <QSettings>
#include <ui_adduniquewindow.h>
#include "downloader/download-query-image.h"
#include "helpers.h"
#include "models/api/api.h"
#include "models/filename.h"
#include "models/image.h"
#include "models/page.h"
#include "models/profile.h"
#include "models/site.h"


/**
 * Constructor of the AddUniqueWindow class, generating its window.
 * @param	favorites	List of favorites tags, needed for coloration
 * @param	parent		The parent window
 */
AddUniqueWindow::AddUniqueWindow(Site *selected, Profile *profile, QWidget *parent)
	: QDialog(parent), ui(new Ui::AddUniqueWindow), m_sites(profile->getSites()), m_profile(profile)
{
	ui->setupUi(this);

	auto items = m_sites.keys();
	ui->comboSites->addItems(items);
	ui->comboSites->setCurrentIndex(items.indexOf(selected->url()));

	QSettings *settings = profile->getSettings();
	ui->lineFolder->setText(settings->value("Save/path").toString());
	ui->lineFilename->setText(settings->value("Save/filename").toString());
}

/**
 * Ui events
 */
void AddUniqueWindow::on_buttonFolder_clicked()
{
	QString folder = QFileDialog::getExistingDirectory(this, tr("Choose a save folder"), ui->lineFolder->text());
	if (!folder.isEmpty())
	{ ui->lineFolder->setText(folder); }
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
	if (api != Q_NULLPTR)
	{
		QString url = api->detailsUrl(ui->lineId->text().toULongLong(), ui->lineMd5->text(), site).url;

		auto details = QMap<QString, QString>();
		details.insert("page_url", url);
		details.insert("id", ui->lineId->text());
		details.insert("md5", ui->lineMd5->text());
		details.insert("website", ui->comboSites->currentText());
		details.insert("site", QString::number((qintptr)m_sites[ui->comboSites->currentText()]));

		m_image = QSharedPointer<Image>(new Image(site, details, m_profile));
		connect(m_image.data(), &Image::finishedLoadingTags, this, &AddUniqueWindow::addLoadedImage);
		m_image->loadDetails();
	}
	else
	{
		QString query = (ui->lineId->text().isEmpty() ? "md5:"+ui->lineMd5->text() : "id:"+ui->lineId->text());
		QStringList search = QStringList() << query << "status:any";
		m_page = new Page(m_profile, m_sites[ui->comboSites->currentText()], m_sites.values(), search, 1, 1);
		connect(m_page, &Page::finishedLoading, this, &AddUniqueWindow::replyFinished);
		m_page->load();
	}
}

/**
 * Signal triggered when the search is finished.
 * @param r		The QNetworkReply associated with the search
 */
void AddUniqueWindow::replyFinished(Page *p)
{
	if (p->images().isEmpty())
	{
		p->deleteLater();
		error(this, tr("No image found."));
		return;
	}

	addImage(p->images().first());
	p->deleteLater();
}

void AddUniqueWindow::addLoadedImage()
{
	addImage(m_image);
}
void AddUniqueWindow::addImage(QSharedPointer<Image> img)
{
	emit sendData(DownloadQueryImage(*img, m_sites[ui->comboSites->currentText()], ui->lineFilename->text(), ui->lineFolder->text()));

	if (m_close)
		close();
}
