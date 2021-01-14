#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QSettings>
#include <QDebug>
#include <QLabel>
#include <QDir>
#include <QLineEdit>
#include <QRadioButton>
#include <QResizeEvent>
#include <QShowEvent>
#include <QAction>
#include <QFileDialog>

//#include <QtSql/QSql>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlError>

#define ALBUMID 1005

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QSettings settings("settings.ini",QSettings::IniFormat);
    if(settings.contains("App/dbPath")){
        _db = QSqlDatabase::addDatabase("QSQLITE");
        _db.setDatabaseName(qApp->applicationDirPath()
                            + "/"
                            + settings.value("App/dbPath").toString());

        _db.open();
    }

    _query = QSqlQuery(_db);
    _query.exec("Select * from album");

    _albumsModel = new QStandardItemModel(this);

    while(_query.next()){
        int id = _query.value("albumID").toInt();
        QString name = _query.value("name").toString();

        QStandardItem* item = new QStandardItem(name);
        item->setData(id,ALBUMID);
        item->setEditable(false);
        item->setSelectable(false);
        _albumsModel->appendRow(item);
    }

    ui->listViewAlbums->setModel(_albumsModel);
    connect(ui->listViewAlbums,&QListView::clicked,this,&MainWindow::OnAlbumSelection);

    _songsModel = new QStandardItemModel(this);

    _currentAlbumID = 0;

    if(ui->listViewAlbums->model()->rowCount()>0)
        _currentAlbumID = ui->listViewAlbums->model()->index(0,0).data(ALBUMID).toInt();

    SelectAndShowAlbum(_currentAlbumID);

    connect(ui->albumsFilter,&QLineEdit::textEdited,this,&MainWindow::SelectAlbums);

    connect(ui->radioButtonAuthor,&QRadioButton::clicked,this,&MainWindow::SetFilterByAuthors);
    connect(ui->radioButtonGenre,&QRadioButton::clicked,this,&MainWindow::SetFilterByGenre);
    connect(ui->radioButtonYear,&QRadioButton::clicked,this,&MainWindow::SetFilterByYear);

    connect(ui->btnChangeImage,&QPushButton::released,this,&MainWindow::OnBtnChangeImageClick);

    SetFilterByAuthors();
}

MainWindow::~MainWindow()
{
    delete _albumsModel;
    delete _songsModel;
    delete ui;
}

void MainWindow::SetFilterByAuthors()
{
    ui->radioButtonAuthor->setChecked(true);
    ui->radioButtonGenre->setChecked(false);
    ui->radioButtonYear->setChecked(false);

    SelectAlbums(ui->albumsFilter->text());
}

void MainWindow::SetFilterByGenre()
{
    ui->radioButtonAuthor->setChecked(false);
    ui->radioButtonGenre->setChecked(true);
    ui->radioButtonYear->setChecked(false);

    SelectAlbums(ui->albumsFilter->text());
}

void MainWindow::SetFilterByYear()
{
    ui->radioButtonAuthor->setChecked(false);
    ui->radioButtonGenre->setChecked(false);
    ui->radioButtonYear->setChecked(true);

    SelectAlbums(ui->albumsFilter->text());
}

void MainWindow::SelectAlbums(const QString &text)
{
    _albumsModel->clear();

    QString query;

    if(!text.isEmpty() && text != ""){
        if(ui->radioButtonAuthor->isChecked()){
            // select albums where author
            query = QString("SELECT album.albumID AS albumID, album.name AS name, author.name AS author "
                            "FROM discs "
                           "INNER JOIN album ON discs.albumID = album.albumID "
                           "INNER JOIN author ON discs.authorID = author.authorID "
                           "WHERE author.name like '%1%2'").arg(text).arg("%");
        }
        else if(ui->radioButtonGenre->isChecked()){
            // select albums where genre
            query = QString("SELECT album.albumID AS albumID, album.name AS name, author.name AS author "
                                    "FROM discs "
                                    "INNER JOIN album ON discs.albumID = album.albumID "
                                    "INNER JOIN genre ON discs.genreID = genre.genreID "
                                    "INNER JOIN author ON discs.authorID = author.authorID "
                                    "WHERE genre.name like '%1%2'").arg(text).arg("%");
        }
        else{
            // select albums where year
            query = QString("SELECT album.albumID AS albumID, album.name AS name, author.name AS author "
                                    "FROM discs "
                                    "INNER JOIN album ON discs.albumID = album.albumID "
                                    "INNER JOIN author ON discs.authorID = author.authorID "
                                    "WHERE discs.year like '%1%2'").arg(text).arg("%");
        }
    }
    else{
        // select all albums
        query = QString("SELECT album.albumID AS albumID, album.name AS name, author.name AS author "
                        "FROM discs "
                       "INNER JOIN album ON discs.albumID = album.albumID "
                       "INNER JOIN author ON discs.authorID = author.authorID ");
    }

    _query.exec(query);

    while(_query.next()){
        int id = _query.value("albumID").toInt();
        QString author = _query.value("author").toString();
        QString name = _query.value("name").toString();

        QStandardItem* item = new QStandardItem(QString("%1 - %2").arg(author).arg(name));
        item->setData(id,ALBUMID);
        item->setEditable(false);
        item->setSelectable(false);
        _albumsModel->appendRow(item);
    }
}

