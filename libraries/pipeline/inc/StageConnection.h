#pragma once

class StageConnection {
public:
  virtual ~StageConnection() = default;

  virtual void shutdown() = 0;

  virtual bool isShutdown() = 0;
};
