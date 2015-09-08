#include "stolenfont.h"
#include <BOSBOOT.h>
#include <svga.h>
#include <stdarg.h>

int sgn(int x);
int sgn(int x)
{
    if(x == 0)
        return 0;
    else if(x > 0)
        return 1;
    else
        return -1;
}

// THIS WILL NOT BE A HAL DEVICE, I WANT IT FASTER SO LETS NOT DO THAT!
bool SecondBuffer;
void* FrameBuffer;
void* SecondFrameBuffer;
uint8_t *VBE_LFB_8;
uint16_t *VBE_LFB_16;
uint32_t *VBE_LFB_32;

uint8_t Depth;
uint16_t Width;
uint16_t Height;
int CurX, CurY;
char *MsgTemp;


extern uint32_t VBE_LFB;
extern uint32_t VBE_LFB1;
extern uint32_t VBE_FB;
extern uint32_t VBE_FBSize;
extern uint32_t VBE_FUNCTION;
extern uint16_t VBE_PAGE0;
extern uint16_t VBE_PAGE1;

extern void FlipPage(void);

int init()
{
#ifdef DEBUG
    DEBUG_printf("BOS v. 0.0.3\t%s\tCompiled at %s on %s Line %i\tFunction \"%s\"\n", __FILE__, __TIME__, __DATE__, (__LINE__ - 3), __func__);
#endif
    if(!BOOTINF.CurVBEInfo->WindowPosFunct)
        return -1;
    VBE_PAGE0 = 0;
    VBE_PAGE1 = ((Width*Height*(Depth >> 3)) / (BOOTINF.CurVBEInfo->WinGranularity * 0x400));
    if(((Width*Height*(Depth >> 3)) % (BOOTINF.CurVBEInfo->WinGranularity * 0x400)))
        VBE_PAGE1++;
    SecondBuffer = FALSE;
    MsgTemp = (char*) calloc(4096);
    Width = BOOTINF.CurVBEInfo->Width;
    Height = BOOTINF.CurVBEInfo->Height;
    Depth = BOOTINF.CurVBEInfo->BitsPerPix;
    CurX = 0;
    CurY = 0;
    for(uint32_t x = 0; x < (((Width*Height*(Depth >> 3))+0x1000) * 2); x+=0x1000)
        _VMM_map((BOOTINF.FrameBuffer + x), (0xF00000 + x), FALSE, TRUE);
    uint32_t temp = (BOOTINF.CurVBEInfo->WindowPosFunct - (BOOTINF.CurVBEInfo->WindowPosFunct % 0x1000));
    _VMM_map(temp, 0xFF8000, FALSE, TRUE);
    _VMM_map(temp + 0x1000, 0xFF9000, FALSE, TRUE);
    _VMM_map(temp + 0x2000, 0xFFA000, FALSE, TRUE);
    _VMM_map(temp + 0x3000, 0xFFB000, FALSE, TRUE);

    FrameBuffer = (void*) 0xF00000;
    SecondFrameBuffer = (void*) (0xF00000 + (VBE_PAGE1 * 0x400));
    VBE_LFB = 0xF00000;
    VBE_LFB1 = (0xF00000 + (VBE_PAGE1 * 0x400));
    VBE_FUNCTION = (0xFF8000 + (BOOTINF.CurVBEInfo->WindowPosFunct % 0x1000));
    VBE_FB = (uint32_t) calloc(((Width*Height*(Depth >> 3))+0x1000));
    VBE_FBSize = (uint32_t) ((Width*Height*(Depth >> 3))+0x1000);

    memset(FrameBuffer, 0, (Width*Height*(Depth >> 3)));
    memset(SecondFrameBuffer, 0xFF, (Width*Height*(Depth >> 3)));

    for(;;)
        FlipPage();

    if(Depth == 8) {
        VBE_LFB_8 = (uint8_t*) VBE_FB;
    } else if(Depth == 16) {
        VBE_LFB_16 = (uint16_t*) VBE_FB;
    } else if(Depth == 32) {
        VBE_LFB_32 = (uint32_t*) VBE_FB;
    } else {
        return -1;
    }
    // refreshScreen();
    _API_Add_Symbol("pset", (uint32_t) &puts);
    _API_Add_Symbol("FillRect", (uint32_t) &FillRect);
    _API_Add_Symbol("DrawRect", (uint32_t) &DrawRect);
    _API_Add_Symbol("DrawLine", (uint32_t) &DrawLine);
    _API_Add_Symbol("ClearScreen", (uint32_t) &ClearScreen);
    _API_Add_Symbol("putch", (uint32_t) &putch);
    _API_Add_Symbol("puts", (uint32_t) &puts);
    _API_Add_Symbol("printf", (uint32_t) &printf);
    _API_Add_Symbol("cprintf", (uint32_t) &cprintf);
    _API_Add_Symbol("refreshScreen", (uint32_t) &refreshScreen);
    return 0;
}

