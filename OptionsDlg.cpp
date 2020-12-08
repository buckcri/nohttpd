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

#include "OptionsDlg.h"
#include "Nohttpd2Dlg.h"
#include "ReceiverThread.h"

#include <wx/config.h>

// Declaration of custom events caught by class Nohttpd2Dlg
BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EVENT_TYPE(wxEVT_LOGMSG, wxID_HIGHEST+1)
    DECLARE_EVENT_TYPE(wxEVT_SHUTDOWN, wxID_HIGHEST+2)
    DECLARE_EVENT_TYPE(wxEVT_SHOWMAIN, wxID_HIGHEST+3)
    DECLARE_EVENT_TYPE(wxEVT_SETHITS, wxID_HIGHEST+4)
    DECLARE_EVENT_TYPE(wxEVT_LOGVERBOSE, wxID_HIGHEST+5)
    DECLARE_EVENT_TYPE(wxEVT_HIDEDLG, wxID_HIGHEST+6)
    DECLARE_EVENT_TYPE(wxEVT_ADDTRAY, wxID_HIGHEST+7)
    DECLARE_EVENT_TYPE(wxEVT_REMTRAY, wxID_HIGHEST+8)
END_DECLARE_EVENT_TYPES()


BEGIN_EVENT_TABLE(OptionsDlg,wxDialog)
	EVT_CHECKBOX(ID_WXVERBOSELOGCHECKBOX,OptionsDlg::WxVerboseLogCheckBoxClick)
	EVT_CHECKBOX(ID_WXLOGURLCHECKBOX,OptionsDlg::WxLogURLCheckBoxClick)
	EVT_CHECKBOX(ID_WXCLOSETRAYCHECKBOX,OptionsDlg::WxCloseTrayCheckBoxClick)
	EVT_CHECKBOX(ID_WXSAVEHITSCHECKBOX,OptionsDlg::WxSaveHitsCheckBoxClick)
	EVT_CHECKBOX(ID_WXNOTRAYHIDECHECKBOX,OptionsDlg::WxNoTrayHideCheckBoxClick)
	EVT_CHECKBOX(ID_WXSTATISTICSCHECKBOX,OptionsDlg::WxStatisticsCheckBoxClick)
	EVT_RADIOBUTTON(ID_WXFULLQUERYRADIO,OptionsDlg::WxQueryRadioClick)
	EVT_RADIOBUTTON(ID_WXBRIEFQUERYRADIO,OptionsDlg::WxQueryRadioClick)
	EVT_TEXT_ENTER(ID_WXHISTLENEDIT,OptionsDlg::WxHistLenEditEnter)
END_EVENT_TABLE()


