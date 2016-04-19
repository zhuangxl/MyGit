
// p2pDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "p2p.h"
#include "p2pDlg.h"
#include "afxdialogex.h"
#include "Down.h"
#include "Share.h"   
#include "Search.h"
#include "Config.h"
#include "ADOConn.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

NOTIFYICONDATA nid;
// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// Cp2pDlg �Ի���



Cp2pDlg::Cp2pDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(Cp2pDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Cp2pDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_tabctrl);
}

BEGIN_MESSAGE_MAP(Cp2pDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_SYSTEMTRAY,OnSystemTray)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &Cp2pDlg::OnTcnSelchangeTab1)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// Cp2pDlg ��Ϣ�������

BOOL Cp2pDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

//	g_MainWnd=this;      //������ָ�븳��һ��ȫ�ֱ���,add 20131202
	//TAB�ؼ���ʼ����ʼ
	SetWindowText(_T("��ӭ��½F&F�ļ�����ϵͳ��"));   //����������;
	_fnToTray();						//���̹���;
	//��������ʾ
	if(::GetPrivateProfileInt(_T("�û�����"),_T("����������"),0,_T("C:\\P2P.ini"))==1)
	{
		m_FloatWnd.Create(CFloatWnd::IDD,this);//ע�⸸����Ϊthis
		m_FloatWnd.ShowWindow(SW_SHOW);
	}
	//��ʼ��	
	InitTab();

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void Cp2pDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else if(nID == SC_CLOSE)
	{
		if(::GetPrivateProfileInt(_T("�û�����"),_T("��С��������"),0,_T("C:\\P2P.ini"))==1)
		{
			ShowWindow(SW_HIDE);
			_fnToTray();//��С�������̺���   
			return;
		}
		else
		{
			ADOConn m_AdoConn;
			CString sql;
			sql.Format(_T("update userinfo set online=0 where name='%s'"),Username);
			m_AdoConn.ExecuteSQL((_bstr_t)sql);
			m_AdoConn.ExitConnect();
			DisConnectServer();
			OnDestroy();			
			exit(0);
		}
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void Cp2pDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		//CDialogEx::OnPaint();
		
		CPaintDC  dc(this);          
		CRect  rect;      
		GetClientRect(&rect);    
		CDC  dcMem;       
		dcMem.CreateCompatibleDC(&dc);      
		CBitmap  bmpBackground;       
		bmpBackground.LoadBitmap(IDB_BITMAP2);     //IDB_BITMAP�����Լ���ͼ��Ӧ��ID   
		BITMAP  bitmap;       
		bmpBackground.GetBitmap(&bitmap);       
		CBitmap  *pbmpOld=dcMem.SelectObject(&bmpBackground); 
		dc.StretchBlt(0,0,rect.Width(),rect.Height(),&dcMem,0,0,bitmap.bmWidth,bitmap.bmHeight,SRCCOPY);  
	
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR Cp2pDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void Cp2pDlg::InitTab()
{
	 m_tabctrl.InsertItem(0, _T("���ع���"));
     m_tabctrl.InsertItem(1, _T("�������"));
     m_tabctrl.InsertItem(2, _T("��������"));
     m_tabctrl.InsertItem(3, _T("���ù���"));

	 downdlg.Create(IDD_DOWN_DIALOG, &m_tabctrl);  
	 sharedlg.Create(IDD_SHARE_DIALOG, &m_tabctrl);   
	 searchdlg.Create(IDD_SEARCH_DIALOG, &m_tabctrl);   
	 configdlg.Create(IDD_CONFIG_DIALOG, &m_tabctrl);

	 CRect rc;   
	 m_tabctrl.GetClientRect(&rc);
	 rc.top  += 30;  
	 rc.left  += 0;   
	 rc.bottom -= 0;   
	 rc.right -= 0;
     downdlg.MoveWindow(&rc);   
     sharedlg.MoveWindow(&rc);   
     searchdlg.MoveWindow(&rc);   
     configdlg.MoveWindow(&rc);

	 downdlg.ShowWindow(true); 
	 sharedlg.ShowWindow(false); 
	 searchdlg.ShowWindow(false);
	 configdlg.ShowWindow(false);
	 m_tabctrl.SetCurSel(0);
}

/***������ʾ***/
void Cp2pDlg::_fnToTray() 
{ 
	nid.cbSize = sizeof( NOTIFYICONDATA ); 
	nid.hWnd = m_hWnd; 
	nid.uID = IDR_MAINFRAME; 
	nid.uFlags = NIF_MESSAGE|NIF_ICON|NIF_TIP; 
	nid.uCallbackMessage = WM_SYSTEMTRAY; 
	nid.hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME); 
	wcscpy_s(nid.szTip, _T("F&F�ļ�����ϵͳ")); 
	::Shell_NotifyIcon(NIM_ADD,&nid ); 
}

