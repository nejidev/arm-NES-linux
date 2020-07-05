#include "NesScreenWidget.h"

#include "../InfoNES_System.h"
#include "../InfoNES.h"
#include "../InfoNES_pAPU.h"

#include <QMutexLocker>
#include <QPainter>
#include <QPoint>
#include <QThread>
#include <QDebug>


NesScreenWidget::NesScreenWidget(QWidget *parent):QOpenGLWidget(parent)
{
    image = QImage((uchar *)WorkFrame, NES_DISP_WIDTH, NES_DISP_HEIGHT, QImage::Format_RGB16);
}

NesScreenWidget::~NesScreenWidget()
{
    QMutexLocker locker(&mutex);
}

void NesScreenWidget::resizeEvent(QResizeEvent *event)
{
    qDebug()<<"resize width:" << width() << " height:" << height();
}

void NesScreenWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter;
    QMutexLocker locker(&mutex);

    painter.begin(this);
    painter.drawImage(QPoint(0, 0), image);
    painter.end();
}

void NesScreenWidget::loadFrame()
{
    image = QImage((uchar *)WorkFrame, NES_DISP_WIDTH, NES_DISP_HEIGHT, QImage::Format_RGB16);
    update();
}
