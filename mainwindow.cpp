#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QDebug>
#include <QDomDocument>
#include <QGraphicsEllipseItem>
#include <QColorDialog>
#include <QDateTime>
#include <QtMath>
//#include <QDesktopWidget>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    showMaximized();
    toolButton_colorBorder = new QToolButton(this);
    toolButton_colorBorder->setText("□");
    toolButton_colorBorder->setToolTip("边框色");
    connect(toolButton_colorBorder, SIGNAL(clicked()), this, SLOT(setColorBorder()));
    ui->mainToolBar->addWidget(toolButton_colorBorder);

    ui->treeWidget->setHeaderLabel("图层");    
    ui->splitter->setStretchFactor(0,2);
    ui->splitter->setStretchFactor(1,8);
    ui->treeWidget->hide();
    scene = new QGraphicsScene;
    scene->setSceneRect(0, 0, 256 * 7, 256 * 4);    //根据瓦片大小和数目设置
    ui->graphicsView->setScene(scene);    

    connect(ui->action_side, &QAction::triggered, [=](bool b){
        if (b) {
            ui->treeWidget->show();
        } else {
            ui->treeWidget->hide();
        }
    });
    connect(ui->action_refresh,  &QAction::triggered, [=]{
        drawTiles(lgt, ltt, z);
    });
    connect(ui->treeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(treeWidgetItemSelectionChanged()));
    connect(ui->treeWidget, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(treeWidgetItemChanged(QTreeWidgetItem*, int)));

    initTiles();
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
    QMessageBox MB(QMessageBox::NoIcon, "关于", "海天鹰地图 1.0\n一款基于Qt5的地图轨迹工具。\n作者：海天鹰\nE-mail: sonichy@163.com\n主页：https://github.com/sonichy\nDOM读写增删改：https://blog.csdn.net/u012234115/article/details/43203001\n栅格瓦片坐标计算：https://mp.weixin.qq.com/s?__biz=MzAwNDk4MTg3Mg==&mid=2247483661&idx=1&sn=4248dc1fa9f97094527903330f1c11a6");
    MB.setIconPixmap(QPixmap(":/HTYMap.png"));
    MB.exec();
}

void MainWindow::on_action_open_project_triggered()
{
    path = QFileDialog::getOpenFileName(this, "打开工程", path, "地图工程 (*.mpj)");
    if (!path.isEmpty()) {
        open(path);
    }
}

void MainWindow::on_action_save_project_triggered()
{

}

void MainWindow::on_action_save_image_triggered()
{
    if (path == "")
        path = "./未命名.png";
    path = QFileDialog::getSaveFileName(this, "保存图片", path, "图片文件(*.png)");
    if (path.length() != 0) {
        save(path);
    }
}

void MainWindow::save(QString filepath)
{
    scene->clearSelection();
    qDebug() << scene->width() << scene->height();
    QSize size(static_cast<int>(scene->width()), static_cast<int>(scene->height()));
    QImage image(size, QImage::Format_ARGB32);
    image.fill(Qt::white);
    QPainter painter(&image);
    scene->render(&painter);
    image.save(filepath);
}

void MainWindow::open(QString filePath)
{
    path = filePath;
}

void MainWindow::on_action_import_triggered()
{
    if (path == "")
        path = ".";
    path = QFileDialog::getOpenFileName(this, "添加轨迹", path, "地图轨迹 (*.gpx)");
    if (!path.isEmpty()) {
        add(path);
    }
}

