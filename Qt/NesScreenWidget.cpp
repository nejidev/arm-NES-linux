#include "NesScreenWidget.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../InfoNES_System.h"
#include "../InfoNES.h"
#include "../InfoNES_pAPU.h"

#include <QMutexLocker>
#include <QPainter>
#include <QPoint>
#include <QThread>
#include <QDebug>


NesScreenWidget::NesScreenWidget(QWidget *parent):QOpenGLWidget(parent), isGray(false)
{
    grayFrameBuff = (unsigned char *)malloc(sizeof(WorkFrame));

    image = QImage((uchar *)WorkFrame, NES_DISP_WIDTH, NES_DISP_HEIGHT, QImage::Format_RGB16);
}

NesScreenWidget::~NesScreenWidget()
{
    QMutexLocker locker(&mutex);
    free(grayFrameBuff);
}

void NesScreenWidget::resizeEvent(QResizeEvent *event)
{
    qDebug()<<"resize width:" << width() << " height:" << height();
}

void NesScreenWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter;

    painter.begin(this);
    painter.drawImage(QPoint(0, 0), image);
    painter.end();
}

void NesScreenWidget::loadFrame()
{
    QMutexLocker locker(&mutex);
    if(isGray)
    {
        //565
        uchar R,G,B;
        int i;

        for(i=0; i<NES_DISP_WIDTH * NES_DISP_HEIGHT; i++)
        {
            unsigned short color = WorkFrame[i];
            R = (color>>11 & 0xff) <<3;
            G = (color>>5 & 0x3f) <<2;
            B = (color & 0x1f) <<3;

            // Gray = (R*30 + G*59 + B*11 + 50) / 100
            grayFrameBuff[i] = (R*30 + G*59 + B*11 + 50) / 100;
        }
        image = QImage((uchar *)grayFrameBuff, NES_DISP_WIDTH, NES_DISP_HEIGHT, QImage::Format_Grayscale8);
    }
    else
    {
        image = QImage((uchar *)WorkFrame, NES_DISP_WIDTH, NES_DISP_HEIGHT, QImage::Format_RGB16);
    }

    update();
}
