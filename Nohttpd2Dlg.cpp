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
#include "ReceiverThread.h"
#include "Nohttpd2Dlg.h"
#include "Nohttpd2App.h"
#include "AboutDlg.h"
#include "OptionsDlg.h"
#include "StatisticsDlg.h"
#include <wx/config.h>
#include <wx/datetime.h>
#include <wx/mimetype.h>
#include <wx/filename.h>

#ifdef __WIN95__
	#include <wx/msw/registry.h>
#endif

#ifndef __WIN95__
	#include "res/nh2-0.xpm"	// 32x32 pixel sized icon.
	#include "res/nh2-2.xpm"	// 16x16 pixel sized icon.
#endif

// Declaration of static members.
wxIcon *Nohttpd2Dlg::s_mainicon;
wxIcon *Nohttpd2Dlg::s_mainicon3232;
bool Nohttpd2Dlg::s_trayavailable;

// Declaration of Nohttpd2App for ::wxGetApp()
DECLARE_APP(Nohttpd2App)

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

DEFINE_EVENT_TYPE(wxEVT_LOGMSG)
DEFINE_EVENT_TYPE(wxEVT_SHUTDOWN)
DEFINE_EVENT_TYPE(wxEVT_SHOWMAIN)
DEFINE_EVENT_TYPE(wxEVT_SETHITS)
DEFINE_EVENT_TYPE(wxEVT_LOGVERBOSE)
DEFINE_EVENT_TYPE(wxEVT_HIDEDLG)
DEFINE_EVENT_TYPE(wxEVT_ADDTRAY)
DEFINE_EVENT_TYPE(wxEVT_REMTRAY)

#define EVT_LOGMSG(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        wxEVT_LOGMSG, id, wxID_ANY, \
        (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxCommandEventFunction, &fn ), \
        (wxObject *) NULL \
    ),

#define EVT_SHUTDOWN(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        wxEVT_SHUTDOWN, id, wxID_ANY, \
        (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxCommandEventFunction, &fn ), \
        (wxObject *) NULL \
    ),

#define EVT_SHOWMAIN(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        wxEVT_SHOWMAIN, id, wxID_ANY, \
        (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxCommandEventFunction, &fn ), \
        (wxObject *) NULL \
    ),

#define EVT_SETHITS(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        wxEVT_SETHITS, id, wxID_ANY, \
        (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxCommandEventFunction, &fn ), \
        (wxObject *) NULL \
    ),

#define EVT_LOGVERBOSE(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        wxEVT_LOGVERBOSE, id, wxID_ANY, \
        (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxCommandEventFunction, &fn ), \
        (wxObject *) NULL \
    ),

#define EVT_HIDEDLG(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        wxEVT_HIDEDLG, id, wxID_ANY, \
        (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxCommandEventFunction, &fn ), \
        (wxObject *) NULL \
    ),

#define EVT_ADDTRAY(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        wxEVT_ADDTRAY, id, wxID_ANY, \
        (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxCommandEventFunction, &fn ), \
        (wxObject *) NULL \
    ),

#define EVT_REMTRAY(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        wxEVT_REMTRAY, id, wxID_ANY, \
        (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxCommandEventFunction, &fn ), \
        (wxObject *) NULL \
    ),

BEGIN_EVENT_TABLE(Nohttpd2Dlg,wxFrame)
	EVT_LOGMSG(wxID_ANY,Nohttpd2Dlg::onLogMsgEvt)
	EVT_SHUTDOWN(wxID_ANY,Nohttpd2Dlg::onShutdownEvt)
	EVT_SHOWMAIN(wxID_ANY,Nohttpd2Dlg::onShowMainEvt)
	EVT_ICONIZE(Nohttpd2Dlg::onIconizeEvt)
	EVT_SETHITS(wxID_ANY,Nohttpd2Dlg::onSetHitsEvt)
	EVT_LOGVERBOSE(wxID_ANY,Nohttpd2Dlg::onLogVerboseMsgEvt)
	EVT_HIDEDLG(wxID_ANY,Nohttpd2Dlg::onHideDlgMsgEvt)
	EVT_ADDTRAY(wxID_ANY,Nohttpd2Dlg::onAddTrayEvt)
	EVT_REMTRAY(wxID_ANY,Nohttpd2Dlg::onRemTrayEvt)

	EVT_CLOSE(Nohttpd2Dlg::Nohttpd2DlgClose)
	EVT_BUTTON(ID_WXSTATBUTTON,Nohttpd2Dlg::WxStatButtonClick)
	EVT_MENU(ID_MNU_EXIT_1002,Nohttpd2Dlg::WxExitButtonClick)
	EVT_MENU(ID_MNU_ABOUT_1007,Nohttpd2Dlg::WxAboutButtonClick)
	EVT_MENU(ID_MNU_OPTIONS_1004,Nohttpd2Dlg::WxOptionsButtonClick)
	EVT_MENU(ID_MNU_CLEARACCESSCOUNT_ALL__1009,Nohttpd2Dlg::WxCleanAllButtonClick)
	EVT_MENU(ID_MNU_CLEARLOG_1010,Nohttpd2Dlg::WxClearLogButtonClick)
	EVT_MENU(ID_MNU_MENUITEM7_1008,Nohttpd2Dlg::WxClearSessionButtonClick)
	EVT_MENU(ID_MNU_EXTLOG,Nohttpd2Dlg::WxLogExtViewClick)
