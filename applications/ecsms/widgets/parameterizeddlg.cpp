#include "ParameterizedDlg.h"

#include <QLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QEvent>

ParameterizedDlg::ParameterizedDlg(const QString& title,
                                   IParameterized& parameterized,
                                   QWidget* parent)
    : QDialog(parent), m_parameterized(parameterized) {
  setWindowTitle(title);
  FillParameterValues();
  m_enterButton = new QPushButton("Apply", this);
  layout()->addWidget(m_enterButton);
  connect(m_enterButton, &QPushButton::clicked, this,
          &ParameterizedDlg::SetParameterFromWidgets);

  installEventFilter(this);
}

void ParameterizedDlg::FillParameterValues() {
  m_paramInfos.clear();
  while (QWidget* w = findChild<QWidget*>())
    delete w;

  QVBoxLayout* mainLayout = new QVBoxLayout(this);

  auto paramValues = m_parameterized.GetPatameterValues();
  for (auto&& paramValue : paramValues) {
    QHBoxLayout* subLayout = new QHBoxLayout(this);
    mainLayout->addLayout(subLayout);

    QString obviousParamName;
    if (auto obviousParamNameOpt =
            m_parameterized.GetObviousParamName(paramValue.paramName))
      obviousParamName = QString::fromStdWString(*obviousParamNameOpt);

    else
      obviousParamName = "<Error : unknown parameter name>";

    QLabel* paramNameLabel = new QLabel(obviousParamName, this);
    QLineEdit* paramValueEdit =
        new QLineEdit(QString::fromStdString(paramValue.paramValue), this);

    subLayout->addWidget(paramNameLabel);
    subLayout->addWidget(paramValueEdit);

    m_paramInfos.push_back({QString::fromStdString(paramValue.paramName),
                            paramNameLabel, paramValueEdit});
  }

  setLayout(mainLayout);
}

bool ParameterizedDlg::eventFilter(QObject* object, QEvent* e) {
  if (e->type() == QEvent::EnterWhatsThisMode) {
    e->accept();
    QString helpString =
        QString::fromStdWString(m_parameterized.GetHelpString());
    if (!helpString.isEmpty())
      QMessageBox::information(this, "Справка", helpString);
    return true;
  }
  return QObject::eventFilter(object, e);
}

void ParameterizedDlg::SetParameterFromWidgets() {
  bool success = true;
  for (auto&& [paramName, obviousParamNameLabel, paramValueLineEdit] :
       m_paramInfos) {
    auto&& paramValue = paramValueLineEdit->text();
    bool status = m_parameterized.SetParameterValue(paramName.toStdString(),
                                                    paramValue.toStdString());
    if (!status)
      QMessageBox::warning(
          this, "Warning",
          "Bad parameter or value : " + paramName + " - " + paramValue);
    success &= status;
  }

  if (success) {
    try {
      m_parameterized.ApplyParameterValues();
      accept();
    } catch (const std::exception& e) {
      QMessageBox::warning(this, "Warning", e.what());
    }
  }
}