void MainWindow::add(QString filePath)
{
    QFile file(filePath);
    if (!file.open(QFile::ReadOnly))
        return;
    path = filePath;
    QDomDocument doc;
    if (!doc.setContent(&file)) {
        file.close();
        return;
    }
    file.close();

    QDateTime dateTime = QDateTime::currentDateTime();
    QString itemId = dateTime.toString("yyyyMMddhhmmsszzz");

    QTreeWidgetItem *TWI_root = new QTreeWidgetItem(ui->treeWidget);
    TWI_root->setIcon(0, QIcon::fromTheme("application-gpx+xml", QIcon(":/HTYMap.png")));
    TWI_root->setData(0, ITEMID, itemId);
    TWI_root->setCheckState(0, Qt::Checked);

    QDomElement root = doc.documentElement();
    QDomNodeList DNL = root.elementsByTagName("trkpt");
    TWI_root->setText(0, QFileInfo(filePath).fileName() + "(" + QString::number(DNL.count()) + ")");
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
            if (i == 0) {
                lonc = lon;
                latc = lat;
            } else {
                qreal x = (lon - lonc) * 10000;
                qreal y = - (lat - latc) * 10000;
                //qDebug() << QString("%1").arg(x, 0, 'f', 14) << "," << QString("%1").arg(y, 0, 'f', 14);
                PP.lineTo(x, y);
            }
        }
    }    
    QPalette plt = toolButton_colorBorder->palette();
    QColor color = plt.color(QPalette::ButtonText);
    QPen pen(color, 1);
    QGraphicsPathItem *GPI = scene->addPath(PP, pen);
    GPI->setPos(500, 500);
    GPI->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
    GPI->setData(ITEMID, itemId);
}

void MainWindow::on_action_zoomout_triggered()
{
    if (list_surl.length() > 0) {
        z--;
        drawTiles(lgt, ltt, z);
    }
}

void MainWindow::on_action_zoomin_triggered()
{
    if (list_surl.length() > 0) {
        z++;
        drawTiles(lgt, ltt, z);
    }
}

void MainWindow::on_action_moveUp_triggered()
{
    if (list_surl.length() > 0) {
        int tp = dp * mpp; //点距转米
        ltt = ltt * M_PI / 180; //角度转弧度
        ltt = qAtan((R * qTan(ltt) + tp) / R);
        ltt = 180 * ltt / M_PI;  //弧度转角度
        drawTiles(lgt, ltt, z);
    }
}

void MainWindow::on_action_moveDown_triggered()
{
    if (list_surl.length() > 0) {
        int tp = dp * mpp; //点距转米
        ltt = ltt * M_PI / 180; //角度转弧度
        ltt = qAtan((R * qTan(ltt) - tp) / R);
        ltt = 180 * ltt / M_PI;  //弧度转角度
        drawTiles(lgt, ltt, z);
    }
}

void MainWindow::on_action_moveLeft_triggered()
{
    if (list_surl.length() > 0) {
        double a = static_cast<double>(dp * mpp) / R;
        a = 180 * a / M_PI;  //弧度转角度
        lgt -= a;
        drawTiles(lgt, ltt, z);
    }
}

void MainWindow::on_action_moveRight_triggered()
{
    if (list_surl.length() > 0) {
        double a = static_cast<double>(dp * mpp) / R;
        a = 180 * a / M_PI;  //弧度转角度
        lgt += a;
        drawTiles(lgt, ltt, z);
    }
}

void MainWindow::on_action_selectAll_triggered()
{
    QList<QGraphicsItem*> list_item = scene->items();
    qDebug() << list_item;
    for (int i=0; i<list_item.size(); i++) {
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
        for (int i=0; i<list_item.size(); i++) {
            if (list_item[i]->type() == QGraphicsPathItem::Type) {
                QPen pen(color);
                QGraphicsPathItem *GPI = qgraphicsitem_cast<QGraphicsPathItem*>(list_item[i]);
                GPI->setPen(pen);
            }
        }
        QRgb rgb = qRgb(color.red(), color.green(), color.blue());
        ui->statusBar->showMessage("轨迹颜色改为 " + QString::number(rgb,16));
    }
}

void MainWindow::treeWidgetItemSelectionChanged()
{
    scene->clearSelection();
    QList<QTreeWidgetItem*> list_TWI = ui->treeWidget->selectedItems();
    QList<QGraphicsItem*> list_GI = scene->items();
    for (int i=0; i<list_GI.length(); i++) {
        for (int j=0; j<list_TWI.length(); j++) {
            QString itemId = list_TWI.at(j)->data(0, ITEMID).toString();
            QString itemId1 = list_GI.at(i)->data(ITEMID).toString();
            if (itemId == itemId1) {
                list_GI.at(i)->setSelected(true);
            }
        }
    }
}

