/*
    This file is part of nohttpd version 2, see http://www.cdhk.de/buck/nohttpd/
    Copyright (C) 2006 Christian Buck <c.buck@gmx.de>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef __ABOUTDLG_HPP_
#define __ABOUTDLG_HPP_

#include <wx/wxprec.h>
#ifdef __BORLANDC__
        #pragma hdrstop
#endif
#ifndef WX_PRECOMP
        #include <wx/wx.h>
#endif

#include <wx/button.h>
#include <wx/textctrl.h>
#include <wx/dialog.h>

//Compatibility for 2.4.x code
#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif
#ifndef wxFIXED_MINSIZE
#define wxFIXED_MINSIZE 0
#endif

#undef AboutDlg_STYLE
#define AboutDlg_STYLE wxCAPTION | wxSYSTEM_MENU | wxDIALOG_NO_PARENT | wxCLOSE_BOX

class AboutDlg:public wxDialog{

public:
    AboutDlg( wxWindow *parent, wxWindowID id = 1, const wxString &title = wxT("About nohttpd"),
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = AboutDlg_STYLE);

	virtual ~AboutDlg();

	wxButton *WxOkButton;
	wxTextCtrl *WxMemo1;
	wxStaticBoxSizer *WxStaticBoxSizer1;
	wxTextCtrl *WxMemo2;
	wxStaticBoxSizer *WxStaticBoxSizer2;
	wxFlexGridSizer *WxFlexGridSizer1;

	enum{
		ID_WXBUTTON1 = 1015,
		ID_WXMEMO1 = 1013,
		ID_WXMEMO2 = 1010,
		ID_DUMMY_VALUE_
	};

	void AboutDlgClose(wxCloseEvent &WXUNUSED(event));
	void CreateGUIControlsCustom(void);

private:
	wxBitmap *nhbm;
	DECLARE_EVENT_TABLE()
};

#endif