END_EVENT_TABLE()

// Taskbar code begin
BEGIN_EVENT_TABLE(Nohttpd2TaskBarIcon,wxTaskBarIcon)
    EVT_TASKBAR_LEFT_DCLICK(Nohttpd2TaskBarIcon::onDClick)
END_EVENT_TABLE()

Nohttpd2TaskBarIcon::Nohttpd2TaskBarIcon(Nohttpd2Dlg *dlg){

	m_mdlg=dlg;
}

void Nohttpd2TaskBarIcon::onDClick(wxTaskBarIconEvent &WXUNUSED(event)){

	LOGVERBOSE(_T("Taskbar: Double click."));
	wxCommandEvent showevent(wxEVT_SHOWMAIN);
	::wxPostEvent(m_mdlg,showevent);
}
// Taskbar code end

// Implementation of class Nohttpd2Dlg follows
Nohttpd2Dlg::Nohttpd2Dlg( wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &position, const wxSize& size, long style )
    : wxFrame( parent, id, title, position, size, style){

	bool dummyboolean;

	CreateGUIControlsCustom();

	// Import settings of nohttpd 1.x directly from the registry.
	#ifdef 	__WIN95__
	wxRegKey *oldreg;
	long oldhits;
	wxString oldusetray,oldclosetotray,oldglobalcount;

	wxConfigBase::Get()->Read(_T("importedconf"),&dummyboolean);
	if (!dummyboolean){
		wxLogNull nolog;	// Suppress errors if keys could not be read.
		oldreg=new wxRegKey(_T("HKEY_LOCAL_MACHINE\\Software\\nohttpd"));
		if (oldreg->Exists() /*&& oldreg->IsOpened()*/){
			if (oldreg->QueryValue(_T("usetray"),oldusetray) &&
				oldreg->QueryValue(_T("closetotray"),oldclosetotray) &&
				oldreg->QueryValue(_T("globalcount"),oldglobalcount) &&
				oldreg->QueryValue(_T("cnt"),&oldhits)){

				/**
				 If all old settings were read successfully, delete old regkey
				 and save values to new settings.
				 Note: The config may only imported once because if the current user
				 has rights to read but not to delete keys in HKLM, these settings
				 would be imported at every start of nohttpd2.
				*/
				oldreg->DeleteSelf();
				wxConfigBase::Get()->Write(_T("notrayhide"),oldusetray==_T("1"));
				wxConfigBase::Get()->Write(_T("closetotray"),oldclosetotray==_T("1"));
				wxConfigBase::Get()->Write(_T("savehits"),oldglobalcount==_T("1"));
				wxConfigBase::Get()->Write(_T("hits"),oldhits);

				wxMessageDialog(NULL,_T("Settings from previous 1.x version have been imported and deleted."),_T("nohttpd: Notification"),wxOK).ShowModal();
			}
		}
		wxConfigBase::Get()->Write(_T("importedconf"),true);
		wxConfigBase::Get()->Flush();
		delete oldreg;
	}
	#endif

	wxConfigBase::Get()->Read(_T("verboselog"),&dummyboolean);
	wxLog::SetVerbose(dummyboolean);

 	m_inited=false;
 	m_receiverrunning=false;
 	m_shutdown=false;
 	s_mainicon=NULL;
 	m_taskbar=NULL;

	wxConfigBase::Get()->Read(_T("historylen"),&StatItem::s_histlength);

	wxConfigBase::Get()->Read(_T("hits"),&m_allhitsbase);
 	m_allhits=m_allhitsbase;

	// Initialize main icon
	#ifdef 	__WIN95__
	s_mainicon=new wxIcon(_T("AAANOHTTPDMAINICON"),wxBITMAP_TYPE_ICO_RESOURCE);
	s_mainicon3232=s_mainicon;
	#else
	s_mainicon=new wxIcon(wxICON(nh2_1616));
	s_mainicon3232=new wxIcon(wxICON(nh2_3232));
	#endif

	this->SetIcon(*s_mainicon);

	// set active log target
	m_ltc=new wxLogTextCtrl(this->WxLogMemo);
	wxLog::SetActiveTarget(m_ltc);
}

