#pragma once

#include "icommand.h"

#include <vector>

class CommandManager {
public:
  CommandManager() = default;
  bool HasCommandsToRedo() const;
  bool HasCommandsToUndo() const;
  void Do(ICommand *command);
  void Redo();
  void Undo();

private:
  int _ind = 0;
  std::vector<ICommand *> _commands;
};
