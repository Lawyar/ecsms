#include "commandmanager.h"

bool CommandManager::HasCommandsToRedo() const {
  return _ind + 1 < _commands.size();
}

bool CommandManager::HasCommandsToUndo() const { return _ind - 1 > 0; }

void CommandManager::Do(ICommand *command) {
  if (_ind < _commands.size()) {
    auto temp = _commands.size();
    _commands.erase(_commands.begin() + _ind, _commands.end());
  }
  command->Execute();
  _commands.push_back(command);
  ++_ind;
}

void CommandManager::Redo() {
  if (HasCommandsToRedo) {
    ++_ind;
    _commands[_ind]->Execute();
  }
}

void CommandManager::Undo() {
  if (HasCommandsToUndo) {
    --_ind;
    _commands[_ind]->UnExecute();
  }
}
