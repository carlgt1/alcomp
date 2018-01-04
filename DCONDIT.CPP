// inherit from TDlgTrackEdit, which has the track settings
class TDlgConditional   :public TDialog, TDlgTrackEdit
{
	// controls for note selections follow
	PTCheckBox pCBNote[MAX_NOTENUM];
	class TMyGroup :public TGroupBox
	{
	public:
		BOOL bSettingUp;
		TMyGroup(PTWindowsObject pwParent, int nResourceID)
			:TGroupBox(pwParent, nResourceID) { bSettingUp = TRUE; }
		virtual void SelectionChanged(int nControlID)
		{
			if (bSettingUp)
				return;   // don't bother if we're just setting up
			else
				SendMessage(this->Parent->HWindow, WM_COMMAND, (WPARAM) CC_SELCHANGE, (LPARAM) nControlID);
		}
	};
	TMyGroup* pGBNote;
public:
	TDlgConditional(PTWindowsObject pwParent, int nResourceID);
	~TDlgConditional()
	{
		WinHelp(this->HWindow, "alcomp.hlp", HELP_QUIT, 0L);
   }

	virtual void SetupWindow();
	virtual BOOL CanClose();

	virtual void WMCommand(RTMessage Msg) = [WM_FIRST + WM_COMMAND];
	virtual void CheckedSomething(UINT nControl);
	virtual void Ok(RTMessage Msg)       = [ID_FIRST + IDOK];
	virtual void Rhythm(RTMessage Msg)   = [ID_FIRST + ID_RHYTHM];
	virtual void Help(RTMessage Msg) = [ID_FIRST + IDHELP];

	// functions to update tempo text when scroll bar moved
	virtual void ShowTempo(/*RTMessage Msg*/)  = [ID_FIRST + SB_TEMPO];

	// functions to update volume text when scroll bar moved
	virtual void ShowVolume(/*RTMessage Msg*/) = [ID_FIRST + SB_VOLUME];

	// functions to update tempo text when scroll bar moved
	virtual void ShowChannel(RTMessage Msg)  = [ID_FIRST + CB_MIDICHANNEL];

	// functions to update volume text when scroll bar moved
	virtual void ShowPatch(RTMessage Msg) = [ID_FIRST + CB_PATCH];
};

TDlgConditional::TDlgConditional(PTWindowsObject pwParent, int nResourceID)
		: TDialog(pwParent, nResourceID), TDlgTrackEdit(this)
{
	pGBNote = new TMyGroup( this, GB_CONDEDIT );
	for (int i = 0; i < MAX_NOTENUM; i++)
			pCBNote[i] = new TCheckBox( this, NOTE_OFFSET + i, pGBNote );
}

void TDlgConditional::SetupWindow()
{
	TDialog::SetupWindow();

	for (int i = 0; i < MAX_NOTENUM; i++)
			pCBNote[i]->Uncheck();

	// now see what check boxes have notes
	int current = -1;  // save current state, no sense in keep rechecking
	int max = tempTrack.Note.count();
	for ( i = 0; i < max; i++ )
		// see if we changed note From's from the current
		if ( tempTrack.Note[i].From != current )  {
      	current = tempTrack.Note[i].From;
			pCBNote[current]->Check();
		}

	// now show the rest of the screen
	TDlgTrackEdit::SetupWindow();

	// now make the group set-up flag false
	pGBNote->bSettingUp = FALSE;
}

void TDlgConditional::CheckedSomething(UINT nControl)
{
	BYTE from = (BYTE) (nControl - NOTE_OFFSET);

	pGBNote->bSettingUp = TRUE;

	// check if it's on now, that means it was just turned on
	pCBNote[from]->SetCheck(BF_GRAYED);

	BYTE oct;
	if ( pRBOctaveMinus2->GetCheck() == BF_CHECKED )
		oct = 0;

	if ( pRBOctaveMinus1->GetCheck() == BF_CHECKED )
		oct = 1;

	if ( pRBOctaveZero->GetCheck() == BF_CHECKED )
		oct = 2;

	if ( pRBOctavePlus1->GetCheck() == BF_CHECKED )
		oct = 3;

	if ( pRBOctavePlus2->GetCheck() == BF_CHECKED )
		oct = 4;

	// so it must be checked, let's bring up it's song dialog
	TDlgCondNote* pDCN = new TDlgCondNote( this, DG_SETCOND, from, oct );

	// now execute this dialog and let'er rip!
	// if hit OK, the user wanted to save notes

	if ( (GetModule()->ExecDialog(pDCN)) == IDOK )  {
		pCBNote[from]->Check();

		// now turn on checks for this one's notes
		for (int i = 0; i<tempTrack.Note.count(); i++)  {
			// if we picked a note but it's not checked, check it now
			if (pCBNote[ tempTrack.Note[i].To ]->GetCheck() != BF_CHECKED)
				pCBNote[ tempTrack.Note[i].To ]->Check();
			if (pCBNote[ tempTrack.Note[i].From ]->GetCheck() != BF_CHECKED)
				pCBNote[ tempTrack.Note[i].From ]->Check();
		}
	}
	else
		pCBNote[from]->Uncheck();

	pGBNote->bSettingUp = FALSE;
}

