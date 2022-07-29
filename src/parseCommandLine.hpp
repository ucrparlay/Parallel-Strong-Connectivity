#ifndef COMMAND_H
#define COMMAND_H

using namespace std;

struct CommandLine {
  int argc;
  char** argv;
  CommandLine(int c, char** v) : argc(c), argv(v) {}
  bool getOption(string option) {
    for (int i = 1; i < argc; i++) {
      if (argv[i] == option) return true;
    }
    return false;
  }
  char* getOptionValue(string option) {
    for (int i = 1; i < argc; i++) {
      if (argv[i] == option) return argv[i + 1];
    }
    return NULL;
  }
  template <class T>
  T getOptionInt(string option, T default_value) {
    char* opt_string = getOptionValue(option);
    if (opt_string)
      return (T)atol(opt_string);
    else
      return default_value;
  }
  template <class T>
  T getOptionDouble(string option, T default_value) {
    char* opt_string = getOptionValue(option);
    if (opt_string)
      return (T)atof(opt_string);
    else
      return default_value;
  }
};

#endif