Nohttpd2Dlg::~Nohttpd2Dlg(){

	// Save current value of "Hits in all sessions".
	// See onSetHitsEvt() for details why this is necessary.
	wxConfigBase::Get()->Write(_T("hits"),m_allhits);
	wxConfigBase::Get()->Flush();

	removeTray();

	if (s_mainicon){
		delete s_mainicon;
	}

	// Set default log target.
	wxLog::SetActiveTarget(NULL);
	delete m_ltc;

}

bool Nohttpd2Dlg::startReceiver(){

	m_recvthread=new ReceiverThread(this);
	if (m_recvthread->initNetwork()){
		if (m_recvthread->Create()!=wxTHREAD_NO_ERROR){
			LOGMESSAGE(_T("Error: Unable to create receiver thread."));
			return false;
		}
		if (m_recvthread->Run()!=wxTHREAD_NO_ERROR){
			LOGMESSAGE(_T("Error: Unable to run receiver thread."));
			return false;
		}
		m_receiverrunning=true;
		LOGMESSAGE(_T("Receiver thread started."));
		LOGMESSAGE(_T("Nohttpd is running."));
		return true;
	}
	return false;
}

void Nohttpd2Dlg::CreateGUIControlsCustom(void){

	// Just in case you are wondering: Yes, this code ist mostly machine generated (by wx-DevCpp).

	wxMenuBar *WxMenuBar1 =  new wxMenuBar();
	wxMenu *ID_MNU_FILE_1001_Mnu_Obj = new wxMenu(0);
	WxMenuBar1->Append(ID_MNU_FILE_1001_Mnu_Obj, wxT("File"));
	ID_MNU_FILE_1001_Mnu_Obj->Append(ID_MNU_EXTLOG, wxT("View log in external viewer"), wxT(""), wxITEM_NORMAL);
	ID_MNU_FILE_1001_Mnu_Obj->AppendSeparator();
	ID_MNU_FILE_1001_Mnu_Obj->Append(ID_MNU_EXIT_1002, wxT("Exit"), wxT(""), wxITEM_NORMAL);
	wxMenu *ID_MNU_OPTIONS_1003_Mnu_Obj = new wxMenu(0);
	WxMenuBar1->Append(ID_MNU_OPTIONS_1003_Mnu_Obj, wxT("Settings"));
	ID_MNU_OPTIONS_1003_Mnu_Obj->Append(ID_MNU_MENUITEM7_1008, wxT("Reset access count (session)"), wxT(""), wxITEM_NORMAL);
	ID_MNU_OPTIONS_1003_Mnu_Obj->Append(ID_MNU_CLEARACCESSCOUNT_ALL__1009, wxT("Reset access count (all)"), wxT(""), wxITEM_NORMAL);
	ID_MNU_OPTIONS_1003_Mnu_Obj->Append(ID_MNU_CLEARLOG_1010, wxT("Clear log"), wxT(""), wxITEM_NORMAL);
	ID_MNU_OPTIONS_1003_Mnu_Obj->AppendSeparator();
	ID_MNU_OPTIONS_1003_Mnu_Obj->Append(ID_MNU_OPTIONS_1004, wxT("Options"), wxT(""), wxITEM_NORMAL);
	wxMenu *ID_MNU_HELP_1005_Mnu_Obj = new wxMenu(0);
	WxMenuBar1->Append(ID_MNU_HELP_1005_Mnu_Obj, wxT("Help"));
	ID_MNU_HELP_1005_Mnu_Obj->Append(ID_MNU_ABOUT_1007, wxT("About"), wxT(""), wxITEM_NORMAL);
	this->SetMenuBar(WxMenuBar1);

	wxBoxSizer* WxBoxSizer0 = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(WxBoxSizer0);
	this->SetAutoLayout(true);

	WxPanel1=new wxPanel(this,ID_WXPANEL1);

	wxFlexGridSizer* WxFlexGridSizer1 = new wxFlexGridSizer(2,1,0,0);

	WxFlexGridSizer1->AddGrowableCol(0);
	WxFlexGridSizer1->AddGrowableRow(1);

	WxPanel1->SetSizer(WxFlexGridSizer1);
	WxBoxSizer0->Add(WxPanel1,1,wxEXPAND|wxALL,0);

	wxFlexGridSizer* WxFlexGridSizer2 = new wxFlexGridSizer(1,2,0,0);
	WxFlexGridSizer1->Add(WxFlexGridSizer2,1,wxGROW|wxALL,1);

	WxFlexGridSizer2->AddGrowableCol(1);

	wxStaticBox* WxStaticBoxSizer1_StaticBoxObj = new wxStaticBox(WxPanel1, wxID_ANY, wxT("Blocked Accesses:"));
	wxStaticBoxSizer* WxStaticBoxSizer1 = new wxStaticBoxSizer(WxStaticBoxSizer1_StaticBoxObj,wxHORIZONTAL);
	WxFlexGridSizer2->Add(WxStaticBoxSizer1,1,wxGROW | wxALL,1);

	wxGridSizer* WxGridSizer1 = new wxGridSizer(2,2,0,0);
	WxStaticBoxSizer1->Add(WxGridSizer1,1,wxGROW | wxALL,1);

	WxStaticText1 = new wxStaticText(WxPanel1, ID_WXSTATICTEXT1, wxT("In this session:"));
	WxGridSizer1->Add(WxStaticText1,1,wxGROW | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL | wxALL,5);

	WxSessionHitsStaticText = new wxStaticText(WxPanel1, ID_WXSESSIONHITSSTATICTEXT, wxT("n/a"));
	WxGridSizer1->Add(WxSessionHitsStaticText,1,wxGROW | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL | wxALL,5);

	WxStaticText3 = new wxStaticText(WxPanel1, ID_WXSTATICTEXT3, wxT("In all sessions:"));
 	WxGridSizer1->Add(WxStaticText3,1,wxGROW | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL | wxALL,5);

	WxAllHitsStaticText = new wxStaticText(WxPanel1, ID_WXALLHITSSTATICTEXT, wxT("n/a"));
	WxGridSizer1->Add(WxAllHitsStaticText,1,wxGROW | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL | wxALL,5);

	WxStatButton=new wxButton(WxPanel1,ID_WXSTATBUTTON,_T("Statistics"));
	WxFlexGridSizer2->Add(WxStatButton,1,wxALIGN_CENTER_HORIZONTAL |wxALIGN_CENTER_VERTICAL | wxALL,5);
	WxStatButton->SetDefault();

	wxStaticBox* WxStaticBoxSizer4_StaticBoxObj = new wxStaticBox(WxPanel1, wxID_ANY, wxT("Log:"));
	wxStaticBoxSizer* WxStaticBoxSizer4 = new wxStaticBoxSizer(WxStaticBoxSizer4_StaticBoxObj,wxHORIZONTAL);
	WxFlexGridSizer1->Add(WxStaticBoxSizer4,1,wxGROW | wxALIGN_CENTER_VERTICAL | wxALL,1);

	WxLogMemo = new wxTextCtrl(WxPanel1, ID_WXLOGMEMO, wxT(""), wxDefaultPosition, wxSize(300,100), wxVSCROLL | wxHSCROLL | wxTE_READONLY | wxTE_MULTILINE);
	WxLogMemo->SetMaxLength(0);
	WxLogMemo->SetBackgroundColour(*wxLIGHT_GREY);
	WxStaticBoxSizer4->Add(WxLogMemo,1,wxGROW | wxALL,1);

	GetSizer()->SetSizeHints(this);

	this->SetTitle(wxT("nohttpd v2.1.0"));
	this->Center();
}

