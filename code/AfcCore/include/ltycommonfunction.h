/*************************************************
  File name:
  Author:

  Date:
  Description:
    通用的功能函数
*************************************************/
#ifndef __LTYCOMMONFUNCTION_H__
#define __LTYCOMMONFUNCTION_H__
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <errno.h>
#include <pthread.h>
//#include "ltycommondefine.h"
#include "ltycommondatatype.h"
#include <sys/ioctl.h>

int ConvertHexStringToValue(const char *HexStr, unsigned long *value);
int IsHexCharacter(char hex);
//int ConvertDecStringToValue(const char *DecStr, int *value);
int ConvertDecStringToValue(const char *DecStr, unsigned long *value);
int IsDecCharacter(char hex);
void uSleep(unsigned int MicroSecond);
void mSleep(unsigned int  MilliSecond);
TBOOL MakeDirectory(const char *directory);
TBOOL IsValidDateTime(datetime_t *pDateTime);
TBOOL GetMaxDayForMonth(unsigned char year, unsigned char month, unsigned char *day);
TBOOL CaculateWeek(unsigned char year, unsigned char month, unsigned char day, week_t *pWeek);
TBOOL RectangleOverlap(rectanglearea_t *pRect1, rectanglearea_t *pRect2);
TBOOL DotInRectangle(int x, int y, rectanglearea_t *pRect);
TBOOL GetMaxValueForSpecifyDigit(int digit, unsigned long *pValue);
void PrintCurTimeOfDay(char *header);
void PrintDateTime(char *header, const datetime_t *pDatetime);
typedef void *(*ThreadEntryPtrType)(void *);
int CreateNormalThread(ThreadEntryPtrType entry, void *para, pthread_t *pid);
picturetype_t GetPictureType(char *pName);
TBOOL CalculateRectangleOverlap(rectanglearea_t *pRect1, rectanglearea_t *pRect2, rectanglearea_t *pResult);
void CaluculateMiddleAlignCoordiante(int xin, int yin, int win, int hin, int w, int h, int *x, int *y);
TBOOL AddBiasForDirPath(char *path);
int Utf8ToUnicode(const char *pString, unsigned char *unicode, int unicodelen);
#endif/*__RMCOMMONFUNCTION_H__*/



