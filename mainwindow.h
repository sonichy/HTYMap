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
    QString path=".";
    QGraphicsScene *scene;
    void open(QString filePath);
    void add(QString filePath);
    int ITEMID = Qt::UserRole + 1;
    int z = 12;
    double lgt=116.3, ltt=39.85;
    int dp = 100;//每次移动距离
    int mpp;//米/点
    int R = 6378137;//赤道半径米
    void drawTiles(double lgt, double ltt, int z);
    void save(QString filepath);

private slots:
    void on_action_open_project_triggered();
    void on_action_save_project_triggered();
    void on_action_save_image_triggered();
    void on_action_import_triggered();
    void on_action_selectAll_triggered();
    void on_action_Qt_triggered();
    void on_action_about_triggered();
    void on_action_zoomin_triggered();
    void on_action_zoomout_triggered();
    void on_action_moveUp_triggered();
    void on_action_moveDown_triggered();
    void on_action_moveLeft_triggered();
    void on_action_moveRight_triggered();
    void setColorBorder();
    void treeWidgetItemSelectionChanged();
    void treeWidgetItemChanged(QTreeWidgetItem *TWI, int column);

};

#endif // MAINWINDOW_H