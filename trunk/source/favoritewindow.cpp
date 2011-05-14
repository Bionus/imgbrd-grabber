#include "favoritewindow.h"
#include "ui_favoriteWindow.h"



/**
 * Constructor of the favoriteWindow class, completing its window.
 * @param	tag			The favorite that will be edited
 * @param	note		Its note, from 0 to 100
 * @param	lastviewed	The date when the user last cliked on "Set as viewed"
 * @param	imagepath	Path to the favorite's image
 * @param	parent		The parent window
 */
favoriteWindow::favoriteWindow(QString tag, int note, QDateTime lastviewed, QString imagepath, mainWindow *parent) : QDialog(parent), m_parent(parent), ui(new Ui::favoriteWindow), m_note(note), m_tag(tag), m_imagepath(imagepath), m_lastviewed(lastviewed)
{
	ui->setupUi(this);
	ui->tagLineEdit->setText(tag);
	ui->noteSpinBox->setValue(note);
	ui->lastViewedDateTimeEdit->setDateTime(lastviewed);
	ui->imageLineEdit->setText(imagepath);
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
 * Opens a window to choose an image to set the imagepath value.
 */
void favoriteWindow::on_openButton_clicked()
{
	QString file = QFileDialog::getOpenFileName(this, tr("Choisir une image"), QSettings("settings.ini", QSettings::IniFormat).value("Save/path").toString(), "Images (*.png *.gif *.jpg *.jpeg)");
	if (!file.isEmpty())
	{ ui->imageLineEdit->setText(file); }
}

/**
 * Update the local favorites file.
 */
void favoriteWindow::save()
{
	QFile f("favorites.txt");
	f.open(QIODevice::ReadOnly);
		QString favorites = f.readAll();
	f.close();
	favorites.replace("\r\n", "\n").replace("\r", "\n").replace("\n", "\r\n");
	favorites.remove(m_tag+"|"+QString::number(m_note)+"|"+m_lastviewed.toString(Qt::ISODate)+"|"+m_imagepath+"\r\n");
	favorites += ui->tagLineEdit->text()+"|"+QString::number(ui->noteSpinBox->value())+"|"+ui->lastViewedDateTimeEdit->dateTime().toString(Qt::ISODate)+"|"+ui->imageLineEdit->text()+"\r\n";
	f.open(QIODevice::WriteOnly);
		f.write(favorites.toAscii());
	f.close();
	m_parent->updateFavorites();
}
