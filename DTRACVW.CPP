_CLASSDEF(TDlgTrackView)
// Declare TDlgTrackView, a TDialog descendant for Track View window
class TDlgTrackView : public TDialog
{
	LPSTR lpszTrackString;
   BOOL  bNewAdd;
   virtual void Cancel(RTMessage Msg);
	virtual void Ok(RTMessage Msg);
public:
	PTListBox pTrackSelect;
	TDlgTrackView (PTWindowsObject pwParent, int nResourceID);
	~TDlgTrackView();
	virtual void SetupWindow();
	virtual void TrackListBox(RTMessage Msg) = [ID_FIRST + LB_TRACKVIEW];
	virtual void Generate(RTMessage Msg) = [ID_FIRST +ID_GENERATE];
	virtual void Edit(RTMessage Msg) = [ID_FIRST +ID_EDIT];
	virtual void Add(RTMessage Msg) = [ID_FIRST +ID_ADD];
	virtual void Delete(RTMessage Msg) = [ID_FIRST +ID_DELETE];
	virtual void Help(RTMessage Msg) = [ID_FIRST + IDHELP];
	virtual LPSTR ListBoxString(Track newone);
	virtual void MakeListBoxStrings();
	virtual void AddNewString(int oldindex);  // deletes old one, adds new one at right spot
	virtual void CheckMenu();   //enable/disable menu options as appropriate
};

TDlgTrackView::TDlgTrackView (PTWindowsObject pwParent, int nResourceID)
	: TDialog(pwParent, nResourceID)
{
	bNewAdd = TRUE;
	lpszTrackString = new char[100];
	pTrackSelect = new TListBox(this, LB_TRACKVIEW);
	if (nProbType == CONDITIONAL)
		SetCaption("Track View for Conditional Probabilities");
	else
		SetCaption("Track View for Random Probabilities");
}

TDlgTrackView::~TDlgTrackView()
{
	delete lpszTrackString;
	delete pTrackSelect;
	WinHelp(this->HWindow, "alcomp.hlp", HELP_QUIT, 0L);
}

void TDlgTrackView::TrackListBox(RTMessage Msg)
{
	if (Msg.LP.Hi == LBN_DBLCLK)
		Edit(Msg);
}

void TDlgTrackView::SetupWindow()
{
	TDialog::SetupWindow();
	// set up a listing of all the tracks
	MakeListBoxStrings();
}

void TDlgTrackView::Edit(RTMessage Msg)
{
	if ( Msg.LP.Hi != BN_CLICKED && Msg.LP.Hi != LBN_DBLCLK )
     	return;  // not the right message to get in here
	int index = pTrackSelect->GetSelIndex();
	if ( ! dTrack.count() )   // 0 tracks
			BWCCMessageBox(HWindow, "Nothing to Edit.\n\nYou Must Add a Track First.", "Edit Track", MB_OK | MB_ICONEXCLAMATION);
	else  {
		if (index == -1)
			BWCCMessageBox(HWindow, "You Must Select a Track First.", "Edit Track", MB_OK | MB_ICONEXCLAMATION);
		else  {
			// first copy track stuff over to temp buffer
			tempTrack = dTrack[index];

			BOOL hitOK = FALSE;

			// show the dialog for editing track values
			if (nProbType == CONDITIONAL)
				hitOK = (GetModule()->ExecDialog(new TDlgConditional(this, DG_CONDEDIT)) == IDOK);
			else
				hitOK = (GetModule()->ExecDialog(new TDlgRandom(this, DG_TRACKEDIT)) == IDOK);

			// NOTE LAST PARAMETER WHICH IS OFFSET OF TRACK PICKED
			if ( hitOK )
			{
				AddNewString(index);
				bModified = TRUE;  // track was modified
			}
		}
	}
}

void TDlgTrackView::Generate(RTMessage Msg)
{
	if ( Msg.LP.Hi != BN_CLICKED )
     	return;

	if (! dTrack.count())
		BWCCMessageBox(HWindow, "No Tracks to Play!", "Generate Song", MB_OK|MB_ICONSTOP);
	else  {
		TMIDIFile* mmf = new TMIDIFile(this);
		mmf->MakeMIDIFile();  // made MIDI file OK, continue
		delete mmf;
	}
}

void TDlgTrackView::Add(RTMessage Msg)
{
	if ( Msg.LP.Hi != BN_CLICKED )
     	return;
	int track = dTrack.count();
	if (track == MAX_TRACKS)  {
		char errmsg[35];
     	sprintf(errmsg, "Sorry, %d Tracks is the Limit!", MAX_TRACKS);
		BWCCMessageBox(HWindow, errmsg, "That's All, Folks!", MB_OK|MB_ICONSTOP);
		return;
	}
	tempTrack = nullTrack;  // initialize with all blank stuff
	if ( track )  {// set last tracks tempo and time signature if not 1st track
		tempTrack.TimeSigNum = dTrack[track-1].TimeSigNum;
		tempTrack.TimeSigDen = dTrack[track-1].TimeSigDen;
		tempTrack.Tempo      = dTrack[track-1].Tempo;
	}

	int index;
	if (  ( index = pTrackSelect->GetSelIndex() ) > -1  )
		switch(BWCCMessageBox(HWindow, "Use Selected Track as a\nBase For the New Track?", "Add Track", MB_YESNOCANCEL|MB_ICONQUESTION))
		{
			case IDYES:
			{
				tempTrack = dTrack[index];
				break;
			}
			case IDNO: break;
			case IDCANCEL: return;
		}

	BOOL hitOK = FALSE;

	// show the dialog for editing track values
			// show the dialog for editing track values
	if (nProbType == CONDITIONAL)
		hitOK = (GetModule()->ExecDialog(new TDlgConditional(this, DG_CONDEDIT)) == IDOK);
	else
		hitOK = (GetModule()->ExecDialog(new TDlgRandom(this, DG_TRACKEDIT)) == IDOK);

	// NOTE LAST PARAMETER WHICH IS OFFSET OF TRACK PICKED

	if ( hitOK )  {
		bModified = TRUE;  // track was modified

		AddNewString(-1);
	}
}