void MainWindow::treeWidgetItemChanged(QTreeWidgetItem *TWI, int column)
{
    Q_UNUSED(column);
    QString itemId = TWI->data(0, ITEMID).toString();
    QList<QGraphicsItem*> list_GI = scene->items();
    for (int i=0; i<list_GI.length(); i++) {
        QString itemId1 = list_GI.at(i)->data(ITEMID).toString();
        if (itemId == itemId1) {
            if (TWI->checkState(0) == Qt::Checked){
                list_GI.at(i)->setVisible(true);
            } else {
                list_GI.at(i)->setVisible(false);
            }
        }
    }
}

void MainWindow::drawTiles(double lgt, double ltt, int z)
{
    ui->statusBar->showMessage("缩放：" + QString::number(z));    
    int c = static_cast<int>(2 * M_PI * R);
    qDebug() << "c" << c;
    int xo = - c / 2;
    int yo = c / 2;
    qDebug() << "xo, yo" << xo << yo;
    int tcols = static_cast<int>(qPow(2,z));
    mpp = c / tcols / 256;
    lgt = lgt * M_PI / 180;
    ltt = ltt * M_PI / 180;
    int x1 = static_cast<int>(R * lgt);
    int y1 = static_cast<int>(R * qTan(ltt));
    qDebug() << "x1, y1" << x1 << y1;
    int dx = x1 - xo;
    int dy = yo - y1;
    qDebug() << "dx, dy" << dx << dy;
    int x = static_cast<int>(static_cast<long>(dx) * tcols / c);
    int y = static_cast<int>(static_cast<long>(dy) * tcols / c);
    qDebug() << "x, y" << x << y;

    //int yi = 0;
    qDebug() << list_layer.length();
    for (int l=0; l<list_layer.length(); l++) {
        int cc = 0;
        for (int j=y; j<y+4; j++) { //左上角-+移动到中心
            for (int i=x-3; i<x+5; i++) {
                QString surl = list_surl.at(l).arg(QString::number(i)).arg(QString::number(j)).arg(QString::number(z));
                //qDebug() << "layer" << l << cc << surl;
                list_layer.at(l).at(cc)->setPixmapFormUrl(surl);
                cc++;
            }
        }
    }
}

void MainWindow::initTiles()
{    
    //list_surl.append("http://mt3.google.cn/vt/lyrs=y&hl=zh-CN&gl=CN&x=%1&y=%2&z=%3");    //谷歌卫星
    QString surl = "http://t0.tianditu.gov.cn/img_w/wmts?SERVICE=WMTS&REQUEST=GetTile&VERSION=1.0.0&LAYER=img&STYLE=default&TILEMATRIXSET=w&FORMAT=tiles&TILEMATRIX=%3&TILEROW=%2&TILECOL=%1&tk=736d78e979b7da008bd4a1222dcf6313";
    list_surl.append(surl);   //天地图影像
    surl = "http://t0.tianditu.gov.cn/cia_w/wmts?SERVICE=WMTS&REQUEST=GetTile&VERSION=1.0.0&LAYER=cia&STYLE=default&TILEMATRIXSET=w&FORMAT=tiles&TILEMATRIX=%3&TILEROW=%2&TILECOL=%1&tk=736d78e979b7da008bd4a1222dcf6313";
    list_surl.append(surl);    //天地图影像注记
    for (int u=0; u<list_surl.length(); u++) {
        QTreeWidgetItem *TWI_root = new QTreeWidgetItem(ui->treeWidget);
        TWI_root->setText(0, "图层" + QString::number(u));
        TWI_root->setIcon(0, QIcon(":/layer.png"));
        TWI_root->setToolTip(0, "layer" + QString::number(u));
        TWI_root->setCheckState(0, Qt::Checked);
        TWI_root->setData(0, ITEMID, "layer" + QString::number(u));

        int cc = 0;
        QList<TileItem*> list_tileItem;
        for (int y=0; y<4*256; y+=256) {
            for (int x=0; x<8*256; x+=256) {
                TileItem *tileItem = new TileItem;
                tileItem->setData(ITEMID, "layer" + QString::number(u));
                scene->addItem(tileItem);
                //qDebug() << "pos: " << x << y;
                tileItem->setPos(x, y);
                list_tileItem.append(tileItem);
                cc++;
            }
        }
        list_layer.append(list_tileItem);
        qDebug() << cc;
    }
    drawTiles(lgt, ltt, z);
}