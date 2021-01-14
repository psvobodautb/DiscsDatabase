#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QGraphicsScene>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    QSqlDatabase _db;
    QSqlQuery _query;

    QStandardItemModel* _albumsModel;
    QStandardItemModel* _songsModel;

    QString _currentImgPath;
    int _currentAlbumID;

    // filters
    void SetFilterByAuthors();
    void SetFilterByGenre();
    void SetFilterByYear();

    // selections
    void SelectAlbums(const QString &text);
    void SelectAndShowAlbum(int albumID);

    // slots
    void OnAlbumSelection(const QModelIndex &index);
    void OnBtnChangeImageClick();

    // others
    void SetImage();
    void SetAndShowSongs(int albumID);

    // events
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;
};
#endif // MAINWINDOW_H
