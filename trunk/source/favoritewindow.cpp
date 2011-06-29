#include "favoritewindow.h"
#include "functions.h"



/**
 * Constructor of the favoriteWindow class, completing its window.
 * @param	tag			The favorite that will be edited
 * @param	note		Its note, from 0 to 100
 * @param	lastviewed	The date when the user last cliked on "Set as viewed"
 * @param	imagepath	Path to the favorite's image
 * @param	parent		The parent window
 */
favoriteWindow::favoriteWindow(QString tag, int note, QDateTime lastviewed, mainWindow *parent) : QDialog(parent), m_parent(parent), ui(new Ui::favoriteWindow), m_note(note), m_tag(tag), m_lastviewed(lastviewed)
{
	ui->setupUi(this);
	ui->tagLineEdit->setText(tag);
	ui->noteSpinBox->setValue(note);
	ui->lastViewedDateTimeEdit->setDateTime(lastviewed);
	connect(this, SIGNAL(accepted()), this, SLOT(save()));
}

/**
 * Destructor of the favoriteWindow class
 */
favoriteWindow::~favoriteWindow()
{
	delete ui;
}

/**
 * Removes the favorite from the list and closes the window
 */
void favoriteWindow::on_buttonRemove_clicked()
{
	QFile f(savePath("favorites.txt"));
	f.open(QIODevice::ReadOnly);
		QString favs = f.readAll();
	f.close();
	favs.replace("\r\n", "\n").replace("\r", "\n").replace("\n", "\r\n");
	QRegExp reg(m_tag+"\\|(.+)\\r\\n");
	reg.setMinimal(true);
	favs.remove(reg);
	f.open(QIODevice::WriteOnly);
		f.write(favs.toAscii());
	f.close();
	if (QFile::exists(savePath("thumbs/"+m_tag+".png")))
	{ QFile::remove(savePath("thumbs/"+m_tag+".png")); }
	m_parent->updateFavorites();

	close();
}

/**
 * Opens a window to choose an image to set the imagepath value.
 */
void favoriteWindow::on_openButton_clicked()
{
	QString file = QFileDialog::getOpenFileName(this, tr("Choisir une image"), QSettings(savePath("settings.ini"), QSettings::IniFormat).value("Save/path").toString(), "Images (*.png *.gif *.jpg *.jpeg)");
	if (!file.isEmpty())
	{ ui->imageLineEdit->setText(file); }
}

/**
 * Update the local favorites file and add thumb if necessary.
 */
void favoriteWindow::save()
{
	if (m_tag != ui->tagLineEdit->text() && QFile::exists(savePath("thumbs/"+m_tag+".png")))
	{ QFile::rename(savePath("thumbs/"+m_tag+".png"), savePath("thumbs/"+ui->tagLineEdit->text()+".png")); }
	if (QFile::exists(ui->imageLineEdit->text()))
	{
		QPixmap img(ui->imageLineEdit->text());
		if (!img.isNull())
		{
			if (img.width() > 150 || img.height() > 150)
			{ img = img.scaled(QSize(150,150), Qt::KeepAspectRatio, Qt::SmoothTransformation); }
			img.save(savePath("thumbs/"+ui->tagLineEdit->text()+".png"), "PNG");
		}
	}
	QFile f(savePath("favorites.txt"));
	f.open(QIODevice::ReadOnly);
		QString favorites = f.readAll();
	f.close();
	favorites.replace("\r\n", "\n").replace("\r", "\n").replace("\n", "\r\n");
	favorites.remove(m_tag+"|"+QString::number(m_note)+"|"+m_lastviewed.toString(Qt::ISODate)+"\r\n");
	favorites += ui->tagLineEdit->text()+"|"+QString::number(ui->noteSpinBox->value())+"|"+ui->lastViewedDateTimeEdit->dateTime().toString(Qt::ISODate)+"\r\n";
	f.open(QIODevice::WriteOnly);
		f.write(favorites.toAscii());
	f.close();
	m_parent->updateFavorites();
}
