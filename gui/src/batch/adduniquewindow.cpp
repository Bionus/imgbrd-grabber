#include <QSettings>
#include <QFileDialog>
#include "adduniquewindow.h"
#include "ui_adduniquewindow.h"
#include "functions.h"
#include "helpers.h"
#include "vendor/json.h"
#include "models/page.h"
#include "models/filename.h"
#include "models/site.h"
#include "models/profile.h"
#include "models/image.h"
#include "downloader/download-query-image.h"


/**
 * Constructor of the AddUniqueWindow class, generating its window.
 * @param	favorites	List of favorites tags, needed for coloration
 * @param	parent		The parent window
 */
AddUniqueWindow::AddUniqueWindow(QString selected, QMap<QString,Site*> sites, Profile *profile, QWidget *parent)
	: QDialog(parent), ui(new Ui::AddUniqueWindow), m_sites(sites), m_profile(profile)
{
	ui->setupUi(this);

	ui->comboSites->addItems(m_sites.keys());
	ui->comboSites->setCurrentIndex(m_sites.keys().indexOf(selected));

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
void AddUniqueWindow::on_lineFilename_textChanged(QString text)
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
	bool useDirectLink = true;
	if (
		(site->value("Urls/Html/Post").contains("{id}") && ui->lineId->text().isEmpty()) ||
		(site->value("Urls/Html/Post").contains("{md5}") && ui->lineMd5->text().isEmpty()) ||
		!site->contains("Regex/ImageUrl")
	)
	{ useDirectLink = false; }
	if (useDirectLink)
	{
		QString url = site->value("Urls/Html/Post");
		url.replace("{id}", ui->lineId->text());
		url.replace("{md5}", ui->lineMd5->text());

		QMap<QString,QString> details = QMap<QString,QString>();
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
		m_page = new Page(m_profile, m_sites[ui->comboSites->currentText()], m_sites.values(), QStringList() << (ui->lineId->text().isEmpty() ? "md5:"+ui->lineMd5->text() : "id:"+ui->lineId->text()) << "status:any", 1, 1);
		connect(m_page, SIGNAL(finishedLoading(Page*)), this, SLOT(replyFinished(Page*)));
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
