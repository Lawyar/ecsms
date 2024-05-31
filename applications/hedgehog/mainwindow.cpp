#include "mainwindow.h"
#include "blockfield.h"
#include "blockwidget.h"
#include "controlls/command/addattributecommand.h"
#include "controlls/command/addblockcommand.h"
#include "controlls/command/addchildtagcommand.h"
#include "controlls/command/addtagcommand.h"
#include "controlls/command/removeattributecommand.h"
#include "controlls/command/removetagcommand.h"
#include "qlineeditdelegate.h"
#include "ui_mainwindow.h"
#include "utility/mainwindowutility.h"
#include "utility/tagutility.h"

#include <QChart>
#include <QChartView>
#include <QDebug>
#include <QFileDialog>
#include <QLineSeries>
#include <QMessageBox>
#include <QStack>
#include <QTextCodec>
#include <QVariant>
#include <QXmlStreamReader>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  auto tree_model = new QStandardItemModel(0, 0, ui->treeView);
  ui->treeView->setModel(tree_model);
  ui->treeView->setItemDelegateForColumn(
      0, new QLineEditDelegate(ui->treeView, WhatValidate::XMLTag));

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
  auto block_list_element = new QStandardItem("block_name");
  block_list_element->setFlags(block_list_element->flags() ^
                               Qt::ItemIsEditable);
  parent_item->appendRow(block_list_element);

  auto w = ui->tab_1->width();
  ui->splitter_4->setSizes({w / 5, w - w / 5});

  ui->splitter_5->setStretchFactor(1, 1);
  ui->splitter_5->setStretchFactor(0, INT_MAX);

  _comm_managers =
      std::vector<std::shared_ptr<CommandManager>>(ui->tabWidget->count());
  for (auto &&cm : _comm_managers) {
    cm = std::make_shared<CommandManager>();
  }
  ui->scrollAreaWidgetContents->SetCommandManager(_comm_managers[1]);

  _processes = std::vector<QProcess *>(2);
  for (auto &&p : _processes) {
    p = new QProcess(this);
  }
  connectConsoleOutputWithWidget(_processes[0], ui->consoleOutput);
  connectConsoleOutputWithWidget(_processes[1], ui->consoleOutput_2);

  QObject::connect(ui->treeView->selectionModel(),
                   &QItemSelectionModel::selectionChanged, this,
                   &MainWindow::on_treeView_selectionModel_selectionChanged);
}

MainWindow::~MainWindow() { delete ui; }

