#include "defines.h"
#define dirUp 0
#define dirDn 1
#define dirMid 2
#define dirLeft 4
#define dirRight 8
#define dirCenter 16

struct h
	{
	DWORD MdeT;
	DWORD localeId;
	DWORD char1No;
	DWORD char2No;
	DWORD fontData1Addr;
	DWORD fontData2Addr;
	DWORD fontData1Size;
	DWORD fontData2Size;
	//unsigned char unk[19]; // messes up sizeof-based reading
	DWORD unk1,unk2,unk3,unk4; //16B
	unsigned char unk5,unk6,unk7; //+3B
	};

//unified char struct
struct ch
	{
	WORD no;
	unsigned char *data;
	WORD unicodeNo;
	};

HWND hMain,hStatus,hChars;
HMENU hMenu;
struct ch *font, *font2, *currFont;
struct h header;
DWORD sel;
unsigned char fontWidth;
WORD cStart, cBlockSize;
SCROLLINFO si;
unsigned char prgState;
DWORD charNo;
char *fPath;
unsigned char ch1Size;
unsigned char showFullPath;

void moveChar(struct ch *in, unsigned char type, unsigned char dir)
  {
  unsigned char i,f,comp;
  if(dir == dirLeft || dir == dirRight)
    {
    if(dir == dirRight)
      comp = 1;
    else
      comp = 128;
    if(type == 0)
      {
      f = 7;
      while(f)
        {
        for(i = 0; i != 16; ++i)
          if(in->data[i] & comp)
            f = 0;
        if(f)
          {
          if(dir == dirLeft)
            for(i = 0; i != 16; ++i)
              in->data[i] <<= 1;
          else
            for(i = 0; i != 16; ++i)
              in->data[i] >>= 1;
          --f;
          }
        }
      }
    }
  else if(dir == dirUp || dir == dirDn)
    {
    f = 15;
    if(dir == dirUp)
      comp = 0;
    else
      comp = 15;
    while(f)
      {
      if(in->data[comp])
        f = 0;
      if(f)
        {
        if(dir == dirUp)
          {
          for(i = 0; i != 15; ++i)
            in->data[i] = in->data[i+1];
          in->data[15] = 0;
          }
        else
          {
          for(i = 15; i != 0; --i)
            in->data[i] = in->data[i-1];
          in->data[0] = 0;
          }
        --f;
        }
      }
    }
  else if(dir == dirMid)
    {
    }
  }

void updateTitle(void)
  {
  char *sTmp = HeapAlloc(GetProcessHeap(),0,lstrlen(fPath)+23+(prgState>1?1:0));
  WORD i = lstrlen(fPath)-1;
  if(!showFullPath)
    {
    while(i != 0 && fPath[i] != '\\')
      --i;
    ++i;
    }
  else
    i = 0;
  wsprintf(sTmp,(prgState>1?"*%s - %s":"%s - %s"),fPath+i,wndName);
  SetWindowText(hMain,sTmp);
  HeapFree(GetProcessHeap(),0,sTmp);
  }

void saveFile(void);

unsigned char askSave(void)
  {
  int tmp = MessageBox(hMain,"Save changes to opened file?","",mbStyle|MB_YESNOCANCEL);
  if(tmp == IDYES)
    saveFile();
  else if(tmp == IDCANCEL)
    return 0;
  return 1;
  }

char *openDlg(HWND *wnd,unsigned char mode)
  {
  BOOL r;
  char *sTmp = HeapAlloc(GetProcessHeap(),0,257);
  ZeroMemory(sTmp,257);
  OPENFILENAME ofn;
  ZeroMemory(&ofn,sizeof(OPENFILENAME));
  ofn.lStructSize = sizeof(OPENFILENAME);
  ofn.hwndOwner = *wnd;
  ofn.lpstrFilter = "Boot font files (*.bin)\0*.bin\0";
  ofn.nFilterIndex = 1; //selected by user
  ofn.lpstrFile = sTmp;
  ofn.nMaxFile = 256;
  ofn.lpstrDefExt = "bin";
  if(!mode)
    {
    ofn.Flags = OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST;
    r = GetOpenFileName(&ofn);
    }
  else
    {
    ofn.Flags = OFN_NOREADONLYRETURN | OFN_OVERWRITEPROMPT;
    // OFN_PATHMUSTEXIST ?
    r = GetSaveFileName(&ofn);
    }
  if(r)
    return sTmp;
  else
    {
    HeapFree(GetProcessHeap(),0,sTmp);
    return NULL;
    }
  }