void TDlgTrackView::Cancel(RTMessage Msg)
{
	if ( ! dTrack.count() )  {
		nProbType = UNSELECTED;
		EnableMenuItem( GetMenu(Parent->HWindow), MN_PROBABILITY, MF_ENABLED );
		EnableMenuItem( GetMenu(Parent->HWindow), MN_CONDITIONAL, MF_ENABLED );
	}
	TDialog::Cancel(Msg);
}
void TDlgTrackView::Ok(RTMessage Msg)
{
	if ( ! dTrack.count() )  { // no records, so enable stuff
   	nProbType = UNSELECTED;
		EnableMenuItem( GetMenu(Parent->HWindow), MN_PROBABILITY, MF_ENABLED );
		EnableMenuItem( GetMenu(Parent->HWindow), MN_CONDITIONAL, MF_ENABLED );
	}
	TDialog::Ok(Msg);
}

void TDlgTrackView::Delete(RTMessage Msg)
{
	if ( Msg.LP.Hi != BN_CLICKED ||
		( ! dTrack.count() && BWCCMessageBox(HWindow, "No Tracks Available.", "Delete Track", MB_OK | MB_ICONEXCLAMATION) ))
		return;
	int index = pTrackSelect->GetSelIndex();
	if (index == -1)
		BWCCMessageBox(HWindow, "You Must Select a Track First.", "Delete Track", MB_OK | MB_ICONEXCLAMATION);
	else  {
		if (BWCCMessageBox(HWindow, "Are you sure?", "Delete Track", MB_YESNO | MB_ICONQUESTION)
				== IDYES)  {
			bModified = TRUE;  // track was modified

			// get rid of the list box string that we deleted
			pTrackSelect->DeleteString(index);
			dTrack.detach( dTrack[index], TRUE );

			if (pTrackSelect->SetSelIndex(index) < 0)
				pTrackSelect->SetSelIndex(index-1);

			if ( ! dTrack.count() ) // nothing left!  enable stuff
            CheckMenu();
		}
	}
}

void TDlgTrackView::Help(RTMessage Msg)
{
	if ( Msg.LP.Hi == BN_CLICKED )  {
		if (! WinHelp(this->HWindow, "alcomp.hlp", HELP_CONTEXT, (nProbType==RANDOM) ? TRACK_VIEW_RANDOM : TRACK_VIEW_CONDITIONAL) )
			BWCCMessageBox(this->HWindow, "Help File Not Found", "Error!", MB_OK|MB_ICONEXCLAMATION);
	}
}

inline LPSTR TDlgTrackView::ListBoxString(Track newone)
{
	char des[41], pat[21];

     // first see if channel 10 (drums)
	if ( newone.Channel == 10 )
		strcpy(pat, "Drums");
     else
		strxfrm(pat, CCH pszPatch[ newone.Patch ], 20);

	strxfrm(des, CCH newone.Description, 40);
	sprintf(CCH lpszTrackString, " %03d    %03d    %02d    %-20s\t %-40s",
		newone.StartMeasure, newone.EndMeasure, newone.Channel, pat, des);
	return lpszTrackString;
}

void TDlgTrackView::MakeListBoxStrings()
{
	pTrackSelect->ClearList();
	for (int i=0; i<dTrack.count() && dTrack[i].StartMeasure; i++)
		pTrackSelect->AddString( ListBoxString( dTrack[i] ) );
}

void TDlgTrackView::AddNewString(int oldindex)
{
	int counter = dTrack.count();
     if (oldindex > -1)  {
		pTrackSelect->DeleteString(oldindex);// remove old LB string
		dTrack.detach( dTrack[oldindex], TRUE );   // delete old one from dTrack
		counter--;
	}  // only if we're editing, not if we added

	int j = 0;

	// now find the position the new track will go
	// check that TrackCount isn't zero, since we'll get an error if edited
	while ( j<counter && dTrack[j] < tempTrack )
		j++;

	dTrack.add( tempTrack );  // actually add the new track now

	MakeListBoxStrings();

	pTrackSelect->SetSelIndex(j);

	if ( bNewAdd )  {
		bNewAdd = FALSE;
		// disable the appropriate edit selection since we've made our choice
      CheckMenu();
	}
}

void TDlgTrackView::CheckMenu()
{
	EnableMenuItem( GetMenu(Parent->HWindow), MN_PROBABILITY, nProbType == RANDOM      ? MF_ENABLED : MF_GRAYED );
	EnableMenuItem( GetMenu(Parent->HWindow), MN_CONDITIONAL, nProbType == CONDITIONAL ? MF_ENABLED : MF_GRAYED );

	// should enable the Save and SaveAs on the menu now if 1st track
	EnableMenuItem( GetMenu(Parent->HWindow), MN_FILESAVEAS , MF_ENABLED );
	if ( strlen(lpszFile) )
		EnableMenuItem( GetMenu(Parent->HWindow), MN_FILESAVE, MF_ENABLED);
	else
		EnableMenuItem( GetMenu(Parent->HWindow), MN_FILESAVE, MF_GRAYED);
}
