#include "commandevalphrase.h"

#include "algorithm.h"
#include "construct.h"
#include "cursor.h"
#include "parser.h"

namespace Typeset{

CommandEvalPhrase::CommandEvalPhrase(Cursor& cursor, const QString& source, Text* t, QTextCursor c)
    : cursor(cursor),
      tL(t),
      cL(c) {
    Q_ASSERT(!source.contains('\n'));

    std::pair<Text*,Text*> ends = Parser::parsePhrase(source, t->getScriptLevel());
    append_str = ends.first->toPlainText();
    c.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
    removed_str = c.selectedText();
    tL_next = ends.first->next;
    Q_ASSERT(tL_next);
    delete ends.first;
    tL_next->prev = t;

    for(Construct* c = tL_next; c; c = c->next->next){
        c->setParentItem(t->parent);
        c->next->setParentPhrase(t->parent);
    }

    tR = ends.second;
    tR->next = tL->next;

    cR = tR->textCursor();
    cR.movePosition(QTextCursor::End);
    cR.insertText(removed_str);
    cR.setPosition(cR.position() - removed_str.length());
}

CommandEvalPhrase::~CommandEvalPhrase(){
    if(active) return;

    Construct* c = tL_next;
    Construct* final = tR->next;
    while(c != final){
        Construct* to_delete = c;
        c = c->next->next;
        delete to_delete->next;
        to_delete->deletePostorder();
    }
}

void CommandEvalPhrase::redo(){
    active = true;

    cL.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
    cL.removeSelectedText();
    cL.insertText(append_str);
    cL.setPosition(cL.position() - append_str.length());

    for(Construct* c = tL_next; c != tR->next; c = c->next->next){
        c->show();
        c->next->show();
    }

    tL->next = tL_next;
    if(tR->next) tR->next->prev = tR;
    else tR->parent->back = tR;

    tL->updateToTop();
    cursor.setPosition(*tR, cR);
}

void CommandEvalPhrase::undo(){
    active = false;

    cL.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
    cL.removeSelectedText();
    cL.insertText(removed_str);
    cL.setPosition(cL.position() - removed_str.length());

    for(Construct* c = tL_next; c != tR->next; c = c->next->next){
        c->hide();
        c->next->hide();
    }

    tL->next = tR->next;
    if(tR->next) tR->next->prev = tL;
    else tR->parent->back = tL;

    tL->updateToTop();
    cursor.setPosition(*tL, cL);
}

}
