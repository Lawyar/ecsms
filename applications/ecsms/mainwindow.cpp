#include "mainwindow.h"
#include "controlls/command/addattributecommand.h"
#include "controlls/command/addblockcommand.h"
#include "controlls/command/addchildtagcommand.h"
#include "controlls/command/addtagcommand.h"
#include "controlls/command/removeattributecommand.h"
#include "controlls/command/removetagcommand.h"
#include "controlls/command/tagtextchangedcommand.h"
#include "delegate/qlineeditdelegate.h"
#include "ui_mainwindow.h"
#include "utility/mainwindowutility.h"
#include "utility/tagutility.h"
#include "widgets/blockfieldwidget.h"
#include "widgets/blockwidget.h"

#include <QChart>
#include <QChartView>
#include <QDebug>
#include <QLineSeries>
#include <QMessageBox>
#include <QStack>
#include <QTextCodec>
#include <QVariant>
#include <QXmlStreamReader>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  _com_mgrs =
      std::vector<std::shared_ptr<CommandManager>>(ui->tabWidget->count());
  for (auto &&cm : _com_mgrs) {
    cm = std::make_shared<CommandManager>();
  }

  _com_mgrs_states =
      std::vector<std::unique_ptr<CommandManager::State>>(_com_mgrs.size());
  for (auto i = 0; i < _com_mgrs_states.size(); ++i) {
    _com_mgrs_states[i].reset(
        new CommandManager::State(_com_mgrs[i]->GetState()));
  }

  _file_names = std::vector<QString>(ui->tabWidget->count());

  auto tree_model = new QStandardItemModel(0, 0, ui->treeView);
  ui->treeView->setModel(tree_model);
  ui->treeView->setItemDelegateForColumn(
      0, new QLineEditDelegate(ui->treeView, WhatValidate::XMLTag, _com_mgrs[0],
                               ui->treeView));

  for (auto i = 0; i < 4; ++i) {
    auto series = new QtCharts::QLineSeries;
    series->append(0, 6);
    series->append(2, 4);
    series->append(3, 8);
    series->append(7, 4);
    series->append(10, 5);
    *series << QPointF(11, 1) << QPointF(13, 3) << QPointF(17, 6)
            << QPointF(18, 3) << QPointF(20, 2);

    auto chart = new QtCharts::QChart;
    chart->legend()->hide();
    chart->addSeries(series);
    chart->createDefaultAxes();
    chart->setTitle("Simple Line Chart");
    ui->gridLayout_2->addWidget(new QtCharts::QChartView(chart), i / 2, i % 2);
  }

  ui->splitter->setStretchFactor(0, 1);
  ui->splitter->setStretchFactor(1, INT_MAX);

  ui->splitter_2->setStretchFactor(0, 1);
  ui->splitter_2->setStretchFactor(1, INT_MAX);

  ui->splitter_3->setStretchFactor(0, INT_MAX);
  ui->splitter_3->setStretchFactor(1, 1);

  auto list_model = new QStandardItemModel(0, 0, ui->listView);
  ui->listView->setModel(list_model);
  QStandardItem *parent_item = list_model->invisibleRootItem();
  auto block_list_item = new QStandardItem("default_block");
  block_list_item->setFlags(block_list_item->flags() ^ Qt::ItemIsEditable);
  parent_item->appendRow(block_list_item);

  auto w = ui->tab_1->width();
  ui->splitter_4->setSizes({w / 5, w - w / 5});

  ui->splitter_5->setStretchFactor(1, 1);
  ui->splitter_5->setStretchFactor(0, INT_MAX);

  ui->scrollAreaWidgetContents->SetCommandManager(_com_mgrs[1]);

  _processes = std::vector<QProcess *>(2);
  for (auto &&p : _processes) {
    p = new QProcess(this);
  }
  connectProcessOutputWithWidget(_processes[0], ui->consoleOutput);
  connectProcessOutputWithWidget(_processes[1], ui->consoleOutput_2);

  QObject::connect(ui->treeView->selectionModel(),
                   &QItemSelectionModel::selectionChanged, this,
                   &MainWindow::on_treeView_selectionModel_selectionChanged);

  ui->tableView->setItemDelegateForColumn(
      0, new QLineEditDelegate(ui->treeView, WhatValidate::XMLAttribute,
                               _com_mgrs[0], ui->treeView));
  ui->tableView->setItemDelegateForColumn(
      1, new QLineEditDelegate(ui->treeView, WhatValidate::Nothing,
                               _com_mgrs[0], ui->treeView));
}

