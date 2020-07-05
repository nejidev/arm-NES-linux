#ifndef NESTHREAD_H
#define NESTHREAD_H

#include <QObject>
#include <QThread>

class NesThread : public QThread
{
    Q_OBJECT

protected:
    void run();

signals:
    void loadFrame();

public:
    NesThread(QObject *parent=0);
};

#endif // NESTHREAD_H
