
// p2pDlg.cpp : 实现文件
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
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// Cp2pDlg 对话框



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


// Cp2pDlg 消息处理程序

BOOL Cp2pDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

//	g_MainWnd=this;      //主窗口指针赋给一个全局变量,add 20131202
	//TAB控件初始化开始
	SetWindowText(_T("欢迎登陆F&F文件传输系统！"));   //任务栏标题;
	_fnToTray();						//托盘功能;
	//悬浮窗显示
	if(::GetPrivateProfileInt(_T("用户设置"),_T("开启悬浮窗"),0,_T("C:\\P2P.ini"))==1)
	{
		m_FloatWnd.Create(CFloatWnd::IDD,this);//注意父窗口为this
		m_FloatWnd.ShowWindow(SW_SHOW);
	}
	//初始化	
	InitTab();

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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
		if(::GetPrivateProfileInt(_T("用户设置"),_T("最小化到托盘"),0,_T("C:\\P2P.ini"))==1)
		{
			ShowWindow(SW_HIDE);
			_fnToTray();//最小化到托盘函数   
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void Cp2pDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
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
		bmpBackground.LoadBitmap(IDB_BITMAP2);     //IDB_BITMAP是你自己的图对应的ID   
		BITMAP  bitmap;       
		bmpBackground.GetBitmap(&bitmap);       
		CBitmap  *pbmpOld=dcMem.SelectObject(&bmpBackground); 
		dc.StretchBlt(0,0,rect.Width(),rect.Height(),&dcMem,0,0,bitmap.bmWidth,bitmap.bmHeight,SRCCOPY);  
	
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR Cp2pDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void Cp2pDlg::InitTab()
{
	 m_tabctrl.InsertItem(0, _T("下载管理"));
     m_tabctrl.InsertItem(1, _T("共享管理"));
     m_tabctrl.InsertItem(2, _T("搜索管理"));
     m_tabctrl.InsertItem(3, _T("配置管理"));

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

/***托盘显示***/
void Cp2pDlg::_fnToTray() 
{ 
	nid.cbSize = sizeof( NOTIFYICONDATA ); 
	nid.hWnd = m_hWnd; 
	nid.uID = IDR_MAINFRAME; 
	nid.uFlags = NIF_MESSAGE|NIF_ICON|NIF_TIP; 
	nid.uCallbackMessage = WM_SYSTEMTRAY; 
	nid.hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME); 
	wcscpy_s(nid.szTip, _T("F&F文件传输系统")); 
	::Shell_NotifyIcon(NIM_ADD,&nid ); 
}

void Cp2pDlg::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
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
		AfxMessageBox(_T("获取主对话框句柄指针失败!"));
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
    m_dlg->downdlg.m_downlist.SetItemText(nCount,6,_T("待下载"));
}

/*
/***托盘图标处理***/
LRESULT Cp2pDlg::OnSystemTray(WPARAM wParam, LPARAM lParam) 
{ 
	if ( wParam = IDR_MAINFRAME ) 
	{ 
		switch( lParam ) 
		{ 
/*       case WM_RBUTTONUP:                                        // 右键起来时弹出菜单
         {                  
             LPPOINT lpoint = new tagPOINT;
             ::GetCursorPos(lpoint);                    // 得到鼠标位置
             CMenu menu;
             menu.CreatePopupMenu();                    // 声明一个弹出式菜单
             menu.AppendMenu(MF_STRING, WM_DESTROY, "关闭");
             menu.TrackPopupMenu(TPM_LEFTALIGN, lpoint->x ,lpoint->y, this);
             HMENU hmenu = menu.Detach();
             menu.DestroyMenu();
             delete lpoint;
			 break;
          } */      
          case WM_LBUTTONDBLCLK:                        // 双击左键的处理
          {
              this->ShowWindow(SW_SHOWNORMAL);         // 显示主窗口
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
/***关闭程序后销毁托盘图标***/
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
	// TODO: 在此处添加消息处理程序代码
	if(nType == SIZE_MINIMIZED)  
    {  
        ShowWindow(SW_HIDE); // 当最小化时，隐藏主窗口              
    }  
}


BOOL Cp2pDlg::DestroyWindow()
{
	// TODO: 在此添加专用代码和/或调用基类
	Shell_NotifyIcon(NIM_DELETE, &nid); 
	return CDialogEx::DestroyWindow();
}

int Cp2pDlg::MessageShow(CString pTitle, CString pText, UINT uTimeout)
{ 
//	AfxMessageBox(_T("向托盘发送消息开始2!"));
    memset(&nid, 0, sizeof(nid));
    nid.cbSize = sizeof(NOTIFYICONDATA );
    nid.hWnd = m_hWnd;
	nid.uID = IDR_MAINFRAME; 
    nid.uFlags = NIF_MESSAGE|NIF_ICON|NIF_TIP|NIF_INFO;//此成员表明具体哪些其他成员为合法数据
    nid.uTimeout = uTimeout;		  //表示气球提示超时的时间
    nid.dwInfoFlags = NIIF_INFO;    //设置此成员用来给气球提示框增加一个图标,NIIF_INFO:信息图标
//	nid.uCallbackMessage = WM_USER + 0x1010;  
	nid.hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME); 
    wcscpy_s(nid.szInfo,pText);		  //szInfo:TCHAR,指向一个以\0结束的字符串的指针
    wcscpy_s(nid.szInfoTitle, pTitle);//字符串的内容为气球提示的标题
	wcscpy_s(nid.szTip, _T("已完成"));
    ::Shell_NotifyIcon(NIM_MODIFY, &nid);
	return 0;
}


CString Cp2pDlg::CalculateSize(CString file_size)    
{
	//获取文件大小单位表示;
	CString Cal_filesize;
	double f= _ttoi(file_size);
	if (f/1024>1)
	{
		if (f/1024/1024>1)
		{
			if (f/1024/1024/1024>1)
			{
				Cal_filesize.Format(_T("%.2fGB"),f/1024/1024/1024);   //文件大小GB显示;
				return Cal_filesize;
			}
			Cal_filesize.Format(_T("%.2fMB"),f/1024/1024);     //文件大小MB显示;
			return Cal_filesize;
		}
		Cal_filesize.Format(_T("%.2fKB"),f/1024);//文件大小KB显示
		return Cal_filesize;
	}
	else
	{
		Cal_filesize.Format(_T("%.2fB"),f);        //文件大小B显示
		return Cal_filesize;
	}
}

void Cp2pDlg::PauseAll()
{	
	for(int i=0; i<downdlg.m_downlist.GetItemCount(); i++)
	{		
		if(downdlg.m_downlist.GetItemText(i,6)==(_T("正在下载")))
		{
			downdlg.StopFlag[i]=FALSE;		
			downdlg.m_downlist.SetItemText(i,6,_T("暂停下载"));
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
		AfxMessageBox(_T("客户端心跳绑定端口失败!"));
		return;
	}
	if(!endSocket.Connect(m_ServerAddress,_ttoi(m_ServerPort)))//m_ServerAddress-服务器ip,m_ServerPort-服务器端口
	{
		AfxMessageBox(_T("连接服务器失败!"));
		return;
	}	
//	AfxMessageBox(_T("成功连接服务器"));
	int stateCode = 3;
	char buffer[sizeof(stateCode)]="0";
	sprintf_s(buffer,"%d",stateCode);
	int sendbytes= endSocket.Send(buffer,sizeof(stateCode),0);
	if(sendbytes == SOCKET_ERROR)
	{		
		AfxMessageBox(_T("服务器接收客户端发送的退出系统信息失败!"));		
		return ;
	}
	endSocket.Close();
	return;
}