void drawGlyph(HDC dc,unsigned char *data,WORD x,WORD y,WORD xBytes,WORD scale) //buffer? Here or in hChars
	{
	COLORREF color = 0;
	HBRUSH brush = CreateSolidBrush(color);
	RECT r;
	unsigned char i,j,k;
	for(i = 0; i != 16; ++i) //row
		for(j = 0; j != xBytes; ++j) //width
			for(k = 0; k != 8; ++k) //shr
				if(*(data+(i*xBytes)+j) & 128>>k)
					{
					if(scale == 1)
						SetPixel(dc,x+(8*j)+k,y+i,color);
					else
						{
						r.left = x+(8*j*scale)+k*scale;
						r.right = r.left+scale;
						r.top = y+i*scale;
						r.bottom = r.top+scale;
						FillRect(dc,&r,brush);
						}
					}
	DeleteObject(brush);
	}

void saveFile(void)
  {
  DWORD tmp,i;
  HANDLE hOut = CreateFile(fPath,fwAccess,fwShare,NULL,fwOpenType,fwFlags,NULL);
  if(!hOut)
    MessageBox(0,"Posrał się zapis","",mbStyle);
  WriteFile(hOut,&header,sizeof(struct h),&tmp,NULL);
  SetFilePointer(hOut,header.fontData1Addr,NULL,FILE_BEGIN);
  for(i = 0; i != header.char1No; ++i)
    {
    WriteFile(hOut,&font[i].no,1,&tmp,NULL);
    WriteFile(hOut,font[i].data,16,&tmp,NULL);
    if(ch1Size == 19)
      WriteFile(hOut,&font[i].unicodeNo,2,&tmp,NULL);
    }
  SetFilePointer(hOut,header.fontData2Addr,NULL,FILE_BEGIN);
	for(i = 0; i != header.char2No; ++i)
		{
		WriteFile(hOut,&font2[i].no,2,&tmp,NULL);
		WriteFile(hOut,font2[i].data,32,&tmp,NULL);
		if(ch1Size == 19) //only unicode in 8x8 have 16x16?
			WriteFile(hOut,&font2[i].unicodeNo,2,&tmp,NULL);
		}
  CloseHandle(hOut);
  }

void loadFile(void)
	{
	DWORD tmp,i;
	for(i = 0; i != header.char1No; ++i)
		HeapFree(GetProcessHeap(),0,font[i].data);
	for(i = 0; i != header.char2No; ++i)
		HeapFree(GetProcessHeap(),0,font2[i].data);
	HeapFree(GetProcessHeap(),0,font);
	HeapFree(GetProcessHeap(),0,font2);
	HANDLE hIn = CreateFile(fPath,foAccess,foShare,NULL,foOpenType,foFlags,NULL);
	if(!hIn)
		MessageBox(0,"Posrało się","Nie ma pliku",mbStyle);
	ReadFile(hIn,&header,sizeof(struct h),&tmp,NULL);
	charNo = header.char1No;
	ch1Size = header.fontData1Size/header.char1No;
	SetFilePointer(hIn,header.fontData1Addr,NULL,FILE_BEGIN);
	font = HeapAlloc(GetProcessHeap(),0,sizeof(struct ch)*header.char1No);
	for(i = 0; i != header.char1No; ++i)
		{
		ReadFile(hIn,&font[i].no,1,&tmp,NULL);
		font[i].data = HeapAlloc(GetProcessHeap(),0,16);
		ReadFile(hIn,font[i].data,16,&tmp,NULL);
		if(ch1Size == 19)
			ReadFile(hIn,&font[i].unicodeNo,2,&tmp,NULL);
		else
			font[i].unicodeNo = 0;
		}
	SetFilePointer(hIn,header.fontData2Addr,NULL,FILE_BEGIN);
	font2 = HeapAlloc(GetProcessHeap(),0,sizeof(struct ch)*header.char2No);
	//TODO: other size of ch2 in 2k?
	for(i = 0; i != header.char2No; ++i)
		{
		ReadFile(hIn,&font2[i].no,2,&tmp,NULL);
		font2[i].data = HeapAlloc(GetProcessHeap(),0,32);
		ReadFile(hIn,font2[i].data,32,&tmp,NULL);
		if(ch1Size == 19)
			ReadFile(hIn,&font2[i].unicodeNo,2,&tmp,NULL);
		}
	CloseHandle(hIn);
	updateTitle();
	SendMessage(hMain,WM_COMMAND,menu816,0);
	ShowWindow(hChars,SW_SHOW);
	}

