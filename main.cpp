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

struct ColorOption {
  ColorOption() :
  region(REGION_LEFT)
  ,color(COLOR_RED)
  ,intensity(INTENSITY_HIGH)
  {}

  Region region;
  Color color;
  Intensity intensity;
};

struct KeyboardOptions {
  KeyboardOptions() :
  modeOption(MODE_NORMAL)
  ,colorOptions()
  ,modeSet(false)
  ,colorSet(false)
  {}

  Mode modeOption;
  QList<ColorOption*> colorOptions;
  bool modeSet;
  bool colorSet;

  void setMode(QString mode) {
    if(mode == "normal") { modeOption = MODE_NORMAL; }
    if(mode == "gaming") { modeOption = MODE_GAMING; }
    if(mode == "breathe") { modeOption = MODE_BREATHE; }
    if(mode == "demo") { modeOption = MODE_DEMO; }
    if(mode == "wave") { modeOption = MODE_WAVE; }

    modeSet = true;
  }

  void setColor(QString colorString) {
    QStringList fields = colorString.split(',');

    if(fields.count() != 3) {
      std::cerr << "invalid color selection" << std::endl;
      qApp->quit();
      return;
    }

    QString region = fields.at(0);
    QString color = fields.at(1);
    QString intensity = fields.at(2);

    ColorOption *colorOption = new ColorOption;

    if(region == "left") colorOption->region = REGION_LEFT;
    if(region == "middle") colorOption->region = REGION_MIDDLE;
    if(region == "right") colorOption->region = REGION_RIGHT;

    if(color == "off") colorOption->color = COLOR_OFF;
    if(color == "red") colorOption->color = COLOR_RED;
    if(color == "orange") colorOption->color = COLOR_ORANGE;
    if(color == "yellow") colorOption->color = COLOR_YELLOW;
    if(color == "green") colorOption->color = COLOR_GREEN;
    if(color == "sky") colorOption->color = COLOR_SKY;
    if(color == "blue") colorOption->color = COLOR_BLUE;
    if(color == "purple") colorOption->color = COLOR_PURPLE;
    if(color == "white") colorOption->color = COLOR_WHITE;

    if(intensity == "high") colorOption->intensity = INTENSITY_HIGH;
    if(intensity == "medium") colorOption->intensity = INTENSITY_MEDIUM;
    if(intensity == "low") colorOption->intensity = INTENSITY_LOW;
    if(intensity == "light") colorOption->intensity = INTENSITY_LIGHT;

    colorSet = true;

    colorOptions.append(colorOption);
  }
};

CommandLineParseResult parseCommandLine(QCommandLineParser &parser, KeyboardOptions *keyboardOptions, QString *errorMessage) {
  QCommandLineOption helpOption = parser.addHelpOption();
  QCommandLineOption versionOption = parser.addVersionOption();

  QCommandLineOption mode(QStringList() << "m" << "mode", "set color <mode>: normal, gaming, breathe, demo, wave", "mode");
  QCommandLineOption color(QStringList() << "c" << "color", "set a <color> using the format: region,color,intensity", "color");

  parser.addOption(mode);
  parser.addOption(color);

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
  app.setApplicationVersion("1.0");

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

      QStringList regions = QStringList() << "left" << "middle" << "right";
      QStringList colors = QStringList() << "off" << "red" << "orange" << "yellow" << "green" << "sky" << "blue" << "purple" << "white";
      QStringList intensities = QStringList() << "high" << "medium" << "low" << "light";

      QString colorHelp = QString(R"(Available regions:

%1

Available colors:

%2

Available intensities:

%3

Example:

%4
)").arg(regions.join('\n')).arg(colors.join('\n')).arg(intensities.join('\n')).arg(QString(argv[0]) + " -m normal -c left,red,high -c middle,purple,high -c right,sky,high");

      std::cout << qPrintable(colorHelp) << std::endl;

      return 0;
    }
  }

  if(!keyboardOptions.modeSet || !keyboardOptions.colorSet) {
    std::cerr << "Please set a mode as well as at least one color region to change." << std::endl;
    return 1;
  }else{
    Keyboard k;

    if(keyboardOptions.colorSet) {
      for(int i = 0; i < keyboardOptions.colorOptions.count(); ++i) {
        ColorOption *colorOption = keyboardOptions.colorOptions.at(i);

        k.setColor(colorOption->region, colorOption->color, colorOption->intensity);
      }
    }

    if(keyboardOptions.modeSet) {
      k.setMode(keyboardOptions.modeOption);
    }

  }

  return 0;
}
