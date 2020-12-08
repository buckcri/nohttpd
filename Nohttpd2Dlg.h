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

#ifndef __NOHTTPD2DLG_HPP_
#define __NOHTTPD2DLG_HPP_

#include <wx/wxprec.h>
#ifdef __BORLANDC__
        #pragma hdrstop
#endif
#ifndef WX_PRECOMP
        #include <wx/wx.h>
#endif

#include <wx/log.h>
#include <wx/taskbar.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include <wx/dialog.h>

//Compatibility for 2.4.x code
#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif
#ifndef wxFIXED_MINSIZE
#define wxFIXED_MINSIZE 0
#endif

#undef Nohttpd2Dlg_STYLE
//#define Nohttpd2Dlg_STYLE wxCAPTION | wxRESIZE_BORDER | wxSYSTEM_MENU | wxDIALOG_NO_PARENT | wxMINIMIZE_BOX |wxCLOSE_BOX
#define Nohttpd2Dlg_STYLE wxDEFAULT_FRAME_STYLE

class Nohttpd2TaskBarIcon:public wxTaskBarIcon{

public:
	Nohttpd2TaskBarIcon(class Nohttpd2Dlg *dlg);
	void onDClick(wxTaskBarIconEvent &WXUNUSED(event));

private:
	class Nohttpd2Dlg *m_mdlg;
	DECLARE_EVENT_TABLE()
};

class Nohttpd2Dlg:public wxFrame{

public:
    Nohttpd2Dlg( wxWindow *parent, wxWindowID id = 1, const wxString &title = wxT("nohttpd v2.0.0"),
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = Nohttpd2Dlg_STYLE);
    virtual ~Nohttpd2Dlg();

	wxTextCtrl *WxLogMemo;
	wxButton *WxStatButton;
	wxStaticText *WxAllHitsStaticText;
	wxStaticText *WxStaticText3;
	wxStaticText *WxSessionHitsStaticText;
	wxStaticText *WxStaticText1;
	wxPanel *WxPanel1;

	enum {
		ID_WXLOGMEMO=1100,
		ID_WXCLEARLOGBUTTON,
		ID_WXALLHITSSTATICTEXT,
		ID_WXSTATICTEXT3,
		ID_WXSESSIONHITSSTATICTEXT,
		ID_WXSTATICTEXT1,
		ID_WXOPTIONSBUTTON,
		ID_WXSTATBUTTON,
		ID_WXPANEL1,
		ID_MNU_FILE_1001,
		ID_MNU_EXIT_1002,
		ID_MNU_OPTIONS_1003,
		ID_MNU_OPTIONS_1004,
		ID_MNU_HELP_1005,
		ID_MNU_ABOUT_1007,
		ID_MNU_MENUITEM7_1008,
		ID_MNU_CLEARACCESSCOUNT_ALL__1009,
		ID_MNU_CLEARLOG_1010,
		ID_MNU_EXTLOG
	};

	/** This is a pointer to the .ico resource on Windows and a pointer to the 16x16 xpm otherwise.
	    Used for window icons.
	*/
	static wxIcon *s_mainicon;
	/** This is equal to s_mainicon on Windows and a pointer to the 32x32 xpm otherwise.
	    Used for the traybar icon.
	*/
	static wxIcon *s_mainicon3232;

	bool m_shutdown;		// true if nohttpd is about to shut down.
	class ReceiverThread *m_recvthread;
	long m_allhits;			// Number of hits in all sessions at runtime;

	static bool s_trayavailable;	// false if tray could not be installed.

	void Nohttpd2DlgClose(wxCloseEvent& event);
	void CreateGUIControlsCustom(void);
	void WxAboutButtonClick(wxCommandEvent &WXUNUSED(event));

	bool startReceiver();
	void Nohttpd2DlgInitDialog();

	// Custom event triggers
	void onLogMsgEvt(wxCommandEvent &event);
	void onShutdownEvt(wxCommandEvent &WXUNUSED(event));
	void onIconizeEvt(wxIconizeEvent &event);
	void onShowMainEvt(wxCommandEvent &WXUNUSED(event));
	void onSetHitsEvt(wxCommandEvent &WXUNUSED(event));
	void onLogVerboseMsgEvt(wxCommandEvent &event);
	void onHideDlgMsgEvt(wxCommandEvent &WXUNUSED(event));
	void onAddTrayEvt(wxCommandEvent &WXUNUSED(event));
	void onRemTrayEvt(wxCommandEvent &WXUNUSED(event));

	// Widget event triggers
	// Note: These are actually no more button, but menu triggers.
	// The Wx[*]ButtonClick naming is a legacy from previous versions.
	void WxExitButtonClick(wxCommandEvent &WXUNUSED(event));
	void WxClearSessionButtonClick(wxCommandEvent &WXUNUSED(event));
	void WxCleanAllButtonClick(wxCommandEvent &WXUNUSED(event));
	void WxClearLogButtonClick(wxCommandEvent &WXUNUSED(event));
	void WxOptionsButtonClick(wxCommandEvent &WXUNUSED(event));
	void WxStatButtonClick(wxCommandEvent &WXUNUSED(event));
	void WxLogExtViewClick(wxCommandEvent &WXUNUSED(event));

private:
	wxLogTextCtrl *m_ltc;
	bool m_inited;			// true if Nohttpd2DlgInitDialog() was called.
	bool m_receiverrunning;		// true if receiver thread was successfully started.
	long m_allhitsbase;		// Number of hits in all sessions at startup.
							// please see onSetHitsEvt() on details why those
							// two variables are needed.

	Nohttpd2TaskBarIcon *m_taskbar;

	void initiateClose();
	void hideDialog();
	bool addTray();
	void removeTray();

    DECLARE_EVENT_TABLE()
};

#endif