OptionsDlg::OptionsDlg(wxWindow *parent,wxWindowID id,const wxString &title,const wxPoint &position,const wxSize& size,long style ):wxDialog(parent,id,title,position,size,style){

	bool dummyboolean;

	wxFlexGridSizer* WxFlexGridSizer1=new wxFlexGridSizer(4,1,0,0);
	SetSizer(WxFlexGridSizer1);
	SetAutoLayout(true);

	wxStaticBox* WxStaticBoxSizer2_StaticBoxObj = new wxStaticBox(this, wxID_ANY, wxT("General:"));
	wxStaticBoxSizer* WxStaticBoxSizer2 = new wxStaticBoxSizer(WxStaticBoxSizer2_StaticBoxObj,wxHORIZONTAL);
	WxFlexGridSizer1->Add(WxStaticBoxSizer2,1,wxGROW | wxALIGN_CENTER_VERTICAL | wxALL,1);

	wxGridSizer* WxGridSizer2 = new wxGridSizer(2,2,0,0);
	WxStaticBoxSizer2->Add(WxGridSizer2,1,wxALIGN_CENTER_VERTICAL | wxALL,1);

	WxNoTrayHideCheckBox = new wxCheckBox(this, ID_WXNOTRAYHIDECHECKBOX, wxT("No tray icon"));
	WxNoTrayHideCheckBox->SetToolTip(wxT("If enabled, nohttpd doesn't display an icon in the tray bar."));
	WxGridSizer2->Add(WxNoTrayHideCheckBox,0,wxGROW | wxALIGN_CENTER_VERTICAL | wxALL,5);

	WxSaveHitsCheckBox = new wxCheckBox(this, ID_WXSAVEHITSCHECKBOX, wxT("Save hit counter"));
	WxSaveHitsCheckBox->SetToolTip(wxT("If enabled, nohttpd saves hit counts between sessions."));
	WxGridSizer2->Add(WxSaveHitsCheckBox,0,wxGROW | wxALIGN_CENTER_VERTICAL | wxALL,5);

	WxCloseTrayCheckBox = new wxCheckBox(this, ID_WXCLOSETRAYCHECKBOX, wxT("Close to tray"));
	WxCloseTrayCheckBox->SetToolTip(wxT("If enabled, nohttpd minimizes to the tray bar if the close button is clicked.\nThis is inactive if \"No tray icon\" is enabled."));
	WxGridSizer2->Add(WxCloseTrayCheckBox,0,wxGROW | wxALIGN_CENTER_VERTICAL | wxALL,5);

	WxVerboseLogCheckBox = new wxCheckBox(this, ID_WXVERBOSELOGCHECKBOX, wxT("Log debug messages"));
	WxVerboseLogCheckBox->SetToolTip(wxT("If enabled, debug messages will be logged.\nThis is useful only for debugging and development purposes."));
	WxGridSizer2->Add(WxVerboseLogCheckBox,0,wxGROW | wxALIGN_CENTER_VERTICAL | wxALL,5);

	wxStaticBox* WxStaticBoxSizer1_StaticBoxObj = new wxStaticBox(this, wxID_ANY, wxT("Query Logging:"));
	wxStaticBoxSizer* WxStaticBoxSizer1 = new wxStaticBoxSizer(WxStaticBoxSizer1_StaticBoxObj,wxHORIZONTAL);
	WxFlexGridSizer1->Add(WxStaticBoxSizer1,1,wxGROW | wxALIGN_CENTER_VERTICAL | wxALL,1);
	wxGridSizer* WxGridSizer1 = new wxGridSizer(2,2,0,0);
	WxStaticBoxSizer1->Add(WxGridSizer1,1,wxALIGN_CENTER_VERTICAL | wxALL,1);
	WxLogURLCheckBox = new wxCheckBox(this, ID_WXLOGURLCHECKBOX, wxT("Log queries"));
	WxLogURLCheckBox->SetToolTip(wxT("If enabled, all queries are logged.\nOnly useful for the technically interested.\nNote: Enabling this costs some processing time."));
	WxGridSizer1->Add(WxLogURLCheckBox,0,wxGROW | wxALIGN_CENTER_VERTICAL | wxALL,5);

	WxRadioButton1=new wxRadioButton(this,ID_WXFULLQUERYRADIO,_T("Full query string"),wxDefaultPosition,wxDefaultSize,wxRB_GROUP);
	WxRadioButton1->SetToolTip(_T("Log queries verbatim."));
	WxGridSizer1->Add(WxRadioButton1,0,wxGROW | wxALIGN_CENTER_VERTICAL | wxALL,5);
	// Invisible dummy to get alignment right.
	// Is there a better method to do this?
	wxRadioButton *WxRadioButton0=new wxRadioButton(this,wxID_ANY,_T("Dummy"));
	WxRadioButton0->Hide();
	WxGridSizer1->Add(WxRadioButton0,0,wxGROW | wxALIGN_CENTER_VERTICAL | wxALL,5);
	WxRadioButton2=new wxRadioButton(this,ID_WXBRIEFQUERYRADIO,_T("Summary only"));
	WxRadioButton2->SetToolTip(_T("Log only host, URL, user agent, and referrer."));
	WxGridSizer1->Add(WxRadioButton2,0,wxGROW | wxALIGN_CENTER_VERTICAL | wxALL,5);

	wxStaticBox* WxStaticBoxSizer5_StaticBoxObj = new wxStaticBox(this, wxID_ANY, wxT("Statistics:"));
	wxStaticBoxSizer* WxStaticBoxSizer5 = new wxStaticBoxSizer(WxStaticBoxSizer5_StaticBoxObj,wxHORIZONTAL);
	WxFlexGridSizer1->Add(WxStaticBoxSizer5,1,wxGROW | wxALIGN_CENTER_VERTICAL | wxALL,1);
	wxGridSizer* WxGridSizer3 = new wxGridSizer(2,2,0,0);
	WxStaticBoxSizer5->Add(WxGridSizer3,1,wxGROW | wxALIGN_CENTER_VERTICAL | wxALL,1);
	WxStatisticsCheckBox = new wxCheckBox(this, ID_WXSTATISTICSCHECKBOX, wxT("Enable statistics"));
	WxStatisticsCheckBox->SetToolTip(_T("If enabled, nohttpd stores some information about\nthe queries it receives for blocked hosts,\ne.g. the number of times a specific host is blocked.\nNote: Disable to save some memory and processing time."));
	WxGridSizer3->Add(WxStatisticsCheckBox,1,wxGROW | wxALIGN_CENTER_VERTICAL | wxALL,5);
	// Invisible dummy to get alignment right.
	// Is there a better method to do this?
	wxCheckBox *wxCheckBox0=new wxCheckBox(this,wxID_ANY,_T("Dummy"));
	wxCheckBox0->Hide();
	WxGridSizer3->Add(wxCheckBox0,1,wxGROW | wxALIGN_CENTER_VERTICAL | wxALL,1);
	wxStaticText *wxStaticText1=new wxStaticText(this,wxID_ANY,wxT("Max history length:"));
	WxGridSizer3->Add(wxStaticText1,1,wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL | wxALL,5);
	WxHistLenEdit = new wxTextCtrl(this, ID_WXHISTLENEDIT, wxT("WxHistLenEdit"), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
	WxHistLenEdit->SetToolTip(_T("Max number of single queries regarding one host to store.\nLarger numbers result in higher memory usage!\nNote: Changed values take effect for each host when that specfic host is blocked again.\n\nPress Return to set new value."));
	WxGridSizer3->Add(WxHistLenEdit,1,wxGROW | wxALIGN_CENTER_VERTICAL | wxALL,5);

	wxButton *WxOkButton=new wxButton(this,wxID_OK);
	WxOkButton->SetDefault();
	WxFlexGridSizer1->Add(WxOkButton,1,wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxALL,1);

	GetSizer()->Fit(this);
	GetSizer()->SetSizeHints(this);
	this->SetIcon(*Nohttpd2Dlg::s_mainicon);

	// Initialize checkboxes with saved values
 	wxConfigBase::Get()->Read(_T("notrayhide"),&dummyboolean);
	WxNoTrayHideCheckBox->SetValue(dummyboolean);
	wxConfigBase::Get()->Read(_T("closetotray"),&dummyboolean);
	WxCloseTrayCheckBox->SetValue(dummyboolean);
	wxConfigBase::Get()->Read(_T("savehits"),&dummyboolean);
	WxSaveHitsCheckBox->SetValue(dummyboolean);
	wxConfigBase::Get()->Read(_T("statistics"),&dummyboolean);
	WxStatisticsCheckBox->SetValue(dummyboolean);
	// If "Statistics" is disabled:
	if (!dummyboolean){
		WxHistLenEdit->Disable();
	}

	wxConfigBase::Get()->Read(_T("logqueries"),&dummyboolean);
	WxLogURLCheckBox->SetValue(dummyboolean);
	// If "Log queries" is disabled:
	if (!dummyboolean){
		WxRadioButton1->Disable();
		WxRadioButton2->Disable();
	}

	wxConfigBase::Get()->Read(_T("verboselog"),&dummyboolean);
	WxVerboseLogCheckBox->SetValue(dummyboolean);

	wxConfigBase::Get()->Read(_T("fullqueries"),&dummyboolean);
	WxRadioButton1->SetValue(dummyboolean);
	WxRadioButton2->SetValue(!dummyboolean);

	if (WxNoTrayHideCheckBox->GetValue()){
		WxCloseTrayCheckBox->Disable();
	}

	/**
	If adding the tray icon failed, disable all tray options for this session.
	*/
	if (!Nohttpd2Dlg::s_trayavailable){
		WxNoTrayHideCheckBox->SetValue(true);
		WxNoTrayHideCheckBox->Disable();
		WxCloseTrayCheckBox->Disable();
	}

	WxHistLenEdit->SetValue(wxString::Format(_T("%lu"),StatItem::s_histlength));
}

OptionsDlg::~OptionsDlg(){
}

void OptionsDlg::WxNoTrayHideCheckBoxClick(wxCommandEvent &WXUNUSED(event)){

	wxConfigBase::Get()->Write(_T("notrayhide"),WxNoTrayHideCheckBox->GetValue());
	wxConfigBase::Get()->Flush();

	if (WxNoTrayHideCheckBox->GetValue()){
		WxCloseTrayCheckBox->Disable();
		wxCommandEvent event(wxEVT_REMTRAY);
		::wxPostEvent(m_mdlg,event);
	} else {
		WxCloseTrayCheckBox->Enable();
		wxCommandEvent event(wxEVT_ADDTRAY);
		::wxPostEvent(m_mdlg,event);
	}
}

void OptionsDlg::WxSaveHitsCheckBoxClick(wxCommandEvent &WXUNUSED(event)){

	wxConfigBase::Get()->Write(_T("savehits"),WxSaveHitsCheckBox->GetValue());
	wxConfigBase::Get()->Flush();
}

void OptionsDlg::WxCloseTrayCheckBoxClick(wxCommandEvent &WXUNUSED(event)){

	wxConfigBase::Get()->Write(_T("closetotray"),WxCloseTrayCheckBox->GetValue());
	wxConfigBase::Get()->Flush();
}

void OptionsDlg::WxLogURLCheckBoxClick(wxCommandEvent &WXUNUSED(event)){

	wxConfigBase::Get()->Write(_T("logqueries"),WxLogURLCheckBox->GetValue());
	wxConfigBase::Get()->Flush();

	if (WxLogURLCheckBox->GetValue()){
		WxRadioButton1->Enable();
		WxRadioButton2->Enable();
	} else {
		WxRadioButton1->Disable();
		WxRadioButton2->Disable();
	}

	ReceiverThread::s_logqueries=WxLogURLCheckBox->GetValue();
}

void OptionsDlg::WxVerboseLogCheckBoxClick(wxCommandEvent &WXUNUSED(event)){

	wxConfigBase::Get()->Write(_T("verboselog"),WxVerboseLogCheckBox->GetValue());
	wxConfigBase::Get()->Flush();
	wxLog::SetVerbose(WxVerboseLogCheckBox->GetValue());
}

void OptionsDlg::WxStatisticsCheckBoxClick(wxCommandEvent &WXUNUSED(event)){

	wxConfigBase::Get()->Write(_T("statistics"),WxStatisticsCheckBox->GetValue());
	wxConfigBase::Get()->Flush();
	if (WxStatisticsCheckBox->GetValue()){
		WxHistLenEdit->Enable();
	} else {
		WxHistLenEdit->Disable();
	}
	ReceiverThread::s_statistics=WxStatisticsCheckBox->GetValue();
}

void OptionsDlg::WxQueryRadioClick(wxCommandEvent &WXUNUSED(event)){

	wxConfigBase::Get()->Write(_T("fullqueries"),WxRadioButton1->GetValue());
	wxConfigBase::Get()->Flush();
	ReceiverThread::s_fullqueries=WxRadioButton1->GetValue();
}

void OptionsDlg::WxHistLenEditEnter(wxCommandEvent& WXUNUSED(event)){

	long histlen;

	WxHistLenEdit->GetValue().ToLong(&histlen);
	if (histlen<0){
		wxMessageDialog(NULL,_T("Max history length must be larger than or equal to zero."),_T("nohttpd:  Error"),wxOK|wxICON_ERROR).ShowModal();
		return;
	}
	wxConfigBase::Get()->Write(_T("historylen"),histlen);
	wxConfigBase::Get()->Flush();
	StatItem::s_histlength=histlen;
}