MainWindow::~MainWindow() {
  for (auto &&process : _processes)
    disconectProcessFromAll(process);
  delete ui;
}

void MainWindow::on_consoleInput_returnPressed() {
  auto str = (ui->consoleInput->text() + "\n");
  QTextCodec *codec = QTextCodec::codecForName("IBM 866");
  auto data = codec->fromUnicode(str.data(), str.size());

  _processes[0]->write(data);
  ui->consoleInput->clear();
}

void MainWindow::on_consoleInput_2_returnPressed() {
  auto str = (ui->consoleInput_2->text() + "\n");
  QTextCodec *codec = QTextCodec::codecForName("IBM 866");
  auto data = codec->fromUnicode(str.data(), str.size());

  _processes[1]->write(data);
  ui->consoleInput_2->clear();
}

void MainWindow::on_menuFile_aboutToShow() {
  auto &&curr_ind = ui->tabWidget->currentIndex();
  ui->actionSave->setDisabled(*_com_mgrs_states[curr_ind] ==
                              _com_mgrs[curr_ind]->GetState());
}

void MainWindow::on_menuEdit_aboutToShow() {
  auto &&curr_ind = ui->tabWidget->currentIndex();
  ui->actionRedo->setEnabled(_com_mgrs[curr_ind]->HasCommandsToRedo());
  ui->actionUndo->setEnabled(_com_mgrs[curr_ind]->HasCommandsToUndo());
}

void MainWindow::on_menuView_aboutToShow() {
  auto &&curr_ind = ui->tabWidget->currentIndex();
  ui->actionGoToFirstBlock->setEnabled(curr_ind == 1);
  ui->actionGoToNextBlock->setEnabled(curr_ind == 1);
}

void MainWindow::updateAllButtons() {
  auto isSmthSelectedInTree = [this]() {
    return ui->treeView->selectionModel()->currentIndex() != QModelIndex();
  };
  auto isSmthSelectedInTable = [this]() {
    return ui->tableView->selectionModel()->currentIndex() != QModelIndex();
  };
  auto isTreeEmpty = [this]() {
    return ui->treeView->model()->rowCount() == 0;
  };
  auto isTableEmpty = [this]() {
    return ui->tableView->model()->rowCount() == 0;
  };
  auto isTreeRootSelected = [this]() {
    return ui->treeView->selectionModel()->currentIndex().parent() ==
           QModelIndex();
  };

  // update plus tree button
  bool is_enabled =
      isTreeEmpty() || !isTreeRootSelected() && isSmthSelectedInTree();
  ui->pushButton_plus_tree->setEnabled(is_enabled);

  // update minus tree button
  is_enabled = isSmthSelectedInTree() && !isTreeEmpty();
  ui->pushButton_minus_tree->setEnabled(is_enabled);

  // update new child tree button
  is_enabled = isSmthSelectedInTree() && !isTreeEmpty();
  ui->pushButton_new_child_row_tree->setEnabled(is_enabled);

  // update plus table button
  is_enabled = isSmthSelectedInTree() && !isTreeEmpty();
  ui->pushButton_plus_table->setEnabled(is_enabled);

  // update minus table button
  is_enabled = isSmthSelectedInTree() && !isTreeEmpty() &&
               isSmthSelectedInTable() && !isTableEmpty();
  ui->pushButton_minus_table->setEnabled(is_enabled);
}

void MainWindow::updateMainPage() {
  auto isSmthSelectedInTree = [this]() {
    return ui->treeView->selectionModel()->currentIndex() != QModelIndex();
  };
  auto isTreeEmpty = [this]() {
    return ui->treeView->model()->rowCount() == 0;
  };

  // update table view
  QStandardItemModel *table_model = nullptr;
  if (isSmthSelectedInTree() && !isTreeEmpty()) {
    auto &&index = ui->treeView->selectionModel()->currentIndex();
    if (index != QModelIndex()) {
      auto data = index.data(Qt::UserRole + 1);
      table_model = data.value<QStandardItemModel *>();
    }
  }
  ui->tableView->setModel(table_model);

  updateAllButtons();
}

