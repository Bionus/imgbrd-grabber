#include "adduniquewindow.h"
#include "functions.h"
#include "json.h"
#include <QtXml>



/**
 * Constructor of the AddUniqueWindow class, generating its window.
 * @param	favorites	List of favorites tags, needed for coloration
 * @param	parent		The parent window
 */
AddUniqueWindow::AddUniqueWindow(QMap<QString,QMap<QString,QString> > sites, mainWindow *parent) : QWidget(parent), m_parent(parent), m_sites(sites)
{
	QVBoxLayout *layout = new QVBoxLayout;
		QFormLayout *formLayout = new QFormLayout;
			m_comboSite = new QComboBox;
				m_comboSite->addItems(m_sites.keys());
				formLayout->addRow(tr("&Site"), m_comboSite);
			m_lineId = new QLineEdit;
				formLayout->addRow(tr("&Id"), m_lineId);
			m_lineMd5 = new QLineEdit;
				formLayout->addRow(tr("&Md5"), m_lineMd5);
			layout->addLayout(formLayout);
		QHBoxLayout *layoutButtons = new QHBoxLayout;
			QPushButton *buttonOk = new QPushButton(tr("Ok"));
				connect(buttonOk, SIGNAL(clicked()), this, SLOT(ok()));
				layoutButtons->addWidget(buttonOk);
			QPushButton *buttonClose = new QPushButton(tr("Fermer"));
				connect(buttonClose, SIGNAL(clicked()), this, SLOT(close()));
				layoutButtons->addWidget(buttonClose);
			layout->addLayout(layoutButtons);
	this->setLayout(layout);
	this->setWindowIcon(QIcon(":/images/icon.ico"));
	this->setWindowTitle(tr("Grabber")+" - "+tr("Ajouter image"));
	this->setWindowFlags(Qt::Window);
	this->resize(QSize(200, 0));
}

/**
 * Search for image in available websites.
 */
void AddUniqueWindow::ok()
{
	m_page = new Page(&m_sites, m_comboSite->currentText(), QStringList(m_lineId->text().isEmpty() ? "md5:"+m_lineMd5->text() : "id:"+m_lineId->text()), 1, 1);
	connect(m_page, SIGNAL(finishedLoading(Page*)), this, SLOT(replyFinished(Page*)));
	m_page->load();
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
	QStringList tags;
	foreach (Tag tag, img->tags())
	{ tags.append(tag.text()); }

	QMap<QString,QString> values;
	values.insert("id", QString::number(img->id()));
	values.insert("md5", img->md5());
	values.insert("rating", img->rating());
	values.insert("tags", tags.join(" "));
	values.insert("file_url", img->fileUrl().toString());
	values.insert("site", m_comboSite->currentText());

	values.insert("page_url", m_sites[m_comboSite->currentText()]["Urls/Html/Post"]);
	QString t = m_sites[m_comboSite->currentText()].contains("DefaultTag") ? m_sites[m_comboSite->currentText()]["DefaultTag"] : "";
	values["page_url"].replace("{tags}", t);
	values["page_url"].replace("{id}", values["id"]);

	m_parent->batchAddUnique(values);
	this->close();
}
