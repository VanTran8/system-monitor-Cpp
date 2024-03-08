#include <string>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
  int h = seconds / 3600;
  seconds %= 3600;
  int m = seconds / 60;
  seconds %= 60;
  string h_string = (h >= 10) ? std::to_string(h) : "0" + std::to_string(h);
  string m_string = (m >= 10) ? std::to_string(m) : "0" + std::to_string(m);
  string s_string = (seconds >= 10) ? std::to_string(seconds) : "0" + std::to_string(seconds);
  h_string += ":" + m_string + ":" + s_string;
  return h_string;
}