void moveStr(char **dest,char *src)
  {
  HeapFree(GetProcessHeap(),0,*dest); //***
  *dest = HeapAlloc(GetProcessHeap(),0,lstrlen(src));
  lstrcpy(*dest,src);
  HeapFree(GetProcessHeap(),0,src); //***
  }
LRESULT CALLBACK wpMain(HWND hWnd,UINT msg, WPARAM wPar, LPARAM lPar)
	{
	DWORD tmp;
	HPEN penNew,penOld;
	DWORD i;
	RECT r;
	char *sTmp;
	switch(msg)
		{
    case WM_DROPFILES:
      if(DragQueryFile((HDROP)wPar,0xFFffFFff,NULL,0) != 1)
        MessageBox(hWnd,"Drag single file plz","Error",mbStyle|MB_ICONHAND);
      else
        {
        tmp = DragQueryFile((HDROP)wPar,0,NULL,1);
        sTmp = HeapAlloc(GetProcessHeap(),0,tmp+1);
        DragQueryFile((HDROP)wPar,0,sTmp,tmp+1);
        if(!askSave())
          break;
        if(!lstrcmpi(sTmp+tmp-4,".bin") || MessageBox(hWnd,"Dragged file is probably not .bin file\n\n?Open it anyway","",mbStyle|MB_YESNO|MB_ICONQUESTION) == IDYES)
          {
          HeapFree(GetProcessHeap(),0,fPath);
          fPath = HeapAlloc(GetProcessHeap(),0,lstrlen(sTmp));
          lstrcpy(fPath,sTmp);
          MENUITEMINFO mii;
          loadFile();
          prgState = 1;
          mii.cbSize = sizeof(MENUITEMINFO);
          mii.fMask = MIIM_STATE;
          mii.fState = MFS_ENABLED;
          SetMenuItemInfo(hMenu,menuInfo,0,&mii);
          }
        HeapFree(GetProcessHeap(),0,sTmp);
        }
      break;
		case WM_MOUSEWHEEL:
			if((short)HIWORD(wPar) > 0)
				i = SB_PAGEUP;
			else
				i = SB_PAGEDOWN;
			SendMessage(hChars,WM_VSCROLL,i,0);
			break;
		case WM_CREATE:
			cStart = chWStart+cMargin;
			prgState = 0;
			break;
		case WM_PAINT:
      ;
			PAINTSTRUCT psMain;
			HDC dcMain = BeginPaint(hMain,&psMain);
			if(prgState)
				{
				//for sizing, maybe some kind of `if`?
				GetClientRect(hWnd,&r);
				tmp = (r.bottom-r.top)-20; //new hChars height
				si.nPage = (tmp/(2+16+2));
				si.fMask = SIF_PAGE;
				SetScrollInfo(hChars,SB_VERT,&si,0);
				MoveWindow(hChars,0,0,chWStart,tmp,1); //something redraws scrollbar strangely
				//sizing end

				Rectangle(dcMain,cStart-1,9,cStart+1+cBlockSize*(8*fontWidth),11+16*cBlockSize);
				drawGlyph(dcMain,currFont[sel].data,cStart,10,fontWidth,cBlockSize);
				penNew = CreatePen(PS_SOLID,1,0xAAAAAA);
				penOld = SelectObject(dcMain,penNew);
				for(i = 1; i != 16; ++i)
					{
					MoveToEx(dcMain,cStart,10+(i*cBlockSize),NULL);
					LineTo(dcMain,cStart+(8*fontWidth)*cBlockSize,10+(i*cBlockSize));
					}
				for(i = 1; i != (8*fontWidth); ++i)
					{
					MoveToEx(dcMain,cStart+i*cBlockSize,10,NULL);
					LineTo(dcMain,cStart+i*cBlockSize,10+16*cBlockSize);
					}
				SelectObject(dcMain,penOld);
				DeleteObject(penNew);
				penNew = CreatePen(PS_SOLID,1,0xFF);
				penOld = SelectObject(dcMain,penNew);
				//TODO: Add movable lines
				MoveToEx(dcMain,cStart,10+12*cBlockSize,NULL);
				LineTo(dcMain,cStart+(8*fontWidth)*cBlockSize,10+12*cBlockSize); //lower red line
				MoveToEx(dcMain,cStart,10+3*cBlockSize,NULL);
				LineTo(dcMain,cStart+(8*fontWidth)*cBlockSize,10+3*cBlockSize); //upper red line
				SelectObject(dcMain,penOld);
				DeleteObject(penNew);
				}
			else
				{
				tmp = SetTextAlign(dcMain,VTA_CENTER|TA_CENTER);
				GetClientRect(hWnd,&r);
				i= SetBkMode(dcMain,TRANSPARENT);
				TextOut(dcMain,r.right/2,(r.bottom-50)/2,"Drag file here to open",22);
				SetBkMode(dcMain,i);
				SetTextAlign(dcMain,tmp);
				}
			ReleaseDC(hWnd,dcMain);
			EndPaint(hMain,&psMain);
			break;
    case WM_KEYDOWN:
      if(wPar == VK_PRIOR || wPar == VK_NEXT || wPar == VK_DOWN || wPar == VK_UP)
        {
        switch(wPar)
          {
          case VK_PRIOR:
            i = SB_PAGEUP;
            break;
          case VK_NEXT:
            i = SB_PAGEDOWN;
            break;
          case VK_DOWN:
            i = SB_LINEDOWN;
            break;
          case VK_UP:
            i = SB_LINEUP;
            break;
          }
        SendMessage(hChars,WM_VSCROLL,i,0);
        }
      break;
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MOUSEMOVE:
			if(prgState)
				{
        unsigned char d = 0;
				if(LOWORD(lPar) >= cStart && LOWORD(lPar) <= cStart+(cBlockSize *8*fontWidth) &&
				    HIWORD(lPar) >= 10 && HIWORD(lPar) <= 10+(cBlockSize << 4))
					{
					i = fontWidth*((HIWORD(lPar)-10)/cBlockSize)+(LOWORD(lPar)-cStart)/(8*cBlockSize);
					sTmp = HeapAlloc(GetProcessHeap(),0,12);
					wsprintf(sTmp,"x: %u, y: %u",(LOWORD(lPar)-cStart)/cBlockSize,(HIWORD(lPar)-10)/cBlockSize);
					SendMessage(hStatus,SB_SETTEXT,0,(int)sTmp);
					HeapFree(GetProcessHeap(),0,sTmp);
					tmp = 128 >> (((LOWORD(lPar)-cStart)/cBlockSize)%8);
					if(!(currFont[sel].data[i] & tmp))
						{
						if(wPar & MK_LBUTTON)
							{
              currFont[sel].data[i] |= tmp;
              d = 1;
							}
						}
					else if(wPar & MK_RBUTTON)
						{
            currFont[sel].data[i] ^= tmp;
            d = 1;
						}
					if(d)
						{
            if(prgState == 1)
              {
              prgState = 2;
              updateTitle();
              }
						r.top = 10+cBlockSize*((HIWORD(lPar)-10)/cBlockSize);
						r.left = cStart + cBlockSize*((LOWORD(lPar)-cStart)/cBlockSize);
						r.right = r.left+cBlockSize;
						r.bottom = r.top+cBlockSize;
						InvalidateRect(hWnd,&r,1);

						tmp = 8*fontWidth+2;
						i = sel- (si.nPos*16/fontWidth);
						r.top = 1+((i/(16/fontWidth))*20);
						r.left = 5+(i%(16/fontWidth))*tmp;
						r.bottom = r.top+18;
						r.right = r.left+tmp;
						InvalidateRect(hChars,&r,1);
						}
					}
				else
					SendMessage(hStatus,SB_SETTEXT,0,(int)"");
				sTmp = HeapAlloc(GetProcessHeap(),0,25);
				wsprintf(sTmp,"editing char: %u",sel);
				SendMessage(hStatus,SB_SETTEXT,1,(int)sTmp);
				HeapFree(GetProcessHeap(),0,sTmp);
				}
			else
				return DefWindowProc(hWnd,msg,wPar,lPar);
			break;
		case WM_GETMINMAXINFO:
			((LPMINMAXINFO)lPar)->ptMinTrackSize.x = wStart;
			((LPMINMAXINFO)lPar)->ptMinTrackSize.y = hStart;
			break;
		case WM_SIZE:
		case WM_SIZING:
			if(!prgState)
        InvalidateRect(hWnd,NULL,1);  //erase only text?
			else
				{
				GetClientRect(hWnd,&r);
				i = cBlockSize;
				cBlockSize = (r.right-(cStart+cMargin))/(8*fontWidth);
				tmp = (r.bottom-(2*10+20))/16;
				if(cBlockSize > tmp)
					cBlockSize = tmp;
				if(i != cBlockSize)
					{
					r.left = cStart-1;
					InvalidateRect(hWnd,&r,0);
					}
				}
			SendMessage(hStatus,msg,0,0);
			break;
		case WM_COMMAND:
			;
			MENUITEMINFO mii;
			switch(LOWORD(wPar))
				{
				case menuOpen:
				case accOpen:
					if(prgState > 1)
            if(!askSave())
              break;
					sTmp = openDlg(&hWnd,0);
					if(sTmp != NULL)
						{
						moveStr(&fPath,sTmp);
						loadFile();
						prgState = 1;
						updateTitle();
						mii.cbSize = sizeof(MENUITEMINFO);
						mii.fMask = MIIM_STATE;
						mii.fState = MFS_ENABLED;
						SetMenuItemInfo(hMenu,menuInfo,0,&mii);
						SetMenuItemInfo(hMenu,menuSave,0,&mii);
						SetMenuItemInfo(hMenu,menuSaveAs,0,&mii);
						for(i = 0; i != 7; ++i)
              SetMenuItemInfo(hMenu,menuLeft+i,0,&mii);
						}
					break;
        case accSave:
					if(prgState != 2)
            {
            return DefWindowProc(hWnd,msg,wPar,lPar);
            break;
            }
					// fall through
        case menuSave:
          //what if there is new file?
          saveFile();
          prgState = 1;
          updateTitle();
          break;
        case accSaveAs:
					if(!prgState)
            {
            return DefWindowProc(hWnd,msg,wPar,lPar);
            break;
            }
					// fall through
        case menuSaveAs:
          sTmp = openDlg(&hWnd,1);
          if(sTmp != NULL)
            {
            moveStr(&fPath,sTmp);
            saveFile();
            prgState = 1;
            updateTitle();
            }
          break;
				case menuQuit:
					SendMessage(hWnd,WM_CLOSE,0,0);
					break;
				case acc816:
				case menu816:
				case acc1616:
				case menu1616:
					mii.cbSize = sizeof(MENUITEMINFO);
					mii.fMask = MIIM_STATE;
					mii.fState = MFS_CHECKED;
					SetMenuItemInfo(hMenu,LOWORD(wPar),0,&mii);
					mii.fState = MFS_UNCHECKED;
					if(LOWORD(wPar) == menu816 || LOWORD(wPar) == acc816)
						{
						fontWidth = 1;
						currFont = font;
						charNo = header.char1No;
						tmp = menu1616;
						}
					else
						{
						fontWidth = 2;
						currFont = font2;
						charNo = header.char2No;
						tmp = menu816;
						}
					sel = 0;
					SetMenuItemInfo(hMenu,tmp,0,&mii);
					si.nMin = si.nPos = 0;
					si.nMax = (charNo/(16/fontWidth))-1; //replace with dynamic row length
					si.fMask = sifMask;
					SetScrollInfo(hChars,SB_VERT,&si,0);
					GetClientRect(hWnd,&r);
					cBlockSize = (r.right-(cStart+cMargin))/(8*fontWidth);
					tmp = (r.bottom-(2*10+20))/16;
					if(cBlockSize > tmp)
						cBlockSize = tmp;
					InvalidateRect(hWnd,NULL,1);
					break;
				case accInfo:
					if(!prgState)
            {
            return DefWindowProc(hWnd,msg,wPar,lPar);
            break;
            }
          // fall through
				case menuInfo:
						{
						sTmp = HeapAlloc(GetProcessHeap(),0,80);
						char *fType;
						fType = HeapAlloc(GetProcessHeap(),0,6);
						if(ch1Size == 17)
							fType = "win2k";
						else
							fType = "winxp";
						wsprintf(sTmp,"File type: %s\nANSI chars: %u\n16x16 chars: %u\nLocale ID: %u",fType,header.char1No,header.char2No,header.localeId);
						MessageBox(hWnd,sTmp,"File Info",mbStyle);
						HeapFree(GetProcessHeap(),0,fType);
						HeapFree(GetProcessHeap(),0,sTmp);
						}
					break;
        case menuPath:
        case accPath:
					mii.cbSize = sizeof(MENUITEMINFO);
					mii.fMask = MIIM_STATE;
					if(!showFullPath)
            mii.fState = MFS_CHECKED;
          else
            mii.fState = MFS_UNCHECKED;
          showFullPath = !showFullPath;
					SetMenuItemInfo(hMenu,menuPath,0,&mii);
					if(prgState)
            updateTitle();
          break;
        case menuLeft:
        case menuRight:
        case menuCenter:
          ;
          unsigned char d;
          if(LOWORD(wPar) == menuLeft)
            d = dirLeft;
          else if(LOWORD(wPar) == menuRight)
            d = dirRight;
          else
            {
            //d = dirCenter;
            }
          if(0) // for all
            for(i = 0; i != charNo; ++i)
              moveChar(&currFont[i],0,d);
          moveChar(&currFont[sel],0,d);
          r.top = 10;
          r.left = cStart;
          r.right = r.left+8*cBlockSize;
          r.bottom = r.top+16*cBlockSize;
          InvalidateRect(hWnd,&r,1);
          break;
        case menuUp:
        case menuDn:
          if(LOWORD(wPar) == menuUp)
            moveChar(&currFont[sel],0,dirUp);
          else
            moveChar(&currFont[sel],0,dirDn);
          r.top = 10;
          r.left = cStart;
          r.right = r.left+8*cBlockSize;
          r.bottom = r.top+16*cBlockSize;
          InvalidateRect(hWnd,&r,1);
          break;
				case menuAbout:
					MessageBox(hWnd,"Bootfont editor by KaiN\nver 0.1\nKopimi 2010\n\nhttps://github.com/tehKaiN/nt_bootfont_edit","About",mbStyle);
					break;
				}
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_CLOSE:
			if(prgState == 2 && !askSave())
        break;
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd,msg,wPar,lPar);
		}
	return 0;
	}

