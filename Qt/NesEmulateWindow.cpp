#include "NesEmulateWindow.h"
#include "NesScreenWidget.h"

#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QMediaPlayer>
#include <QAudioFormat>
#include <QAudioOutput>
#include <QAudioDeviceInfo>

#include "NesSoundDevice.h"
#include "ui_NesEmulateWindow.h"

#include "../InfoNES_System.h"
#include "../InfoNES.h"
#include "../InfoNES_pAPU.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

WORD NesPalette[64] =
{
    0x39ce, 0x1071, 0x0015, 0x2013, 0x440e, 0x5402, 0x5000, 0x3c20,
    0x20a0, 0x0100, 0x0140, 0x00e2, 0x0ceb, 0x0000, 0x0000, 0x0000,
    0x5ef7, 0x01dd, 0x10fd, 0x401e, 0x5c17, 0x700b, 0x6ca0, 0x6521,
    0x45c0, 0x0240, 0x02a0, 0x0247, 0x0211, 0x0000, 0x0000, 0x0000,
    0x7fff, 0x1eff, 0x2e5f, 0x223f, 0x79ff, 0x7dd6, 0x7dcc, 0x7e67,
    0x7ae7, 0x4342, 0x2769, 0x2ff3, 0x03bb, 0x0000, 0x0000, 0x0000,
    0x7fff, 0x579f, 0x635f, 0x6b3f, 0x7f1f, 0x7f1b, 0x7ef6, 0x7f75,
    0x7f94, 0x73f4, 0x57d7, 0x5bf9, 0x4ffe, 0x0000, 0x0000, 0x0000
};

static NesSoundDevice *soundDevice = NULL;
static QWidget *applicationQWidget = NULL;
static unsigned char *pcmBuf = NULL;
static int pcmBufSize = 0;
static bool nesThreadRun = false;
static unsigned int joypad = 0;

NesEmulateWindow::NesEmulateWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::NesEmulateWindow)
{
    ui->setupUi(this);

    nesThread = new NesThread(this);
    applicationQWidget = this;

    connect(nesThread, SIGNAL(loadFrame()), ui->nesScreenWidget, SLOT(loadFrame()));
}

NesEmulateWindow::~NesEmulateWindow()
{
    delete ui;
}

void NesEmulateWindow::on_actionOpen_triggered()
{
    QString nesFile = QFileDialog::getOpenFileName(this, "select NES file", "/", "nes files(*.nes)");
    std::string nesFileStd = nesFile.toStdString();

    qDebug()<<nesFileStd.c_str();

    if(0 == InfoNES_Load(nesFileStd.c_str()))
    {
        nesThreadRun = true;
        nesThread->start();
    }
}

void NesEmulateWindow::on_actionExit_triggered()
{
    nesThreadRun = false;

    nesThread->terminate();
    nesThread->wait();

    exit(0);
}

void NesEmulateWindow::on_actionTrueColour_triggered()
{
    ui->actionGray->setChecked(false);
    ui->nesScreenWidget->isGray = false;
}

void NesEmulateWindow::on_actionGray_triggered()
{
    ui->actionTrueColour->setChecked(false);
    ui->nesScreenWidget->isGray = true;
}

void NesEmulateWindow::closeEvent(QCloseEvent *event)
{
    nesThreadRun = false;

    nesThread->terminate();
    nesThread->wait();
}

/**
 * 0  1   2       3       4    5      6     7
 * A  B   Select  Start  Up   Down   Left  Right
 */
void NesEmulateWindow::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
        case Qt::Key_Space://space : A
        {
            joypad &= ~(1);
            joypad |= 1;
        }break;
        case Qt::Key_B://B : B
        {
            joypad &= ~(1<<1);
            joypad |= 1<<1;
        }break;
        case Qt::Key_1://1 : Select
        {
            joypad &= ~(1<<2);
            joypad |= 1<<2;
        }break;
        case Qt::Key_2://2 : Start
        {
            joypad &= ~(1<<3);
            joypad |= 1<<3;
        }break;
        case Qt::Key_Up://Up : up
        {
            joypad &= ~(1<<4);
            joypad |= 1<<4;
        }break;
        case Qt::Key_Down://Down : Down
        {
            joypad &= ~(1<<5);
            joypad |= 1<<5;
        }break;
        case Qt::Key_Left://Left : Left
        {
            joypad &= ~(1<<6);
            joypad |= 1<<6;
        }break;
        case Qt::Key_Right://Right : Right
        {
            joypad &= ~(1<<7);
            joypad |= 1<<7;
        }break;
        default:break;
    }

    qDebug()<<"keyPressEvent:"<<joypad;
}

void NesEmulateWindow::keyReleaseEvent(QKeyEvent *event)
{
    switch(event->key())
    {
        case Qt::Key_Space://space : A
        {
            joypad &= ~(1);
        }break;
        case Qt::Key_B://B : B
        {
            joypad &= ~(1<<1);
        }break;
        case Qt::Key_1://1 : Select
        {
            joypad &= ~(1<<2);
        }break;
        case Qt::Key_2://2 : Start
        {
            joypad &= ~(1<<3);
        }break;
        case Qt::Key_Up://Up : up
        {
            joypad &= ~(1<<4);
        }break;
        case Qt::Key_Down://Down : Down
        {
            joypad &= ~(1<<5);
        }break;
        case Qt::Key_Left://Left : Left
        {
            joypad &= ~(1<<6);
        }break;
        case Qt::Key_Right://Right : Right
        {
            joypad &= ~(1<<7);
        }break;
        default:break;
    }
    qDebug()<<"keyReleaseEvent:"<<joypad;
}

