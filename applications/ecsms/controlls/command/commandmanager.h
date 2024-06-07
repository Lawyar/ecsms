#pragma once

#include "icommand.h"

#include <memory>
#include <vector>

class CommandManager {
public:
  class State {
  public:
    bool operator==(const State &other) {
      return other._executed_command_count == _executed_command_count &&
             other._commands == _commands;
    }
    bool operator!=(const State &other) { return !(*this == other); }

  private:
    State(int executed_command_count,
          const std::vector<std::unique_ptr<ICommand>> &commands)
        : _executed_command_count(executed_command_count) {
      for (auto &&command : commands)
        _commands.push_back(command.get());
    }

    friend CommandManager;

  private:
    int _executed_command_count;
    std::vector<void *> _commands;
  };

  CommandManager() = default;
  void ClearCommands();
  bool HasCommandsToRedo() const;
  bool HasCommandsToUndo() const;
  void Do(std::unique_ptr<ICommand> command);
  void Redo();
  void Undo();
  void AddExecuted(std::unique_ptr<ICommand> command);
  State GetState() const;

private:
  int _executed_command_count = 0;
  std::vector<std::unique_ptr<ICommand>> _commands;
};
