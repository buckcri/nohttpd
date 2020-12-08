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

#include "Common.h"
#include "AboutDlg.h"
#include "Nohttpd2Dlg.h"

#include <wx/bitmap.h>
#include <wx/statbmp.h>

#include "license.c" // char array of the license text

BEGIN_EVENT_TABLE(AboutDlg,wxDialog)
	EVT_CLOSE(AboutDlg::AboutDlgClose)
END_EVENT_TABLE()

AboutDlg::AboutDlg( wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &position, const wxSize& size, long style )
    : wxDialog( parent, id, title, position, size, style){

	nhbm=NULL;

	CreateGUIControlsCustom();

	this->SetIcon(*Nohttpd2Dlg::s_mainicon);
}

AboutDlg::~AboutDlg(){

	if (nhbm!=NULL){
		delete nhbm;
	}
}

void AboutDlg::CreateGUIControlsCustom(void){

	wxFlexGridSizer* WxFlexGridSizer1 = new wxFlexGridSizer(3,1,0,0);
	this->SetSizer(WxFlexGridSizer1);
	this->SetAutoLayout(true);

	wxFlexGridSizer* bmfgsizer=new wxFlexGridSizer(1,2,0,0);
	nhbm=new wxBitmap();
	nhbm->CopyFromIcon(*Nohttpd2Dlg::s_mainicon3232);
	wxStaticBitmap *nhsbm=new wxStaticBitmap(this,wxID_ANY,*nhbm,wxDefaultPosition);
	bmfgsizer->Add(nhsbm,0,wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxALL,5);
	wxStaticText *nhbmst=new wxStaticText(this,wxID_ANY,wxT("Welcome to nohttpd v2.1.0, built 18.06.2006 (date is in dd.mm.yyyy format).\n"));
	bmfgsizer->Add(nhbmst,0,wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxALL,5);

	WxFlexGridSizer1->Add(bmfgsizer,0,wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxALL,5);

	wxStaticBox* WxStaticBoxSizer2_StaticBoxObj = new wxStaticBox(this, wxID_ANY, wxT("About"));
	wxStaticBoxSizer* WxStaticBoxSizer2 = new wxStaticBoxSizer(WxStaticBoxSizer2_StaticBoxObj,wxHORIZONTAL);
	WxFlexGridSizer1->Add(WxStaticBoxSizer2,0,wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxALL,5);

	WxMemo2 = new wxTextCtrl(this, ID_WXMEMO2, wxT(""), wxPoint(10,20), wxSize(480,89), wxTE_READONLY | wxTE_CENTRE | wxTE_MULTILINE);

	wxTextAttr ta(*wxBLACK,*wxLIGHT_GREY);
	WxMemo2->SetDefaultStyle(ta);

	WxMemo2->SetMaxLength(0);
	WxMemo2->AppendText(wxT("Copyright (C) 2006 Christian Buck.\n"));
	WxMemo2->AppendText(wxT("nohttpd comes with ABSOLUTELY NO WARRANTY.\n"));
	WxMemo2->AppendText(wxT("This is free software, and you are welcome to redistribute it under certain conditions.\n"));
	WxMemo2->AppendText(wxT("nohttpd is released under the GNU General Public License (GPL);\n"));
	WxMemo2->AppendText(wxT("the full text of the license is available in the file LICENSE in the nohttpd directory.\n"));
	WxMemo2->AppendText(wxT("Additional information about and translations of the GPL can be found online at\n"));
	WxMemo2->AppendText(wxT("http://www.gnu.org/copyleft/gpl.html\n"));
	WxMemo2->AppendText(wxT("\n"));
	WxMemo2->AppendText(wxT("For additional information and program updates please see\n"));
	WxMemo2->AppendText(wxT("http://www.cdhk.de/buck/nohttpd/\n"));
	WxMemo2->AppendText(wxT("If you have got a bug report, feature request or just want to say thank you,\n"));
	WxMemo2->AppendText(wxT("feel free to email to\nc.buck@gmx.de\n"));
	WxMemo2->SetBackgroundColour(*wxLIGHT_GREY);
	WxStaticBoxSizer2->Add(WxMemo2,0,wxALIGN_CENTER_VERTICAL | wxALL,5);

	wxStaticBox* WxStaticBoxSizer1_StaticBoxObj = new wxStaticBox(this, wxID_ANY, wxT("License details"));
	wxStaticBoxSizer* WxStaticBoxSizer1 = new wxStaticBoxSizer(WxStaticBoxSizer1_StaticBoxObj,wxHORIZONTAL);
	WxFlexGridSizer1->Add(WxStaticBoxSizer1,0,wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxALL,5);

	WxMemo1 = new wxTextCtrl(this, ID_WXMEMO1, wxT(""), wxPoint(10,20), wxSize(480,195), wxTE_READONLY | wxTE_DONTWRAP | wxTE_MULTILINE|wxTE_RICH2);
	WxMemo1->SetMaxLength(0);

	wxFont font=wxFont(8,wxFONTFAMILY_TELETYPE,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL);
	ta.SetFont(font);
	WxMemo1->SetDefaultStyle(ta);

	wxString licensetext(LICENSE,*wxConvCurrent);
	WxMemo1->AppendText(licensetext);
	WxMemo1->SetBackgroundColour(*wxLIGHT_GREY);

	// This is a workaround for a scrolling bug in Rich2 edit controls.
	WxMemo1->ScrollLines(-1);
	WxMemo1->ShowPosition(0);

	WxStaticBoxSizer1->Add(WxMemo1,0,wxALIGN_CENTER_VERTICAL | wxALL,0);

	WxOkButton=new wxButton(this,wxID_OK);
	WxOkButton->SetDefault();
	WxFlexGridSizer1->Add(WxOkButton,0,wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxALL,5);

	GetSizer()->Fit(this);
	GetSizer()->SetSizeHints(this);
	this->SetTitle(wxT("nohttpd: About"));
	this->Center();
}

void AboutDlg::AboutDlgClose(wxCloseEvent &WXUNUSED(event)){

	Destroy();
}
