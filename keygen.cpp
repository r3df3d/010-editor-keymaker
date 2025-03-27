#define _WIN32_IE 0x0600
#define WINVER 0x0600
#define _WIN32_WINNT 0x0600
#include <windows.h>
#include <commctrl.h>
#include <string>
#include <ctime>
#include <vector>
#include <sstream>
#include <iomanip>
#include <winreg.h>
#include <cmath>
#include "resource.h"
#pragma comment(lib, "comctl32.lib")
#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
HWND hName,hDatePicker,hUsers,hKey,hClearBtn,hInfoBtn,hActivateBtn,hMainWindow,hUsersUpDown;
HFONT hFont;
HBRUSH g_hBackgroundBrush=NULL,g_hEditBrush=NULL,g_hKeyBrush=NULL;
const char* REGISTRY_PATH="SOFTWARE\\SweetScape\\010 Editor";
const char* REGISTRY_NAME_KEY="Name";
const char* REGISTRY_PASSWORD_KEY="Password";
const unsigned int RAW[]={
969622712,594890599,1593930257,1052452058,890701766,1677293387,
394424968,266815521,1593930259,1211194088,2019260265,729421127,
953225874,1117854514,892543556,2000911200,514538256,1400963072,
486675118,1862498216,1136668818,758909582,1653935295,821063674,
888606944,687085563,890056597,1513495898,365692427,184357836,
677395407,863045227,818746596,391985767,1842768403,758385145,
1478392706,1985112985,1552765320,746944881,368385984,1758203153,
1240817244,660489060,756944316,1290697955,844453952,288239112,
1769473626,1922176006,826636519,391520695,1081548223,1069693142,
1244729994,766313326,1101031894,624951698,14501479,1794907983,
1460682958,1660839647,1104890686,897721119,1442187162,480708164,
454443986,1064446153,1595150448,1041527979,1145775470,1399869657,
255985995,802693350,2005610078,1897360642,2146073193,1538606632,
431647857,964049561,395138253,19164808,856904574,730737943,
708645054,1506870658,933323739,819349658,1780571206,236747382,
533160167,2042104933,670325172,2040165158,1354372994,705785180,
1669754395,1066536508,1426207888,1437950089,741941201,796931522,
1694313338,1290302874,1367672048,2039808424,1062939821,954597728,
1668694488,859122242,1369582617,140269649,53024683,729221831,
816609203,736893191,55706320,262747091,1629838835,581764799,
1488480625,1607077349,1879925846,1453945819,1521965565,856558562,
1530662365,1230847072,1404918182,1281256849,1238970765,272453753,
1640907491,2127893021,350314733,556617458,654390256,1648581270,
531062411,1862873022,1241517385,1471028336,5121143,1444839026,
1183580211,1573659650,2018540230,1487873223,234237236,898254600,
1023090193,728843548,2007454357,1451820833,267351539,302982385,
26807015,865879122,664886158,195503981,1625037691,1330347906,
1742434311,1330272217,1645368040,542321916,1782121222,411042851,
435386250,1176704752,1454246199,1136813916,1707755005,224415730,
201138891,989750331,1006010278,1147286905,406860280,840388503,
1282017578,1605698145,23396724,862145265,1898780916,1855549801,
1571519230,2083204840,1859876276,1602449334,1009413590,690816450,
86131931,345661263,1565025600,857544170,1329948960,1211787679,
994381573,991984748,1956475134,1098146294,1655714289,659576699,
689116467,1485584392,451884118,255590636,2108114754,1266252396,
1589326471,2019907768,15552498,1651075358,614606175,1656823678,
797605325,1681594366,2005080248,624648446,884695971,1526931791,
1595240948,439447199,2060396292,680093752,409028215,469068267,
195583689,1791650630,507724330,1364025102,1094582668,813049577,
32316922,1240756058,1176200235,2104494066,325396055,1796606917,
1709197385,525495836,1510101430,735526761,767523533,1374043776,
1559389967,567085571,1560216161,867042846,1001796703,1568754293,
628841972,173812827,379868455,384973125
};
LRESULT CALLBACK WindowProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
void CreateControls(HWND hwnd);
void UpdateKey();
void ClearRegistry();
void GetInfo();
void ActivateRegistration();
unsigned int encode_name(const std::string& name,int left,int num_users){
unsigned int result=0;
left*=17;
num_users*=15;
int x=0,y=0;
for(char c:name){
unsigned char char_code=toupper(c);
result+=RAW[char_code];
result^=RAW[(char_code+13)&0xFF];
result*=RAW[(char_code+0x2F)&0xFF];
result+=RAW[x&0xFF];
result+=RAW[left&0xFF]+RAW[num_users&0xFF];
x+=19;
y+=7;
left+=9;
num_users+=13;
}
return result;
}
unsigned int encode_users(int num_users){
return(num_users*11^0x3421)-0x4D30^0x7892;
}
unsigned int encode_password_date(int days_left,unsigned int offset){
return(days_left*17^0xA8E53167)+0x2C175^0xFF22C078^offset;
}
std::string format_license(const std::vector<unsigned char>& license_bytes){
std::stringstream ss;
for(size_t i=0;i<license_bytes.size();i+=2){
if(i>0)ss<<"-";
ss<<std::uppercase<<std::setfill('0')<<std::setw(4)<<std::hex
<<((license_bytes[i]<<8)|license_bytes[i+1]);
}
return ss.str();
}
std::string generate_year_license(const std::string& name,int num_users,int target_year,int target_month,int target_day){
std::vector<unsigned char> p(10,0);
p[3]=0xAC;
int days_left=0;
for(int year=1970;year<target_year;year++){
if((year%4==0&&year%100!=0)||(year%400==0)){
days_left+=366;
}else{
days_left+=365;
}
}
int days_in_month[]={31,28,31,30,31,30,31,31,30,31,30,31};
bool is_leap_year=(target_year%4==0&&target_year%100!=0)||(target_year%400==0);
if(is_leap_year){
days_in_month[1]=29;
}
for(int month=0;month<target_month-1;month++){
days_left+=days_in_month[month];
}
days_left+=target_day-1;
unsigned int checksum=encode_name(name,days_left,num_users);
p[4]=checksum&0xFF;
p[5]=(checksum>>8)&0xFF;
p[6]=(checksum>>16)&0xFF;
p[7]=(checksum>>24)&0xFF;
unsigned int encoded_date=encode_password_date(days_left,0x5B8C27);
unsigned int encoded_users=encode_users(num_users);
p[0]=p[6]^(encoded_date&0xFF);
p[8]=p[4]^((encoded_date>>8)&0xFF);
p[9]=p[5]^((encoded_date>>16)&0xFF);
p[2]=p[5]^(encoded_users&0xFF);
p[1]=p[7]^((encoded_users>>8)&0xFF);
return format_license(p);
}
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow){
INITCOMMONCONTROLSEX icc={sizeof(INITCOMMONCONTROLSEX)};
icc.dwICC=ICC_WIN95_CLASSES;
InitCommonControlsEx(&icc);
WNDCLASSEX wc={sizeof(WNDCLASSEX)};
wc.style=CS_HREDRAW|CS_VREDRAW;
wc.lpfnWndProc=WindowProc;
wc.hInstance=hInstance;
wc.hCursor=LoadCursor(NULL,IDC_ARROW);
wc.hbrBackground=CreateSolidBrush(RGB(32,32,32));
wc.lpszClassName="KeygenClass";
HICON hIcon=LoadIcon(hInstance,MAKEINTRESOURCE(IDI_ICON1));
if(hIcon){
wc.hIcon=hIcon;
wc.hIconSm=(HICON)LoadImage(hInstance,
MAKEINTRESOURCE(IDI_ICON1),
IMAGE_ICON,
16,
16,
LR_DEFAULTCOLOR
);
}
if(!RegisterClassEx(&wc)){
return 1;
}
hMainWindow=CreateWindowEx(
0,"KeygenClass"," Keygen By RedFed",
WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX,
CW_USEDEFAULT,CW_USEDEFAULT,400,270,
NULL,NULL,hInstance,NULL
);
if(!hMainWindow){
return 2;
}
g_hBackgroundBrush=CreateSolidBrush(RGB(32,32,32));
g_hEditBrush=CreateSolidBrush(RGB(45,45,45));
g_hKeyBrush=CreateSolidBrush(RGB(32,32,32));
CreateControls(hMainWindow);
ShowWindow(hMainWindow,nCmdShow);
UpdateWindow(hMainWindow);
MSG msg={};
while(GetMessage(&msg,NULL,0,0)){
TranslateMessage(&msg);
DispatchMessage(&msg);
}
DeleteObject(g_hBackgroundBrush);
DeleteObject(g_hEditBrush);
DeleteObject(g_hKeyBrush);
DeleteObject(hFont);
return 0;
}
void CreateControls(HWND hwnd){
LOGFONT lf={0};
lf.lfHeight=16;
lf.lfQuality=CLEARTYPE_QUALITY;
lstrcpy(lf.lfFaceName,"Segoe UI");
hFont=CreateFontIndirect(&lf);
lf.lfWeight=FW_BOLD;
HFONT hButtonFont=CreateFontIndirect(&lf);
lf.lfHeight=28;
HFONT hTitleFont=CreateFontIndirect(&lf);
HFONT hLabelFont=CreateFont(16,0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,
DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,
CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_DONTCARE,"Segoe UI");
HWND hTitle=CreateWindow("STATIC","010 Editor Keygen",
WS_CHILD|WS_VISIBLE|SS_CENTER,
0,25,400,35,hwnd,NULL,NULL,NULL);
DWORD editStyle=WS_CHILD|WS_VISIBLE|WS_BORDER|ES_AUTOHSCROLL;
int editHeight=24;
CreateWindow("STATIC","Name:",WS_CHILD|WS_VISIBLE|SS_LEFT,
30,75,50,editHeight,hwnd,NULL,NULL,NULL);
hName=CreateWindowEx(WS_EX_CLIENTEDGE,"EDIT","",editStyle&~WS_BORDER,
85,73,130,editHeight,hwnd,NULL,NULL,NULL);
CreateWindow("STATIC","Year:",WS_CHILD|WS_VISIBLE|SS_LEFT,
235,75,40,editHeight,hwnd,NULL,NULL,NULL);
hDatePicker=CreateWindowEx(0,DATETIMEPICK_CLASS,"",
WS_CHILD|WS_VISIBLE|DTS_SHORTDATECENTURYFORMAT,
280,73,100,editHeight,
hwnd,NULL,NULL,NULL);
SYSTEMTIME stMin,stMax;
GetLocalTime(&stMin);
stMax=stMin;
stMax.wYear=2999;
DateTime_SetRange(hDatePicker,GDTR_MIN|GDTR_MAX,&stMin);
CreateWindow("STATIC","Users:",WS_CHILD|WS_VISIBLE|SS_LEFT,
30,105,50,editHeight,hwnd,NULL,NULL,NULL);
hUsers=CreateWindowEx(WS_EX_CLIENTEDGE,"EDIT","1",
WS_CHILD|WS_VISIBLE|ES_NUMBER|ES_READONLY|ES_CENTER,
85,103,50,editHeight,
hwnd,(HMENU)1001,NULL,NULL);
hUsersUpDown=CreateWindowEx(0,UPDOWN_CLASS,"",
WS_CHILD|WS_VISIBLE|UDS_ALIGNRIGHT|
UDS_SETBUDDYINT|UDS_NOTHOUSANDS|UDS_ARROWKEYS,
0,0,0,0,
hwnd,NULL,NULL,NULL);
SendMessage(hUsersUpDown,UDM_SETRANGE,0,MAKELPARAM(9999,1));
SendMessage(hUsersUpDown,UDM_SETBUDDY,(WPARAM)hUsers,0);
SendMessage(hUsersUpDown,UDM_SETPOS,0,MAKELPARAM(1,0));
hKey=CreateWindowEx(0,"EDIT","",
WS_CHILD|WS_VISIBLE|ES_READONLY|ES_CENTER,
0,145,400,35,hwnd,NULL,NULL,NULL);
HFONT hKeyFont=CreateFont(25,0,0,0,FW_BOLD,FALSE,FALSE,FALSE,
DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,
CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_DONTCARE,"Consolas");
SendMessage(hKey,WM_SETFONT,(WPARAM)hKeyFont,TRUE);
DWORD btnStyle=WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_CENTER;
int btnWidth=80;
int btnHeight=26;
int totalBtnWidth=(btnWidth*3)+(20*2);
int startX=(400-totalBtnWidth)/2;
hActivateBtn=CreateWindow("BUTTON","Activate",btnStyle,
startX,200,btnWidth,btnHeight,hwnd,(HMENU)1,NULL,NULL);
hClearBtn=CreateWindow("BUTTON","Clear",btnStyle,
startX+btnWidth+20,200,btnWidth,btnHeight,hwnd,(HMENU)2,NULL,NULL);
hInfoBtn=CreateWindow("BUTTON","Info",btnStyle,
startX+(btnWidth+20)*2,200,btnWidth,btnHeight,hwnd,(HMENU)3,NULL,NULL);
SendMessage(hTitle,WM_SETFONT,(WPARAM)hTitleFont,TRUE);
HWND labels[]={
GetDlgItem(hwnd,-1),
GetDlgItem(hwnd,-2),
GetDlgItem(hwnd,-3)
};
for(HWND label:labels){
if(label)SendMessage(label,WM_SETFONT,(WPARAM)hLabelFont,TRUE);
}
SendMessage(hName,WM_SETFONT,(WPARAM)hFont,TRUE);
SendMessage(hUsers,WM_SETFONT,(WPARAM)hFont,TRUE);
SendMessage(hClearBtn,WM_SETFONT,(WPARAM)hButtonFont,TRUE);
SendMessage(hInfoBtn,WM_SETFONT,(WPARAM)hButtonFont,TRUE);
SendMessage(hActivateBtn,WM_SETFONT,(WPARAM)hButtonFont,TRUE);
SetWindowLongPtr(hKey,GWLP_USERDATA,(LONG_PTR)CreateSolidBrush(RGB(45,45,45)));
SendMessage(hDatePicker,WM_SETFONT,(WPARAM)hFont,TRUE);
DeleteObject(hTitleFont);
DeleteObject(hButtonFont);
}
LRESULT CALLBACK WindowProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam){
switch(uMsg){
case WM_COMMAND:
switch(LOWORD(wParam)){
case 1:ActivateRegistration();break;
case 2:ClearRegistry();break;
case 3:GetInfo();break;
}
break;
case WM_NOTIFY:{
NMHDR* nmhdr=(NMHDR*)lParam;
if(nmhdr->hwndFrom==hDatePicker){
UpdateKey();
}
break;
}
case WM_VSCROLL:
case WM_HSCROLL:{
HWND hScroll=(HWND)lParam;
if(hScroll==hUsersUpDown){
DefWindowProc(hwnd,uMsg,wParam,lParam);
UpdateKey();
return 0;
}
break;
}
case WM_CTLCOLOREDIT:{
HDC hdc=(HDC)wParam;
HWND hEdit=(HWND)lParam;
if(hEdit==hKey){
SetTextColor(hdc,RGB(255,140,0));
SetBkColor(hdc,RGB(32,32,32));
return(LRESULT)g_hKeyBrush;
}else{
SetTextColor(hdc,RGB(240,240,240));
SetBkColor(hdc,RGB(45,45,45));
return(LRESULT)g_hEditBrush;
}
}
case WM_CTLCOLORSTATIC:{
HDC hdcStatic=(HDC)wParam;
SetTextColor(hdcStatic,RGB(240,240,240));
SetBkColor(hdcStatic,RGB(32,32,32));
return(LRESULT)g_hBackgroundBrush;
}
case WM_DESTROY:
PostQuitMessage(0);
return 0;
}
return DefWindowProc(hwnd,uMsg,wParam,lParam);
}
void UpdateKey(){
static char name[256];
static char key_buffer[256];
GetWindowText(hName,name,256);
if(name[0]=='\0'){
SetWindowText(hKey,"");
return;
}
try{
int usersNum=GetDlgItemInt(hMainWindow,1001,NULL,FALSE);
SYSTEMTIME st;
DateTime_GetSystemtime(hDatePicker,&st);
std::string key=generate_year_license(name,usersNum,st.wYear,st.wMonth,st.wDay);
SetWindowText(hKey,key.c_str());
}catch(...){
SetWindowText(hKey,"Invalid input");
}
}
void DeleteRegistryKey(HKEY hKeyRoot,LPCSTR subKey){
HKEY hKey;
LONG result=RegOpenKeyExA(hKeyRoot,subKey,0,KEY_ALL_ACCESS,&hKey);
if(result!=ERROR_SUCCESS){
return;
}
char szDelKey[MAX_PATH];
DWORD dwSize=MAX_PATH;
FILETIME ftWrite;
while(RegEnumKeyExA(hKey,0,szDelKey,&dwSize,NULL,NULL,NULL,&ftWrite)==ERROR_SUCCESS){
DeleteRegistryKey(hKey,szDelKey);
dwSize=MAX_PATH;
}
RegCloseKey(hKey);
RegDeleteKeyA(hKeyRoot,subKey);
}
void ClearRegistry(){
try{
DeleteRegistryKey(HKEY_CURRENT_USER,"SOFTWARE\\SweetScape");
MessageBox(hMainWindow,"Registry cleared successfully!","Success",MB_OK|MB_ICONINFORMATION);
}
catch(...){
MessageBox(hMainWindow,"Error clearing registry.","Error",MB_OK|MB_ICONERROR);
}
}
void GetInfo(){
HKEY hKey;
if(RegOpenKeyEx(HKEY_CURRENT_USER,REGISTRY_PATH,0,KEY_READ,&hKey)==ERROR_SUCCESS){
char name[256],password[256];
DWORD size=sizeof(name);
RegQueryValueEx(hKey,REGISTRY_NAME_KEY,NULL,NULL,(LPBYTE)name,&size);
size=sizeof(password);
RegQueryValueEx(hKey,REGISTRY_PASSWORD_KEY,NULL,NULL,(LPBYTE)password,&size);
RegCloseKey(hKey);
std::string info="Registered to: "+std::string(name)+"\nLicense Key: "+std::string(password);
MessageBox(hMainWindow,info.c_str(),"Registration Info",MB_OK|MB_ICONINFORMATION);
}else{
MessageBox(hMainWindow,"010 Editor is not registered.","Registration Info",MB_OK|MB_ICONWARNING);
}
}
void ActivateRegistration(){
char name[256];
char license[256];
GetWindowText(hName,name,256);
GetWindowText(hKey,license,256);
if(strlen(license)==0){
MessageBox(hMainWindow,"Please generate License key first.","Error",MB_OK|MB_ICONWARNING);
return;
}
HKEY hRegKey;
DWORD disp;
if(RegCreateKeyExA(HKEY_CURRENT_USER,REGISTRY_PATH,0,NULL,0,KEY_ALL_ACCESS,NULL,&hRegKey,&disp)==ERROR_SUCCESS){
RegSetValueExA(hRegKey,REGISTRY_NAME_KEY,0,REG_SZ,(BYTE*)name,strlen(name)+1);
RegSetValueExA(hRegKey,REGISTRY_PASSWORD_KEY,0,REG_SZ,(BYTE*)license,strlen(license)+1);
RegCloseKey(hRegKey);
MessageBox(hMainWindow,"License Activated. Enjoy!","Success",MB_OK|MB_ICONINFORMATION);
}else{
MessageBox(hMainWindow,"Error during activation.","Error",MB_OK|MB_ICONERROR);
}
}
