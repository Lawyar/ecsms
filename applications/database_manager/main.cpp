
#include <iostream>
#include <string>
#include <vector>

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

void PrintPrompt() {
  std::cout << ">>> ";
}

void PrintHelp() {}

std::vector<std::string> Split(const std::string& str,
                               const std::string& delim) {
  std::vector<std::string> res;

  size_t start = 0;
  for (size_t end = end = str.find(delim, start); end != std::string::npos;
       end = str.find(delim, start)) {
    std::string token = str.substr(start, end - start);
    start = end + delim.size();
    res.push_back(token);
  }

  res.push_back(str.substr(start));
  return res;
}

void HandleCommand(IConnection& connection,
                   IExecutorEAV& executorEAV,
                   const std::string& command) {
  const auto tokens = Split(command, " ");
  if (tokens.empty())
    return;

  auto&& firstToken = tokens.front();
  if (firstToken == "EAV") {
  } else {
    auto&& result = connection.Execute(command);
    auto&& status = result->GetCurrentExecuteStatus();
    if (status->HasError()) {
      std::cout << status->GetErrorMessage();
      return;
    }

    size_t rowCount = result->GetRowCount();
    size_t colCount = result->GetColCount();
    for (size_t j = 0; j < colCount; ++j)
      std::cout << '\t' << result->GetColName(j);
    std::cout << '\n';
    for (size_t i = 0; i < rowCount; ++i) {
      for (size_t j = 0; j < colCount; ++j) {
        auto value = result->GetValue(i, j);
        std::cout << '\t' << value.ExtractString();
      }
      std::cout << '\n';
    }
  }
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

  auto&& dbManager = GetDatabaseManager();
  auto&& connection =
      dbManager.GetConnection("postgresql://" + username + ":" + password +
                              "@" + server + ":" + port + "/" + database);
  if (!connection || !connection->IsValid()) {
    std::cout << "Bad connection" << std::endl;
    return 1;
  }

  auto&& executorEAV = dbManager.GetExecutorEAV(connection);
  if (!executorEAV) {
    std::cout << "Bad executor EAV" << std::endl;
    return 2;
  }

  PrintHelp();

  while (true) {
    PrintPrompt();
    std::string command = ReadStringFromStdin();
    HandleCommand(*connection, *executorEAV, command);
  }
}
