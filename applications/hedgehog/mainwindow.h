#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);

  ~MainWindow();

  bool event(QEvent *event) override;

private:
  QStandardItem *createTag(QStandardItem *parent_tag,
                           QStandardItemModel *attribute_table_view,
                           const QString &text);
  void on_actionNewFile_triggered_tab0();
  void on_actionOpen_triggered_tab0();
  void on_actionSave_triggered_tab0();
  void on_actionRedo_triggered_tab0();
  void on_actionUndo_triggered_tab0();

  void on_actionNewFile_triggered_tab1();
  void on_actionOpen_triggered_tab1();
  void on_actionSave_triggered_tab1();
  void on_actionRedo_triggered_tab1();
  void on_actionUndo_triggered_tab1();

private slots:
  void on_actionNewFile_triggered();
  void on_actionOpen_triggered();
  void on_actionSave_triggered();
  void on_actionRedo_triggered();
  void on_actionUndo_triggered();
  void on_treeView_clicked(const QModelIndex &index);
  void on_pushButton_plus_tree_clicked();
  void on_pushButton_minus_tree_clicked();
  void on_pushButton_new_child_row_tree_clicked();
  void on_pushButton_plus_table_clicked();
  void on_pushButton_minus_table_clicked();
  void on_listView_doubleClicked(const QModelIndex &index);
  void on_pushButton_pausePipeline_pressed();
  void on_pushButton_stopPipeline_pressed();

private:
  Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
