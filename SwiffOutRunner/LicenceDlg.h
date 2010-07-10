struct CLicenceDlg : CDialog {
    enum { IDD = IDD_LICENCE_DIALOG };
    CFont f;
    CLicenceDlg(int daysLeft) : CDialog(IDD) {
        m_daysLeft=daysLeft;
        f.CreateFont(80,0,0,0,0,0,0,0,0,0,0,0,0,L"Arial");
    }
    BOOL OnInitDialog() {
        CString daysLeftStr; daysLeftStr.Format(L"%d", m_daysLeft);
        GetDlgItem(IDC_TRIAL)->SetWindowText(daysLeftStr);
        
        
        GetDlgItem(IDC_TRIAL)->SetFont(&f);


        if(m_daysLeft <= 0) {            
            GetDlgItem(IDC_STATIC)->SetWindowText(L"Oh no !!! You used all your tries on websites that aren't partners of SwiffOut.\n\n"
                                                  L"Don't worry, you can still play on SwiffOutGames.com unlimited, however as a faithful SwiffOut user, you should consider buying a licence : it is cheap, "
                                                  L"it'll allow you to use SwiffOut unlimited everywhere and will support future developpements.\n\n"
                                                  L"Now click BUY to play this game fullscreen or go to SwiffOutGames.com.");
            SetWindowPos(0,0,0,510,205,SWP_NOMOVE);
        } else {
            GetDlgItem(IDC_STATIC)->SetWindowText(L"Playing on websites that aren't partners of SwiffOut is a feature of SwiffOut Extended.\n\nDon't worry, you still have "+daysLeftStr+L" remaining tries. "
                                                  L"As a faithful SwiffOut user, you should consider buying a licence : it is cheap, "
                                                  L"will allow you to use SwiffOut unlimited everywhere and will support future developpements.\n\n"
                                                  L"Now click BUY to get a license or SKIP and enjoy a great game.");
        }

        return CDialog::OnInitDialog();
    }
    void DoDataExchange(CDataExchange* pDX)
    {
        CDialog::DoDataExchange(pDX);
        DDX_Control(pDX, IDC_LIC1, m_licence);
    }
    void OnBnBuy() {
        ShellExecute(0,0,L"http://www.grownsoftware.com/swiffout", 0, 0, SW_NORMAL);
    }
    void OnBnConfirm() {
        CString lic;
        m_licence.GetWindowText(lic);

        CLSID clsidLic;
        CLSIDFromString(CComBSTR(lic),&clsidLic);
        if(CheckLicKey((char*)&clsidLic, 0x5fd7ef97, 0x97b8a7d2)) {                        
            MessageBox(L"Thank you",L"Licence is correct", MB_OK);

            CRegKey r;
            r.Create(HKEY_CURRENT_USER, L"Software\\SwiffOut");
            r.SetStringValue(L"licence", lic);

            EndDialog(0);
        } else {
            MessageBox(L"This licence is not valid. Try to copy paste it to avoid errors.",L"Ooops, invalid licence", MB_OK);
        }

        //MessageBox(lic);
        //EndDialog(0);
    }
    void OnBnSkip() {
        EndDialog(0);
    }

    DECLARE_MESSAGE_MAP()

    int    m_daysLeft;
    HBRUSH m_hRedBrush;
    CEdit  m_licence;
};

BEGIN_MESSAGE_MAP(CLicenceDlg, CDialog)
    ON_BN_CLICKED(IDC_BUY_LICENCE, &CLicenceDlg::OnBnBuy)
    ON_BN_CLICKED(IDC_CONFIRM, &CLicenceDlg::OnBnConfirm)
    ON_BN_CLICKED(IDC_SKIP, &CLicenceDlg::OnBnSkip)    
END_MESSAGE_MAP()