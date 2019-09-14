// --------------------------------------------------------------------------------
/*
    object_view.cpp

  Copyright (c) 2009-2015  Thomas Mueller, Frank Grave


   This file is part of the GeodesicViewer.

   The GeodesicViewer is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   The GeodesicViewer is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with the GeodesicViewer.  If not, see <http://www.gnu.org/licenses/>.
*/
// -------------------------------------------------------------------------------

#include <QGridLayout>
#include "object_view.h"

/*! Standard constructor.
 *  \param parent : pointer to parent widget.
 */
ObjectView::ObjectView(QWidget* parent)
    : QWidget(parent) {
    init();
}

/*! Standard destructor.
 */
ObjectView::~ObjectView() {
}


// *********************************** public methods *********************************

/*! Delete all objects.
 */
void ObjectView::clearAll() {
    ted_objects->clear();
    emit  objectsChanged();
}

/*! Write text.
 * \param text : text to be written.
 */
void ObjectView::writeText(QString text) {
    ted_objects->setText(text);
}

/*! Get text.
 */
QString
ObjectView::getText() {
    return ted_objects->toPlainText();
}


/*! Add object to the script engine.
 * \param engine : pointer to master script engine.
 */
//void ObjectView::addObjectsToScriptEngine(QScriptEngine* engine) {
//    QScriptValue obj = engine->newQObject(this);
//    engine->globalObject().setProperty("obj", obj);
//}

// ************************************* public slots ***********************************

/*! Show syntax
 *  \param index : index of syntax description.
 */
void ObjectView::slot_showSyntax(int index) {
    std::string syntax;
    MyObject::printSyntax(enum_object_type(index), syntax);
    led_syntax->setText(syntax.c_str());
}

/*!
 */
void ObjectView::slot_apply() {
    emit  objectsChanged();
}

/*!
 */
void ObjectView::slot_close() {
    hide();
}

/*!
 */
void ObjectView::slot_clear() {
    clearAll();
}

/*!
 */
void ObjectView::slot_add_object() {
    enum_object_type  type =  enum_object_type(cob_objects->currentIndex());

    if (type == enum_object_undefined) {
        return;
    }

    std::string text;
    MyObject::printStandard(type, text);

    ted_objects->append(text.c_str());
}

/*!  Add object text line.
 *  \param objectText : text line.
 */
void ObjectView::addObject(QString objectText) {
    ted_objects->append(objectText);
    emit  objectsChanged();
}


// *********************************** protected methods *********************************

/*! Initialize view.
 */
void ObjectView::init() {
    initElements();
    initGUI();
    initActions();
    initControl();
}

/*! Initialize view elements.
 */
void ObjectView::initElements() {
    lab_text = new QLabel("Enter coordinate objects into the text field:");
    ted_objects = new QTextEdit();
    ted_objects->setMinimumWidth(1000);

    QFont cf("Courier", 10);
    ted_objects->setFont(cf);

    lab_syntax = new QLabel("Syntax:");
    lab_syntax->setMaximumWidth(60);
    led_syntax = new QLineEdit();
    led_syntax->setReadOnly(true);
    led_syntax->setFont(cf);

    pub_apply = new QPushButton("Apply");
    pub_close = new QPushButton("Close");
    pub_clear = new QPushButton("Remove all");

    lab_add_object = new QLabel("Add object");
    lab_add_object->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    cob_objects = new QComboBox();
    cob_objects->addItems(stl_object_type);

    pub_add_object = new QPushButton(QIcon(":/add.png"), "");
    pub_add_object->setMaximumSize(52, 22);
}

/*! Construct graphical user interface.
 */
void ObjectView::initGUI() {
    QGridLayout* layout_complete = new QGridLayout();
    layout_complete->addWidget(lab_text, 0, 0, 1, 7);
    layout_complete->addWidget(ted_objects, 1, 0, 1, 7);
    layout_complete->addWidget(lab_syntax, 2, 0);
    layout_complete->addWidget(led_syntax, 2, 1, 1, 6);
    layout_complete->addWidget(pub_apply, 3, 0, 1, 2);
    layout_complete->addWidget(pub_close, 3, 2);
    layout_complete->addWidget(pub_clear, 3, 3);
    layout_complete->addWidget(lab_add_object, 3, 4);
    layout_complete->addWidget(cob_objects,    3, 5);
    layout_complete->addWidget(pub_add_object, 3, 6);
    setLayout(layout_complete);

    setWindowTitle("GeodesicViewer - Objects");
}

/*! Initialize actions.
 */
void ObjectView::initActions() {
    mActionApply = new QAction(this);
    mActionApply->setShortcut(Qt::CTRL | Qt::Key_A);
    mActionApply->setShortcutContext(Qt::ApplicationShortcut);
    addAction(mActionApply);
    connect(mActionApply, SIGNAL(triggered()), this, SLOT(slot_apply()));
}

/*! Connect signals and slots.
 */
void ObjectView::initControl() {
    connect(cob_objects, SIGNAL(activated(int)), this, SLOT(slot_showSyntax(int)));
    connect(pub_apply, SIGNAL(pressed()), this, SLOT(slot_apply()));
    connect(pub_close, SIGNAL(pressed()), this, SLOT(slot_close()));
    connect(pub_clear, SIGNAL(pressed()), this, SLOT(slot_clear()));
    connect(pub_add_object, SIGNAL(pressed()), this, SLOT(slot_add_object()));
}
