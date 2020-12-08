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

#ifndef __STATISTICSDLG_h__
#define __STATISTICSDLG_h__

#include "Common.h"

#ifdef __BORLANDC__
	#pragma hdrstop
#endif


#ifndef WX_PRECOMP
	#include <wx/wx.h>
	#include <wx/dialog.h>
#else
	#include <wx/wxprec.h>
#endif

#include <wx/button.h>
#include <wx/statline.h>
#include <wx/listctrl.h>
#include <wx/stattext.h>

//Compatibility for 2.4 code
#ifndef wxCLOSE_BOX
	#define wxCLOSE_BOX 0x1000
#endif
#ifndef wxFIXED_MINSIZE
	#define wxFIXED_MINSIZE 0
#endif

#undef StatisticsDlg_STYLE
//#define StatisticsDlg_STYLE wxCAPTION | wxSYSTEM_MENU | wxDIALOG_NO_PARENT | wxCLOSE_BOX
#define StatisticsDlg_STYLE wxMAXIMIZE_BOX | wxRESIZE_BORDER | wxCAPTION | wxSYSTEM_MENU | wxDIALOG_NO_PARENT | wxCLOSE_BOX

class StatisticsDlg:public wxDialog{

	public:
		wxButton *WxOkButton;
		wxListCtrl *WxDetailListCtrl;
		wxStaticLine *WxStaticLine1;
		static wxListCtrl *WxGeneralListCtrl;
		wxStaticText *WxStaticText1;
		wxFlexGridSizer *WxFlexGridSizer1;

		enum{
			ID_WXOKBUTTON = 1100,
			ID_WXDETAILLISTCTRL,
			ID_WXSTATICLINE1,
			ID_WXGENERALLISTCTRL,
			ID_WXSTATICTEXT1,
			ID_WXSTATICTEXT2
		};

		static bool sortascending[3];

		StatisticsDlg(wxWindow *parent, wxWindowID id = 1, const wxString &title = wxT("nohttpd:  Statistics"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = StatisticsDlg_STYLE);
		virtual ~StatisticsDlg();

		void StatisticsDlgClose(wxCloseEvent &WXUNUSED(event));
		void CreateGUIControlsCustom(void);
		void OnColumnClick(wxListEvent &event);
		void OnItemSelected(wxListEvent &event);

		static wxString getTextByColumn(long index,int column);

	private:
		void insertGeneralItems();
		void setBestColumnWidth(wxListCtrl *listctrl,int column);
		DECLARE_EVENT_TABLE();
};

#endif
