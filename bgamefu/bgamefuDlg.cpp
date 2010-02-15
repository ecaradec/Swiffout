// bgamefuDlg.cpp : implementation file
//

#include "stdafx.h"
#include "bgamefu.h"
#include "bgamefuDlg.h"
#include <afxhtml.h>
#include <Ole2.h>
#include <atlbase.h>

#import "c:\\windows\\system32\\macromed\\flash\\flash.ocx" named_guids

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// TODO :
// check if the version of flash installed for IE is the last
// separate between the settings window and the runner, store settings in the registry
// get the runner working fast
DEVMODE defaultDM={0};

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CbgamefuDlg dialog




CbgamefuDlg::CbgamefuDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CbgamefuDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CbgamefuDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);

    DDX_Control(pDX, IDC_LIST1, m_resolutionList);
    DDX_Control(pDX, IDC_BUTTON2, m_apply);    
}

BEGIN_MESSAGE_MAP(CbgamefuDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
    ON_BN_CLICKED(IDC_BUTTON2, &CbgamefuDlg::OnBnClickedButton2)
    ON_BN_CLICKED(IDC_BUTTON3, &CbgamefuDlg::OnBnClickedButton3)
END_MESSAGE_MAP()

int compareArea(const void *p1, const void *p2)
{
    DEVMODE *dm1=(DEVMODE*)p1;
    DEVMODE *dm2=(DEVMODE*)p2;
    return (dm2->dmPelsWidth*dm2->dmPelsHeight)-(dm1->dmPelsWidth*dm1->dmPelsHeight);
}

BOOL CbgamefuDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here    
    int mode=0;    
    while(1) {
        if(mode>500)
            break;
        dm[mode].dmSize=sizeof(DEVMODE);
        if(!EnumDisplaySettings(0, mode, &dm[mode]))
            break;       
        mode++;
    }

    qsort(dm, mode, sizeof(DEVMODE), compareArea);

    for(int i=0;i<mode;i++) {
        CString f;
        if(dm[i].dmBitsPerPel==32 && dm[i].dmDisplayFrequency==60)
        {
            f.Format(L"%dx%d", dm[i].dmPelsWidth, dm[i].dmPelsHeight);
            m_resolutionList.InsertString(0,f);
            m_resolutionList.SetItemData(0,i);
        }
    }
        
    //CRect r;
    //m_resolutionBt.GetWindowRect(&r);
    //int selectedRes=m.TrackPopupMenu(TPM_NONOTIFY,r.right,r.top,this);
    //devmode=dm[selectedRes];

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CbgamefuDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CbgamefuDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CbgamefuDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//<OBJECT id=gamefile name=gamefile codeBase="http://fpdownload.macromedia.com/get/flashplayer/current/swflash.cab"
//        classid=clsid:d27cdb6e-ae6d-11cf-96b8-444553540000 width=800 align=middle height=500 type=application/x-shockwave-flash>
//<PARAM NAME="_cx" VALUE="21166">
//<PARAM NAME="_cy" VALUE="13229">
//<PARAM NAME="FlashVars" VALUE="">
//<PARAM NAME="Movie" VALUE="/files/games/civilizations-wars-5151.swf">
//<PARAM NAME="Src" VALUE="/files/games/civilizations-wars-5151.swf">
//<PARAM NAME="WMode" VALUE="Window">
//<PARAM NAME="Play" VALUE="0">
//<PARAM NAME="Loop" VALUE="-1">
//<PARAM NAME="Quality" VALUE="High">
//<PARAM NAME="SAlign" VALUE="">
//<PARAM NAME="Menu" VALUE="-1">
//<PARAM NAME="Base" VALUE="">
//<PARAM NAME="AllowScriptAccess" VALUE="">
//<PARAM NAME="Scale" VALUE="ShowAll">
//<PARAM NAME="DeviceFont" VALUE="0">
//<PARAM NAME="EmbedMovie" VALUE="0">
//<PARAM NAME="BGColor" VALUE="">
//<PARAM NAME="SWRemote" VALUE="">
//<PARAM NAME="MovieData" VALUE="">
//<PARAM NAME="SeamlessTabbing" VALUE="1">
//<PARAM NAME="Profile" VALUE="0">
//<PARAM NAME="ProfileAddress" VALUE="">
//<PARAM NAME="ProfilePort" VALUE="0">
//<PARAM NAME="AllowNetworking" VALUE="all">
//<PARAM NAME="AllowFullScreen" VALUE="false">
// > > > ></OBJECT>

// javascript:alert(document.getElementsByTagName('embed').item(1).src)

void CbgamefuDlg::OnBnClickedButton2()
{
    // TODO: Add your control notification handler code here
    int sel=m_resolutionList.GetCurSel();
    DWORD index=m_resolutionList.GetItemData(sel);
    CRegKey r;
    r.Create(HKEY_CURRENT_USER, L"Software\\BGameFu");
    r.SetDWORDValue(L"width", dm[index].dmPelsWidth);
    r.SetDWORDValue(L"height", dm[index].dmPelsHeight);
    r.SetDWORDValue(L"freq", dm[index].dmDisplayFrequency);
}

void CbgamefuDlg::OnBnClickedButton3()
{
    CDialog::OnOK();
    // TODO: Add your control notification handler code here
}