LRESULT CALLBACK wpChars(HWND hWnd,UINT msg, WPARAM wPar, LPARAM lPar)
	{
	HDC dcChars;
	PAINTSTRUCT psChars;
	DWORD i,tmp, tmp2;
	RECT r;
	char *sTmp;
	switch(msg)
		{
		case WM_CREATE:
			si.cbSize = sizeof(SCROLLINFO);
			si.nPage = 16;
			break;
		case WM_VSCROLL:
			i = si.nPos;
			switch((int)LOWORD(wPar)) // crash on end
				{
				case SB_LINEDOWN:
					if(si.nPos < si.nMax - si.nPage+1)
						++si.nPos;
					break;
				case SB_PAGEDOWN:
					if(si.nMax > si.nPage)
						{
						if(si.nMax - si.nPos <= si.nPage)
							si.nPos = si.nMax-si.nPage+1;
						else
							si.nPos+=si.nPage;
						}
					break;
				case SB_LINEUP:
					if(si.nPos)
						--si.nPos;
					break;
				case SB_PAGEUP:
					if(si.nPos < si.nPage)
						si.nPos = 0;
					else
						si.nPos-=si.nPage;
					break;
				case SB_THUMBPOSITION:
				case SB_THUMBTRACK:
					si.nPos = (short int)HIWORD(wPar);
					break;
				}
			if(si.nPos != i)
				{
				si.fMask = SIF_POS;
				SetScrollInfo(hChars,SB_VERT,&si,1);
				InvalidateRect(hWnd,NULL,1);
				}
			break;
		case WM_PAINT:
			if(prgState)
				{
				dcChars = BeginPaint(hWnd,&psChars);
				tmp = ((si.nPage+1)*16/fontWidth);
				tmp2 = si.nPos*16/fontWidth;
				for(i = 0; i != charNo-tmp2 && i != tmp; ++i)
					drawGlyph(dcChars,currFont[tmp2+i].data,5+((i%(16/fontWidth))*((8*fontWidth)+2)),1+((i/(16/fontWidth))*20),fontWidth,1);
				ReleaseDC(hWnd,dcChars);
				EndPaint(hWnd,&psChars);
				}
			else
				return DefWindowProc(hWnd,msg,wPar,lPar);
			break;
		case WM_LBUTTONDOWN:
		case WM_MOUSEMOVE:
			tmp = si.nPos*16/fontWidth;
			i = tmp+((LOWORD(lPar)-5)/((8*fontWidth)+2))+(16/fontWidth)*((HIWORD(lPar)-1)/20);
			if(msg == WM_LBUTTONDOWN)
				{
				if(i < charNo)
					{
					sel = i;
					r.top = 10;
					r.left = cStart;
					r.right = r.left+(cBlockSize *8*fontWidth);
					r.bottom = r.top+(cBlockSize << 4);
					InvalidateRect(hMain,&r,1);
					}
				}
			sTmp = HeapAlloc(GetProcessHeap(),0,43);
			wsprintf(sTmp,"char: %u, editing char: %u",i,sel);
			SendMessage(hStatus,SB_SETTEXT,1,(int)sTmp);
			SendMessage(hStatus,SB_SETTEXT,0,(int)"");
			HeapFree(GetProcessHeap(),0,sTmp);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_CLOSE:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd,msg,wPar,lPar);
		}
	return 0;
	}

