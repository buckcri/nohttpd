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

#ifndef __OPTIONSDLG_HPP_
#define __OPTIONSDLG_HPP_

#include <wx/wxprec.h>
#ifdef __BORLANDC__
        #pragma hdrstop
#endif
#ifndef WX_PRECOMP
        #include <wx/wx.h>
#endif

#include <wx/dialog.h>

class OptionsDlg:public wxDialog{

public:

	OptionsDlg(wxWindow *parent,wxWindowID id=2,const wxString &title=_T("nohttpd: Options"),const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize,long style=wxCAPTION|wxSYSTEM_MENU|wxDIALOG_NO_PARENT|wxCLOSE_BOX);
	virtual ~OptionsDlg();

	wxCheckBox *WxVerboseLogCheckBox;
	wxCheckBox *WxLogURLCheckBox;
	wxCheckBox *WxCloseTrayCheckBox;
	wxCheckBox *WxSaveHitsCheckBox;
	wxCheckBox *WxNoTrayHideCheckBox;
	wxCheckBox *WxStatisticsCheckBox;
	wxFlexGridSizer *WxFlexGridSizer1;
	wxGridSizer *WxGridSizer1;
	wxGridSizer *WxGridSizer2;
	wxGridSizer *WxGridSizer3;
	wxStaticBoxSizer *WxStaticBoxSizer1;
	wxStaticBoxSizer *WxStaticBoxSizer2;
	wxRadioButton *WxRadioButton1;
	wxRadioButton *WxRadioButton2;
	wxTextCtrl *WxHistLenEdit;

	wxWindow *m_mdlg;

	enum{
		ID_WXVERBOSELOGCHECKBOX=2000,
		ID_WXLOGURLCHECKBOX,
		ID_WXCLOSETRAYCHECKBOX,
		ID_WXSAVEHITSCHECKBOX,
		ID_WXNOTRAYHIDECHECKBOX,
		ID_WXFULLQUERYRADIO,
		ID_WXBRIEFQUERYRADIO,
		ID_WXSTATISTICSCHECKBOX,
		ID_WXHISTLENEDIT
	};

	void WxNoTrayHideCheckBoxClick(wxCommandEvent &WXUNUSED(event));
	void WxSaveHitsCheckBoxClick(wxCommandEvent &WXUNUSED(event));
	void WxCloseTrayCheckBoxClick(wxCommandEvent &WXUNUSED(event));
	void WxLogURLCheckBoxClick(wxCommandEvent &WXUNUSED(event));
	void WxVerboseLogCheckBoxClick(wxCommandEvent &WXUNUSED(event));
	void WxStatisticsCheckBoxClick(wxCommandEvent &WXUNUSED(event));
	void WxQueryRadioClick(wxCommandEvent &WXUNUSED(event));
	void WxHistLenEditEnter(wxCommandEvent& WXUNUSED(event));

private:
	DECLARE_EVENT_TABLE()
};

#endif
