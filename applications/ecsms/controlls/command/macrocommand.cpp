#include "macrocommand.h"

MacroCommand::MacroCommand(std::vector<std::unique_ptr<ICommand>> && commands)
    : _commands(std::move(commands)) {}

void MacroCommand::Execute() {
  for (auto &&command : _commands)
    command->Execute();
}

void MacroCommand::UnExecute() {
  for (auto iter = _commands.rbegin(); iter != _commands.rend(); ++iter) {
    auto &&command = *iter;
    command->UnExecute();
  }
}
