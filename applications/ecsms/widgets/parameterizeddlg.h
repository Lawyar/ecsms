#pragma once

#include <IParameterized.h>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>

class ParameterizedDlg : public QDialog {
  Q_OBJECT

 public:
  ParameterizedDlg(const QString & title, IParameterized& parameterized, QWidget* parent = nullptr);

  void FillParameterValues();

 public slots:
  void SetParameterFromWidgets();

 private:
  IParameterized& m_parameterized;

  struct ParametersInfo {
    QString paramName;
    QLabel* obviousParamName;
    QLineEdit* paramValue;
  };
  std::vector<ParametersInfo> m_paramInfos;
  QPushButton* m_enterButton;
};