bool MainWindow::event(QEvent *e) {
  if (e->type() == QEvent::KeyPress)
    return qobject_cast<QObject *>(ui->scrollAreaWidgetContents)->event(e);
  return QMainWindow::event(e);
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

void MainWindow::on_menuEdit_aboutToShow() {
  auto &&curr_ind = ui->tabWidget->currentIndex();
  ui->actionRedo->setEnabled(_comm_managers[curr_ind]->HasCommandsToRedo());
  ui->actionUndo->setEnabled(_comm_managers[curr_ind]->HasCommandsToUndo());
}

void MainWindow::on_actionNewFile_triggered_tab0() {
  delete ui->treeView->model();
  auto tree_model = new QStandardItemModel(0, 0, ui->treeView);
  ui->treeView->setModel(tree_model);
  ui->treeView->setItemDelegateForColumn(
      0, new QLineEditDelegate(ui->treeView, WhatValidate::XMLTag));

  delete ui->tableView->model();
  ui->tableView->setItemDelegateForColumn(
      0, new QLineEditDelegate(ui->tableView, WhatValidate::XMLAttribute));

  setDisableForButtonsInLayout(ui->horizontalLayout, true);
  setDisableForButtonsInLayout(ui->horizontalLayout_2, true);
  ui->pushButton_plus_tree->setDisabled(false);
  _comm_managers[0]->ClearCommands();
}

void MainWindow::on_actionOpen_triggered_tab0() {
  QString file_name = QFileDialog::getOpenFileName(this, tr("Open file"), "C:/",
                                                   tr("XML files (*.xml)"));
  if (file_name.size() == 0) {
    QMessageBox::warning(this, "Внимание", "Файл не был выбран");
    setDisableForButtonsInLayout(ui->horizontalLayout, true);
    setDisableForButtonsInLayout(ui->horizontalLayout_2, true);
    ui->pushButton_plus_tree->setDisabled(false);
    return;
  }
  QFile file(file_name);
  if (!file.open(QIODevice::ReadOnly)) {
    QMessageBox::critical(this, "Ошибка",
                          "Не удалось открыть файл " + file_name);
    setDisableForButtonsInLayout(ui->horizontalLayout, true);
    setDisableForButtonsInLayout(ui->horizontalLayout_2, true);
    ui->pushButton_plus_tree->setDisabled(false);
    return;
  }

  _comm_managers[0]->ClearCommands();
  delete ui->treeView->model();
  delete ui->tableView->model();
  QStandardItemModel *model = new QStandardItemModel(0, 0, ui->treeView);
  ui->treeView->setModel(model);
  QStandardItem *parent_item = model->invisibleRootItem();

  QXmlStreamReader xml_reader(&file);
  QStack<QStandardItem *> tags;
  tags.push(parent_item);
  while (!xml_reader.atEnd()) {
    xml_reader.readNext();
    switch (xml_reader.tokenType()) {
    case QXmlStreamReader::StartElement: { // открывающий тэг
      auto table_model = new QStandardItemModel();
      for (auto &attr : xml_reader.attributes()) {
        auto attr_name_item = new QStandardItem(attr.name().toString());
        auto attr_value_item = new QStandardItem(attr.value().toString());
        table_model->appendRow(
            QList<QStandardItem *>({attr_name_item, attr_value_item}));
      }
      table_model->setHorizontalHeaderLabels(
          QStringList({"Attribute", "Value"}));
      auto &&new_tag = createTag(table_model, xml_reader.name().toString());
      auto &&parent_tag = tags.top();
      parent_tag->appendRow(new_tag);
      tags.push(new_tag);
      break;
    }
    case QXmlStreamReader::Characters: { // текст внутри тэга
      QString str = xml_reader.text().toString().trimmed();
      if (!str.isEmpty()) {
        auto tag = tags.top();
        tag->setText(tag->text() + ": " + str);
      }
      break;
    }
    case QXmlStreamReader::EndElement: {
      tags.pop();
      break;
    }
    case QXmlStreamReader::StartDocument:
    case QXmlStreamReader::EndDocument:
      break;
    default: {
      qDebug() << "error: unexpected token type " << xml_reader.tokenType();
      break;
    }
    }
  }
  ui->treeView->resizeColumnToContents(0);
  file.close();
  setDisableForButtonsInLayout(ui->horizontalLayout, true);
  setDisableForButtonsInLayout(ui->horizontalLayout_2, true);
}

void MainWindow::on_actionSave_triggered_tab0() {
  QString file_name = QFileDialog::getSaveFileName(
      this, tr("Save As"), "C:/*.xml", tr("XML files (*.xml)"));
  if (file_name.size() == 0) {
    QMessageBox::warning(this, "Внимание", "Файл не был выбран");
    return;
  }
  QFile file(file_name);
  if (!file.open(QIODevice::WriteOnly)) {
    QMessageBox::critical(this, "Ошибка",
                          "Не удалось сохранить в файл " + file_name);
    return;
  }

  QXmlStreamWriter xml_writer(&file);
  xml_writer.setAutoFormatting(true); // Устанавливаем автоформатирование текста

  auto &&model = dynamic_cast<QStandardItemModel *>(ui->treeView->model());
  xml_writer.writeStartDocument();
  writeTagsFromModel(xml_writer, model);
  xml_writer.writeEndDocument();
  file.close();
}

void MainWindow::on_actionNewFile_triggered_tab1() {
  _comm_managers[1]->ClearCommands();
  ui->scrollAreaWidgetContents->Clear();
}

void MainWindow::on_actionOpen_triggered_tab1() {
  QString file_name = QFileDialog::getOpenFileName(this, tr("Open file"), "C:/",
                                                   tr("YAML files (*.yaml)"));
  if (file_name.size() == 0) {
    QMessageBox::warning(this, "Внимание", "Файл не был выбран");
    setDisableForButtonsInLayout(ui->horizontalLayout, true);
    setDisableForButtonsInLayout(ui->horizontalLayout_2, true);
    ui->pushButton_plus_tree->setDisabled(false);
    return;
  }
  QFile file(file_name);
  if (!file.open(QIODevice::ReadOnly)) {
    QMessageBox::critical(this, "Ошибка",
                          "Не удалось открыть файл " + file_name);
    setDisableForButtonsInLayout(ui->horizontalLayout, true);
    setDisableForButtonsInLayout(ui->horizontalLayout_2, true);
    ui->pushButton_plus_tree->setDisabled(false);
    return;
  }

  _comm_managers[1]->ClearCommands();
  ui->scrollAreaWidgetContents->Clear();
}

void MainWindow::on_actionSave_triggered_tab1() {
  QString file_name = QFileDialog::getSaveFileName(
      this, tr("Save As"), "C:/*.yaml", tr("YAML files (*.yaml)"));
  if (file_name.size() == 0) {
    QMessageBox::warning(this, "Внимание", "Файл не был выбран");
    return;
  }
  QFile file(file_name);
  if (!file.open(QIODevice::WriteOnly)) {
    QMessageBox::critical(this, "Ошибка",
                          "Не удалось сохранить в файл " + file_name);
    return;
  }
}

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

void MainWindow::on_actionRedo_triggered() {
  auto &&curr_ind = ui->tabWidget->currentIndex();
  _comm_managers[curr_ind]->Redo();
}

void MainWindow::on_actionUndo_triggered() {
  auto &&curr_ind = ui->tabWidget->currentIndex();
  _comm_managers[curr_ind]->Undo();
}

void MainWindow::on_treeView_clicked(const QModelIndex &index) {
  ui->tableView->setModel(
      index.data(Qt::UserRole + 1).value<QStandardItemModel *>());
  ui->tableView->setItemDelegateForColumn(
      0, new QLineEditDelegate(ui->treeView, WhatValidate::XMLAttribute));
  setDisableForButtonsInLayout(ui->horizontalLayout, false);
  ui->pushButton_plus_tree->setEnabled(index.parent() != QModelIndex());
}

void MainWindow::on_tableView_clicked(const QModelIndex &index) {
  ui->pushButton_minus_table->setDisabled(ui->tableView->model()->rowCount() ==
                                          0);
}

void MainWindow::on_treeView_selectionModel_selectionChanged(
    const QItemSelection &selection_now,
    const QItemSelection &selection_before) {
  auto &&tree_view_model =
      qobject_cast<QStandardItemModel *>(ui->treeView->model());
  if (!tree_view_model) {
    assert(false);
    return;
  }

  bool contains_root = false;
  if (selection_now.empty()) {
    setDisableForButtonsInLayout(ui->horizontalLayout, true);
    setDisableForButtonsInLayout(ui->horizontalLayout_2, true);
    qDebug() << tree_view_model->rowCount();
    ui->pushButton_plus_tree->setEnabled(tree_view_model->rowCount() == 0);
    return;
  }

  for (auto &&item : selection_now.indexes()) {
    ui->tableView->setModel(
        item.data(Qt::UserRole + 1).value<QStandardItemModel *>());
    ui->tableView->setItemDelegateForColumn(
        0, new QLineEditDelegate(ui->treeView, WhatValidate::XMLAttribute));
    if (item.parent() == tree_view_model->invisibleRootItem()->index()) {
      contains_root = true;
      break;
    }
  }

  ui->pushButton_plus_tree->setDisabled(contains_root);
  ui->pushButton_minus_tree->setDisabled(false);
  ui->pushButton_new_child_row_tree->setDisabled(false);
  ui->pushButton_plus_table->setDisabled(false);
  ui->pushButton_minus_table->setDisabled(
      ui->tableView->model() && ui->tableView->model()->rowCount() == 0);
}

void MainWindow::on_pushButton_plus_tree_clicked() {
  auto &&tree_view_model =
      qobject_cast<QStandardItemModel *>(ui->treeView->model());
  if (!tree_view_model) {
    assert(false);
    return;
  }

  auto &&selection_model = ui->treeView->selectionModel();

  QModelIndex index_before_insert;
  if (tree_view_model->rowCount() == 0) { // if there is no elements, add first
    index_before_insert = tree_view_model->invisibleRootItem()->index();
  } else { // else add to row after selection
    QModelIndexList indexes = selection_model->selectedIndexes();
    if (indexes.empty() || indexes.size() > 1)
      return;
    index_before_insert = indexes.at(0);
  }

  _comm_managers[0]->Do(std::make_unique<AddTagCommand>(index_before_insert,
                                                        tree_view_model, ""));

  QModelIndex index_to_select;
  if (index_before_insert == QModelIndex())
    index_to_select = tree_view_model->index(0, 0);
  else
    index_to_select =
        index_before_insert.siblingAtRow(index_before_insert.row() + 1);

  // когда добавляется новый элемент, селектируем его
  selection_model->setCurrentIndex(index_to_select,
                                   QItemSelectionModel::ClearAndSelect);
}

void MainWindow::on_pushButton_minus_tree_clicked() {
  auto &&tree_view_model =
      qobject_cast<QStandardItemModel *>(ui->treeView->model());
  if (!tree_view_model) {
    assert(false);
    return;
  }

  auto &&selection_model = ui->treeView->selectionModel();
  QModelIndexList rows = selection_model->selectedRows();
  if (rows.empty())
    return;

  auto &&selected_index = rows.at(0);
  _comm_managers[0]->Do(
      std::make_unique<RemoveTagCommand>(selected_index, tree_view_model));

  if (selected_index.parent() !=
      QModelIndex()) { // если удалили не первый эелемент
    ui->treeView->expand(selected_index.parent());
    // когда удаляем элемент, селектируем его родителя
    selection_model->setCurrentIndex(selected_index.parent(),
                                     QItemSelectionModel::ClearAndSelect);
  } else {
    setDisableForButtonsInLayout(ui->horizontalLayout, true);
    ui->pushButton_plus_tree->setEnabled(true);
    delete ui->tableView->model();
  }
}

void MainWindow::on_pushButton_new_child_row_tree_clicked() {
  auto &&tree_view_model =
      qobject_cast<QStandardItemModel *>(ui->treeView->model());
  if (!tree_view_model) {
    assert(false);
    return;
  }

  auto &&selection_model = ui->treeView->selectionModel();
  QModelIndexList indexes = selection_model->selectedIndexes();
  if (indexes.empty() || indexes.size() > 1)
    return;
  QModelIndex parent_tag_index = indexes.at(0);
  if (parent_tag_index == tree_view_model->invisibleRootItem()->index())
    return;

  _comm_managers[0]->Do(std::make_unique<AddChildTagCommand>(
      parent_tag_index, tree_view_model, ""));
  ui->treeView->expand(parent_tag_index);

  // когда добавляется новый элемент, селектируем его
  selection_model->setCurrentIndex(parent_tag_index.child(0, 0),
                                   QItemSelectionModel::ClearAndSelect);
}

void MainWindow::on_pushButton_plus_table_clicked() {
  auto &&tree_selection_model = ui->treeView->selectionModel();
  auto &&table_model =
      qobject_cast<QStandardItemModel *>(ui->tableView->model());
  auto &&table_selection_model = ui->tableView->selectionModel();

  QModelIndexList tree_indexes = tree_selection_model->selectedIndexes();
  if (tree_indexes.empty()) {
    return;
  }

  auto &&row_to_insert = table_model->rowCount();
  _comm_managers[0]->Do(
      std::make_unique<AddAttributeCommand>(row_to_insert, table_model));

  // когда добавляется новый элемент, селектируем его
  table_selection_model->select(table_model->index(0, 0),
                                QItemSelectionModel::Select |
                                    QItemSelectionModel::Rows);

  ui->pushButton_minus_table->setEnabled(true);
}

void MainWindow::on_pushButton_minus_table_clicked() {
  auto &&table_model =
      qobject_cast<QStandardItemModel *>(ui->tableView->model());
  auto &&table_selection_model = ui->tableView->selectionModel();

  QModelIndexList table_rows = table_selection_model->selectedRows();
  if (table_rows.empty())
    return;

  auto &&index_to_remove = table_rows.at(0);

  _comm_managers[0]->Do(
      std::make_unique<RemoveAttributeCommand>(index_to_remove, table_model));

  ui->pushButton_minus_table->setDisabled(ui->tableView->model()->rowCount() ==
                                          0);
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
