// --------------------------------------------------------------------------------
/*
    doc_view.h

  Copyright (c) 2009-2013  Thomas Mueller, Frank Grave


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

/*!  \class  DocView
     \brief  View for manual.

*/
// --------------------------------------------------------------------------------

#ifndef DOC_VIEW_H
#define DOC_VIEW_H

#include <iostream>

#include <QPushButton>
#include <QWebView>


#include <gdefs.h>

// ---------------------------------------------------
//    class definition:   DocView
// ---------------------------------------------------
class DocView : public QWidget {
public:
    DocView();

// ----------- protected methods -----------
protected:
    void  init();
    void  initElements();
    void  initGUI();
    void  initControl();

// ----------- private attributes ----------
private:
    QWebView*     view;

    QPushButton*  pub_back;
    QPushButton*  pub_forward;
    QPushButton*  pub_close;
};

#endif // DOC_VIEW_H
