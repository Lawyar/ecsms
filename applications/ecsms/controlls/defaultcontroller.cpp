#include "defaultcontroller.h"
#include "../utility/selectionutility.h"
#include "../widgets/blockfieldwidget.h"
#include "command/macrocommand.h"
#include "command/moveblockcommand.h"
#include "command/removecommand.h"

#include <QDebug>

DefaultController::DefaultController(
    FieldModel &field_model, SelectionModel &selection_model,
    PhantomLineModel &phantom_line_model,
    PhantomRectangleModel &phantom_rectangle_model,
    VisualizationModel &vis_model, CommandManager &cm)
    : _field_model(field_model), _selection_model(selection_model),
      _phantom_line_model(phantom_line_model),
      _phantom_rectangle_model(phantom_rectangle_model), _vis_model(vis_model),
      _cm(cm) {}

void DefaultController::onMouseMoveEvent(QWidget *widget, QMouseEvent *event) {
  const QPoint vis_point = event->pos();

  if (qobject_cast<BlockFieldWidget *>(widget)) {
    if (event->buttons() == Qt::MiddleButton && _old_mouse_pos) {
      QPoint delta = vis_point - *_old_mouse_pos;
      _vis_model.SetNewCoordCenter(*_old_field_pos + delta);
    }
  }

  else if (auto &&block_w = qobject_cast<BlockWidget *>(widget)) {
    if (event->buttons() == Qt::LeftButton && _old_mouse_pos) {
      QPoint delta = vis_point - *_old_mouse_pos;

      auto &&selected_blocks = _selection_model.GetSelectedBlocks();
      for (auto &&selected_block_id : selected_blocks) {
        auto &&block_data = _field_model.GetBlockData(selected_block_id);
        if (!block_data) {
          assert(false);
          return;
        }
        block_data->pos += delta;
        _field_model.SetBlockData(selected_block_id, *block_data);
      }
    }
  }
}

void DefaultController::onMousePressEvent(QWidget *widget, QMouseEvent *event) {
  const QPoint vis_point = event->pos();

  if (qobject_cast<BlockFieldWidget *>(widget)) {
    onFieldMousePress(event);
  }

  else if (auto &&block_w = qobject_cast<BlockWidget *>(widget)) {
    if (event->button() == Qt::LeftButton) {
      _old_mouse_pos = vis_point;

      if (auto block_data = _field_model.GetBlockData(block_w->GetId()))
        _old_block_model_pos = block_data->pos;
      else {
        assert(false);
        _old_block_model_pos = std::nullopt;
      }
      _selection_model.AddSelection(block_w->GetId());
    }
  }

  else if (auto &&connect_node_w = qobject_cast<ConnectNodeWidget *>(widget)) {
    auto &&node_center =
        _vis_model.MapToModel(connect_node_w->getCenterCoordToBlockField());
    _phantom_line_model.SetBegin(connect_node_w->GetId(), node_center);
  }
}

void DefaultController::onKeyPressEvent(QWidget *widget, QKeyEvent *event) {
  if (qobject_cast<BlockFieldWidget *>(widget)) {
    onFieldKeyPress(event);
  }
}

void DefaultController::onEnterEvent(QWidget *widget, QEvent *event) {
  if (auto &&block_w = qobject_cast<BlockWidget *>(widget)) {
    _active_nodes_lock.reset(new ActiveNodesLock(
        _field_model,
        {block_w->GetLeftNode()->GetId(), block_w->GetRightNode()->GetId()},
        [this](const NodeId &node) {
          return _field_model.IsNodeConnected(node);
        }));
  }
}

void DefaultController::onLeaveEvent(QWidget *widget, QEvent *event) {
  if (auto &&block_w = qobject_cast<BlockWidget *>(widget)) {
    _active_nodes_lock.reset();
  }
}

void DefaultController::onMouseReleaseEvent(QWidget *widget,
                                            QMouseEvent *event) {
  if (qobject_cast<BlockFieldWidget *>(widget)) {
    _old_field_pos = std::nullopt;
    _old_mouse_pos = std::nullopt;
  }

  else if (auto &&block_w = qobject_cast<BlockWidget *>(widget)) {
    auto delta = _vis_model.MapToModel(block_w->pos()) - * _old_block_model_pos;
    std::vector<std::unique_ptr<ICommand>> move_coms_vec;
    auto &&selected_blocks = _selection_model.GetSelectedBlocks();
    for (auto &&selected_block_id : selected_blocks) {
      auto block_data = _field_model.GetBlockData(selected_block_id);
      if (block_data && _old_block_model_pos &&
          *_old_block_model_pos != block_data->pos) {
        move_coms_vec.push_back(std::make_unique<MoveBlockCommand>(
            _field_model, selected_block_id, delta));
      }
    }
    if (!move_coms_vec.empty())
      _cm.AddExecuted(std::make_unique<MacroCommand>(std::move(move_coms_vec)));
    _old_block_model_pos = std::nullopt;
    _old_mouse_pos = std::nullopt;
  }
}

void DefaultController::onFieldMousePress(const QMouseEvent *event) {
  const QPoint vis_point = event->pos();
  const QPoint model_point = _vis_model.MapToModel(vis_point);

  if (event->button() == Qt::MiddleButton) {
    _selection_model.Clear();
    _old_field_pos = _vis_model.GetCenterCoord();
    _old_mouse_pos = vis_point;
  } else if (event->button() == Qt::LeftButton) {
    bool point_on_line = checkLine(_field_model, _selection_model, model_point);
    if (!point_on_line) {
      _selection_model.Clear();
      _phantom_rectangle_model.SetP2(model_point);
    }
    return;
  }
}

void DefaultController::onFieldKeyPress(const QKeyEvent *event) {
  auto &&_connection_map = _field_model.GetConnectionMap();
  auto &&_map_of_selected_nodes = _selection_model.GetSelectedConnections();
  if (event->key() == Qt::Key::Key_Delete) {
    auto selected_connections = _selection_model.GetSelectedConnections();
    auto selected_blocks = _selection_model.GetSelectedBlocks();
    if (_active_nodes_lock) {
      auto &&lock_nodes = _active_nodes_lock->GetLockedNodes();
      for (auto &&block : selected_blocks) {
        for (auto &&lock_node : lock_nodes) {
          if (lock_node.GetParentId() == block) {
            _active_nodes_lock.reset();
            break;
          }
        }
      }
    }
    _selection_model.Clear();
    _cm.Do(std::make_unique<RemoveCommand>(_field_model, selected_connections,
                                           selected_blocks));
  }
}