void MainWindow::SelectAndShowAlbum(int albumID)
{
    // rok
    QString queryText = QString("Select discs.imgPath, discs.year, album.name as album, author.name as author, genre.name as genre "
                                "from discs "
                                "inner join album on discs.albumID = album.albumID "
                                "inner join author on discs.authorID = author.authorID "
                                "inner join genre on discs.genreID = genre.genreID "
                                "where discs.albumID=%1").arg(albumID);
    _query.exec(queryText);

    int year = 0;
    QString authorText = "";
    QString albumText = "";
    QString genreText = "";
    QString imgPath = "";

    if(_query.next()){
        year = _query.value("year").toInt();
        authorText = _query.value("author").toString();
        albumText = _query.value("album").toString();
        genreText = _query.value("genre").toString();
        imgPath = _query.value("imgPath").toString();
    }

    if(year==0)
        ui->labelYear->setText(QString("Year: n/a"));
    else
        ui->labelYear->setText(QString("Year: %1").arg(year));

    if(authorText == "" || authorText.isEmpty())
        ui->labelAuthor->setText("Author: n/a");
    else
        ui->labelAuthor->setText(QString("Author: %1").arg(authorText));

    if(albumText == "" || albumText.isEmpty())
        ui->labelAlbum->setText("Album: n/a");
    else
        ui->labelAlbum->setText(QString("Album: %1").arg(albumText));

    if(genreText == "" || genreText.isEmpty())
        ui->labelGenre->setText("Genre: n/a");
    else
        ui->labelGenre->setText(QString("Genre: %1").arg(genreText));

    //ui->graphicsView->scene()->clear();

    if(imgPath.isEmpty() || imgPath == ""){
        imgPath = "cd.bmp";
    }

    _currentImgPath = imgPath;
    SetImage();
    SetAndShowSongs(albumID);
}

void MainWindow::OnAlbumSelection(const QModelIndex &index)
{
    _currentAlbumID = _albumsModel->itemFromIndex(index)->data(ALBUMID).toInt();

    SelectAndShowAlbum(_currentAlbumID);
}

void MainWindow::OnBtnChangeImageClick()
{
    QString curDir(QDir(".").absolutePath());

    QString filePath = QFileDialog::getOpenFileName(this,
        tr("Open Image"), "images/", tr("Image Files (*.bmp)"));

    QString fileName = filePath.replace(curDir,"").replace("/images/","");

    QString queryText = QString("UPDATE discs SET imgPath='%1' where albumID='%2'").arg(fileName).arg(_currentAlbumID);
    _query.exec(queryText);

    SelectAndShowAlbum(_currentAlbumID);
}

void MainWindow::SetImage()
{
    QPixmap img(QString("images/%1").arg(_currentImgPath));

    int w = ui->labelImg->width();
    int h = ui->labelImg->height();

    if( w > h )
        ui->labelImg->setPixmap(img.scaled(h,h));
    else if(w < h)
        ui->labelImg->setPixmap(img.scaled(w,w));
    else
        ui->labelImg->setPixmap(img.scaled(w,h));
}

void MainWindow::SetAndShowSongs(int albumID)
{
    QLayoutItem *item;
    while((item= ui->songsLayout->takeAt(0)) != 0)
    {
        item->widget()->setParent(0); // you can use this or setHidden... up to you
        delete item;
    }

    _query.exec(QString("Select name, lengthInSec from song where albumID=%1").arg(albumID));

    int i = 0;
    while(_query.next()){
        if(i%2 == 0){
            ui->songsLayout->addWidget(new QLabel(QString("%1.").arg(i+1)),i,0,1,1,Qt::AlignRight);

            ui->songsLayout->addWidget(new QLabel(_query.value("name").toString()),i,1,1,1,Qt::AlignCenter);
            int songLength = _query.value("lengthInSec").toInt();
            int minutes = 0;

            while(songLength >= 60){
                songLength -= 60;
                minutes += 1;
            }

            if(songLength<10){
                ui->songsLayout->addWidget(new QLabel(QString("%1:%2").arg(minutes).arg(QString("0%1").arg(songLength))),i,2,1,1,Qt::AlignCenter);
            }
            else
                ui->songsLayout->addWidget(new QLabel(QString("%1:%2").arg(minutes).arg(songLength)),i,2,1,1,Qt::AlignCenter);
        }
        else{
            ui->songsLayout->addWidget(new QLabel(QString("%1.").arg(i+1)),i-1,3,1,1,Qt::AlignRight);

            ui->songsLayout->addWidget(new QLabel(_query.value("name").toString()),i-1,4,1,1,Qt::AlignCenter);
            int songLength = _query.value("lengthInSec").toInt();
            int minutes = 0;

            while(songLength >= 60){
                songLength -= 60;
                minutes += 1;
            }

            if(songLength<10){
                ui->songsLayout->addWidget(new QLabel(QString("%1:%2").arg(minutes).arg(QString("0%1").arg(songLength))),i-1,5,1,1,Qt::AlignCenter);
            }
            else
                ui->songsLayout->addWidget(new QLabel(QString("%1:%2").arg(minutes).arg(songLength)),i-1,5,1,1,Qt::AlignCenter);
        }
        i++;
    }

}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    SetImage();
    event->accept();
    //QWidget::resizeEvent(event);
}

void MainWindow::showEvent(QShowEvent *event)
{
    SetImage();
    event->accept();
}

