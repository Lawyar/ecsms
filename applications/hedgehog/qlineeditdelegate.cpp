#include "qlineeditdelegate.h"

#include <QDebug>
#include <QToolTip>

static bool isXMLName(const QString &text, bool has_child_element, QString *err_text = nullptr)
{
    bool res = false;
    bool starts_with_xml = QRegExp("^[xX][mM][lL].*").indexIn(text) == 0;
    bool correct_begining = QRegExp("^[a-zA-Z_]+").indexIn(text) == 0;

    if(starts_with_xml)
    {
        if(err_text)
                *err_text = "имя тэга не должно начинаться с xml (в любом регистре)";
        return res;
    }

    if(!correct_begining)
    {
        if(err_text)
                *err_text = "имя тэга должно начинаться с _ или буквы";
        return res;
    }

    bool correct_body;
    if(has_child_element)
    {
        correct_body = QRegExp("^[a-zA-Z_]+[\\w\\-\\.]*$").indexIn(text) == 0;
        if(!correct_body)
        {
            if(err_text)
                    *err_text = "данный тэг может содержать только элементный контент";
            return res;
        }
    }
    else
    {
        correct_body = QRegExp("^[a-zA-Z_]+[\\w\\-\\.]*(: .+)?$").indexIn(text) == 0;
        if(!correct_body)
        {
            if(err_text)
                    *err_text = "<имя тэга>: <текст>";
            return res;
        }
    }
    return true;
}

class XMLValidator: public QValidator
{
public:

    XMLValidator(const QModelIndex &index)
        :_index(index)
    {}

    virtual QValidator::State validate(QString &text, int &pos) const override
    {
        if(text.isEmpty())
            return Intermediate;
        auto res = isXMLName(text, _index.child(0, 0) != QModelIndex()) ? Acceptable: Intermediate;
        return res;
    }

private:
    QModelIndex _index;
};

QLineEditDelegate::QLineEditDelegate(QObject *parent)
    : QItemDelegate(parent){}

QWidget *QLineEditDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    auto Validator = new XMLValidator(index);

    auto editor = new QLineEdit(parent);
    QObject::connect(editor, &QLineEdit::textEdited, [editor, index](const QString& text){
        QString err;
        if (!isXMLName(text, index.child(0, 0) != QModelIndex(), &err))
        {
            QToolTip::showText(editor->mapToGlobal(QPoint()), err);
        }
    });
    editor -> setValidator(Validator);

    return editor;
}

void QLineEditDelegate::setEditorData( QWidget           * Editor ,
                    const QModelIndex & Index  ) const
{
    auto Value = Index.model() -> data( Index, Qt::EditRole );
    auto CastedEditor = static_cast< QLineEdit * >( Editor );
    CastedEditor -> setText( Value.toString() );
}

void QLineEditDelegate::setModelData( QWidget            * Editor ,
                   QAbstractItemModel * Model  ,
                   const QModelIndex  & Index  ) const
{
    auto CastedEditor = static_cast< QLineEdit * >( Editor );
    auto Value = CastedEditor->text();
    Model -> setData( Index, Value, Qt::EditRole );
}


void QLineEditDelegate::updateEditorGeometry( QWidget *Editor,
                           const QStyleOptionViewItem & Option ,
                           const QModelIndex          & Index  ) const
{
    Editor -> setGeometry( Option.rect );
}
