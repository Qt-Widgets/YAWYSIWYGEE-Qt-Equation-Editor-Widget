#ifndef COMMANDINSERTMULTILINE_H
#define COMMANDINSERTMULTILINE_H

#include <QUndoCommand>

#include "line.h"
#include "text.h"
#include <QTextCursor>

namespace Typeset{

class Cursor;

class CommandInsertMultiline : public QUndoCommand{
private:
    Cursor& cursor;
    Document& doc;
    bool active;
    Line* lL;
    Text* tL;
    QTextCursor cL;
    QString append_str;
    QString removed_str;
    Construct* tL_next;
    Line* lL_next;
    Line* lR;
    Text* tR;
    QTextCursor cR;

public:
    CommandInsertMultiline(Cursor& cursor, Document& doc, const QString& source, Text* t, QTextCursor c);
    ~CommandInsertMultiline();

protected:
    virtual void redo() final;
    virtual void undo() final;
};

}

#endif // COMMANDINSERTMULTILINE_H
