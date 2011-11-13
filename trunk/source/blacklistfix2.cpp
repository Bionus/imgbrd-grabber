#include "blacklistfix2.h"
#include "blacklistfix3.h"
#include "functions.h"
#include "ui_blacklistfix2.h"

BlacklistFix2::BlacklistFix2(QMap<QString,QMap<QString,QString> > sites, QString folder, QString filename, QStringList blacklist, QMap<QString,QString> site, QWidget *parent) : QDialog(parent), ui(new Ui::BlacklistFix2), m_folder(folder), m_filename(filename), m_blacklist(blacklist), m_site(site), m_sites(sites)
{
	ui->setupUi(this);
	ui->treeWidget->insertTopLevelItems(0, mkTree(QDir(folder)));
	ui->treeWidget->selectAll();
}
BlacklistFix2::~BlacklistFix2()
{
	delete ui;
}

QList<QTreeWidgetItem*> BlacklistFix2::mkTree(QDir dir)
{
	QList<QTreeWidgetItem*> items;
	QStringList dirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
	for (int i = 0; i < dirs.size(); i++)
	{
		QTreeWidgetItem *item = new QTreeWidgetItem(QStringList(dirs.at(i)));
		item->insertChildren(0, mkTree(QDir(dir.path()+"/"+dirs.at(i))));
		items.append(item);
	}
	return items;
}
QString BlacklistFix2::remakePath(QTreeWidgetItem* item)
{
	QString path = item->text(0);
	if (item->parent())
	{ path = remakePath(item->parent())+"/"+path; }
	return path;
}
QStringList BlacklistFix2::getAllFiles(QString d)
{
	QStringList files;
	QDir dir(d);
	QStringList dirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
	for (int i = 0; i < dirs.size(); i++)
	{ files.append(getAllFiles(d+"/"+dirs.at(i))); }
	QStringList fles = dir.entryList(QDir::Files);
	for (int i = 0; i < fles.size(); i++)
	{ files.append(QDir::toNativeSeparators(d+"/"+fles.at(i))); }
	return files;
}

void BlacklistFix2::on_buttonCancel_clicked()
{
	emit rejected();
	close();
}
void BlacklistFix2::on_buttonContinue_clicked()
{
	m_details.clear();

	// Get folder list
	QStringList paths;
	QList<QTreeWidgetItem*> sel = ui->treeWidget->selectedItems();
	for (int i = 0; i < sel.size(); i++)
	{ paths.append(QDir::toNativeSeparators(m_folder+"/"+remakePath(sel.at(i)))); }

	// Get image list
	QStringList images;
	for (int i = 0; i < paths.size(); i++)
	{ images.append(getAllFiles(paths.at(i))); }
	for (int i = 0; i < images.size(); i++)
	{ images[i].remove(QDir::toNativeSeparators(m_folder+"/")).replace("\\", "/"); }
	QString regex = m_filename;
	QStringList list;
	QRegExp reg("%([^%]+)%");
	int pos = 0;
	while (reg.indexIn(m_filename, pos) != -1)
	{
		list.append(reg.cap(1));
		pos += m_filename.right(m_filename.size()-pos).indexOf(reg.cap(0)) + reg.matchedLength();
	}
	regex.replace(reg, "([a-zA-Z0-9_ ]+)");

	// Get image details
	for (int i = 0; i < images.size(); i++)
	{
		if (images.at(i).count('/') == regex.count('/'))
		{
			QMap<QString,QString> det;
			QRegExp reg(regex);
			reg.indexIn(images.at(i));
			for (int r = 0; r < list.size(); r++)
			{ det.insert(list.at(r), reg.cap(r+1)); }
			det.insert("path", QDir::toNativeSeparators(images.at(i)));
			det.insert("path_full", QDir::toNativeSeparators(m_folder+"/"+images.at(i)));
			m_details.append(det);
		}
	}


	int reponse = QMessageBox::question(this, tr("Réparateur de liste noire"), tr("Vous vous apprêtez à télécharger les informations de %n image(s). Êtes-vous sûr de vouloir continuer ?", "", m_details.size()), QMessageBox::Yes | QMessageBox::No);
	if (reponse == QMessageBox::Yes)
	{ getAll(); }
}

void BlacklistFix2::getAll(Page *p)
{
	if (p != NULL)
	{
		if (p->images().size() > 0)
		{
			QList<Tag*> ts = p->images().at(0)->tags();
			QStringList tags;
			for (int i = 0; i < ts.size(); i++)
			{ tags.append(ts.at(i)->text()); }
			m_getAll[p->images().at(0)->md5()].insert("tags", tags.join(" "));
		}
	}

	if (!m_details.empty())
	{
		QMap<QString,QString> det = m_details.takeFirst();
		m_getAll.insert(det.value("md5"), det);
		QString query = "md5:"+det.value("md5");
		Page *page = new Page(&m_sites, m_site.value("Url"), QStringList(query), 1, 1);
		connect(page, SIGNAL(finishedLoading(Page*)), this, SLOT(getAll(Page*)));
		page->load();
	}
	else
	{
		BlacklistFix3 *bf3 = new BlacklistFix3(m_getAll.values(), m_blacklist);
		hide();
		bf3->show();
	}
}
