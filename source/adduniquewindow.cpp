#include <QSettings>
#include <QFileDialog>
#include "adduniquewindow.h"
#include "ui_adduniquewindow.h"
#include "functions.h"
#include "json.h"



/**
 * Constructor of the AddUniqueWindow class, generating its window.
 * @param	favorites	List of favorites tags, needed for coloration
 * @param	parent		The parent window
 */
AddUniqueWindow::AddUniqueWindow(QString selected, QMap<QString,Site*> sites, QWidget *parent) : QDialog(parent), ui(new Ui::AddUniqueWindow), m_sites(sites)
{
	ui->setupUi(this);

	ui->comboSites->addItems(m_sites.keys());
	ui->comboSites->setCurrentIndex(m_sites.keys().indexOf(selected));

	QSettings settings(savePath("settings.ini"), QSettings::IniFormat);
	ui->lineFolder->setText(settings.value("Save/path").toString());
	ui->lineFilename->setText(settings.value("Save/filename").toString());
}

/**
 * Ui events
 */
void AddUniqueWindow::on_buttonFolder_clicked()
{
	QString folder = QFileDialog::getExistingDirectory(this, tr("Choisir un dossier de sauvegarde"), ui->lineFolder->text());
	if (!folder.isEmpty())
	{ ui->lineFolder->setText(folder); }
}
void AddUniqueWindow::on_lineFilename_textChanged(QString text)
{ ui->labelFilename->setText(validateFilename(text)); }

/**
 * Search for image in available websites.
 */
void AddUniqueWindow::add()
{ ok(false); }
void AddUniqueWindow::ok(bool close)
{
	m_close = close;
	bool useDirectLink = true;
	if (
		(m_sites[ui->comboSites->currentText()]->value("Urls/Html/Post").contains("{id}") && ui->lineId->text().isEmpty()) ||
		(m_sites[ui->comboSites->currentText()]->value("Urls/Html/Post").contains("{md5}") && ui->lineMd5->text().isEmpty()) ||
		!m_sites[ui->comboSites->currentText()]->contains("Regex/ImageUrl")
	)
	{ useDirectLink = false; }
	if (useDirectLink)
	{
		QString url = m_sites[ui->comboSites->currentText()]->value("Urls/Html/Post");
		url.replace("{id}", ui->lineId->text());
		url.replace("{md5}", ui->lineMd5->text());

		QMap<QString,QString> details = QMap<QString,QString>();
		details.insert("page_url", url);
		details.insert("id", ui->lineId->text());
		details.insert("md5", ui->lineMd5->text());
		details.insert("website", ui->comboSites->currentText());
		details.insert("site", QString::number((qintptr)m_sites[ui->comboSites->currentText()]));
		Image *img = new Image(details);
		img->loadDetails();
		connect(img, SIGNAL(finishedLoadingTags(Image*)), this, SLOT(addImage(Image*)));
	}
	else
	{
		m_page = new Page(m_sites[ui->comboSites->currentText()], &m_sites, QStringList() << (ui->lineId->text().isEmpty() ? "md5:"+ui->lineMd5->text() : "id:"+ui->lineId->text()) << "status:any", 1, 1);
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
		error(this, tr("Aucune image n'a été trouvée."));
		return;
	}

	Image *img = p->images().first();
	addImage(img);
}

void AddUniqueWindow::addImage(Image *img)
{
	QStringList tags;
	foreach (Tag tag, img->tags())
	{ tags.append(tag.text()); }

	QMap<QString,QString> values;
	values.insert("id", QString::number(img->id()));
	values.insert("md5", img->md5());
	values.insert("rating", img->rating());
	values.insert("tags", tags.join(" "));
	values.insert("file_url", img->fileUrl().toString());
	values.insert("site", ui->comboSites->currentText());
	values.insert("filename", ui->lineFilename->text());
	values.insert("folder", ui->lineFolder->text());

	values.insert("page_url", m_sites[ui->comboSites->currentText()]->value("Urls/Html/Post"));
	QString t = m_sites[ui->comboSites->currentText()]->contains("DefaultTag") ? m_sites[ui->comboSites->currentText()]->value("DefaultTag") : "";
	values["page_url"].replace("{tags}", t);
	values["page_url"].replace("{id}", values["id"]);

	emit sendData(values);

	if (m_close)
	{ this->close(); }
}
