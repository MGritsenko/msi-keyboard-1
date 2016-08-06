/*
  main.cpp

  (C) Copyright 2015, Brad Parker
  All rights reserved.

  License: 3-clause BSD. See COPYING
*/

#include <QCoreApplication>
#include <QCommandLineParser>
#include <iostream>
#include "keyboard.h"

enum CommandLineParseResult
{
  CommandLineOk,
  CommandLineError,
  CommandLineVersionRequested,
  CommandLineHelpRequested
};

struct ColorPresetOption {
  ColorPresetOption() :
  region(REGION_LEFT)
  ,color(COLOR_RED)
  ,intensity(INTENSITY_HIGH)
  {}

  Region region;
  Color color;
  Intensity intensity;
};

struct ColorOption {
  ColorOption() :
  region(TOUCHPAD)
  ,rgb{0,0,0}
  {}

  Region region;
  char rgb[3];
};


struct KeyboardOptions {
  KeyboardOptions() :
  modeOption(MODE_NORMAL)
  ,colorOptions()
  ,modeSet(false)
  ,colorSet(false)
  ,presetSet(false)
  {}

  Mode modeOption;
  QList<ColorPresetOption*> presetOptions;
  QList<ColorOption*> colorOptions;
  bool modeSet;
  bool colorSet;
  bool presetSet;

  void setMode(QString mode) {
    if(mode == "normal") { modeOption = MODE_NORMAL; }
    if(mode == "gaming") { modeOption = MODE_GAMING; }
    if(mode == "breathe") { modeOption = MODE_BREATHE; }
    if(mode == "demo") { modeOption = MODE_DEMO; }
    if(mode == "wave") { modeOption = MODE_WAVE; }

    modeSet = true;
  }

  void setColorPreset(QString colorString) {
    QStringList fields = colorString.split(',');

    if(fields.count() != 3) {
      std::cerr << "invalid color selection" << std::endl;
      qApp->quit();
      return;
    }

    QString region = fields.at(0);
    QString color = fields.at(1);
    QString intensity = fields.at(2);

    ColorPresetOption *presetOption = new ColorPresetOption;

    if(region == "left") presetOption->region = REGION_LEFT;
    else if(region == "middle") presetOption->region = REGION_MIDDLE;
    else if(region == "right") presetOption->region = REGION_RIGHT;
    else{
        std::cerr << "invalid region selection" << std::endl;
        qApp->quit();
        return;
    }

    if(color == "off") presetOption->color = COLOR_OFF;
    if(color == "red") presetOption->color = COLOR_RED;
    if(color == "orange") presetOption->color = COLOR_ORANGE;
    if(color == "yellow") presetOption->color = COLOR_YELLOW;
    if(color == "green") presetOption->color = COLOR_GREEN;
    if(color == "sky") presetOption->color = COLOR_SKY;
    if(color == "blue") presetOption->color = COLOR_BLUE;
    if(color == "purple") presetOption->color = COLOR_PURPLE;
    if(color == "white") presetOption->color = COLOR_WHITE;

    if(intensity == "high") presetOption->intensity = INTENSITY_HIGH;
    if(intensity == "medium") presetOption->intensity = INTENSITY_MEDIUM;
    if(intensity == "low") presetOption->intensity = INTENSITY_LOW;
    if(intensity == "light") presetOption->intensity = INTENSITY_LIGHT;

    presetSet = true;

    presetOptions.append(presetOption);
  }

  void setColor(QString colorString) {
    QStringList fields = colorString.split(',');

    if(fields.count() != 4) {
      std::cerr << "invalid color selection" << std::endl;
      qApp->quit();
      return;
    }

    QString region = fields.at(0);

    ColorOption *colorOption = new ColorOption;

    if(region == "left") colorOption->region = REGION_LEFT;
    if(region == "middle") colorOption->region = REGION_MIDDLE;
    if(region == "right") colorOption->region = REGION_RIGHT;
    if(region == "logo") colorOption->region = LOGO;
    if(region == "front-left") colorOption->region = FRL_LEFT;
    if(region == "front-right") colorOption->region = FRL_RIGHT;
    if(region == "touchpad") colorOption->region = TOUCHPAD;

    bool ok=false;
    for (uint k=0;k<3;k++){
        colorOption->rgb[k]=fields.at(k+1).toInt(&ok);
        if (!ok){
            std::cerr << "invalid color selection" << std::endl;
            qApp->quit();
            return;
        }
    }
    colorSet = true;

    colorOptions.append(colorOption);
  }
};