void Nohttpd2Dlg::initiateClose(){

	LOGVERBOSE(_T("initiateClose() called."));

	if (!m_receiverrunning){			// No receiver thread running,
		this->Destroy();			// can close dialog right away.
	} else {					// Receiver thread is running,
		if (!m_recvthread->m_closing){			// and not already closing:
			m_recvthread->m_closing=true;		// Set close mode.
			m_recvthread->killNetwork();	// Close socket and thread.

			// The receiver thread will now post a real shutdown event prior to
			// exiting itself.
		}
	}
}

bool Nohttpd2Dlg::addTray(){

	if (m_taskbar!=NULL){
		LOGMESSAGE(_T("Internal warning: Taskbar already initalized in addTray()."));
		return true;
	}

	m_taskbar=new Nohttpd2TaskBarIcon(this);
	m_taskbar->SetIcon(*s_mainicon3232,_T("Double-click to show nohttpd."));

	if (!m_taskbar->IsOk() || !m_taskbar->IsIconInstalled()){
		LOGMESSAGE(_T("Warning: Taskbar icon not installed successfully.\nDisabling tray options."));
		return false;
	}
	return true;
}

void Nohttpd2Dlg::removeTray(){

	if (m_taskbar!=NULL){
		delete m_taskbar;
		m_taskbar=NULL;
	} else {
		LOGMESSAGE(_T("Internal warning: Taskbar already deleted in removeTray()."));
	}
}

