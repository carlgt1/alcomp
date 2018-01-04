_CLASSDEF(TDlgRhythm)
// Control dialog box for editing Song Settings
class TDlgRhythm : public TDialog
{
public:
	PTScrollBar    pSBRhythm[MAX_RHYTHM];  // rhythm scroll bars
	PTBStatic      pTXRhythm[MAX_RHYTHM];  // static text for sb's
	BYTE   			rhythm[MAX_RHYTHM];     // temp values for prob sets

	TDlgRhythm(PTWindowsObject pwParent, int nResourceID);
	~TDlgRhythm()
	{
		WinHelp(this->HWindow, "alcomp.hlp", HELP_QUIT, 0L);
   }

	virtual void SetupWindow();
	virtual void ShowProbs();
	virtual void SetTempRhythms();

	virtual void Ok(RTMessage Msg)  = [ID_FIRST + IDOK];
	virtual void Help(RTMessage Msg)= [ID_FIRST + IDHELP];

	// boxes for probabilities
	virtual void Cauchy  (RTMessage Msg) = [ID_FIRST + ID_CAUCHY];
	virtual void Linear  (RTMessage Msg) = [ID_FIRST + ID_LINEAR];
	virtual void Uniform (RTMessage Msg) = [ID_FIRST + ID_UNIFORM];
	virtual void Gaussian(RTMessage Msg) = [ID_FIRST + ID_GAUSSIAN];
	virtual void Exponent(RTMessage Msg) = [ID_FIRST + ID_EXPONENT];
	virtual void Poisson (RTMessage Msg) = [ID_FIRST + ID_POISSON];

	// functions to update tempo text when scroll bar moved
	virtual void Whole()     = [ID_FIRST + SB_WHOLE];
	virtual void Half()      = [ID_FIRST + SB_HALF];
	virtual void Quarter()   = [ID_FIRST + SB_QUARTER];
	virtual void Eighth()    = [ID_FIRST + SB_EIGHTH];
	virtual void Sixteenth() = [ID_FIRST + SB_SIXTEENTH];
	virtual void ThirtyTwo() = [ID_FIRST + SB_THIRTYSECOND];
};

// setup scroll bars for random rhythms
TDlgRhythm::TDlgRhythm(PTWindowsObject pwParent, int nResourceID)
	: TDialog(pwParent, nResourceID)
{
	for (int i=0; i<MAX_RHYTHM; i++)  {
		pSBRhythm[i] = new TScrollBar(this, SB_WHOLE + i);   // setup scroll bars
		pTXRhythm[i] = new TBStatic(this, TX_WHOLE + i, 4);  // setup static text
	}
}

void TDlgRhythm::SetupWindow()
{
	TDialog::SetupWindow();

	memset( rhythm, 0x00, MAX_RHYTHM );

	for (int i=0; i<MAX_RHYTHM; i++)  {
		pSBRhythm[i]->SetRange(0, 99);
		rhythm[i] = 0;
	}

	for (i=0; i<tempTrack.Rhythm.count(); i++)
		// fill the array with values from tempNote (non-zero values only)
		rhythm[ tempTrack.Rhythm[i].To ] = tempTrack.Rhythm[i].Prob;

	ShowProbs();
}

void TDlgRhythm::ShowProbs()
{
	char buf2[3];
	// setup initial rhythms and text strings
	for (int i=0; i<MAX_RHYTHM; i++)  {
		pSBRhythm[i]->SetPosition( rhythm[i] );

		sprintf(buf2, "%02d", rhythm[i] );
		pTXRhythm[i]->SetText(buf2);
	}
}

void TDlgRhythm::Ok(RTMessage Msg)
{
	if( Msg.LP.Hi == BN_CLICKED )
	{
		// detach all objects
		tempTrack.Rhythm.flush(TRUE);

		StrNote tn = *(new StrNote);
		tn.To = 0;

		SetTempRhythms();

		tn.From = UCHAR_MAX;
		// detached all tempNote.Rhythm's, now add non-zero rhythm[]
		for (int j = 0; j < MAX_RHYTHM; j++)
			if ( rhythm[j] ) {
				// non-zero value, add to array
            tn.Prob = rhythm[j];
				tn.To = j;
				tempTrack.Rhythm.add( tn );
			}
		TDialog::Ok(Msg);
	} 
} 