void MainWindow::on_actionNewFile_triggered_tab0() {
  _file_names[0].clear();

  auto &&vec = windowTitle().split(": ");
  if (vec.size() > 1)
    setWindowTitle(vec[1]);

  _com_mgrs[0]->ClearCommands();
  if (ui->treeView->model())
    delete ui->treeView->model();
  auto tree_model = new QStandardItemModel(0, 0, ui->treeView);
  ui->treeView->setModel(tree_model);

  updateMainPage();
}

void MainWindow::on_actionOpen_triggered_tab0() {
  auto &&file_name = _file_names[0];
  file_name = getOpenFileName(this, "C:/", "XML files (*.xml)");
  if (file_name.isEmpty()) {
    updateMainPage();
    return;
  }

  _com_mgrs[0]->ClearCommands();
  _com_mgrs_states[0].reset(
      new CommandManager::State(_com_mgrs[0]->GetState()));
  if (ui->tableView->model())
    delete ui->tableView->model();
  if (ui->treeView->model())
    delete ui->treeView->model();

  bool success = getXMLFromFile(this, file_name, ui->treeView);
  if (!success) {
    _file_names[0].clear();
    setWindowTitle(_app_name);
    return;
  } else {
    setWindowTitle(file_name + ": " + _app_name);
  }
  updateMainPage();
}

void MainWindow::on_actionSave_triggered_tab0() {
  auto &&file_name = _file_names[0];
  if (file_name.isEmpty()) {
    file_name = getSaveFileName(this, "C:/*.xml", "XML files (*.xml)");
    if (file_name.isEmpty()) {
      updateMainPage();
      return;
    }
  }

  auto model = dynamic_cast<QStandardItemModel *>(ui->treeView->model());
  bool success = saveXMLToFile(this, file_name, model);
  if (!success) {
    _file_names[0].clear();
    setWindowTitle(_app_name);
    return;
  } else {
    setWindowTitle(file_name + ": " + _app_name);
  }
  updateMainPage();
  _com_mgrs_states[0].reset(
      new CommandManager::State(_com_mgrs[0]->GetState()));
}

void MainWindow::on_actionSaveAs_triggered_tab0() {
  auto &&file_name = _file_names[0];
  file_name = getSaveFileName(this, "C:/*.xml", "XML files (*.xml)");
  if (file_name.isEmpty()) {
    updateMainPage();
    return;
  }

  auto model = dynamic_cast<QStandardItemModel *>(ui->treeView->model());
  bool success = saveXMLToFile(this, file_name, model);
  if (!success) {
    _file_names[0].clear();
    setWindowTitle(_app_name);
    return;
  } else {
    setWindowTitle(file_name + ": " + _app_name);
  }
  updateMainPage();
  _com_mgrs_states[0].reset(
      new CommandManager::State(_com_mgrs[0]->GetState()));
}

void MainWindow::on_actionNewFile_triggered_tab1() {
  _file_names[1].clear();

  auto &&vec = windowTitle().split(": ");
  if (vec.size() > 1)
    setWindowTitle(vec[1]);

  _com_mgrs[1]->ClearCommands();
  _com_mgrs_states[1].reset(
      new CommandManager::State(_com_mgrs[1]->GetState()));
  ui->scrollAreaWidgetContents->Clear();
}

void MainWindow::on_actionOpen_triggered_tab1() {
  auto &&file_name = _file_names[1];
  file_name = getOpenFileName(this, "C:/", "YAML files (*.yaml)");
  if (file_name.isEmpty()) {
    updateMainPage();
    return;
  }

  ui->scrollAreaWidgetContents->Clear();
  bool success = getYAMLFromFile(this, file_name, ui->scrollAreaWidgetContents);
  if (!success) {
    _file_names[0].clear();
    setWindowTitle(_app_name);
    return;
  } else {
    setWindowTitle(file_name + ": " + _app_name);
  }
  _com_mgrs[1]->ClearCommands();
  _com_mgrs_states[1].reset(
      new CommandManager::State(_com_mgrs[1]->GetState()));
}

void MainWindow::on_actionSave_triggered_tab1() {
  auto &&file_name = _file_names[0];
  file_name = getSaveFileName(this, "C:/*.yaml", "YAML files (*.yaml)");
  if (file_name.isEmpty()) {
    updateMainPage();
    return;
  }

  bool success = saveYAMLToFile(this, file_name);
  if (!success) {
    _file_names[0].clear();
    setWindowTitle(_app_name);
    return;
  } else {
    setWindowTitle(file_name + ": " + _app_name);
  }
  _com_mgrs_states[1].reset(
      new CommandManager::State(_com_mgrs[1]->GetState()));
}

