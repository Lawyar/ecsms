
#include <iostream>
#include <vector>
#include <string>

#include <IDatabaseManager.h>

std::string ReadStringFromStdin() {
  std::string res;
  std::getline(std::cin, res);
  return res;
}

void GetInputString(const std::string& what,
                    const std::string& defaultValue,
                    std::string& res) {
  std::cout << what << " [" << defaultValue << "]:";
  std::string readStr = ReadStringFromStdin();
  res = readStr.empty() ? defaultValue : readStr;
}

void PrintPrompt()
{
  std::cout << ">>> ";
}

void PrintHelp()
{
  
}

std::vector<std::string> split(const std::string & str, const std::string & delimiter) {
  size_t pos_start = 0, pos_end, delim_len = delimiter.length();
  std::string token;
  std::vector<std::string> res;

  for ((pos_end = str.find(delimiter, pos_start)) != std::string::npos) {
    token = str.substr(pos_start, pos_end - pos_start);
    pos_start = pos_end + delim_len;
    res.push_back(token);
  }

  res.push_back(str.substr(pos_start));
  return res;
}

void HandleCommand(const std::string & command)
{

}

int main() {
  std::string server;
  std::string database;
  std::string port;
  std::string username;
  std::string password;

  GetInputString("Server", "localhost", server);
  GetInputString("Database", "postgres", database);
  GetInputString("Port", "5432", port);
  GetInputString("Username", "postgres", username);
  GetInputString("Password", "password", password);

  PrintHelp();

  while (true)
  {
    PrintPrompt();
    std::string command = ReadStringFromStdin();
    HandleCommand(command);
  }
}