void TDlgRhythm::Help(RTMessage Msg)
{
	if ( Msg.LP.Hi == BN_CLICKED )
		if (! WinHelp(this->HWindow, "alcomp.hlp", HELP_CONTEXT, RHYTHM_RANDOM ))
			BWCCMessageBox(this->HWindow, "Help File Not Found", "Error!", MB_OK|MB_ICONEXCLAMATION);
}

void TDlgRhythm::SetTempRhythms()
{
	for (int i=0; i<MAX_RHYTHM; i++)
		rhythm[i] = pSBRhythm[i]->GetPosition();
}

void TDlgRhythm::Whole()
{
	char buf2[3];
	sprintf(buf2, "%02d", pSBRhythm[0]->GetPosition() );
	pTXRhythm[0]->SetText(buf2);
}

void TDlgRhythm::Half()
{
	char buf2[3];
	sprintf(buf2, "%02d", pSBRhythm[1]->GetPosition() );
	pTXRhythm[1]->SetText(buf2);
}

void TDlgRhythm::Quarter()
{
	char buf2[3];
	sprintf(buf2, "%02d", pSBRhythm[2]->GetPosition() );
	pTXRhythm[2]->SetText(buf2);
}

void TDlgRhythm::Eighth()
{
	char buf2[3];
	sprintf(buf2, "%02d", pSBRhythm[3]->GetPosition() );
	pTXRhythm[3]->SetText(buf2);
}

void TDlgRhythm::Sixteenth()
{
	char buf2[3];
	sprintf(buf2, "%02d", pSBRhythm[4]->GetPosition() );
	pTXRhythm[4]->SetText(buf2);
}

void TDlgRhythm::ThirtyTwo()
{
	char buf2[3];
	sprintf(buf2, "%02d", pSBRhythm[5]->GetPosition() );
	pTXRhythm[5]->SetText(buf2);
}

void TDlgRhythm::Cauchy(RTMessage Msg)
{
	if (Msg.LP.Hi == BN_CLICKED)  {
		SetTempRhythms();
		Probability prob(rhythm, MAX_RHYTHM);  // allow the probability functions her
		prob.Cauchy();
		ShowProbs();
	}
}

void TDlgRhythm::Linear  (RTMessage Msg)
{
	if (Msg.LP.Hi == BN_CLICKED)  {
		SetTempRhythms();
		Probability prob(rhythm, MAX_RHYTHM);  // allow the probability functions her
		prob.Linear();
		ShowProbs();
	}
}

void TDlgRhythm::Uniform (RTMessage Msg)
{
	if (Msg.LP.Hi == BN_CLICKED)  {
		SetTempRhythms();
		Probability prob(rhythm, MAX_RHYTHM);  // allow the probability functions her
		prob.Uniform();
		ShowProbs();
	}
}

void TDlgRhythm::Gaussian(RTMessage Msg)
{
	if (Msg.LP.Hi == BN_CLICKED)  {
		SetTempRhythms();
		Probability prob(rhythm, MAX_RHYTHM);  // allow the probability functions her
		prob.Gaussian();
		ShowProbs();
	}
}

void TDlgRhythm::Exponent(RTMessage Msg)
{
	if (Msg.LP.Hi == BN_CLICKED)  {
		SetTempRhythms();
		Probability prob(rhythm, MAX_RHYTHM);  // allow the probability functions her
		prob.Exponent();
		ShowProbs();
	}
}

void TDlgRhythm::Poisson (RTMessage Msg)
{
	if (Msg.LP.Hi == BN_CLICKED)  {
		SetTempRhythms();
		Probability prob(rhythm, MAX_RHYTHM);  // allow the probability functions her
		prob.Poisson();
		ShowProbs();
	}
}