void MainWindow::on_actionSaveAs_triggered_tab1() {
  auto &&file_name = _file_names[0];
  file_name = getSaveFileName(this, "C:/*.yaml", "YAML files (*.yaml)");
  if (file_name.isEmpty()) {
    updateMainPage();
    return;
  }

  bool success = saveYAMLToFile(this, file_name);
  if (!success) {
    _file_names[0].clear();
    setWindowTitle(_app_name);
    return;
  } else {
    setWindowTitle(file_name + ": " + _app_name);
  }
  _com_mgrs_states[1].reset(
      new CommandManager::State(_com_mgrs[1]->GetState()));
}

bool MainWindow::event(QEvent *e) {
  if (e->type() == QEvent::KeyPress)
    return qobject_cast<QObject *>(ui->scrollAreaWidgetContents)->event(e);
  return QMainWindow::event(e);
}

void MainWindow::closeEvent(QCloseEvent *event) { event->ignore(); }

void MainWindow::on_actionNewFile_triggered() {
  auto &&curr_ind = ui->tabWidget->currentIndex();
  switch (curr_ind) {
  case 0: {
    on_actionNewFile_triggered_tab0();
    break;
  }
  case 1: {
    on_actionNewFile_triggered_tab1();
    break;
  }
  default:
    assert(false);
    break;
  }
}

void MainWindow::on_actionOpen_triggered() {
  auto &&curr_ind = ui->tabWidget->currentIndex();
  switch (curr_ind) {
  case 0: {
    on_actionOpen_triggered_tab0();
    break;
  }
  case 1: {
    on_actionOpen_triggered_tab1();
    break;
  }
  default:
    assert(false);
    break;
  }
}

void MainWindow::on_actionSave_triggered() {
  auto &&curr_ind = ui->tabWidget->currentIndex();
  switch (curr_ind) {
  case 0: {
    on_actionSave_triggered_tab0();
    break;
  }
  case 1: {
    on_actionSave_triggered_tab1();
    break;
  }
  default:
    assert(false);
    break;
  }
}

void MainWindow::on_actionSaveAs_triggered() {
  auto &&curr_ind = ui->tabWidget->currentIndex();
  switch (curr_ind) {
  case 0: {
    on_actionSaveAs_triggered_tab0();
    break;
  }
  case 1: {
    on_actionSaveAs_triggered_tab1();
    break;
  }
  default:
    assert(false);
    break;
  }
}

void MainWindow::on_actionRedo_triggered() {
  auto &&curr_ind = ui->tabWidget->currentIndex();
  _com_mgrs[curr_ind]->Redo();
  updateMainPage();
}

void MainWindow::on_actionUndo_triggered() {
  auto &&curr_ind = ui->tabWidget->currentIndex();
  _com_mgrs[curr_ind]->Undo();
  updateMainPage();
}

void MainWindow::on_actionGoToFirstBlock_triggered() {
  ui->scrollAreaWidgetContents->GoToFirstBlock();
}

void MainWindow::on_actionGoToNextBlock_triggered() {
  ui->scrollAreaWidgetContents->GoToNextBlock();
}

void MainWindow::on_treeView_clicked(const QModelIndex &index) {
  updateMainPage();
}

void MainWindow::on_tableView_clicked(const QModelIndex &index) {
  updateMainPage();
}

void MainWindow::on_treeView_selectionModel_selectionChanged(
    const QItemSelection &selection_now,
    const QItemSelection &selection_before) {
  updateMainPage();
}

void MainWindow::on_pushButton_plus_tree_clicked() {
  auto &&tree_view_model =
      qobject_cast<QStandardItemModel *>(ui->treeView->model());

  auto &&selection_model = ui->treeView->selectionModel();

  QModelIndex index_before_insert;
  if (tree_view_model->rowCount() == 0) { // if there is no items, add first
    index_before_insert = QModelIndex();
  } else { // else add to row after selection
    index_before_insert = selection_model->currentIndex();
  }

  _com_mgrs[0]->Do(
      std::make_unique<AddTagCommand>(index_before_insert, tree_view_model));

  // select new item
  QModelIndex index_to_select;
  if (index_before_insert == QModelIndex())
    index_to_select = tree_view_model->index(0, 0);
  else
    index_to_select =
        index_before_insert.siblingAtRow(index_before_insert.row() + 1);

  selection_model->setCurrentIndex(index_to_select,
                                   QItemSelectionModel::ClearAndSelect);
  updateMainPage();
}

