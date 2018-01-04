/* ********************************
        REGULAR ABOUT STUFF
   ******************************** */


_CLASSDEF(TDlgAbout)
// Declare TDlgAbout, a TDialog descendant for the Help - About window
class TDlgAbout : public TDialog 
{
public:
	TDlgAbout(PTWindowsObject pwParent, int nResourceID)
		: TDialog(pwParent, nResourceID) {}
	virtual void More(RTMessage Msg) = [ID_FIRST + ID_MORE];
};

_CLASSDEF(TDlgAboutMore)
// Declare TDlgAbout, a TDialog descendant for the Help - About window
class TDlgAboutMore : public TDialog 
{
public:
	TDlgAboutMore(PTWindowsObject pwParent, int nResourceID)
		: TDialog(pwParent, nResourceID) {}
};

// Hit More for About Dialog, open up About More window
void TDlgAbout::More(RTMessage Msg)
{
	if (Msg.LP.Hi == BN_CLICKED)
		GetModule()->ExecDialog(
					new TDlgAboutMore(this, DG_ABOUTMORE));
}