void Nohttpd2Dlg::hideDialog(){

	LOGVERBOSE(_T("hideDialog() called."));
	if (!this->IsIconized()){
		this->Iconize(true);
	}
}

void Nohttpd2Dlg::Nohttpd2DlgClose(wxCloseEvent& event){

	bool dummyboolean,dummyboolean2;

	if (!m_shutdown && event.CanVeto()){

		event.Veto();

		wxConfigBase::Get()->Read(_T("closetotray"),&dummyboolean);
		wxConfigBase::Get()->Read(_T("notrayhide"),&dummyboolean2);
		if (dummyboolean && s_trayavailable && !dummyboolean2){
			hideDialog();
		} else {
			initiateClose();
		}
	} else {
		this->Destroy();
	}
}

void Nohttpd2Dlg::onIconizeEvt(wxIconizeEvent &event){

	bool dummyboolean;

	LOGVERBOSE(_T("Iconize Event caught."));

	event.Skip();

	wxConfigBase::Get()->Read(_T("notrayhide"),&dummyboolean);
	if (event.Iconized() && !dummyboolean && s_trayavailable){
		LOGVERBOSE(_T("Hiding dialog."));
	 	this->Show(false);
	}
}

void Nohttpd2Dlg::onShowMainEvt(wxCommandEvent &WXUNUSED(event)){

	LOGVERBOSE(_T("Show Main Event caught."));

	this->SetFocus();
	this->Raise();
	this->Show(true);
	#ifdef __WIN95__
	this->Maximize(false);	// Windows doesn't bring the window back
				// to the front by default. Glitch in wxWidgets?
				// Feature of Windows? I don't know.
	#endif
}

void Nohttpd2Dlg::onSetHitsEvt(wxCommandEvent &WXUNUSED(event)){

	long sessionhits;
	bool dummyboolean;

	sessionhits=ReceiverThread::getSessionHits();

	WxSessionHitsStaticText->SetLabel(wxString::Format(_T("%lu"),sessionhits));

	// Current "Hits in all sessions" is session hits plus "Hits in all sessions"
	// at startup.
	m_allhits=sessionhits+m_allhitsbase;
	WxAllHitsStaticText->SetLabel(wxString::Format(_T("%lu"),m_allhits));

	wxConfigBase::Get()->Read(_T("savehits"),&dummyboolean);
	if (dummyboolean){
		wxConfigBase::Get()->Write(_T("hits"),m_allhits);
		wxConfigBase::Get()->Flush();
	}

/*
	Note: Caching was disabled because the log off/shutdown events somehow don't get
		 caught in Linux. The unsaved hits will then be lost.
		 I could implement a terminate signal handler under Linux, but want to stay
		 away from platform-dependent code as much as possible.

	// Save all hits to configuration at an interval of modulo 20 hits.
	// This interval saves us from often calling the expensive config saving.
	// m_allhits is also saved at program exit.
	if (m_allhits%20==0){
		wxConfigBase::Get()->Write(_T("hits"),m_allhits);
		wxConfigBase::Get()->Flush();
	}
*/
}

void Nohttpd2Dlg::onLogMsgEvt(wxCommandEvent &event){

	// Workaround for a bug in wxWidgets<=2.6.3:
	const wxDateTime &now=wxDateTime::Now();
	this->WxLogMemo->AppendText(now.Format(_T("%x %X: "))+event.GetString()+_T("\n"));
//	this->WxLogMemo->AppendText(wxDateTime::Now().Format(_T("%x %X: "))+event.GetString()+_T("\n"));
}

void Nohttpd2Dlg::onLogVerboseMsgEvt(wxCommandEvent &event){

	if (wxLog::GetVerbose()){
		onLogMsgEvt(event);
	}
}

void Nohttpd2Dlg::onShutdownEvt(wxCommandEvent &WXUNUSED(event)){

	this->Destroy();
}

