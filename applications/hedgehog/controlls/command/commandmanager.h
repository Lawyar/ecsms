#pragma once

#include "icommand.h"

#include <vector>
#include <memory>

class CommandManager {
public:
  CommandManager() = default;
  void ClearCommands();
  bool HasCommandsToRedo() const;
  bool HasCommandsToUndo() const;
  void Do(std::unique_ptr<ICommand> command);
  void Redo();
  void Undo();

private:
  int _executed_command_count = 0;
  std::vector<std::unique_ptr<ICommand>> _commands;
};
