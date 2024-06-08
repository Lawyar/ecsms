#pragma once

#include "icommand.h"

#include <vector>
#include <memory>

class MacroCommand : public ICommand {
public:
  MacroCommand(std::vector<std::unique_ptr<ICommand>> && commands);
  virtual void Execute() override;
  virtual void UnExecute() override;

private:
  std::vector<std::unique_ptr<ICommand>> _commands;
};
