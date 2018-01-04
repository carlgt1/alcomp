_CLASSDEF(TDlgSongSettings)
// Control dialog box for editing Song Settings
class TDlgSongSettings : public TDialog
{
private:
	PTComboBox  pCBTimeSigNum, pCBTimeSigDen;
	PTScrollBar pSBTempo, pSBMeasures;
	PTStatic    pTXTempo, pTXMeasures;
public:
	TDlgSongSettings(PTWindowsObject pwParent, int nResourceID);

	virtual void Tempo()           = [ID_FIRST + SB_TEMPO];
	virtual void Measures()        = [ID_FIRST + SB_MEASURES];
	virtual void SetupWindow();
	virtual BOOL CanClose();
	virtual void Ok(RTMessage Msg) = [ID_FIRST + IDOK];
   
	TransferSongSettings TrSongSettings;   // the buffer for it all
};

// setup combo boxes and scroll bars for Time Signatures, tempo, & measures
TDlgSongSettings::TDlgSongSettings(PTWindowsObject pwParent, int nResourceID)
	: TDialog(pwParent, nResourceID)
{
	// setup controls for Song Settings
	TrSongSettings.pCBTimeSigNum = new TComboBox (this, CB_TIMESIGNUM, 3);
	TrSongSettings.pCBTimeSigDen = new TComboBox (this, CB_TIMESIGDEN, 3);
	TrSongSettings.pSBTempo      = new TScrollBar(this, SB_TEMPO);
	TrSongSettings.pSBMeasures   = new TScrollBar(this, SB_MEASURES);
	TrSongSettings.pTXTempo      = new TEdit(this, TX_TEMPO, 4);
	TrSongSettings.pTXMeasures   = new TEdit(this, TX_MEASURES, 4);
}

void TDlgSongSettings::SetupWindow()
{
	TDialog::SetupWindow();

	char buf2[3], buf3[4];

	// Time Signature Numerator Selection Strings
	TrSongSettings.pCBTimeSigNum->ClearList();
	TrSongSettings.pCBTimeSigNum->AddString(" 2");
	TrSongSettings.pCBTimeSigNum->AddString(" 3");
	TrSongSettings.pCBTimeSigNum->AddString(" 4");
	TrSongSettings.pCBTimeSigNum->AddString(" 5");
	TrSongSettings.pCBTimeSigNum->AddString(" 6");
	TrSongSettings.pCBTimeSigNum->AddString(" 7");
	TrSongSettings.pCBTimeSigNum->AddString(" 8");
	TrSongSettings.pCBTimeSigNum->AddString(" 9");
	TrSongSettings.pCBTimeSigNum->AddString("10");
	TrSongSettings.pCBTimeSigNum->AddString("11");
	TrSongSettings.pCBTimeSigNum->AddString("12");
	TrSongSettings.pCBTimeSigNum->AddString("13");
	TrSongSettings.pCBTimeSigNum->AddString("14");
	TrSongSettings.pCBTimeSigNum->AddString("15");
	TrSongSettings.pCBTimeSigNum->AddString("16");

	// Time Signature Denominator Selection Strings
	TrSongSettings.pCBTimeSigDen->ClearList();
	TrSongSettings.pCBTimeSigDen->AddString(" 2");
	TrSongSettings.pCBTimeSigDen->AddString(" 4");
	TrSongSettings.pCBTimeSigDen->AddString(" 8");
	TrSongSettings.pCBTimeSigDen->AddString("16");
	TrSongSettings.pCBTimeSigDen->AddString("32");

	// set initial text strings for time signature from regular data
	sprintf(buf2, "%2d", pSongSettings->TimeSigNum);
	TrSongSettings.pCBTimeSigNum->SetText(buf2);

	sprintf(buf2, "%2d", pSongSettings->TimeSigDen);
	TrSongSettings.pCBTimeSigDen->SetText(buf2);

	// setup ranges, page magnitude, and position of Tempo
	TrSongSettings.pSBTempo->PageMagnitude = 20;
	TrSongSettings.pSBTempo->SetRange(10, 400);
	TrSongSettings.pSBTempo->SetPosition(pSongSettings->Tempo);

	// setup ranges, page magnitude, and position of Measures
	TrSongSettings.pSBMeasures->PageMagnitude = 50;
	TrSongSettings.pSBMeasures->SetRange(1, 999);
	TrSongSettings.pSBMeasures->SetPosition(pSongSettings->Measures);

	// setup static text for scroll bar displays
	sprintf(buf3, "%3d", pSongSettings->Tempo);
   TrSongSettings.pTXTempo->SetText(buf3);

	sprintf(buf3, "%3d", pSongSettings->Measures);
	TrSongSettings.pTXMeasures->SetText(buf3);
}

void TDlgSongSettings::Tempo()
{
	char buf3[4];
	sprintf(buf3, "%3d", TrSongSettings.pSBTempo->GetPosition());
	TrSongSettings.pTXTempo->SetText(buf3);
}

void TDlgSongSettings::Measures()
{
	char buf3[4];
	sprintf(buf3, "%3d", TrSongSettings.pSBMeasures->GetPosition());
	TrSongSettings.pTXMeasures->SetText(buf3);
}

BOOL TDlgSongSettings::CanClose()
{
	char buf2[3];
	int  temp = 0;
   BOOL check = TRUE;

	// check valid Time Sig Numerator
	TrSongSettings.pCBTimeSigNum->GetText(buf2, 3);
	temp = atoi(buf2);
	if (temp < 1 || temp > 99)   // don't let have less than 1 or > 99
		check = FALSE;

	// check valid Time Sig Denominator
	TrSongSettings.pCBTimeSigDen->GetText(buf2, 3);
	temp = atoi(buf2);
	if (temp < 1 || temp > 99)   // don't let have less than 1 or > 99
		check = FALSE;

	if (! check)
		BWCCMessageBox(NULL, "Invalid Time Signature.", "Error!", MB_OK | MB_ICONEXCLAMATION);

	return check;
}
void TDlgSongSettings::Ok(RTMessage Msg)
{
	if( CanClose())
	{
		char buf2[3];
		int  temp = 0;

		TrSongSettings.pCBTimeSigNum->GetText(buf2, 3);
		temp = atoi(buf2);
		pSongSettings->TimeSigNum = temp;

		TrSongSettings.pCBTimeSigDen->GetText(buf2, 3);
		temp = atoi(buf2);
		pSongSettings->TimeSigDen = temp;

		pSongSettings->Tempo    = TrSongSettings.pSBTempo->GetPosition();
		pSongSettings->Measures = TrSongSettings.pSBMeasures->GetPosition();

		TDialog::Ok(Msg);
	}
}
