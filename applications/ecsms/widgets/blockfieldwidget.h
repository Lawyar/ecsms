#ifndef BLOCKFIELD_H
#define BLOCKFIELD_H

#include "../controlls/command/commandmanager.h"
#include "../controlls/defaultcontroller.h"
#include "../controlls/drawlinecontroller.h"
#include "../observer/iobserver.h"

#include <QMap>
#include <QWidget>

class ConnectNodeWidget;

class BlockFieldWidget : public QWidget, public IObserver {
  Q_OBJECT
 public:
  BlockFieldWidget(QWidget* parent = nullptr);
  void SetCommandManager(std::shared_ptr<CommandManager> cm);
  void AddBlock(const QString& block_name);
  virtual void Update(std::shared_ptr<Event> e) override;
  std::unique_ptr<IController>& GetController();
  QWidget* FindById(Id id);
  void Clear();
  void GoToFirstBlock();
  void GoToNextBlock();

  const FieldModel& GetFieldModel() const;
  const VisualizationModel& GetVisualizationModel() const;
  void LoadFieldModel(const FieldModel::Memento& field_model_memento);
  void LoadVisualizationModel(
      const VisualizationModel::Memento& vis_model_memento);

  int GetCounter() const;
  void SetCounter(int counter);

 protected:
  void mouseMoveEvent(QMouseEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;
  void keyPressEvent(QKeyEvent* event) override;
  void enterEvent(QEvent* event) override;
  void leaveEvent(QEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;
  void paintEvent(QPaintEvent* event) override;

 private:
  void actualizeBlock(BlockId id);

 private:
  std::shared_ptr<CommandManager> _cm;
  NameMaker _block_name_maker;
  std::unique_ptr<IController> _controller;
  int _current_block = -1;
  FieldModel _field_model;
  SelectionModel _selection_model;
  PhantomLineModel _line_model;
  PhantomRectangleModel _rect_model;
  VisualizationModel _vis_model;
};

#endif  // BLOCKFIELD_H
