/*
  keyboard.cpp

  (C) Copyright 2015, Brad Parker
  All rights reserved.

  License: 3-clause BSD. See COPYING
*/

#include "keyboard.h"
#include <iostream>
#include <QTimer>
#include <QCoreApplication>

#define BUFSIZE 8

Keyboard::Keyboard() :
  m_dev(NULL)
{
  m_dev = hid_open(0x1770, 0xff00, 0);

  if(!m_dev) {
    std::cout << "cannot open usb device" << std::endl;
    QTimer::singleShot(0, qApp, SLOT(quit()));
    return;
  }
}

Keyboard::~Keyboard() {
  if(m_dev) {
    hid_close(m_dev);
    //std::cout << "closed usb device" << std::endl;
  }
}

void Keyboard::setMode(Mode mode) {
  if(!m_dev)
    return;

  unsigned char buf[BUFSIZE] = {0};

  buf[0] = 1;
  buf[1] = 2;
  buf[2] = 65;
  buf[3] = static_cast<unsigned int>(mode);
  buf[4] = 0;
  buf[5] = 0;
  buf[6] = 0;
  buf[7] = 236;

  hid_send_feature_report(m_dev, buf, BUFSIZE);
}

void Keyboard::setColorPreset(Region region, Color color, Intensity intensity) {
  if(!m_dev)
    return;

  unsigned char buf[BUFSIZE] = {0};

  buf[0] = 1;
  buf[1] = 2;
  buf[2] = 66;
  buf[3] = static_cast<unsigned int>(region);
  buf[4] = static_cast<unsigned int>(color);
  buf[5] = static_cast<unsigned int>(intensity);
  buf[6] = 0;
  buf[7] = 236;

  hid_send_feature_report(m_dev, buf, BUFSIZE);
}

void Keyboard::setColor(Region region, char* rgb) {
  if(!m_dev)
    return;

  unsigned char buf[BUFSIZE] = {0};

  buf[0] = 1;
  buf[1] = 2;
  buf[2] = 64;
  buf[3] = static_cast<unsigned int>(region);
  buf[4] = rgb[0];
  buf[5] = rgb[1];
  buf[6] = rgb[2];
  buf[7] = 236;

  hid_send_feature_report(m_dev, buf, BUFSIZE);
}

