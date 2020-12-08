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

#include "StatisticsDlg.h"
#include "Nohttpd2App.h"
#include "ReceiverThread.h"
#include "Nohttpd2Dlg.h"

// Declaration of Nohttpd2App for ::wxGetApp()
DECLARE_APP(Nohttpd2App)

BEGIN_EVENT_TABLE(StatisticsDlg,wxDialog)
	EVT_CLOSE(StatisticsDlg::StatisticsDlgClose)
	EVT_LIST_COL_CLICK(ID_WXGENERALLISTCTRL,StatisticsDlg::OnColumnClick)
	EVT_LIST_ITEM_SELECTED(ID_WXGENERALLISTCTRL,StatisticsDlg::OnItemSelected)
END_EVENT_TABLE()

// Declaration of static members.
bool StatisticsDlg::sortascending[3];
wxListCtrl *StatisticsDlg::WxGeneralListCtrl;

// Prototype for the sort function of the listctrl.
int wxCALLBACK wxListCompareFunction(long item1,long item2,long sortdata);

StatisticsDlg::StatisticsDlg(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &position, const wxSize& size, long style)
: wxDialog(parent, id, title, position, size, style){

	sortascending[0]=true;
	sortascending[1]=true;
	sortascending[2]=true;

	CreateGUIControlsCustom();
	insertGeneralItems();
	setBestColumnWidth(WxDetailListCtrl,0);
	setBestColumnWidth(WxDetailListCtrl,1);
	setBestColumnWidth(WxDetailListCtrl,2);
	setBestColumnWidth(WxDetailListCtrl,3);

}

StatisticsDlg::~StatisticsDlg(){}

void StatisticsDlg::CreateGUIControlsCustom(void){

	wxFlexGridSizer* WxFlexGridSizer1 = new wxFlexGridSizer(6,1,0,0);
	this->SetSizer(WxFlexGridSizer1);
	this->SetAutoLayout(true);

	WxFlexGridSizer1->AddGrowableCol(0);
	WxFlexGridSizer1->AddGrowableRow(1);
	WxFlexGridSizer1->AddGrowableRow(4);

	WxStaticText1 = new wxStaticText(this, ID_WXSTATICTEXT1, wxT("Select any entry for detailed view:"), wxPoint(38,5), wxDefaultSize, 0, wxT("WxStaticText1"));
	WxFlexGridSizer1->Add(WxStaticText1,0,wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxALL,5);

	WxGeneralListCtrl = new wxListCtrl(this, ID_WXGENERALLISTCTRL, wxPoint(5,32), wxSize(400,150), wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_HRULES/*|wxLC_VRULES*/);
	WxFlexGridSizer1->Add(WxGeneralListCtrl,0,wxGROW | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxALL,5);

	WxStaticLine1 = new wxStaticLine(this, ID_WXSTATICLINE1, wxPoint(55,192), wxDefaultSize, wxLI_HORIZONTAL);
	WxFlexGridSizer1->Add(WxStaticLine1,0,wxGROW | wxALIGN_CENTER_VERTICAL | wxALL,5);

	wxStaticText *WxStaticText2=new wxStaticText(this, ID_WXSTATICTEXT2, wxT("Detailed view for selected host:"), wxDefaultPosition, wxDefaultSize, 0, wxT("WxStaticText2"));
	WxFlexGridSizer1->Add(WxStaticText2,0,wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxALL,5);

	WxDetailListCtrl = new wxListCtrl(this, ID_WXDETAILLISTCTRL, wxPoint(5,209), wxSize(400,150), wxLC_REPORT|wxLC_HRULES);
	WxFlexGridSizer1->Add(WxDetailListCtrl,0,wxGROW | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxALL,5);

	WxOkButton=new wxButton(this,wxID_OK);
	WxOkButton->SetDefault();
	WxFlexGridSizer1->Add(WxOkButton,0,wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxALL,5);

	GetSizer()->Fit(this);
	GetSizer()->SetSizeHints(this);
	this->SetTitle(wxT("nohttpd: Statistics"));
	this->Center();
	this->SetIcon(*Nohttpd2Dlg::s_mainicon);

	wxListItem li;
	li.SetText(_T("Host"));
	WxGeneralListCtrl->InsertColumn(0,li);
	li.SetText(_T("Access Count"));
	WxGeneralListCtrl->InsertColumn(1,li);
	li.SetText(_T("Last Access"));
	WxGeneralListCtrl->InsertColumn(2,li);

	li.SetText(_T("Access Time"));
	WxDetailListCtrl->InsertColumn(0,li);
	li.SetText(_T("URL"));
	WxDetailListCtrl->InsertColumn(1,li);
	li.SetText(_T("User Agent"));
	WxDetailListCtrl->InsertColumn(2,li);
	li.SetText(_T("Referrer"));
	WxDetailListCtrl->InsertColumn(3,li);
}

