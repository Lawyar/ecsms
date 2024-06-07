#pragma once

#include "../models/fieldmodel.h"
#include "../models/selectionmodel.h"

static float distance(QPoint p1, QPoint p2) {
  float d = sqrt(pow((p1.x() - p2.x()), 2) + pow((p1.y() - p2.y()), 2));
  return d;
}

static bool isPointOnLine(QLine line, QPoint point) {
  float eps = 10;
  bool res = abs(distance(line.p1(), point) + distance(line.p2(), point) -
                 distance(line.p1(), line.p2())) < eps;
  return res;
}

inline void checkLine(FieldModel &_field_model,
                      SelectionModel &_selection_model, QPoint model_point) {
  auto &&connection_map = _field_model.GetConnectionMap();
  auto &&map_of_selected_nodes = _selection_model.GetSelectedConnections();
  for (auto &&start_id : connection_map.keys()) {
    QPoint start_pos, end_pos;

    auto &&start_pd = _field_model.GetBlockData(start_id.GetParentId());
    if (!start_pd) {
      assert(false);
      return;
    }

    auto &&start_data = _field_model.GetNodeData(start_id);
    if (!start_data) {
      assert(false);
      return;
    }
    NodeType start_type = start_data->node_type;

    start_pos = start_pd->pos + start_pd->offset[start_type];

    for (auto &&end_id : connection_map[start_id]) {
      auto &&end_pd = _field_model.GetBlockData(end_id.GetParentId());
      if (!end_pd) {
        assert(false);
        return;
      }

      auto &&end_data = _field_model.GetNodeData(end_id);
      if (!end_data) {
        assert(false);
        return;
      }
      NodeType end_type = end_data->node_type;

      end_pos = end_pd->pos + end_pd->offset[end_type];
      bool find_line = isPointOnLine(QLine(start_pos, end_pos), model_point);
      if (find_line) {
        _selection_model.AddSelection(start_id, end_id);
        return;
      }
    }
  }
}

template <class Functor>
void forEachConnection(const FieldModel &_field_model, Functor &&func) {
  auto &&_connection_map = _field_model.GetConnectionMap();
  for (auto &&start_id : _connection_map.keys()) {
    for (auto &&end_id : _connection_map[start_id]) {
      auto &&start_data = _field_model.GetNodeData(start_id);
      if (!start_data) {
        assert(false);
        return;
      }

      auto &&end_data = _field_model.GetNodeData(end_id);
      if (!end_data) {
        assert(false);
        return;
      }

      NodeType start_type = start_data->node_type;
      NodeType end_type = end_data->node_type;

      QPoint model_start_pos, model_end_pos;
      if (auto &&start_pd = _field_model.GetBlockData(start_id.GetParentId())) {
        model_start_pos = start_pd->pos + start_pd->offset[start_type];
      }

      if (auto &&end_pd = _field_model.GetBlockData(end_id.GetParentId())) {
        model_end_pos = end_pd->pos + end_pd->offset[end_type];
      }

      func(model_start_pos, start_id, model_end_pos, end_id);
    }
  }
}

inline void selectAllInRect(const FieldModel &field_model,
                            SelectionModel &select_model,
                            const VisualizationModel &vis_model,
                            const PhantomRectangleModel &rect_model) {
  auto &&blocks = field_model.GetBlocks();
  for (auto &&pair_iter = blocks.begin(); pair_iter != blocks.end();
       ++pair_iter) {
    auto &&block_id = pair_iter.key();
    auto &&block_data = pair_iter.value();
    if (rect_model.ContainsRect(QRect(block_data.pos, block_data.size))) {
      select_model.AddSelection(block_id);
    } else {
      select_model.RemoveSelection(block_id);
    }
  }

  auto &&func = [&select_model, &vis_model,
                 &rect_model](QPoint model_start_pos, NodeId start_id,
                               QPoint model_end_pos, NodeId end_id) {
    QPoint vis_start_pos = vis_model.MapToVisualization(model_start_pos);
    QPoint vis_end_pos = vis_model.MapToVisualization(model_end_pos);
    if (rect_model.ContainsRect(QRect{vis_start_pos, vis_end_pos}))
      select_model.AddSelection(start_id, end_id);
  };

  forEachConnection(field_model, func);
}