int WINAPI WinMain(
  HINSTANCE hInstance, __attribute__ ((unused)) HINSTANCE hPrevInstance,
  __attribute__ ((unused)) LPSTR lpCmdLine, __attribute__ ((unused)) int nCmdShow
)
	{
  showFullPath = 0;
	fPath = HeapAlloc(GetProcessHeap(),0,1);
	InitCommonControls();
	WNDCLASSEX wcMain,wcChars;
	wcMain.cbSize = sizeof(WNDCLASSEX);
	wcMain.style = CS_BYTEALIGNCLIENT;
	wcMain.lpfnWndProc = wpMain;
	wcMain.cbClsExtra = wcMain.cbWndExtra = 0;
	wcMain.hInstance = hInstance;
	wcMain.hIcon = wcMain.hIconSm = LoadIcon(hInstance,MAKEINTRESOURCE(888));
	wcMain.hCursor = LoadCursor(NULL,IDC_ARROW);
	wcMain.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wcMain.lpszMenuName = NULL;
	wcMain.lpszClassName = cName;
	wcChars = wcMain;
	RegisterClassEx(&wcMain);
	wcChars.lpfnWndProc = wpChars;
	wcChars.lpszClassName = chCName;
	RegisterClassEx(&wcChars);

	hMenu = LoadMenu(hInstance, MAKEINTRESOURCE(1300));
	hMain = CreateWindowEx(exStyle,cName,wndName,dwStyle,xStart,yStart,wStart,hStart,0,hMenu,hInstance,NULL);
	hStatus = CreateWindowEx(stExStyle,STATUSCLASSNAME,stWndName,stStyle,0,0,0,0,hMain,NULL,hInstance,NULL);
	hChars = CreateWindowEx(chExStyle,chCName,chWndName,chStyle,chXStart,chYStart,chWStart,chHStart,hMain,NULL,hInstance,NULL);

	int *parts = HeapAlloc(GetProcessHeap(),0,2*sizeof(int));
	parts[0] = 60;
	parts[1] = -1;
	SendMessage(hStatus,SB_SETPARTS,2,(int)parts);
	HeapFree(GetProcessHeap(),0,parts);
	SendMessage(hMain,WM_COMMAND,menu816,0);
	ShowWindow(hMain,SW_SHOW);
	ShowWindow(hStatus,SW_SHOW);
	UpdateWindow(hMain);
	MSG msg;
	HACCEL acc = LoadAccelerators(hInstance,MAKEINTRESOURCE(resAcc));
	while(GetMessage(&msg,NULL,0,0))
		{
		if(!TranslateAccelerator(hMain,acc,&msg))
			TranslateMessage(&msg);
		DispatchMessage(&msg);
		}
	HeapFree(GetProcessHeap(),0,fPath);
	return msg.wParam;
	}