void TDlgConditional::WMCommand(RTMessage Msg)
{
	// see if we got a check message, else do regular processing
	if (Msg.WParam == CC_SELCHANGE)
		CheckedSomething( (UINT) Msg.LParam );
	else
		TDialog::WMCommand(Msg);
}

// throw these dummy fns here, can't have dispatch msgs in base class
void TDlgConditional::ShowTempo(/*RTMessage Msg*/)
{
	TDlgTrackEdit::Tempo(/*Msg*/);
}

void TDlgConditional::ShowVolume(/*RTMessage Msg*/)
{
	TDlgTrackEdit::Volume(/*Msg*/);
}

void TDlgConditional::ShowPatch(RTMessage Msg)
{
	TDlgTrackEdit::Patch(Msg);
}

void TDlgConditional::ShowChannel(RTMessage Msg)
{
	TDlgTrackEdit::Channel(Msg);
}

BOOL TDlgConditional::CanClose()
{
	char errstr[100];

	BOOL check = TRUE;

	// check for invalid notes (non numeric and not between 0 and 99)
	// setup the notes now

	int sumnotes = 0;

	// check rhythm probabilities
	for (int i=0; i<tempTrack.Rhythm.count(); i++)
   	sumnotes += tempTrack.Rhythm[i].Prob;

	if ( ! sumnotes )  {
		check = FALSE;
		strcpy(errstr, "* No Rhythm's Assigned!\n\n");
	}

   sumnotes = 0;

	for (i=0; i < tempTrack.Note.count(); i++)
		if ( pCBNote[tempTrack.Note[i].From]->GetCheck() == BF_CHECKED )
			if ( tempTrack.Note[i].Prob > 99 )  {
         	check = FALSE;
				strcpy(errstr, "* Invalid Note Prob!\n\n");
				break;
         }
			else
				sumnotes += tempTrack.Note[i].Prob;

	if (! sumnotes)  {
		check = FALSE;
		strcpy(errstr, "* No Notes Assigned.\n");
	}

	if (! check)
		BWCCMessageBox(HWindow, errstr, "Error!", MB_OK | MB_ICONEXCLAMATION);

	// made it past local check, try base classes                        
	return TDlgTrackEdit::CanClose() && check;
}

void TDlgConditional::Help(RTMessage Msg)
{
	if ( Msg.LP.Hi == BN_CLICKED )
		if (! WinHelp(this->HWindow, "alcomp.hlp", HELP_CONTEXT, EDIT_CONDITIONAL_PROB ))
			BWCCMessageBox(this->HWindow, "Help File Not Found", "Error!", MB_OK|MB_ICONEXCLAMATION);
}

void TDlgConditional::Ok(RTMessage Msg)
{
	if( Msg.LP.Hi == BN_CLICKED && CanClose() )
	{
		SaveIt();  // call this from TDlgTrackEdit

      int i=0;
		while ( i<tempTrack.Note.count() )
			if ( tempTrack.Note[i].Prob && (pCBNote[tempTrack.Note[i].From]->GetCheck() != BF_CHECKED) )
				// has a prob value but is unchecked so get rid of it
				tempTrack.Note.detach(i, TRUE);
			else
				i++;
				 
		bModified = TRUE;

		TDialog::Ok(Msg);
	}
}

void TDlgConditional::Rhythm(RTMessage Msg)
{
	if( Msg.LP.Hi == BN_CLICKED )
		GetModule()->ExecDialog( new TDlgCondRhythm(this, DG_RHYTHM_COND) );
}
