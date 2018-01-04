_CLASSDEF(TDlgSong)
class TDlgSong :public TDialog
{
public:
	char tempname[20];
	TDlgSong(PTWindowsObject pwP, int nResourceID, char* filename)
		:TDialog( pwP, nResourceID ) { strcpy(tempname, filename); }
	virtual void SetupWindow()
	{
		TDialog::SetupWindow();
		Play();
	}
	virtual void Cancel(RTMessage)
	{
		CloseWindow();
		Destroy();
	}
	virtual void CloseWindow()
	{
		mciSendString( "stop oursong", NULL, NULL, NULL );
		mciSendString( "close oursong", NULL, NULL, NULL );
		TDialog::CloseWindow();
	}
	virtual void Play()
	{
		// always play if can, otherwise just save
		char mcicommand[50];
        sprintf(mcicommand, "open sequencer!%s alias oursong", tempname);
		if ( ! mciSendString( (LPSTR) mcicommand, NULL, NULL, NULL ) )
			if ( mciSendString( "play oursong from 0 notify", NULL, NULL, this->HWindow ))
				CloseWindow();
	}
	virtual void ShutUp() = [MM_MCINOTIFY]
	{
		CloseWindow();
	}
};