void *InfoNES_MemoryCopy(void *dest, const void *src, int count)
{
    return memcpy(dest, src, count);
}

void InfoNES_ReleaseRom()
{
    if ( ROM )
    {
        free( ROM );
        ROM = NULL;
    }

    if ( VROM )
    {
        free( VROM );
        VROM = NULL;
    }
}

void *InfoNES_MemorySet(void *dest, int c, int count)
{
    return memset(dest, c, count);
}

void InfoNES_LoadFrame()
{
    NesEmulateWindow *newWindow = (NesEmulateWindow *) applicationQWidget;
    emit newWindow->nesThread->loadFrame();
}

void InfoNES_PadState(DWORD *pdwPad1, DWORD *pdwPad2, DWORD *pdwSystem)
{
    *pdwPad1 = joypad;
    *pdwPad2 = 0;
    *pdwSystem = 0;
}

void InfoNES_MessageBox(const char *pszMsg, ...)
{

}

int InfoNES_ReadRom(const char *pszFileName)
{
    /*
     *  Read ROM image file
     *
     *  Parameters
     *    const char *pszFileName          (Read)
     *
     *  Return values
     *     0 : Normally
     *    -1 : Error
     */

    FILE *fp;

    /* Open ROM file */
    fp = fopen( pszFileName, "rb" );

    if ( fp == NULL )
            return(-1);

    /* Read ROM Header */
    fread( &NesHeader, sizeof NesHeader, 1, fp );
    if ( memcmp( NesHeader.byID, "NES\x1a", 4 ) != 0 )
    {
        /* not .nes file */
        fclose( fp );
        return(-1);
    }

    /* Clear SRAM */
    memset( SRAM, 0, SRAM_SIZE );

    /* If trainer presents Read Triner at 0x7000-0x71ff */
    if ( NesHeader.byInfo1 & 4 )
    {
        fread( &SRAM[0x1000], 512, 1, fp );
    }

    /* Allocate Memory for ROM Image */
    ROM = (BYTE *) malloc( NesHeader.byRomSize * 0x4000 );

    /* Read ROM Image */
    fread( ROM, 0x4000, NesHeader.byRomSize, fp );

    if ( NesHeader.byVRomSize > 0 )
    {
        /* Allocate Memory for VROM Image */
        VROM = (BYTE *) malloc( NesHeader.byVRomSize * 0x2000 );

        /* Read VROM Image */
        fread( VROM, 0x2000, NesHeader.byVRomSize, fp );
    }

    /* File close */
    fclose( fp );
    return 0;
}

int InfoNES_Menu()
{
    return nesThreadRun ? 0 : -1;
}

void InfoNES_SoundOutput(int samples, BYTE *wave1, BYTE *wave2, BYTE *wave3, BYTE *wave4, BYTE *wave5)
{
    int i;
    unsigned char wav;
    if(0 == pcmBufSize)
    {
        pcmBuf = (unsigned char *)malloc(samples);
        if(NULL == pcmBuf)
        {
            qDebug()<<"malloc failed";
            return ;
        }
        pcmBufSize = samples;
    }
    else if(pcmBufSize < samples)
    {
        pcmBuf = (unsigned char *)realloc(pcmBuf, samples);
        if(NULL == pcmBuf)
        {
            qDebug()<<"realloc failed";
            return ;
        }
        pcmBufSize = samples;
    }

    for (i=0; i <samples; i++)
    {
        wav = (wave1[i] + wave2[i] + wave3[i] + wave4[i] + wave5[i]) / 5;
        //单声道 8位数据
        pcmBuf[i] = wav;
    }
    soundDevice->write((const char *)pcmBuf, samples);

    //Thread sleep for audio play time ms : 1000 * samples / (channel * bitWidth) / 44100
    int ms = 1000 * samples / (1 * 8) / 44100;
    QThread::msleep(ms);
}

void InfoNES_SoundInit()
{
    qDebug()<<"InfoNES_SoundInit";
}

int InfoNES_SoundOpen(int samples_per_sync, int sample_rate)
{
    qDebug()<<"InfoNES_SoundOpen " << "samples:" << samples_per_sync << " rate:" << sample_rate;

    QAudioDeviceInfo deviceInfo(QAudioDeviceInfo::defaultOutputDevice());

    QAudioFormat format;

    format.setChannelCount(1);
    format.setSampleSize(8);
    format.setCodec("audio/pcm");
    format.setSampleType(QAudioFormat::SignedInt);//fix ubuntu audio output
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleRate(sample_rate);

    if(! deviceInfo.isFormatSupported(format))
    {
        QMessageBox::information(applicationQWidget, "info", "output audio format not supported");
        return -1;
    }

    QAudioOutput *audioOutput = new QAudioOutput(deviceInfo, format, applicationQWidget);
    soundDevice = new NesSoundDevice((QObject*)applicationQWidget);
    soundDevice->open(QIODevice::ReadOnly | QIODevice::WriteOnly);

    audioOutput->start(soundDevice);

    return 0;
}

void InfoNES_SoundClose()
{
    soundDevice->close();
    delete soundDevice;

    soundDevice = NULL;
    if(NULL != pcmBuf)
    {
        free(pcmBuf);
        pcmBuf = NULL;
    }

    qDebug()<<"InfoNES_SoundClose";
}

void InfoNES_Wait()
{

}
