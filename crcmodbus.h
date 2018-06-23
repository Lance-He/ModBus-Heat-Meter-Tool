#ifndef CRCMODBUS_H
#define CRCMODBUS_H
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "crcmodbus.h"
#include <stdio.h>
#include  <QDebug>
# include <stdlib.h>

void PASSCRC(char *data,unsigned int dataLen);
unsigned short CRC16( char *puchMsg, int  usDataLen);

#endif
