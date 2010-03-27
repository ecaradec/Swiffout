
struct CLicenceDlg : CDialog {
    enum { IDD = IDD_LICENCE_DIALOG };
    CLicenceDlg(int daysLeft) : CDialog(IDD) {
        m_daysLeft=daysLeft;
    }
    BOOL OnInitDialog() {
        if(m_daysLeft <= 0) {
            CString daysLeftStr; daysLeftStr.Format(L"%d", m_daysLeft);
            GetDlgItem(IDC_STATIC)->SetWindowText(L"Oh no !!! The trial period for SwiffOut has ended. "
                                                  L"As a faithful SwiffOut user, you should consider buying a licence. "
                                                  L"It'll allow you to skip this annoooying dialog and enjoy SwiffOut for as long as you want\n"
                                                  L"Now BUY a licence and enjoy a great game.");

            SetWindowPos(0,0,0,510,175,SWP_NOMOVE);
        } else {
            CString daysLeftStr; daysLeftStr.Format(L"%d", m_daysLeft);
            GetDlgItem(IDC_STATIC)->SetWindowText(L"You only have "+daysLeftStr+L" days left on your trial period. "
                                                  L"If you like SwiffOut, you should consider buying a licence. "
                                                  L"It'll allow you to skip this annoooying dialog and enjoy SwiffOut for as long as you want.\n\n"
                                                  L"Now click BUY or SKIP and enjoy a great game.");
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