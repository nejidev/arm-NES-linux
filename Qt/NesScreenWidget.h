#ifndef NESSCREENWIDGET_H
#define NESSCREENWIDGET_H

#include <QObject>
#include <QMutex>
#include <QImage>
#include <QOpenGLWidget>

class NesScreenWidget : public QOpenGLWidget
{
    Q_OBJECT

protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);

public slots:
    void loadFrame();

private:
    QImage image;
    QMutex mutex;
    unsigned char *grayFrameBuff;

public:
    bool isGray;

public:
    NesScreenWidget(QWidget *parent);
    ~NesScreenWidget();
};

#endif // NESSCREENWIDGET_H