void Cp2pDlg::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	switch(m_tabctrl.GetCurSel())
    {
        case 0:
            {
                downdlg.ShowWindow(SW_SHOW);
                sharedlg.ShowWindow(SW_HIDE);
				searchdlg.ShowWindow(SW_HIDE);
                configdlg.ShowWindow(SW_HIDE);
                break;
            }
		case 1:
            {
				downdlg.ShowWindow(SW_HIDE);
                sharedlg.ShowWindow(SW_SHOW);
				searchdlg.ShowWindow(SW_HIDE);
                configdlg.ShowWindow(SW_HIDE);
                break;
            }
		case 2:
            {
				downdlg.ShowWindow(SW_HIDE);
                sharedlg.ShowWindow(SW_HIDE);
				searchdlg.ShowWindow(SW_SHOW);
                configdlg.ShowWindow(SW_HIDE);
                break;
            }
		case 3:
            {
				downdlg.ShowWindow(SW_HIDE);
                sharedlg.ShowWindow(SW_HIDE);
				searchdlg.ShowWindow(SW_HIDE);
                configdlg.ShowWindow(SW_SHOW);
                break;
            }
        default:;
        }
	*pResult = 0;
}

void Cp2pDlg::OnDownload(CString filename,CString filetype,CString filesize,CString filepath)
{

	Cp2pDlg *m_dlg=(Cp2pDlg *)(GetParent());
	if(m_dlg == NULL) 
	{
		AfxMessageBox(_T("��ȡ���Ի�����ָ��ʧ��!"));
		return;
	}
	m_dlg->downdlg.ShowWindow(true); 
	m_dlg->sharedlg.ShowWindow(false); 
	m_dlg->searchdlg.ShowWindow(false);
	m_dlg->configdlg.ShowWindow(false);
	m_dlg->m_tabctrl.SetCurSel(0);

	int nCount = m_dlg->downdlg.m_downlist.GetItemCount();	
	m_dlg->downdlg.m_downlist.InsertItem(nCount,_T(""));
	m_dlg->downdlg.m_downlist.SetItemText(nCount,1,filename);
	m_dlg->downdlg.m_downlist.SetItemText(nCount,2,filetype);
	m_dlg->downdlg.m_downlist.SetItemText(nCount,3,filesize);
//	m_dlg->downdlg.m_downlist.SetItemText(0,3,CalculateSize(filesize));
    m_dlg->downdlg.m_downlist.SetItemText(nCount,4,filepath);
	m_dlg->downdlg.m_downlist.SetItemText(nCount,5,_T("0"));
    m_dlg->downdlg.m_downlist.SetItemText(nCount,6,_T("������"));
}

/*
/***����ͼ�괦��***/
LRESULT Cp2pDlg::OnSystemTray(WPARAM wParam, LPARAM lParam) 
{ 
	if ( wParam = IDR_MAINFRAME ) 
	{ 
		switch( lParam ) 
		{ 
/*       case WM_RBUTTONUP:                                        // �Ҽ�����ʱ�����˵�
         {                  
             LPPOINT lpoint = new tagPOINT;
             ::GetCursorPos(lpoint);                    // �õ����λ��
             CMenu menu;
             menu.CreatePopupMenu();                    // ����һ������ʽ�˵�
             menu.AppendMenu(MF_STRING, WM_DESTROY, "�ر�");
             menu.TrackPopupMenu(TPM_LEFTALIGN, lpoint->x ,lpoint->y, this);
             HMENU hmenu = menu.Detach();
             menu.DestroyMenu();
             delete lpoint;
			 break;
          } */      
          case WM_LBUTTONDBLCLK:                        // ˫������Ĵ���
          {
              this->ShowWindow(SW_SHOWNORMAL);         // ��ʾ������
			  break;
          }       
		} 
	} 
	return 0; 
}
/*
void Cp2pDlg::DestroyWindow() 
{ 
	this->PostMessageW(WM_QUIT); 
	SendMessage(WM_CLOSE);
}
*/
/***�رճ������������ͼ��***/
void Cp2pDlg::OnDestroy() 
{ 
	CDialog::OnDestroy();
	NOTIFYICONDATA nid; 
	nid.cbSize = sizeof( NOTIFYICONDATA ); 
	nid.hWnd = m_hWnd; 
	nid.uID = IDR_MAINFRAME; 
	nid.uFlags = 0; 
	::Shell_NotifyIcon( NIM_DELETE,&nid ); 
}

