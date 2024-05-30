#include "mainwindow.h"
#include "blockfield.h"
#include "blockwidget.h"
#include "controlls/command/addblockcommand.h"
#include "controlls/command/addchildtagcommand.h"
#include "controlls/command/addtagcommand.h"
#include "controlls/command/removetagcommand.h"
#include "qlineeditdelegate.h"
#include "ui_mainwindow.h"

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

static void connectConsoleOutputWithWidget(QProcess *myProcess,
                                           QTextEdit *output) {
  myProcess->start("cmd.exe", {"/U"});

  QObject::connect(myProcess, &QProcess::readyRead, [myProcess, output]() {
    QByteArray out = myProcess->readAllStandardOutput();
    QByteArray error = myProcess->readAllStandardError();
    for (auto &&arr : {error, out}) {
      QString output_str(QString::fromUtf16(
          reinterpret_cast<const char16_t *>(arr.data()), arr.size() / 2));
      output->setText(output->toPlainText().append(output_str));
      QTextCursor cursor = output->textCursor();
      cursor.movePosition(QTextCursor::End);
      output->setTextCursor(cursor);
    }
  });
}

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
  ui->pushButton_plus_tree->setEnabled(true);
  ui->pushButton_minus_tree->setDisabled(true);
  ui->pushButton_new_child_row_tree->setDisabled(true);
  ui->pushButton_plus_table->setDisabled(true);
  ui->pushButton_minus_table->setDisabled(true);
}

void MainWindow::on_actionOpen_triggered_tab0() {
  QString file_name = QFileDialog::getOpenFileName(this, tr("Open file"), "C:/",
                                                   tr("XML files (*.xml)"));
  if (file_name.size() == 0) {
    QMessageBox::warning(this, "Внимание", "Файл не был выбран");
    return;
  }
  QFile file(file_name);
  if (!file.open(QIODevice::ReadOnly)) {
    QMessageBox::critical(this, "Ошибка",
                          "Не удалось открыть файл " + file_name);
    return;
  }

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
      auto &&new_tag = nullptr;
      // createTag(tags.top(), table_model, xml_reader.name().toString());
      //_comm_managers[0]->Do(new AddTagCommand(
      //     tags.top(), ui->treeView, xml_reader.name().toString(),
      //     table_model));
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
}

static void writeAttributes(QXmlStreamWriter &xml_writer,
                            const QStandardItemModel *model) {
  for (int r = 0; r < model->rowCount(); ++r) {
    auto &&name = model->item(r, 0)->text();
    auto &&value = model->item(r, 1)->text();
    xml_writer.writeAttribute(name, value);
  }
}

static void writeTree(QXmlStreamWriter &xml_writer, const QStandardItem *root) {
  auto &&tag_data = root->text().split(": ");
  xml_writer.writeStartElement(tag_data.front());
  auto &&model = root->data(Qt::UserRole + 1).value<QStandardItemModel *>();
  writeAttributes(xml_writer, model);
  if (tag_data.size() > 1) {
    tag_data.pop_front();
    xml_writer.writeCharacters(tag_data.join(": "));
  }
  for (int r = 0; root->hasChildren() && r < root->rowCount(); ++r) {
    auto &&item = root->child(r);
    writeTree(xml_writer, item);
  }
  xml_writer.writeEndElement();
}

void MainWindow::on_actionSave_triggered_tab0() {
  QString file_name = QFileDialog::getSaveFileName(this, tr("Save As"), "C:/",
                                                   tr("XML files (*.xml)"));
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
  auto &&first_item = model->item(0, 0);
  xml_writer.writeStartDocument();
  writeTree(xml_writer, first_item);
  xml_writer.writeEndDocument();
  file.close();
}

void MainWindow::on_actionNewFile_triggered_tab1() {}

void MainWindow::on_actionOpen_triggered_tab1() {}

void MainWindow::on_actionSave_triggered_tab1() {}

