_CLASSDEF(TDlgRandom)
// inherit from TDlgTrackEdit, which has the track settings
class TDlgRandom   :public TDialog, TDlgTrackEdit, TNoteEdit
{
public:
	BOOL			bToggleOn;

	TDlgRandom(PTWindowsObject pwParent, int nResourceID)
		: TDialog(pwParent, nResourceID),
		  TNoteEdit(this),
		  TDlgTrackEdit(this) {}
	~TDlgRandom()
	{
			WinHelp(this->HWindow, "alcomp.hlp", HELP_QUIT, 0L);
   }
	virtual void SetupWindow();
	virtual BOOL CanClose();

	virtual void Toggle(RTMessage Msg)   = [ID_FIRST + ID_TOGGLENOTES];
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

	// boxes for probabilities
	virtual void Cauchy  (RTMessage Msg) = [ID_FIRST + ID_CAUCHY];
	virtual void Linear  (RTMessage Msg) = [ID_FIRST + ID_LINEAR];
	virtual void Uniform (RTMessage Msg) = [ID_FIRST + ID_UNIFORM];
	virtual void Gaussian(RTMessage Msg) = [ID_FIRST + ID_GAUSSIAN];
	virtual void Exponent(RTMessage Msg) = [ID_FIRST + ID_EXPONENT];
	virtual void Poisson (RTMessage Msg) = [ID_FIRST + ID_POISSON];
};

void TDlgRandom::SetupWindow()
{
	TDialog::SetupWindow();

	// now show the rest of the screen
	TDlgTrackEdit::SetupWindow();

	InitNotes();  // now we're ready to show the notes

	bToggleOn = TRUE;  // at first, toggle all to on
}

void TDlgRandom::Toggle(RTMessage Msg)
{
	if (Msg.LP.Hi == BN_CLICKED)  {
		for (int i = 0; i < MAX_NOTENUM; i++)
			pTENote[i]->SetText( bToggleOn ? "50" : " 0" );

		bToggleOn = ! bToggleOn;
   }
}

// throw these dummy fns here, can't have dispatch msgs in base class
void TDlgRandom::ShowTempo(/*RTMessage Msg*/)
{
	TDlgTrackEdit::Tempo(/*Msg*/);
}

void TDlgRandom::ShowVolume(/*RTMessage Msg*/)
{
	TDlgTrackEdit::Volume(/*Msg*/);
}

void TDlgRandom::ShowPatch(RTMessage Msg)
{
	TDlgTrackEdit::Patch(Msg);
}

void TDlgRandom::ShowChannel(RTMessage Msg)
{
	TDlgTrackEdit::Channel(Msg);
}

void TDlgRandom::Cauchy(RTMessage Msg)
{
	if (Msg.LP.Hi == BN_CLICKED)  {
		SetTempNotes();
		// allow the probability functions here
		Probability prob(note, MAX_NOTENUM);  // allow the probability functions her
		prob.Cauchy();
		ShowNotes();
	}
}

void TDlgRandom::Linear  (RTMessage Msg)
{
	if (Msg.LP.Hi == BN_CLICKED)  {
		SetTempNotes();
		// allow the probability functions here
		Probability prob(note, MAX_NOTENUM);  // allow the probability functions her
		prob.Linear();
		ShowNotes();
	}
}

void TDlgRandom::Uniform (RTMessage Msg)
{
	if (Msg.LP.Hi == BN_CLICKED)  {
		SetTempNotes();
		// allow the probability functions here
		Probability prob(note, MAX_NOTENUM);  // allow the probability functions her
		prob.Uniform();
	 	ShowNotes();
	}
}

void TDlgRandom::Gaussian(RTMessage Msg)
{
	if (Msg.LP.Hi == BN_CLICKED)  {
		SetTempNotes();
		// allow the probability functions here
  		Probability prob(note, MAX_NOTENUM);  // allow the probability functions her
    		prob.Gaussian();
    		ShowNotes();
	}
}

void TDlgRandom::Exponent(RTMessage Msg)
{
	if (Msg.LP.Hi == BN_CLICKED)  {
	 	SetTempNotes();
		// allow the probability functions here
	   	Probability prob(note, MAX_NOTENUM);  // allow the probability functions her
 		prob.Exponent();
		ShowNotes();
	}
}

void TDlgRandom::Poisson (RTMessage Msg)
{
	if (Msg.LP.Hi == BN_CLICKED)  {
		SetTempNotes();
		// allow the probability functions here
    		Probability prob(note, MAX_NOTENUM);  // allow the probability functions her
   		prob.Poisson();
	 	ShowNotes();
	}
}

BOOL TDlgRandom::CanClose()
{
	// check for invalid notes (non numeric and not between 0 and 99)
	// setup the notes now
	char errstr[100], tempstr[100];
	memset(errstr, 0, 100);
	memset(tempstr, 0, 100);
	UINT sumnotes = 0;
	BOOL check = TRUE;

	SetTempNotes();

     // check rhythm probabilities
	for (int i=0; i < MAX_RHYTHM; i++)
		sumnotes += tempTrack.Rhythm[i].Prob;

	if (! sumnotes)  {
		check = FALSE;
          sprintf(errstr, "* No Rhythms Assigned.\n");
	}

	if (! check)  {
		BWCCMessageBox(HWindow, errstr, "Error!", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;   // don't bother with other checks
	}

	// made it past local check, try base classes                        
	return TDlgTrackEdit::CanClose() && TNoteEdit::CanClose();  // TNoteEdit::CanClose();
}

void TDlgRandom::Help(RTMessage Msg)
{
	if ( Msg.LP.Hi == BN_CLICKED )
		if (! WinHelp(this->HWindow, "alcomp.hlp", HELP_CONTEXT, EDIT_RANDOM_PROB ))
			BWCCMessageBox(this->HWindow, "Help File Not Found", "Error!", MB_OK|MB_ICONEXCLAMATION);
}

void TDlgRandom::Ok(RTMessage Msg)
{
	if( Msg.LP.Hi == BN_CLICKED && CanClose() )
	{
		// detach all objects
		tempTrack.Note.flush(TRUE);

		StrNote tn;

		tn.From = UCHAR_MAX;

		for (int j = 0; j < MAX_NOTENUM; j++)
			// detached all tempNote, now add non-zero note[]
			if ( note[j] )  {// non-zero value, add to array
				tn.To = j;
				tn.Prob = note[j];
				tempTrack.Note.add( tn );
			}

		SaveIt();  // call this from TDlgTrackEdit

		bModified = TRUE;

		TDialog::Ok(Msg);
	}
}

void TDlgRandom::Rhythm(RTMessage Msg)
{
	if( Msg.LP.Hi == BN_CLICKED )
		GetModule()->ExecDialog( new TDlgRhythm(this, DG_RHYTHM) );
}
