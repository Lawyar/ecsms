#include "commandmanager.h"

void CommandManager::ClearCommands() { 
  _commands.clear();
  _executed_command_count = 0;
}

bool CommandManager::HasCommandsToRedo() const {
  return _executed_command_count < _commands.size();
}

bool CommandManager::HasCommandsToUndo() const {
  return _executed_command_count > 0;
}

void CommandManager::Do(std::unique_ptr<ICommand> command) {
  if (HasCommandsToRedo()) {
    _commands.erase(_commands.begin() + _executed_command_count,
                    _commands.end());
  }
  command->Execute();
  _commands.push_back(std::move(command));
  ++_executed_command_count;
}

void CommandManager::Redo() {
  if (HasCommandsToRedo()) {
    _commands[_executed_command_count]->Execute();
    ++_executed_command_count;
  }
}

void CommandManager::Undo() {
  if (HasCommandsToUndo()) {
    --_executed_command_count;
    _commands[_executed_command_count]->UnExecute();
  }
}

void CommandManager::AddExecuted(std::unique_ptr<ICommand> command) {
  if (HasCommandsToRedo()) {
    _commands.erase(_commands.begin() + _executed_command_count,
                    _commands.end());
  }
  _commands.push_back(std::move(command));
  ++_executed_command_count;
}

CommandManager::State CommandManager::GetState() const {
  State res{_executed_command_count, _commands};
  return res;
}
