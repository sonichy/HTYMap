#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QDebug>
#include <QDomDocument>
#include <QGraphicsEllipseItem>
#include <QColorDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    toolButton_colorBorder = new QToolButton(this);
    toolButton_colorBorder->setText("□");
    toolButton_colorBorder->setToolTip("边框色");
    connect(toolButton_colorBorder, SIGNAL(clicked()), this, SLOT(setColorBorder()));
    ui->mainToolBar->addWidget(toolButton_colorBorder);

    ui->treeWidget->setHeaderLabel("图层");
    ui->splitter->setStretchFactor(0,2);
    ui->splitter->setStretchFactor(1,8);
    scene = new QGraphicsScene;
    scene->setSceneRect(ui->graphicsView->rect());
    ui->graphicsView->setScene(scene);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_Qt_triggered()
{
    QMessageBox::aboutQt(nullptr, "关于 Qt");
}

void MainWindow::on_action_about_triggered()
{
    QMessageBox MB(QMessageBox::NoIcon, "关于", "海天鹰地图 1.0\n一款基于Qt5的地图轨迹工具。\n作者：海天鹰\nE-mail: sonichy@163.com\n主页：https://github.com/sonichy\nDOM读写增删改：https://blog.csdn.net/u012234115/article/details/43203001");
    MB.setIconPixmap(QPixmap(":/HTYMap.png"));
    MB.exec();
}

void MainWindow::on_action_open_project_triggered()
{
    filepath = QFileDialog::getOpenFileName(this, "打开工程", path, "地图工程 (*.mpj)");
    if (!filepath.isEmpty()) {
        open(filepath);
    }
}

void MainWindow::on_action_save_project_triggered()
{

}

void MainWindow::open(QString filePath)
{
    path = filePath;
}

void MainWindow::on_action_import_triggered()
{
    filepath = QFileDialog::getOpenFileName(this, "添加轨迹", path, "地图轨迹 (*.gpx)");
    if (!filepath.isEmpty()) {
        add(filepath);
    }
}

void MainWindow::add(QString filePath)
{
    QFile file(filePath);
    if(!file.open(QFile::ReadOnly))
        return;
    path = filePath;
    QDomDocument doc;
    if (!doc.setContent(&file)) {
        file.close();
        return;
    }
    file.close();

    QTreeWidgetItem *TWI_root = new QTreeWidgetItem(ui->treeWidget);
    TWI_root->setText(0, QFileInfo(filePath).fileName());
    TWI_root->setIcon(0, QIcon::fromTheme("application-gpx+xml", QIcon(":/HTYMap.png")));

    QDomElement root = doc.documentElement();
    QDomNodeList DNL = root.elementsByTagName("trkpt");
    QPainterPath PP;
    qreal lonc=0, latc=0;
    for (int i=0; i<DNL.count(); i++) {
        QDomNode DN = DNL.at(i);
        if (DN.isElement()) {
            QDomElement DE = DN.toElement();
            qreal lon = DE.attribute("lon").toDouble();
            qreal lat = DE.attribute("lat").toDouble();
            QTreeWidgetItem *TWI_point = new QTreeWidgetItem(TWI_root);
            TWI_point->setText(0, QString("%1").arg(lon, 0, 'f', 14) + "," + QString("%1").arg(lat, 0, 'f', 14));
            TWI_point->setIcon(0, QIcon(":/marker.png"));
            if (i == 0){
                lonc = lon;
                latc = lat;
            } else {
                qreal x = (lon - lonc) * 10000;
                qreal y = - (lat - latc) * 10000;
                qDebug() << QString("%1").arg(x, 0, 'f', 14) << "," << QString("%1").arg(y, 0, 'f', 14);
                PP.lineTo(x, y);
            }
        }
    }
    QPalette plt = toolButton_colorBorder->palette();
    QColor color = plt.color(QPalette::ButtonText);
    QPen pen(color, 1);
    QGraphicsPathItem *GPI = scene->addPath(PP, pen);
    GPI->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
}

void MainWindow::on_action_zoomout_triggered()
{
    ui->graphicsView->scale(0.9, 0.9);
}

void MainWindow::on_action_zoomin_triggered()
{
    ui->graphicsView->scale(1.1, 1.1);
}

void MainWindow::on_action_selectAll_triggered()
{
    QList<QGraphicsItem*> list_item = scene->items();
    qDebug() << list_item;
    for(int i=0; i<list_item.size(); i++){
        list_item[i]->setSelected(true);        
    }
    ui->statusBar->showMessage("共 " + QString::number(list_item.size()) + " 条轨迹");
}

void MainWindow::setColorBorder()
{
    QPalette plt = toolButton_colorBorder->palette();
    QColor color = plt.color(QPalette::ButtonText);
    color = QColorDialog::getColor(color, this);
    if (color.isValid()) {
        plt.setColor(QPalette::ButtonText, color);
        toolButton_colorBorder->setPalette(plt);
        QList<QGraphicsItem*> list_item = scene->selectedItems();
        for(int i=0; i<list_item.size(); i++){
            if(list_item[i]->type() == QGraphicsPathItem::Type){
                QPen pen(color);
                QGraphicsPathItem *GPI = qgraphicsitem_cast<QGraphicsPathItem*>(list_item[i]);
                GPI->setPen(pen);
            }
        }
        QRgb rgb = qRgb(color.red(), color.green(), color.blue());
        ui->statusBar->showMessage("轨迹颜色改为 " + QString::number(rgb,16));
    }
}