void Cp2pDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	// TODO: �ڴ˴������Ϣ����������
	if(nType == SIZE_MINIMIZED)  
    {  
        ShowWindow(SW_HIDE); // ����С��ʱ������������              
    }  
}


BOOL Cp2pDlg::DestroyWindow()
{
	// TODO: �ڴ����ר�ô����/����û���
	Shell_NotifyIcon(NIM_DELETE, &nid); 
	return CDialogEx::DestroyWindow();
}

int Cp2pDlg::MessageShow(CString pTitle, CString pText, UINT uTimeout)
{ 
//	AfxMessageBox(_T("�����̷�����Ϣ��ʼ2!"));
    memset(&nid, 0, sizeof(nid));
    nid.cbSize = sizeof(NOTIFYICONDATA );
    nid.hWnd = m_hWnd;
	nid.uID = IDR_MAINFRAME; 
    nid.uFlags = NIF_MESSAGE|NIF_ICON|NIF_TIP|NIF_INFO;//�˳�Ա����������Щ������ԱΪ�Ϸ�����
    nid.uTimeout = uTimeout;		  //��ʾ������ʾ��ʱ��ʱ��
    nid.dwInfoFlags = NIIF_INFO;    //���ô˳�Ա������������ʾ������һ��ͼ��,NIIF_INFO:��Ϣͼ��
//	nid.uCallbackMessage = WM_USER + 0x1010;  
	nid.hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME); 
    wcscpy_s(nid.szInfo,pText);		  //szInfo:TCHAR,ָ��һ����\0�������ַ�����ָ��
    wcscpy_s(nid.szInfoTitle, pTitle);//�ַ���������Ϊ������ʾ�ı���
	wcscpy_s(nid.szTip, _T("�����"));
    ::Shell_NotifyIcon(NIM_MODIFY, &nid);
	return 0;
}


CString Cp2pDlg::CalculateSize(CString file_size)    
{
	//��ȡ�ļ���С��λ��ʾ;
	CString Cal_filesize;
	double f= _ttoi(file_size);
	if (f/1024>1)
	{
		if (f/1024/1024>1)
		{
			if (f/1024/1024/1024>1)
			{
				Cal_filesize.Format(_T("%.2fGB"),f/1024/1024/1024);   //�ļ���СGB��ʾ;
				return Cal_filesize;
			}
			Cal_filesize.Format(_T("%.2fMB"),f/1024/1024);     //�ļ���СMB��ʾ;
			return Cal_filesize;
		}
		Cal_filesize.Format(_T("%.2fKB"),f/1024);//�ļ���СKB��ʾ
		return Cal_filesize;
	}
	else
	{
		Cal_filesize.Format(_T("%.2fB"),f);        //�ļ���СB��ʾ
		return Cal_filesize;
	}
}

void Cp2pDlg::PauseAll()
{	
	for(int i=0; i<downdlg.m_downlist.GetItemCount(); i++)
	{		
		if(downdlg.m_downlist.GetItemText(i,6)==(_T("��������")))
		{
			downdlg.StopFlag[i]=FALSE;		
			downdlg.m_downlist.SetItemText(i,6,_T("��ͣ����"));
			continue;
		}			
	}
	return;
}


void Cp2pDlg::DisConnectServer()
{
	CString temp;
	CSocket endSocket;
	AfxSocketInit(NULL);
	if (!endSocket.Create())
	{
		AfxMessageBox(_T("�ͻ��������󶨶˿�ʧ��!"));
		return;
	}
	if(!endSocket.Connect(m_ServerAddress,_ttoi(m_ServerPort)))//m_ServerAddress-������ip,m_ServerPort-�������˿�
	{
		AfxMessageBox(_T("���ӷ�����ʧ��!"));
		return;
	}	
//	AfxMessageBox(_T("�ɹ����ӷ�����"));
	int stateCode = 3;
	char buffer[sizeof(stateCode)]="0";
	sprintf_s(buffer,"%d",stateCode);
	int sendbytes= endSocket.Send(buffer,sizeof(stateCode),0);
	if(sendbytes == SOCKET_ERROR)
	{		
		AfxMessageBox(_T("���������տͻ��˷��͵��˳�ϵͳ��Ϣʧ��!"));		
		return ;
	}
	endSocket.Close();
	return;
}
