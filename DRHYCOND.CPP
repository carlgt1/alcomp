_CLASSDEF(TDlgCondRhythm)
// Control dialog box for editing Song Settings
class TDlgCondRhythm : public TDialog
{
	PTEdit         pTERhythm[ MAX_RHYTHM ] [ MAX_RHYTHM ];  // static text for sb's
	BYTE 			   rhythm   [ MAX_RHYTHM ] [ MAX_RHYTHM ];  // temp values for prob sets
public:
	TDlgCondRhythm(PTWindowsObject pwParent, int nResourceID);
	~TDlgCondRhythm()
	{
		WinHelp(this->HWindow, "alcomp.hlp", HELP_QUIT, 0L);
   }

	virtual void SetupWindow();
	virtual void Ok(RTMessage Msg)     = [ID_FIRST + IDOK];
	virtual void Help(RTMessage Msg)   = [ID_FIRST + IDHELP];
	virtual BOOL CanClose();

	// handy function to show all prob settings
	void ShowProbs(/*int* value*/);
	void SetTempRhythms();  // set this up before pass rhythms
};
// setup scroll bars for random rhythms
TDlgCondRhythm::TDlgCondRhythm(PTWindowsObject pwParent, int nResourceID)
	: TDialog(pwParent, nResourceID)
{
	for (int i=0; i<MAX_RHYTHM; i++)
		for (int j=0; j<MAX_RHYTHM; j++)  {
			pTERhythm[i][j] = new TEdit(this, TX_WW + 6*i + j, 3);  // setup static text
			rhythm[i][j] = 0;
      }
	// fill the array with values from tempNote (non-zero values only)
	for (i=0; i<tempTrack.Rhythm.count(); i++)
		rhythm[ tempTrack.Rhythm[i].From ][ tempTrack.Rhythm[i].To ] 
			= tempTrack.Rhythm[i].Prob;
}

void TDlgCondRhythm::ShowProbs(/*int* value*/)
{
	char buf3[3];
	// setup initial rhythms and text strings
	for (int i=0; i<MAX_RHYTHM; i++)
		for (int j=0; j<MAX_RHYTHM; j++)  {
			sprintf(buf3, "%2d", rhythm[i][j] );
			pTERhythm[i][j]->SetText(buf3);
	}
}

void TDlgCondRhythm::SetupWindow()
{
	TDialog::SetupWindow();

	ShowProbs();
}

void TDlgCondRhythm::SetTempRhythms()
{
	char buf2[3];
	for (int i=0; i<MAX_RHYTHM; i++)
		for (int j=0; j<MAX_RHYTHM; j++) {
			pTERhythm[i][j]->GetText(buf2, 3);
			rhythm[i][j] = atoi( buf2 );
      }
}

BOOL TDlgCondRhythm::CanClose()
{
	int sumrhythm = 0;
	BOOL check = TRUE;

	SetTempRhythms();

	for (int i=0; i<MAX_RHYTHM; i++)
		for (int j=0; j<MAX_RHYTHM; j++)
			if ( rhythm[i][j] > 99 )  {
				BWCCMessageBox(NULL, "Invalid Rhythm.  Please Correct.", "Error!", MB_OK|MB_ICONEXCLAMATION);
				check = FALSE;
            break;
			}
			else
				sumrhythm += rhythm[i][j];

		if ( ! sumrhythm )  {
			check = FALSE;
			BWCCMessageBox(NULL, "No Rhythm's Assigned!", "Error!", MB_OK|MB_ICONEXCLAMATION);
		}

	return check;
}

void TDlgCondRhythm::Ok(RTMessage Msg)
{
	if( Msg.LP.Hi == BN_CLICKED && CanClose() )
	{
		TDialog::Ok(Msg);

		// detach all objects
		tempTrack.Rhythm.flush(TRUE);

		SetTempRhythms();

		StrNote tn;

		// detached all tempNote.Rhythm's, now add non-zero rhythm[]
		for (int i = 0; i < MAX_RHYTHM; i++)
			for (int j = 0; j < MAX_RHYTHM; j++)
				if ( rhythm[i][j] ) {
					// non-zero value, add to array
					tn.From = i;
					tn.To   = j;
					tn.Prob = rhythm[i][j];
					tempTrack.Rhythm.add( tn );
				}
	}
}

void TDlgCondRhythm::Help(RTMessage Msg)
{
	if ( Msg.LP.Hi == BN_CLICKED )
		if (! WinHelp(this->HWindow, "alcomp.hlp", HELP_CONTEXT, RHYTHM_CONDITIONAL ))
			BWCCMessageBox(this->HWindow, "Help File Not Found", "Error!", MB_OK|MB_ICONEXCLAMATION);
}