void MainWindow::on_pushButton_minus_tree_clicked() {
  auto tree_view_model =
      qobject_cast<QStandardItemModel *>(ui->treeView->model());

  auto selection_model = ui->treeView->selectionModel();
  auto index_to_remove = selection_model->currentIndex();
  if (index_to_remove ==
      QModelIndex()) // prevent error from deleting invalid item
    return;

  _com_mgrs[0]->Do(
      std::make_unique<RemoveTagCommand>(index_to_remove, tree_view_model));

  // if its' not a root item when expand and select it's parent
  if (index_to_remove.parent() != QModelIndex()) {
    ui->treeView->expand(index_to_remove.parent());
    selection_model->setCurrentIndex(index_to_remove.parent(),
                                     QItemSelectionModel::ClearAndSelect);
  }
  updateMainPage();
}

void MainWindow::on_pushButton_new_child_row_tree_clicked() {
  auto tree_view_model =
      qobject_cast<QStandardItemModel *>(ui->treeView->model());

  auto selection_model = ui->treeView->selectionModel();
  auto parent_index = selection_model->currentIndex();
  if (parent_index == QModelIndex())
    return;

  if (parent_index ==
      QModelIndex()) // prevent error from adding child to invalid item
    return;

  // remove tag's text
  auto parent_tag = tree_view_model->item(parent_index.row());
  auto tag_text = parent_tag->text();
  auto &&tag_text_vec = tag_text.split(": ");
  if (tag_text_vec.size() > 1) {
    _com_mgrs[0]->Do(std::make_unique<TagTextChangedCommand>(
        parent_index, tag_text, tag_text_vec[0], tree_view_model));
  }

  _com_mgrs[0]->Do(
      std::make_unique<AddChildTagCommand>(parent_index, tree_view_model));

  ui->treeView->expand(parent_index);                        // expand parent
  selection_model->setCurrentIndex(parent_index.child(0, 0), // select new item
                                   QItemSelectionModel::ClearAndSelect);
  updateMainPage();
}

void MainWindow::on_pushButton_plus_table_clicked() {
  auto tree_selection_model = ui->treeView->selectionModel();
  auto tag_index = tree_selection_model->currentIndex();
  if (tag_index == QModelIndex())
    return;

  auto row_to_insert = ui->tableView->model()
                           ->rowCount(); // insert to the end of attributes list

  auto tree_view_model =
      qobject_cast<QStandardItemModel *>(ui->treeView->model());
  _com_mgrs[0]->Do(std::make_unique<AddAttributeCommand>(
      row_to_insert, tag_index, tree_view_model));

  // select new item
  auto table_selection_model = ui->tableView->selectionModel();
  table_selection_model->select(ui->tableView->model()->index(0, 0),
                                QItemSelectionModel::Select |
                                    QItemSelectionModel::Rows);

  updateMainPage();
}

void MainWindow::on_pushButton_minus_table_clicked() {
  auto &&tree_view_model =
      qobject_cast<QStandardItemModel *>(ui->treeView->model());

  auto tree_selection_model = ui->treeView->selectionModel();
  auto tag_index = tree_selection_model->currentIndex();
  if (tag_index == QModelIndex())
    return;

  auto table_selection_model = ui->tableView->selectionModel();
  auto index_to_remove = table_selection_model->currentIndex();

  _com_mgrs[0]->Do(std::make_unique<RemoveAttributeCommand>(
      index_to_remove.row(), tag_index, tree_view_model));

  updateMainPage();
}

void MainWindow::on_listView_doubleClicked(const QModelIndex &index) {
  ui->scrollAreaWidgetContents->AddBlock();
}

void MainWindow::on_pushButton_pausePipeline_pressed() {
  auto text = ui->pushButton_pausePipeline->text();
  if (text == "||")
    ui->pushButton_pausePipeline->setText("▶");
  else {
    ui->pushButton_pausePipeline->setText("||");
    ui->pushButton_stopPipeline->setEnabled(true);
  }
}

void MainWindow::on_pushButton_stopPipeline_pressed() {
  ui->pushButton_stopPipeline->setEnabled(false);
  ui->pushButton_pausePipeline->setText("▶");
}