CommandLineParseResult parseCommandLine(QCommandLineParser &parser, KeyboardOptions *keyboardOptions, QString *errorMessage) {
  QCommandLineOption helpOption = parser.addHelpOption();
  QCommandLineOption versionOption = parser.addVersionOption();

  QCommandLineOption mode(QStringList() << "m" << "mode", "set color <mode>: normal, gaming, breathe, demo, wave", "mode");
  QCommandLineOption color(QStringList() << "r" << "rgb", "set an <rgb> color using the format: region,red,green,blue with rgb values between 0 and 255", "rgb");
  QCommandLineOption colorPreset(QStringList() << "c" << "color", "set a <color> preset using the format: region,color,intensity (only valid for left/middle/right regions). When using this option, a mode must be specified with -m", "color");

  parser.addOption(mode);
  parser.addOption(color);
  parser.addOption(colorPreset);

  if(!parser.parse(QCoreApplication::arguments())) {
    *errorMessage = parser.errorText();
    return CommandLineError;
  }

  if(parser.isSet(versionOption))
    return CommandLineVersionRequested;

  if(parser.isSet(helpOption))
    return CommandLineHelpRequested;

  if(parser.isSet(mode)) {
    keyboardOptions->setMode(parser.value(mode));
  }

  if(parser.isSet(colorPreset)) {
    foreach(const QString &presetValue, parser.values(colorPreset)) {
      keyboardOptions->setColorPreset(presetValue);
    }
  }

  if(parser.isSet(color)) {
    foreach(const QString &colorValue, parser.values(color)) {
      keyboardOptions->setColor(colorValue);
    }
  }

  return CommandLineOk;
}

int main(int argc, char *argv[]) {
  QCoreApplication app(argc, argv);
  app.setApplicationName("msi-keyboard");
  app.setApplicationVersion("1.1");

  QCommandLineParser parser;
  parser.setApplicationDescription("Keyboard color changer for MSI steelseries keyboards");

  QString errorMessage;
  KeyboardOptions keyboardOptions;

  switch(parseCommandLine(parser, &keyboardOptions, &errorMessage)) {
    case CommandLineOk:
      break;
    case CommandLineError:
      fputs(qPrintable(errorMessage), stderr);
      fputs("\n\n", stderr);
      fputs(qPrintable(parser.helpText()), stderr);
      return 1;
    case CommandLineVersionRequested:
      printf("%s %s\n", qPrintable(QCoreApplication::applicationName()),
             qPrintable(QCoreApplication::applicationVersion()));
      return 0;
    case CommandLineHelpRequested:
    {
      std::cout << qPrintable(parser.helpText()) << std::endl;

      QStringList regions = QStringList() << "left" << "middle" << "right"<<"logo"<<"front-left"<<"front-right"<<"touchpad";
      QStringList colors = QStringList() << "off" << "red" << "orange" << "yellow" << "green" << "sky" << "blue" << "purple" << "white";
      QStringList intensities = QStringList() << "high" << "medium" << "low" << "light";

      QString colorHelp = QString(R"(Available regions:

%1

Available color presets (only for left/middle/right regions):

%2

Available preset intensities (only for left/middle/right regions):

%3

Example:

%4
)").arg(regions.join('\n')).arg(colors.join('\n')).arg(intensities.join('\n')).arg(QString(argv[0]) + " -m normal -c left,red,high -c middle,purple,high -c right,sky,high -r touchpad,255,0,0 -r logo,255,255,255");

      std::cout << qPrintable(colorHelp) << std::endl;

      return 0;
    }
  }

  if((!keyboardOptions.modeSet || !keyboardOptions.presetSet) && !keyboardOptions.colorSet ) {
    std::cerr << "Please set either a mode together with at least one region to change a color preset, or a color region to change rgb color." << std::endl;
    return 1;
  }else{
    Keyboard k;

    if(keyboardOptions.presetSet) {
      for(int i = 0; i < keyboardOptions.presetOptions.count(); ++i) {
        ColorPresetOption *presetOption = keyboardOptions.presetOptions.at(i);

        k.setColorPreset(presetOption->region, presetOption->color, presetOption->intensity);
      }
    }

    if(keyboardOptions.colorSet) {
      for(int i = 0; i < keyboardOptions.colorOptions.count(); ++i) {
        ColorOption *colorOption = keyboardOptions.colorOptions.at(i);

        k.setColor(colorOption->region, colorOption->rgb);
      }
    }

    if(keyboardOptions.modeSet) {
      k.setMode(keyboardOptions.modeOption);
    }

  }

  return 0;
}