void pset(int X, int Y, uint32_t Color)
{
    if(Depth == 8) {
        VBE_LFB_8[((Y * Width) + X)] = (uint8_t) Color & 0xFF;
    } else if(Depth == 16) {
        VBE_LFB_16[((Y * Width) + X)] = (uint16_t) Color & 0xFFFF;
    } else if(Depth == 32) {
        VBE_LFB_32[((Y * Width) + X)] = Color;
    }
}

void FillRect(int X1, int Y1, int X2, int Y2, uint32_t C)
{
    for(int Y = Y1; Y < Y2; Y++) {
        for(int X = X1; X < X2; X++) {
           pset(X, Y, C);
        }
    }
}
void DrawRect(int X1, int Y1, int X2, int Y2, uint32_t C)
{
    DrawLine(X1, Y1, X2, Y1, C);
    DrawLine(X1, Y1, X1, Y2, C);
    DrawLine(X2, Y2, X2, Y1, C);
    DrawLine(X2, Y2, X1, Y2, C);
}


// Y = (m * X) + b
void DrawLine(int X1, int Y1, int X2, int Y2, uint32_t C)
{
    int i,dx,dy,sdx,sdy,dxabs,dyabs,x,y,px,py;

    dx=X2-X1;      /* the horizontal distance of the line */
    dy=Y2-Y1;      /* the vertical distance of the line */
    dxabs=(dx >= 0) ? dx : (dx * -1);//abs(dx);
    dyabs=(dy >= 0) ? dy : (dy * -1);//abs(dy);
    sdx=sgn(dx);
    sdy=sgn(dy);
    x=dyabs>>1;
    y=dxabs>>1;
    px=X1;
    py=Y1;

    pset(px, py, C);

    if (dxabs>=dyabs) /* the line is more horizontal than vertical */
    {
        for(i=0;i<dxabs;i++)
        {
            y+=dyabs;
            if (y>=dxabs)
            {
                y-=dxabs;
                py+=sdy;
            }
            px+=sdx;
            pset(px,py,C);
        }
    } else { /* the line is more vertical than horizontal */
        for(i=0;i<dyabs;i++)
        {
            x+=dxabs;
            if (x>=dyabs)
            {
                x-=dyabs;
                px+=sdx;
            }
            py+=sdy;
            pset(px,py,C);
        }
    }
}

void ClearScreen(uint32_t C)
{
    if(Depth == 8) {
        memsetl(VBE_LFB_8, (((C & 0xFF) << 24) | ((C & 0xFF) << 16) | ((C & 0xFF) << 8) | (C & 0xFF)), (Width * Height / 4));
    } else if(Depth == 16) {
        memsetl(VBE_LFB_16, ((C & 0xFFFF) << 16) | (C & 0xFFFF), (Width * Height / 2));
    } else if(Depth == 32) {
        memsetl(VBE_LFB_32, C , (Width * Height));
    }
    CurX = 0;
    CurY = 0;
}

void puts(const char* str, uint32_t colr)
{
    while(*str)
        putch(*str++, colr);
    // refreshScreen();
}

