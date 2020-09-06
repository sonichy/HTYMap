#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QToolButton>
#include <QTreeWidgetItem>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QToolButton *toolButton_colorBorder;
    QString path=".", filepath="";
    QGraphicsScene *scene;
    void open(QString filePath);
    void add(QString filePath);
    int ITEMID = Qt::UserRole + 1;

private slots:
    void on_action_open_project_triggered();
    void on_action_save_project_triggered();
    void on_action_import_triggered();
    void on_action_selectAll_triggered();
    void on_action_Qt_triggered();
    void on_action_about_triggered();
    void on_action_zoomin_triggered();
    void on_action_zoomout_triggered();
    void setColorBorder();
    void treeWidgetItemSelectionChanged();
    void treeWidgetItemChanged(QTreeWidgetItem *TWI, int column);

};

#endif // MAINWINDOW_H