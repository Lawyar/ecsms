#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qlineeditdelegate.h"
#include "blockwidget.h"
#include "blockfield.h"

#include <QFileDialog>
#include <QXmlStreamReader>
#include <QStack>
#include <QDebug>
#include <QVariant>
#include <QMessageBox>
#include <QLineSeries>
#include <QChart>
#include <QChartView>

static void createDefaultBlock(BlockField *parent)
{
    auto default_block = new BlockWidget(parent);
    default_block->show();
    default_block->move(parent->rect().center());
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    auto tree_model = new QStandardItemModel(0, 0, ui->treeView);
    ui->treeView->setModel(tree_model);
    ui->treeView->setItemDelegateForColumn(0, new QLineEditDelegate(ui->treeView));

    for (auto i = 0; i < 4; ++i)
    {
        auto series = new QtCharts::QLineSeries;
        series->append(0, 6);
        series->append(2, 4);
        series->append(3, 8);
        series->append(7, 4);
        series->append(10, 5);
        *series << QPointF(11, 1) << QPointF(13, 3) << QPointF(17, 6) << QPointF(18, 3) << QPointF(20, 2);

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
     QVariant data;
     data.setValue(reinterpret_cast<void*>(&createDefaultBlock));
     block_list_element->setData(data);

     parent_item->appendRow(block_list_element);
     block_list_element->setFlags(block_list_element->flags() ^ Qt::ItemIsEditable);

     auto  sp = ui->scrollAreaWidgetContents->sizePolicy();
     sp.setHorizontalStretch(INT_MAX);
     sp.setVerticalStretch(INT_MAX);
     ui->scrollAreaWidgetContents->setSizePolicy(sp);
     ui->splitter_4->setStretchFactor(0, 1);

     ui->splitter_5->setStretchFactor(1, 1);
     ui->splitter_5->setStretchFactor(0, INT_MAX);
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::event(QEvent* e)
{
    if(e->type() == QEvent::KeyPress)
        return qobject_cast<QObject*>(ui->scrollAreaWidgetContents)->event(e);
    return QMainWindow::event(e);
}

QStandardItem *MainWindow::createTag(QStandardItem * parent_tag, QStandardItemModel *attribute_table_view, const QString &text)
{
    if(attribute_table_view == nullptr)
    {
        attribute_table_view = new QStandardItemModel();
        attribute_table_view->setHorizontalHeaderLabels(QStringList({"Attribute", "Value"}));
    }

    auto new_tag = new QStandardItem(text.isEmpty()? "tag_name" : text);
    parent_tag->appendRow(new_tag);
    QVariant table_model_variant;
    table_model_variant.setValue(attribute_table_view);
    new_tag->setData(table_model_variant);
    new_tag->setFlags(new_tag->flags() | Qt::ItemIsEditable);
    return new_tag;
}

void MainWindow::on_actionOpen_triggered()
{
    QString file_name = QFileDialog::getOpenFileName(this, tr("Open file"), "C:/", tr("XML files (*.xml)"));;
    if(file_name.size() == 0)
    {
        QMessageBox::warning(this, "Внимание", "Файл не был выбран");
        return;
    }
    QFile file(file_name);
    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(this, "Ошибка", "Не удалось открыть файл " + file_name);
        return;
    }

    delete ui->treeView->model();
    delete ui->tableView->model();
    QStandardItemModel* model = new QStandardItemModel(0, 0, ui->treeView);
    ui->treeView->setModel(model);
    QStandardItem *parent_item = model->invisibleRootItem();

    QXmlStreamReader xml_reader(&file);
    QStack<QStandardItem *> tags;
    tags.push(parent_item);
    while(!xml_reader.atEnd()){
        xml_reader.readNext();
        switch (xml_reader.tokenType()){
        case QXmlStreamReader::StartElement:
        { // открывающий тэг
            auto table_model = new QStandardItemModel();
            for(auto &attr: xml_reader.attributes())
            {
                auto attr_name_item = new QStandardItem(attr.name().toString());
                auto attr_value_item = new QStandardItem(attr.value().toString());
                table_model->appendRow(QList<QStandardItem *>({attr_name_item, attr_value_item}));
            }
            table_model->setHorizontalHeaderLabels(QStringList({"Attribute", "Value"}));
            auto &&new_tag = createTag(tags.top(), table_model, xml_reader.name().toString());
            tags.push(new_tag);
            break;
        }
        case QXmlStreamReader::Characters:
        { // текст внутри тэга
            QString str = xml_reader.text().toString().trimmed();
            if (!str.isEmpty())
            {
                auto tag = tags.top();
                tag->setText(tag->text() + ": " + str);
            }
            break;
        }
        case QXmlStreamReader::EndElement:
        {
            tags.pop();
            break;
        }
        case QXmlStreamReader::StartDocument:
        case QXmlStreamReader::EndDocument:
            break;
        default:
        {
            qDebug() << "error: unexpected token type " << xml_reader.tokenType();
            break;
        }
        }
    }
    ui->treeView->resizeColumnToContents(0);
    file.close(); // Закрываем файл
}

static void writeAttributes(QXmlStreamWriter& xml_writer, const QStandardItemModel* model)
{
    for(int r = 0; r < model->rowCount(); ++r) {
        auto &&name = model->item(r, 0)->text();
        auto &&value = model->item(r, 1)->text();
        xml_writer.writeAttribute(name, value);
    }
}

static void writeTree(QXmlStreamWriter& xml_writer, const QStandardItem* root)
{
    auto &&tag_data = root->text().split(": ");
    xml_writer.writeStartElement(tag_data.front());
    auto &&model = root->data(Qt::UserRole + 1).value<QStandardItemModel*>();
    writeAttributes(xml_writer, model);
    if (tag_data.size() > 1)
    {
        tag_data.pop_front();
        xml_writer.writeCharacters(tag_data.join(": "));
    }
    for(int r = 0; root->hasChildren() && r < root->rowCount(); ++r)
    {
        auto &&item = root->child(r);
        writeTree(xml_writer, item);
    }
    xml_writer.writeEndElement();
}

void MainWindow::on_actionSave_triggered()
{
    QString file_name = QFileDialog::getSaveFileName(this, tr("Save As"), "C:/", tr("XML files (*.xml)"));
    if(file_name.size() == 0)
    {
        QMessageBox::warning(this, "Внимание", "Файл не был выбран");
        return;
    }
    QFile file(file_name);
    if (!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::critical(this, "Ошибка", "Не удалось сохранить в файл " + file_name);
        return;
    }

    QXmlStreamWriter xml_writer(&file);
    xml_writer.setAutoFormatting(true);  // Устанавливаем автоформатирование текста

    auto &&model = dynamic_cast<QStandardItemModel*>(ui->treeView->model());
    auto &&first_item = model->item(0, 0);
    xml_writer.writeStartDocument();
    writeTree(xml_writer, first_item);
    xml_writer.writeEndDocument();
    file.close();
}

static void setDisableForLayoutElements(QLayout* layout, bool is_disabled)
{
    for(int i =  0; i < layout->count(); ++i)
    {
        if (auto &&button = qobject_cast<QPushButton*>(layout->itemAt(i)->widget()))
            button->setDisabled(is_disabled);
    }
}

void MainWindow::on_treeView_clicked(const QModelIndex &index)
{
    ui->tableView->setModel(index.data(Qt::UserRole + 1).value<QStandardItemModel*>());
    setDisableForLayoutElements(ui->horizontalLayout, false);
    setDisableForLayoutElements(ui->horizontalLayout_2, false);
    if(index.parent() == ui->treeView->rootIndex())
    {
        ui->pushButton_plus_tree->setDisabled(true);
    }
}

void MainWindow::on_pushButton_plus_tree_clicked()
{
    if(ui->treeView->model()->rowCount() == 0)
    {
        QStandardItemModel* model = nullptr;
        if (!(model = qobject_cast<QStandardItemModel*>(ui->treeView->model())))
        {
            qDebug() << "Error: can't cast treeView->model() to QStandardItemModel*";
            return;
        }
        QStandardItem *parent_item = model->invisibleRootItem();
        auto new_tag = createTag(parent_item, nullptr, "");
        ui->treeView->selectionModel()->select(new_tag->index(), QItemSelectionModel::ClearAndSelect);
        ui->treeView->edit(new_tag->index());
        setDisableForLayoutElements(ui->horizontalLayout, false);
        ui->pushButton_plus_tree->setDisabled(true);
        return;
    }

    QModelIndexList indexes = ui->treeView->selectionModel()->selectedIndexes();
    if (indexes.size() == 1) {
        QModelIndex selected_index = indexes.at(0);
        if(selected_index.parent() == ui->treeView->rootIndex())
            return;

        if (auto && model = qobject_cast<const QStandardItemModel*>(ui->treeView->model()))
        {
            if (auto && selected_tag = model->itemFromIndex(selected_index))
            {
                auto child_tag = createTag(selected_tag->parent(), nullptr, "");
                ui->treeView->selectionModel()->select(child_tag->index(), QItemSelectionModel::ClearAndSelect);
                ui->treeView->edit(child_tag->index());
            }
        }
    }
}

void MainWindow::on_pushButton_minus_tree_clicked()
{
    QModelIndexList indexes = ui->treeView->selectionModel()->selectedIndexes();
    if (indexes.size() == 1) {

        QModelIndex selectedIndex = indexes.at(0);
        ui->treeView->model()->removeRow(selectedIndex.row(), selectedIndex.parent());
    }
    if (ui->treeView->model()->rowCount() == 0)
    {
        ui->tableView->setModel(nullptr);
        setDisableForLayoutElements(ui->horizontalLayout, true);
        ui->pushButton_plus_tree->setDisabled(false);
    }
}

void MainWindow::on_pushButton_new_child_row_tree_clicked()
{
    QModelIndexList indexes = ui->treeView->selectionModel()->selectedIndexes();
    if (indexes.size() == 1) {
        QModelIndex selected_index = indexes.at(0);
        if (auto && model = qobject_cast<const QStandardItemModel*>(ui->treeView->model()))
        {
            if (auto && selected_tag = model->itemFromIndex(selected_index))
            {
                auto &&tag_text = selected_tag->text();
                auto &&tag_text_vec = tag_text.split(": ");
                if(tag_text_vec.size() > 1)
                {
                    selected_tag->setText(tag_text_vec[0]);
                }
                auto child_tag = createTag(selected_tag, nullptr, "");
                ui->treeView->expand(selected_tag->index());
                ui->treeView->selectionModel()->select(child_tag->index(), QItemSelectionModel::ClearAndSelect);
                ui->treeView->edit(child_tag->index());
            }
        }
    }
}

void MainWindow::on_pushButton_plus_table_clicked()
{
    QModelIndexList tree_indexes = ui->treeView->selectionModel()->selectedIndexes();
    if (tree_indexes.size() == 1) {
        int row_to_insert = ui->tableView->model()->rowCount();
        ui->tableView->model()->insertRow(row_to_insert);
        auto &&index_to_insert = ui->tableView->model()->index(row_to_insert, 0);
        ui->tableView->selectionModel()->select(index_to_insert, QItemSelectionModel::ClearAndSelect);
        ui->tableView->edit(index_to_insert);
    }    
}

void MainWindow::on_pushButton_minus_table_clicked()
{
    QModelIndexList tree_indexes = ui->treeView->selectionModel()->selectedIndexes();
    if (tree_indexes.size() == 1) {
        QModelIndexList table_indexes = ui->tableView->selectionModel()->selectedIndexes();
        if (table_indexes.size() == 1) {
            auto &&index_to_remove = table_indexes.at(0);
            ui->tableView->model()->removeRow(index_to_remove.row());
        }
    }
}

void MainWindow::on_listView_doubleClicked(const QModelIndex &index)
{
    auto data = index.data(Qt::UserRole + 1);
    auto func = data.value<void*>();
    auto reallyFunc = reinterpret_cast<decltype(&createDefaultBlock)>(func);
    reallyFunc(ui->scrollAreaWidgetContents);
}


void MainWindow::on_pushButton_3_pressed()
{
    auto text = ui->pushButton_3->text();
    if(text == "||")
        ui->pushButton_3->setText("▶");
    else
    {
        ui->pushButton_3->setText("||");
        ui->pushButton_4->setEnabled(true);
    }
}


void MainWindow::on_pushButton_4_pressed()
{
    ui->pushButton_4->setEnabled(false);
    ui->pushButton_3->setText("▶");
}

