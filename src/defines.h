#include <windows.h>
#include <commctrl.h>

#define mbStyle MB_OK|MB_RTLREADING|MB_RIGHT
#define cName "dupa"
#define exStyle WS_EX_RIGHT|WS_EX_ACCEPTFILES
#define wndName "bootfont.bin Editor"
#define dwStyle WS_OVERLAPPEDWINDOW
#define xStart CW_USEDEFAULT
#define yStart CW_USEDEFAULT
#define wStart 375
#define hStart 390

#define sifMask SIF_RANGE|SIF_PAGE|SIF_POS|SIF_DISABLENOSCROLL

#define stExStyle 0
#define stWndName ""
#define stStyle WS_CHILD

#define chExStyle WS_EX_CLIENTEDGE
#define chWndName "chars"
#define chStyle WS_VSCROLL|WS_CHILD
#define chCName "dupa2"
#define chXStart 0
#define chYStart 0
#define chWStart 185
#define chHStart 325

#define cMargin 15

#define foAccess GENERIC_READ
#define foShare FILE_SHARE_READ
#define foOpenType OPEN_EXISTING
#define foFlags FILE_FLAG_SEQUENTIAL_SCAN

#define fwAccess GENERIC_WRITE
#define fwShare 0
#define fwOpenType CREATE_ALWAYS
#define fwFlags 0

#include "res_id.h"
