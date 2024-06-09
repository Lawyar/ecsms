#include "YamlToPipeline.h"


#include <fstream>

using namespace std;

Pipeline YamlToPipeline::load(const string_view path) {
  ifstream file{path};
  istream is;
  is << file.rdbuf();
  
  return load(is);
}

Pipeline YamlToPipeline::load(const std::istream& input) {
  return Pipeline();
}
