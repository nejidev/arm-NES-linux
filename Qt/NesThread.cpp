#include "NesThread.h"
#include <QDebug>

#include "../InfoNES.h"
#include "../InfoNES_System.h"
#include "../InfoNES_pAPU.h"

NesThread::NesThread(QObject *parent):QThread(parent)
{

}

void NesThread::run()
{
    InfoNES_Main();
}