void putch(const char str, uint32_t Color)
{
    int out = (int) str;
    int offset = 0;
    int l = 0;
    switch(out) {
        case '\n':
            CurY += 8;
            CurX = 0;
            break;
        case '\r':
            CurX = 0;
            break;
        case '\b':
            CurX = CurX - 8;
            break;
        case '\t':
            CurX += 40;
            break;
        case ' ':
            CurX += 8;
            break;
        default:
            for(l = 0; l < 8; l++) {
                if(font8x8_basic[out][l] & 0x01)
                    pset(CurX, CurY, Color);
                    //VBE_MEM[offset] = colr;//pset((uint16_t)(CurX), (uint16_t)(CurY+l), colr);
                if(font8x8_basic[out][l] & 0x02)
                    pset(CurX + 1, CurY, Color);
                    // VBE_MEM[offset + 1] = colr;// pset((uint16_t)(CurX+1), (uint16_t)(CurY+l), colr);
                if(font8x8_basic[out][l] & 0x04)
                    pset(CurX + 2, CurY, Color);
                    // VBE_MEM[offset + 2] = colr;// pset((uint16_t)(CurX+2), (uint16_t)(CurY+l), colr);
                if(font8x8_basic[out][l] & 0x08)
                    pset(CurX + 3, CurY, Color);
                    // VBE_MEM[offset + 3] = colr;// pset((uint16_t)(CurX+3), (uint16_t)(CurY+l), colr);
                if(font8x8_basic[out][l] & 0x10)
                    pset(CurX + 4, CurY, Color);
                    // VBE_MEM[offset + 4] = colr;// pset((uint16_t)(CurX+4), (uint16_t)(CurY+l), colr);
                if(font8x8_basic[out][l] & 0x20)
                    pset(CurX + 5, CurY, Color);
                    // VBE_MEM[offset + 5] = colr;// pset((uint16_t)(CurX+5), (uint16_t)(CurY+l), colr);
                if(font8x8_basic[out][l] & 0x40)
                    pset(CurX + 6, CurY, Color);
                    // VBE_MEM[offset + 6] = colr;// pset((uint16_t)(CurX+6), (uint16_t)(CurY+l), colr);
                if(font8x8_basic[out][l] & 0x80)
                    pset(CurX + 7, CurY, Color);
                    // VBE_MEM[offset + 7] = colr;// pset((uint16_t)(CurX+7), (uint16_t)(CurY+l), colr);
                CurY++;
            }
            CurY -= 8;
            CurX += (uint16_t)8;
    }
    if(CurX > (Width-8)) {
        CurY += 8;
        CurX = 0;
    }
    while(CurY >= (Height-8)) {
        if(Depth == 8) {
            memcpy(VBE_LFB_8, (void*) &VBE_LFB_8[(Width * 8)] , (((Width - 8)*Height*(Depth >> 3))));
            memset(VBE_LFB_8[((Width - 8) * Height)], 0, (Width * 8));
        } else if(Depth == 16) {
            memcpy(VBE_LFB_16, (void*) &VBE_LFB_16[(Width * 8)] , (((Width - 8)*Height*(Depth >> 3))));
            memsetw(VBE_LFB_16[((Width - 8) * Height)], 0, (Width * 8));
        } else if(Depth == 32) {
            memcpy(VBE_LFB_32, (void*) &VBE_LFB_32[(Width * 8)] , (((Width - 8)*Height*(Depth >> 3))));
            memsetl(VBE_LFB_32[((Width - 8) * Height)], 0, (Width * 8));
        }
        CurY -= 8;
    }
}

void printf(const char* str, ...)
{
    va_list ap;
    va_start(ap, str);
    stringf(MsgTemp, str, ap);
    puts(MsgTemp, 0xFFFFFFFF);
}

void cprintf(uint32_t Color, const char* str, ...)
{
    va_list ap;
    va_start(ap, str);
    stringf(MsgTemp, str, ap);
    puts(MsgTemp, Color);
}

/*void refreshScreen(void)
{
    int x = 0;
    while (inb(0x3DA) & 8);
    while (!(inb(0x3DA) & 8));
    if(Depth == 8) {
        for(; x<(Width * Height / 4); x++)
            ((uint32_t*) FrameBuffer)[x] = ((uint32_t*) VBE_LFB_8)[x];
        // memcpy(FrameBuffer, VBE_LFB_8, ((Width*Height*(Depth >> 3))+0x1000));
    } else if(Depth == 16) {
        for(; x<(Width * Height / 2); x++)
            ((uint32_t*) FrameBuffer)[x] = ((uint32_t*) VBE_LFB_16)[x];
        // memcpy(FrameBuffer, VBE_LFB_16, ((Width*Height*(Depth >> 3))+0x1000));
    } else if(Depth == 32) {
        for(; x<(Width * Height); x++)
            ((uint32_t*) FrameBuffer)[x] = VBE_LFB_32[x];
        // memcpy(FrameBuffer, VBE_LFB_32, ((Width*Height*(Depth >> 3))+0x1000));
    }
}
*/