void StatisticsDlg::StatisticsDlgClose(wxCloseEvent& WXUNUSED(event)){

	Destroy();
}

void StatisticsDlg::insertGeneralItems(){

	StatItemHashMap::iterator it;
	int i;

	WxGeneralListCtrl->DeleteAllItems();

	// Lock ReceiverThread::s_sihm
	ReceiverThread::waitOnSem(::wxGetApp().semhashmap);

	for (i=0,it=ReceiverThread::s_sihm->begin();it!=ReceiverThread::s_sihm->end();it++,i++){

			WxGeneralListCtrl->SetItemData(WxGeneralListCtrl->InsertItem(i,it->first,0),i);
			WxGeneralListCtrl->SetItem(i,1,wxString::Format(_T("%lu"),it->second->m_accesses));
			WxGeneralListCtrl->SetItem(i,2,it->second->m_lastaccess.Format(_T("%x %X")));
	}

	::wxGetApp().semhashmap.Post();

	setBestColumnWidth(WxGeneralListCtrl,0);
	setBestColumnWidth(WxGeneralListCtrl,1);
	setBestColumnWidth(WxGeneralListCtrl,2);
}

/**
Try to determine the "best" column width.
If a column is empty and the width is set to wxLIST_AUTOSIZE, the resulting
width is only a few pixels.
This method sets the width to the greater of either the original header size or
wxLIST_AUTOSIZE.
*/
void StatisticsDlg::setBestColumnWidth(wxListCtrl *listctrl,int column){

	int oldwidth;

	listctrl->SetColumnWidth(column,wxLIST_AUTOSIZE_USEHEADER);
	oldwidth=listctrl->GetColumnWidth(column);
	listctrl->SetColumnWidth(column,wxLIST_AUTOSIZE);
	if (listctrl->GetColumnWidth(column)<oldwidth){
		listctrl->SetColumnWidth(column,oldwidth);
	}
}

void StatisticsDlg::OnColumnClick(wxListEvent &event){

	WxGeneralListCtrl->SortItems(wxListCompareFunction,event.GetColumn());
	sortascending[event.GetColumn()]^=1;
}

int wxCALLBACK wxListCompareFunction(long item1,long item2,long sortdata){

	int column=(int)sortdata;
	long index1,index2;
	int cmp,res;

	index1=StatisticsDlg::WxGeneralListCtrl->FindItem(-1,item1);
	index2=StatisticsDlg::WxGeneralListCtrl->FindItem(-1,item2);

	// Sort "Access Count" as numbers,
	if (column==1){
		long num1,num2;
		StatisticsDlg::getTextByColumn(index1,column).ToLong(&num1);
		StatisticsDlg::getTextByColumn(index2,column).ToLong(&num2);
		cmp=num1-num2;
	} else {
	// do a string comparison for the others.
		cmp=StatisticsDlg::getTextByColumn(index1,column).Cmp(StatisticsDlg::getTextByColumn(index2,column));
	}
	res=0;
	if (cmp<0){
		res=-1;
	} else if (cmp>0){
		res=1;
	}
	if (!StatisticsDlg::sortascending[column]){
		res=-res;
	}
	return res;
}

wxString StatisticsDlg::getTextByColumn(long index,int column){

	wxListItem item;

	item.SetId(index);
	item.SetColumn(column);
	item.SetMask(wxLIST_MASK_TEXT);
	WxGeneralListCtrl->GetItem(item);
	return item.GetText();
}

void StatisticsDlg::OnItemSelected(wxListEvent &event){

	wxString host;
	wxsdilistNode *node;
	StatDetailItem *sdi;
	int i=0;

	host=event.GetText();

	if (ReceiverThread::s_sihm->find(host)==ReceiverThread::s_sihm->end()){
		LOGMESSAGE(_T("Internal warning: Unable to find selected host in hashmap."));
		return;
	}

	WxDetailListCtrl->DeleteAllItems();

	// Lock ReceiverThread::s_sihm
	ReceiverThread::waitOnSem(::wxGetApp().semhashmap);

	node=(*ReceiverThread::s_sihm)[host]->m_history.GetFirst();
	while (node){

		sdi=node->GetData();

		WxDetailListCtrl->SetItemData(WxDetailListCtrl->InsertItem(i,sdi->m_accesstime.Format(_T("%x %X")),0),i);
		WxDetailListCtrl->SetItem(i,1,sdi->m_url);
		WxDetailListCtrl->SetItem(i,2,sdi->m_useragent);
		WxDetailListCtrl->SetItem(i,3,sdi->m_referrer);
		i++;
		node=node->GetNext();
	}
	::wxGetApp().semhashmap.Post();

	setBestColumnWidth(WxDetailListCtrl,0);
	setBestColumnWidth(WxDetailListCtrl,1);
	setBestColumnWidth(WxDetailListCtrl,2);
	setBestColumnWidth(WxDetailListCtrl,3);
}
