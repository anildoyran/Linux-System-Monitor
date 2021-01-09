#include <string>

#include "format.h"

using std::string;

string Format::ElapsedTime(long seconds) {
  int secInt = seconds % 60;
  int minInt = (seconds / 60) % 60;
  int hourInt = seconds / 3600;

  string sec = std::to_string(secInt);
  string min = std::to_string(minInt);
  string hour = std::to_string(hourInt);

  if (secInt < 10) {
    sec = "0" + sec;
  }
  if (minInt < 10) {
    min = "0" + min;
  }
  if (hourInt < 10) {
    hour = "0" + hour;
  }

  return hour + ":" + min + ":" + sec;
}