void MainWindow::on_actionNewFile_triggered() {
  auto &&curr_ind = ui->tabWidget->currentIndex();
  switch (curr_ind) {
  case 0: {
    on_actionNewFile_triggered_tab0();
  }
  case 1: {
    on_actionNewFile_triggered_tab1();
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
  }
  case 1: {
    on_actionOpen_triggered_tab1();
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
  }
  case 1: {
    on_actionSave_triggered_tab1();
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

static void setDisableForLayoutElements(QLayout *layout, bool is_disabled) {
  for (int i = 0; i < layout->count(); ++i) {
    if (auto &&button =
            qobject_cast<QPushButton *>(layout->itemAt(i)->widget()))
      button->setDisabled(is_disabled);
  }
}

void MainWindow::on_treeView_clicked(const QModelIndex &index) {
  ui->tableView->setModel(
      index.data(Qt::UserRole + 1).value<QStandardItemModel *>());
  ui->tableView->setItemDelegateForColumn(
      0, new QLineEditDelegate(ui->treeView, WhatValidate::XMLAttribute));
  setDisableForLayoutElements(ui->horizontalLayout_2, false);
}

void MainWindow::on_treeView_selectionModel_selectionChanged(
    const QItemSelection &selection_now,
    const QItemSelection &selection_before) {
  bool contains_root = false;
  if (selection_now.empty()) {
    setDisableForLayoutElements(ui->horizontalLayout, true);
    setDisableForLayoutElements(ui->horizontalLayout_2, true);
    ui->pushButton_plus_tree->setDisabled(contains_root);
    return;
  }

  auto &&tree_view_model =
      qobject_cast<QStandardItemModel *>(ui->treeView->model());
  if (!tree_view_model) {
    assert(false);
    return;
  }

  for (auto &&item : selection_now.indexes()) {
    qDebug() << item.data(Qt::UserRole + 1).value<QStandardItemModel *>();
    ui->tableView->setModel(
        item.data(Qt::UserRole + 1).value<QStandardItemModel *>());
    ui->tableView->setItemDelegateForColumn(
        0, new QLineEditDelegate(ui->treeView, WhatValidate::XMLAttribute));
    setDisableForLayoutElements(ui->horizontalLayout_2, false);
    if (item.parent() == tree_view_model->invisibleRootItem()->index()) {
      contains_root = true;
      break;
    }
  }

  ui->pushButton_plus_tree->setDisabled(contains_root);
  ui->pushButton_minus_tree->setDisabled(false);
  ui->pushButton_new_child_row_tree->setDisabled(false);
}

void MainWindow::on_pushButton_plus_tree_clicked() {
  auto &&tree_view_model =
      qobject_cast<QStandardItemModel *>(ui->treeView->model());
  if (!tree_view_model) {
    assert(false);
    return;
  }

  auto &&selection_model = ui->treeView->selectionModel();
  _comm_managers[0]->Do(
      new AddTagCommand(tree_view_model, selection_model, ""));
}

void MainWindow::on_pushButton_minus_tree_clicked() {
  _comm_managers[0]->Do(new RemoveTagCommand(ui->treeView, ui->tableView));
  setDisableForLayoutElements(ui->horizontalLayout_2, true);
  setDisableForLayoutElements(ui->horizontalLayout,
                              (ui->treeView->model()->rowCount() == 0));
  ui->pushButton_plus_tree->setDisabled(false);
}

void MainWindow::on_pushButton_new_child_row_tree_clicked() {
  _comm_managers[0]->Do(new AddChildTagCommand(ui->treeView, ""));
}

void MainWindow::on_pushButton_plus_table_clicked() {
  auto &&selection_model = ui->treeView->selectionModel();
  if (!selection_model)
    return;
  QModelIndexList tree_indexes = selection_model->selectedIndexes();
  if (tree_indexes.size() == 1) {
    auto &&table_model =
        qobject_cast<QStandardItemModel *>(ui->tableView->model());
    if (!table_model)
      return;

    QStandardItem *parent_tag = table_model->invisibleRootItem();

    int row_to_insert = table_model->rowCount();
    parent_tag->insertRow(row_to_insert,
                          {new QStandardItem("attribute_name"),
                           new QStandardItem("attribute_value")});
    auto &&index_to_insert = table_model->index(row_to_insert, 0);
    ui->tableView->selectionModel()->select(
        index_to_insert, QItemSelectionModel::ClearAndSelect);
    ui->tableView->edit(index_to_insert);
  }
}

void MainWindow::on_pushButton_minus_table_clicked() {
  auto &&model = ui->treeView->selectionModel();
  if (!model)
    return;
  QModelIndexList tree_indexes = model->selectedIndexes();
  if (tree_indexes.size() == 1) {
    QModelIndexList table_indexes =
        ui->tableView->selectionModel()->selectedIndexes();
    if (table_indexes.size() == 1) {
      auto &&index_to_remove = table_indexes.at(0);
      ui->tableView->model()->removeRow(index_to_remove.row());
    }
  }
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