void Nohttpd2Dlg::onHideDlgMsgEvt(wxCommandEvent &WXUNUSED(event)){

	hideDialog();
}

void Nohttpd2Dlg::WxAboutButtonClick(wxCommandEvent &WXUNUSED(event)){

	AboutDlg * aboutdlg;

	aboutdlg=new AboutDlg(NULL);
	aboutdlg->ShowModal();
	aboutdlg->Destroy();
}

void Nohttpd2Dlg::WxExitButtonClick(wxCommandEvent &WXUNUSED(event)){

	initiateClose();
}

void Nohttpd2Dlg::WxClearSessionButtonClick(wxCommandEvent &WXUNUSED(event)){

	ReceiverThread::setSessionHits(0);
	wxCommandEvent tmpevent(wxEVT_SETHITS);
	::wxPostEvent(this,tmpevent);
}

void Nohttpd2Dlg::WxCleanAllButtonClick(wxCommandEvent &WXUNUSED(event)){

	m_allhitsbase=0;
	ReceiverThread::setSessionHits(0);
	wxCommandEvent tmpevent(wxEVT_SETHITS);
	::wxPostEvent(this,tmpevent);
}

void Nohttpd2Dlg::Nohttpd2DlgInitDialog(){

	bool dummyboolean;

	if (!m_inited){
		m_inited=true;
		wxConfigBase::Get()->Read(_T("notrayhide"),&dummyboolean);
		if (!dummyboolean){
			/**
			If adding the tray icon failed, disable all tray options for this session.
			*/
			if (!addTray()){
				s_trayavailable=false;
			} else {
				s_trayavailable=true;
			}
		}
		startReceiver();
	}
}

void Nohttpd2Dlg::WxClearLogButtonClick(wxCommandEvent &WXUNUSED(event)){

	WxLogMemo->Clear();
	LOGMESSAGE(_T("Log cleared."));
}

void Nohttpd2Dlg::onAddTrayEvt(wxCommandEvent &WXUNUSED(event)){

	/**
	If adding the tray icon failed, disable all tray options for this session.
	*/
	if (!addTray()){
		s_trayavailable=false;
	} else {
		s_trayavailable=true;
	}
}

void Nohttpd2Dlg::onRemTrayEvt(wxCommandEvent &WXUNUSED(event)){

	removeTray();
}

void Nohttpd2Dlg::WxOptionsButtonClick(wxCommandEvent &WXUNUSED(event)){

	OptionsDlg *odlg=new OptionsDlg(NULL);
	odlg->m_mdlg=this;
	odlg->ShowModal();
	odlg->Destroy();
}

void Nohttpd2Dlg::WxStatButtonClick(wxCommandEvent &WXUNUSED(event)){

	bool dummyboolean;

	// No more stats if shutting down
	if (m_recvthread->m_closing){
		return;
	}

	wxConfigBase::Get()->Read(_T("statistics"),&dummyboolean);
	if (!dummyboolean){
		wxMessageDialog(NULL,_T("Statistics are disabled in the options, so you'll see\n either an empty or a non-updated list."),_T("nohttpd: Information"),wxOK|wxICON_INFORMATION).ShowModal();
	}

	StatisticsDlg *sdlg=new StatisticsDlg(NULL);
	sdlg->ShowModal();
	sdlg->Destroy();
}

void Nohttpd2Dlg::WxLogExtViewClick(wxCommandEvent &WXUNUSED(event)){

	wxFileType *ftype;
	wxString command,logfname;

	ftype=wxTheMimeTypesManager->GetFileTypeFromExtension(_T(".txt"));
	if (!ftype){
		LOGMESSAGE(_T("Warning: No external editor associated with .txt files."));
		delete ftype;
		return;
	}
	logfname=wxFileName::CreateTempFileName(_T("nohttpd-tmp-log-"));
	if (logfname.IsEmpty()){
		LOGMESSAGE(_T("Warning: Unable to create a temporary file for storing the log text."));
		delete ftype;
		return;
	}
	if (!WxLogMemo->SaveFile(logfname)){
		LOGMESSAGE(_T("Warning: Unable to save log contents to temp file. File name:"));
		LOGMESSAGE(logfname);
		delete ftype;
		return;
	}
	command=ftype->GetOpenCommand(logfname);
	wxExecute(command,wxEXEC_SYNC|wxEXEC_NODISABLE);
	delete ftype;
	LOGMESSAGE(_T("Log saved to ")+logfname+_T("; please delete this file manually."));
}
