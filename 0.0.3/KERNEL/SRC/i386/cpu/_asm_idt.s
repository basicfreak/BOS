[bits 32]

[global	LoadIDT]
[extern	idtp]

[extern 	IDT_HANDLER]

[global IDT0]
[global IDT1]
[global IDT2]
[global IDT3]
[global IDT4]
[global IDT5]
[global IDT6]
[global IDT7]
[global IDT8]
[global IDT9]
[global IDT10]
[global IDT11]
[global IDT12]
[global IDT13]
[global IDT14]
[global IDT15]
[global IDT16]
[global IDT17]
[global IDT18]
[global IDT19]
[global IDT20]
[global IDT21]
[global IDT22]
[global IDT23]
[global IDT24]
[global IDT25]
[global IDT26]
[global IDT27]
[global IDT28]
[global IDT29]
[global IDT30]
[global IDT31]
[global IDT32]
[global IDT33]
[global IDT34]
[global IDT35]
[global IDT36]
[global IDT37]
[global IDT38]
[global IDT39]
[global IDT40]
[global IDT41]
[global IDT42]
[global IDT43]
[global IDT44]
[global IDT45]
[global IDT46]
[global IDT47]
[global IDT48]
[global IDT49]
[global IDT50]
[global IDT51]
[global IDT52]
[global IDT53]
[global IDT54]
[global IDT55]
[global IDT56]
[global IDT57]
[global IDT58]
[global IDT59]
[global IDT60]
[global IDT61]
[global IDT62]
[global IDT63]
[global IDT64]
[global IDT65]
[global IDT66]
[global IDT67]
[global IDT68]
[global IDT69]
[global IDT70]
[global IDT71]
[global IDT72]
[global IDT73]
[global IDT74]
[global IDT75]
[global IDT76]
[global IDT77]
[global IDT78]
[global IDT79]
[global IDT80]
[global IDT81]
[global IDT82]
[global IDT83]
[global IDT84]
[global IDT85]
[global IDT86]
[global IDT87]
[global IDT88]
[global IDT89]
[global IDT90]
[global IDT91]
[global IDT92]
[global IDT93]
[global IDT94]
[global IDT95]
[global IDT96]
[global IDT97]
[global IDT98]
[global IDT99]
[global IDT100]
[global IDT101]
[global IDT102]
[global IDT103]
[global IDT104]
[global IDT105]
[global IDT106]
[global IDT107]
[global IDT108]
[global IDT109]
[global IDT110]
[global IDT111]
[global IDT112]
[global IDT113]
[global IDT114]
[global IDT115]
[global IDT116]
[global IDT117]
[global IDT118]
[global IDT119]
[global IDT120]
[global IDT121]
[global IDT122]
[global IDT123]
[global IDT124]
[global IDT125]
[global IDT126]
[global IDT127]
[global IDT128]
[global IDT129]
[global IDT130]
[global IDT131]
[global IDT132]
[global IDT133]
[global IDT134]
[global IDT135]
[global IDT136]
[global IDT137]
[global IDT138]
[global IDT139]
[global IDT140]
[global IDT141]
[global IDT142]
[global IDT143]
[global IDT144]
[global IDT145]
[global IDT146]
[global IDT147]
[global IDT148]
[global IDT149]
[global IDT150]
[global IDT151]
[global IDT152]
[global IDT153]
[global IDT154]
[global IDT155]
[global IDT156]
[global IDT157]
[global IDT158]
[global IDT159]
[global IDT160]
[global IDT161]
[global IDT162]
[global IDT163]
[global IDT164]
[global IDT165]
[global IDT166]
[global IDT167]
[global IDT168]
[global IDT169]
[global IDT170]
[global IDT171]
[global IDT172]
[global IDT173]
[global IDT174]
[global IDT175]
[global IDT176]
[global IDT177]
[global IDT178]
[global IDT179]
[global IDT180]
[global IDT181]
[global IDT182]
[global IDT183]
[global IDT184]
[global IDT185]
[global IDT186]
[global IDT187]
[global IDT188]
[global IDT189]
[global IDT190]
[global IDT191]
[global IDT192]
[global IDT193]
[global IDT194]
[global IDT195]
[global IDT196]
[global IDT197]
[global IDT198]
[global IDT199]
[global IDT200]
[global IDT201]
[global IDT202]
[global IDT203]
[global IDT204]
[global IDT205]
[global IDT206]
[global IDT207]
[global IDT208]
[global IDT209]
[global IDT210]
[global IDT211]
[global IDT212]
[global IDT213]
[global IDT214]
[global IDT215]
[global IDT216]
[global IDT217]
[global IDT218]
[global IDT219]
[global IDT220]
[global IDT221]
[global IDT222]
[global IDT223]
[global IDT224]
[global IDT225]
[global IDT226]
[global IDT227]
[global IDT228]
[global IDT229]
[global IDT230]
[global IDT231]
[global IDT232]
[global IDT233]
[global IDT234]
[global IDT235]
[global IDT236]
[global IDT237]
[global IDT238]
[global IDT239]
[global IDT240]
[global IDT241]
[global IDT242]
[global IDT243]
[global IDT244]
[global IDT245]
[global IDT246]
[global IDT247]
[global IDT248]
[global IDT249]
[global IDT250]
[global IDT251]
[global IDT252]
[global IDT253]
[global IDT254]
[global IDT255]


; Loads the IDT defined in '_idtp' into the processor.
; This is declared in C as 'extern void LoadIDT();'
LoadIDT:
    lidt [idtp]
    ret

;  0: Divide By Zero Exception
IDT0:
    cli
    push byte 0
    push dword 0
    jmp IDT_COMMON

;  1: Debug Exception
IDT1:
    cli
    push byte 0
    push dword 1
    jmp IDT_COMMON

;  2: Non Maskable Interrupt Exception
IDT2:
    cli
    push byte 0
    push dword 2
    jmp IDT_COMMON

;  3: Int 3 Exception
IDT3:
    cli
    push byte 0
    push dword 3
    jmp IDT_COMMON

;  4: INTO Exception
IDT4:
    cli
    push byte 0
    push dword 4
    jmp IDT_COMMON

;  5: Out of Bounds Exception
IDT5:
    cli
    push byte 0
    push dword 5
    jmp IDT_COMMON

;  6: Invalid Opcode Exception
IDT6:
    cli
    push byte 0
    push dword 6
    jmp IDT_COMMON

;  7: Coprocessor Not Available Exception
IDT7:
    cli
    push byte 0
    push dword 7
    jmp IDT_COMMON

;  8: Double Fault Exception (With Error Code!)
IDT8:
    cli
    push dword 8
    jmp IDT_COMMON

;  9: Coprocessor Segment Overrun Exception
IDT9:
    cli
    push byte 0
    push dword 9
    jmp IDT_COMMON

; 10: Bad TSS Exception (With Error Code!)
IDT10:
    cli
    push dword 10
    jmp IDT_COMMON

; 11: Segment Not Present Exception (With Error Code!)
IDT11:
    cli
    push dword 11
    jmp IDT_COMMON

; 12: Stack Fault Exception (With Error Code!)
IDT12:
    cli
    push dword 12
    jmp IDT_COMMON

; 13: General Protection Fault Exception (With Error Code!)
IDT13:
    cli
    push dword 13
    jmp IDT_COMMON

; 14: Page Fault Exception (With Error Code!)
IDT14:
    cli
    push dword 14
    jmp IDT_COMMON

; 15: Reserved Exception
IDT15:
    cli
    push byte 0
    push dword 15
    jmp IDT_COMMON

; 16: Floating Point Exception
IDT16:
    cli
    push byte 0
    push dword 16
    jmp IDT_COMMON

; 17: Alignment Check Exception
IDT17:
    cli
    push byte 0
    push dword 17
    jmp IDT_COMMON

; 18: Machine Check Exception
IDT18:
    cli
    push byte 0
    push dword 18
    jmp IDT_COMMON

; 19: Reserved
IDT19:
    cli
    push byte 0
    push dword 19
    jmp IDT_COMMON

; 20: Reserved
IDT20:
    cli
    push byte 0
    push dword 20
    jmp IDT_COMMON

; 21: Reserved
IDT21:
    cli
    push byte 0
    push dword 21
    jmp IDT_COMMON

; 22: Reserved
IDT22:
    cli
    push byte 0
    push dword 22
    jmp IDT_COMMON

; 23: Reserved
IDT23:
    cli
    push byte 0
    push dword 23
    jmp IDT_COMMON

; 24: Reserved
IDT24:
    cli
    push byte 0
    push dword 24
    jmp IDT_COMMON

; 25: Reserved
IDT25:
    cli
    push byte 0
    push dword 25
    jmp IDT_COMMON

; 26: Reserved
IDT26:
    cli
    push byte 0
    push dword 26
    jmp IDT_COMMON

; 27: Reserved
IDT27:
    cli
    push byte 0
    push dword 27
    jmp IDT_COMMON

; 28: Reserved
IDT28:
    cli
    push byte 0
    push dword 28
    jmp IDT_COMMON

; 29: Reserved
IDT29:
    cli
    push byte 0
    push dword 29
    jmp IDT_COMMON

; 30: Reserved
IDT30:
    cli
    push byte 0
    push dword 30
    jmp IDT_COMMON

; 31: Reserved
IDT31:
    cli
    push byte 0
    push dword 31
    jmp IDT_COMMON

IDT32:
    cli
    push byte 0
    push dword 0x20
    jmp IDT_COMMON

IDT33:
    cli
    push byte 0
    push dword 0x21
    jmp IDT_COMMON

IDT34:
    cli
    push byte 0
    push dword 0x22
    jmp IDT_COMMON

IDT35:
    cli
    push byte 0
    push dword 0x23
    jmp IDT_COMMON

IDT36:
    cli
    push byte 0
    push dword 0x24
    jmp IDT_COMMON

IDT37:
    cli
    push byte 0
    push dword 0x25
    jmp IDT_COMMON

IDT38:
    cli
    push byte 0
    push dword 0x26
    jmp IDT_COMMON

IDT39:
    cli
    push byte 0
    push dword 0x27
    jmp IDT_COMMON

IDT40:
    cli
    push byte 0
    push dword 0x28
    jmp IDT_COMMON

IDT41:
    cli
    push byte 0
    push dword 0x29
    jmp IDT_COMMON

IDT42:
    cli
    push byte 0
    push dword 0x2a
    jmp IDT_COMMON

IDT43:
    cli
    push byte 0
    push dword 0x2b
    jmp IDT_COMMON

IDT44:
    cli
    push byte 0
    push dword 0x2c
    jmp IDT_COMMON

IDT45:
    cli
    push byte 0
    push dword 0x2d
    jmp IDT_COMMON

IDT46:
    cli
    push byte 0
    push dword 0x2e
    jmp IDT_COMMON

IDT47:
    cli
    push byte 0
    push dword 0x2f
    jmp IDT_COMMON

IDT48:
    cli
    push byte 0
    push dword 0x30
    jmp IDT_COMMON

IDT49:
    cli
    push byte 0
    push dword 0x31
    jmp IDT_COMMON

IDT50:
    cli
    push byte 0
    push dword 0x32
    jmp IDT_COMMON

IDT51:
    cli
    push byte 0
    push dword 0x33
    jmp IDT_COMMON

IDT52:
    cli
    push byte 0
    push dword 0x34
    jmp IDT_COMMON

IDT53:
    cli
    push byte 0
    push dword 0x35
    jmp IDT_COMMON

IDT54:
    cli
    push byte 0
    push dword 0x36
    jmp IDT_COMMON

IDT55:
    cli
    push byte 0
    push dword 0x37
    jmp IDT_COMMON

IDT56:
    cli
    push byte 0
    push dword 0x38
    jmp IDT_COMMON

IDT57:
    cli
    push byte 0
    push dword 0x39
    jmp IDT_COMMON

IDT58:
    cli
    push byte 0
    push dword 0x3a
    jmp IDT_COMMON

IDT59:
    cli
    push byte 0
    push dword 0x3b
    jmp IDT_COMMON

IDT60:
    cli
    push byte 0
    push dword 0x3c
    jmp IDT_COMMON

IDT61:
    cli
    push byte 0
    push dword 0x3d
    jmp IDT_COMMON

IDT62:
    cli
    push byte 0
    push dword 0x3e
    jmp IDT_COMMON

IDT63:
    cli
    push byte 0
    push dword 0x3f
    jmp IDT_COMMON

IDT64:
    cli
    push byte 0
    push dword 0x40
    jmp IDT_COMMON

IDT65:
    cli
    push byte 0
    push dword 0x41
    jmp IDT_COMMON

IDT66:
    cli
    push byte 0
    push dword 0x42
    jmp IDT_COMMON

IDT67:
    cli
    push byte 0
    push dword 0x43
    jmp IDT_COMMON

IDT68:
    cli
    push byte 0
    push dword 0x44
    jmp IDT_COMMON

IDT69:
    cli
    push byte 0
    push dword 0x45
    jmp IDT_COMMON

IDT70:
    cli
    push byte 0
    push dword 0x46
    jmp IDT_COMMON

IDT71:
    cli
    push byte 0
    push dword 0x47
    jmp IDT_COMMON

IDT72:
    cli
    push byte 0
    push dword 0x48
    jmp IDT_COMMON

IDT73:
    cli
    push byte 0
    push dword 0x49
    jmp IDT_COMMON

IDT74:
    cli
    push byte 0
    push dword 0x4a
    jmp IDT_COMMON

IDT75:
    cli
    push byte 0
    push dword 0x4b
    jmp IDT_COMMON

IDT76:
    cli
    push byte 0
    push dword 0x4c
    jmp IDT_COMMON

IDT77:
    cli
    push byte 0
    push dword 0x4d
    jmp IDT_COMMON

IDT78:
    cli
    push byte 0
    push dword 0x4e
    jmp IDT_COMMON

IDT79:
    cli
    push byte 0
    push dword 0x4f
    jmp IDT_COMMON

IDT80:
    cli
    push byte 0
    push dword 0x50
    jmp IDT_COMMON

IDT81:
    cli
    push byte 0
    push dword 0x51
    jmp IDT_COMMON

IDT82:
    cli
    push byte 0
    push dword 0x52
    jmp IDT_COMMON

IDT83:
    cli
    push byte 0
    push dword 0x53
    jmp IDT_COMMON

IDT84:
    cli
    push byte 0
    push dword 0x54
    jmp IDT_COMMON

IDT85:
    cli
    push byte 0
    push dword 0x55
    jmp IDT_COMMON

IDT86:
    cli
    push byte 0
    push dword 0x56
    jmp IDT_COMMON

IDT87:
    cli
    push byte 0
    push dword 0x57
    jmp IDT_COMMON

IDT88:
    cli
    push byte 0
    push dword 0x58
    jmp IDT_COMMON

IDT89:
    cli
    push byte 0
    push dword 0x59
    jmp IDT_COMMON

IDT90:
    cli
    push byte 0
    push dword 0x5a
    jmp IDT_COMMON

IDT91:
    cli
    push byte 0
    push dword 0x5b
    jmp IDT_COMMON

IDT92:
    cli
    push byte 0
    push dword 0x5c
    jmp IDT_COMMON

IDT93:
    cli
    push byte 0
    push dword 0x5d
    jmp IDT_COMMON

IDT94:
    cli
    push byte 0
    push dword 0x5e
    jmp IDT_COMMON

IDT95:
    cli
    push byte 0
    push dword 0x5f
    jmp IDT_COMMON

IDT96:
    cli
    push byte 0
    push dword 0x60
    jmp IDT_COMMON

IDT97:
    cli
    push byte 0
    push dword 0x61
    jmp IDT_COMMON

IDT98:
    cli
    push byte 0
    push dword 0x62
    jmp IDT_COMMON

IDT99:
    cli
    push byte 0
    push dword 0x63
    jmp IDT_COMMON

IDT100:
    cli
    push byte 0
    push dword 0x64
    jmp IDT_COMMON

IDT101:
    cli
    push byte 0
    push dword 0x65
    jmp IDT_COMMON

IDT102:
    cli
    push byte 0
    push dword 0x66
    jmp IDT_COMMON

IDT103:
    cli
    push byte 0
    push dword 0x67
    jmp IDT_COMMON

IDT104:
    cli
    push byte 0
    push dword 0x68
    jmp IDT_COMMON

IDT105:
    cli
    push byte 0
    push dword 0x69
    jmp IDT_COMMON

IDT106:
    cli
    push byte 0
    push dword 0x6a
    jmp IDT_COMMON

IDT107:
    cli
    push byte 0
    push dword 0x6b
    jmp IDT_COMMON

IDT108:
    cli
    push byte 0
    push dword 0x6c
    jmp IDT_COMMON

IDT109:
    cli
    push byte 0
    push dword 0x6d
    jmp IDT_COMMON

IDT110:
    cli
    push byte 0
    push dword 0x6e
    jmp IDT_COMMON

IDT111:
    cli
    push byte 0
    push dword 0x6f
    jmp IDT_COMMON

IDT112:
    cli
    push byte 0
    push dword 0x70
    jmp IDT_COMMON

IDT113:
    cli
    push byte 0
    push dword 0x71
    jmp IDT_COMMON

IDT114:
    cli
    push byte 0
    push dword 0x72
    jmp IDT_COMMON

IDT115:
    cli
    push byte 0
    push dword 0x73
    jmp IDT_COMMON

IDT116:
    cli
    push byte 0
    push dword 0x74
    jmp IDT_COMMON

IDT117:
    cli
    push byte 0
    push dword 0x75
    jmp IDT_COMMON

IDT118:
    cli
    push byte 0
    push dword 0x76
    jmp IDT_COMMON

IDT119:
    cli
    push byte 0
    push dword 0x77
    jmp IDT_COMMON

IDT120:
    cli
    push byte 0
    push dword 0x78
    jmp IDT_COMMON

IDT121:
    cli
    push byte 0
    push dword 0x79
    jmp IDT_COMMON

IDT122:
    cli
    push byte 0
    push dword 0x7a
    jmp IDT_COMMON

IDT123:
    cli
    push byte 0
    push dword 0x7b
    jmp IDT_COMMON

IDT124:
    cli
    push byte 0
    push dword 0x7c
    jmp IDT_COMMON

IDT125:
    cli
    push byte 0
    push dword 0x7d
    jmp IDT_COMMON

IDT126:
    cli
    push byte 0
    push dword 0x7e
    jmp IDT_COMMON

IDT127:
    cli
    push byte 0
    push dword 0x7f
    jmp IDT_COMMON

IDT128:
    cli
    push byte 0
    push dword 0x80
    jmp IDT_COMMON

IDT129:
    cli
    push byte 0
    push dword 0x81
    jmp IDT_COMMON

IDT130:
    cli
    push byte 0
    push dword 0x82
    jmp IDT_COMMON

IDT131:
    cli
    push byte 0
    push dword 0x83
    jmp IDT_COMMON

IDT132:
    cli
    push byte 0
    push dword 0x84
    jmp IDT_COMMON

IDT133:
    cli
    push byte 0
    push dword 0x85
    jmp IDT_COMMON

IDT134:
    cli
    push byte 0
    push dword 0x86
    jmp IDT_COMMON

IDT135:
    cli
    push byte 0
    push dword 0x87
    jmp IDT_COMMON

IDT136:
    cli
    push byte 0
    push dword 0x88
    jmp IDT_COMMON

IDT137:
    cli
    push byte 0
    push dword 0x89
    jmp IDT_COMMON

IDT138:
    cli
    push byte 0
    push dword 0x8a
    jmp IDT_COMMON

IDT139:
    cli
    push byte 0
    push dword 0x8b
    jmp IDT_COMMON

IDT140:
    cli
    push byte 0
    push dword 0x8c
    jmp IDT_COMMON

IDT141:
    cli
    push byte 0
    push dword 0x8d
    jmp IDT_COMMON

IDT142:
    cli
    push byte 0
    push dword 0x8e
    jmp IDT_COMMON

IDT143:
    cli
    push byte 0
    push dword 0x8f
    jmp IDT_COMMON

IDT144:
    cli
    push byte 0
    push dword 0x90
    jmp IDT_COMMON

IDT145:
    cli
    push byte 0
    push dword 0x91
    jmp IDT_COMMON

IDT146:
    cli
    push byte 0
    push dword 0x92
    jmp IDT_COMMON

IDT147:
    cli
    push byte 0
    push dword 0x93
    jmp IDT_COMMON

IDT148:
    cli
    push byte 0
    push dword 0x94
    jmp IDT_COMMON

IDT149:
    cli
    push byte 0
    push dword 0x95
    jmp IDT_COMMON

IDT150:
    cli
    push byte 0
    push dword 0x96
    jmp IDT_COMMON

IDT151:
    cli
    push byte 0
    push dword 0x97
    jmp IDT_COMMON

IDT152:
    cli
    push byte 0
    push dword 0x98
    jmp IDT_COMMON

IDT153:
    cli
    push byte 0
    push dword 0x99
    jmp IDT_COMMON

IDT154:
    cli
    push byte 0
    push dword 0x9a
    jmp IDT_COMMON

IDT155:
    cli
    push byte 0
    push dword 0x9b
    jmp IDT_COMMON

IDT156:
    cli
    push byte 0
    push dword 0x9c
    jmp IDT_COMMON

IDT157:
    cli
    push byte 0
    push dword 0x9d
    jmp IDT_COMMON

IDT158:
    cli
    push byte 0
    push dword 0x9e
    jmp IDT_COMMON

IDT159:
    cli
    push byte 0
    push dword 0x9f
    jmp IDT_COMMON

IDT160:
    cli
    push byte 0
    push dword 0xa0
    jmp IDT_COMMON

IDT161:
    cli
    push byte 0
    push dword 0xa1
    jmp IDT_COMMON

IDT162:
    cli
    push byte 0
    push dword 0xa2
    jmp IDT_COMMON

IDT163:
    cli
    push byte 0
    push dword 0xa3
    jmp IDT_COMMON

IDT164:
    cli
    push byte 0
    push dword 0xa4
    jmp IDT_COMMON

IDT165:
    cli
    push byte 0
    push dword 0xa5
    jmp IDT_COMMON

IDT166:
    cli
    push byte 0
    push dword 0xa6
    jmp IDT_COMMON

IDT167:
    cli
    push byte 0
    push dword 0xa7
    jmp IDT_COMMON

IDT168:
    cli
    push byte 0
    push dword 0xa8
    jmp IDT_COMMON

IDT169:
    cli
    push byte 0
    push dword 0xa9
    jmp IDT_COMMON

IDT170:
    cli
    push byte 0
    push dword 0xaa
    jmp IDT_COMMON

IDT171:
    cli
    push byte 0
    push dword 0xab
    jmp IDT_COMMON

IDT172:
    cli
    push byte 0
    push dword 0xac
    jmp IDT_COMMON

IDT173:
    cli
    push byte 0
    push dword 0xad
    jmp IDT_COMMON

IDT174:
    cli
    push byte 0
    push dword 0xae
    jmp IDT_COMMON

IDT175:
    cli
    push byte 0
    push dword 0xaf
    jmp IDT_COMMON

IDT176:
    cli
    push byte 0
    push dword 0xb0
    jmp IDT_COMMON

IDT177:
    cli
    push byte 0
    push dword 0xb1
    jmp IDT_COMMON

IDT178:
    cli
    push byte 0
    push dword 0xb2
    jmp IDT_COMMON

IDT179:
    cli
    push byte 0
    push dword 0xb3
    jmp IDT_COMMON

IDT180:
    cli
    push byte 0
    push dword 0xb4
    jmp IDT_COMMON

IDT181:
    cli
    push byte 0
    push dword 0xb5
    jmp IDT_COMMON

IDT182:
    cli
    push byte 0
    push dword 0xb6
    jmp IDT_COMMON

IDT183:
    cli
    push byte 0
    push dword 0xb7
    jmp IDT_COMMON

IDT184:
    cli
    push byte 0
    push dword 0xb8
    jmp IDT_COMMON

IDT185:
    cli
    push byte 0
    push dword 0xb9
    jmp IDT_COMMON

IDT186:
    cli
    push byte 0
    push dword 0xba
    jmp IDT_COMMON

IDT187:
    cli
    push byte 0
    push dword 0xbb
    jmp IDT_COMMON

IDT188:
    cli
    push byte 0
    push dword 0xbc
    jmp IDT_COMMON

IDT189:
    cli
    push byte 0
    push dword 0xbd
    jmp IDT_COMMON

IDT190:
    cli
    push byte 0
    push dword 0xbe
    jmp IDT_COMMON

IDT191:
    cli
    push byte 0
    push dword 0xbf
    jmp IDT_COMMON

IDT192:
    cli
    push byte 0
    push dword 0xc0
    jmp IDT_COMMON

IDT193:
    cli
    push byte 0
    push dword 0xc1
    jmp IDT_COMMON

IDT194:
    cli
    push byte 0
    push dword 0xc2
    jmp IDT_COMMON

IDT195:
    cli
    push byte 0
    push dword 0xc3
    jmp IDT_COMMON

IDT196:
    cli
    push byte 0
    push dword 0xc4
    jmp IDT_COMMON

IDT197:
    cli
    push byte 0
    push dword 0xc5
    jmp IDT_COMMON

IDT198:
    cli
    push byte 0
    push dword 0xc6
    jmp IDT_COMMON

IDT199:
    cli
    push byte 0
    push dword 0xc7
    jmp IDT_COMMON

IDT200:
    cli
    push byte 0
    push dword 0xc8
    jmp IDT_COMMON

IDT201:
    cli
    push byte 0
    push dword 0xc9
    jmp IDT_COMMON

IDT202:
    cli
    push byte 0
    push dword 0xca
    jmp IDT_COMMON

IDT203:
    cli
    push byte 0
    push dword 0xcb
    jmp IDT_COMMON

IDT204:
    cli
    push byte 0
    push dword 0xcc
    jmp IDT_COMMON

IDT205:
    cli
    push byte 0
    push dword 0xcd
    jmp IDT_COMMON

IDT206:
    cli
    push byte 0
    push dword 0xce
    jmp IDT_COMMON

IDT207:
    cli
    push byte 0
    push dword 0xcf
    jmp IDT_COMMON

IDT208:
    cli
    push byte 0
    push dword 0xd0
    jmp IDT_COMMON

IDT209:
    cli
    push byte 0
    push dword 0xd1
    jmp IDT_COMMON

IDT210:
    cli
    push byte 0
    push dword 0xd2
    jmp IDT_COMMON

IDT211:
    cli
    push byte 0
    push dword 0xd3
    jmp IDT_COMMON

IDT212:
    cli
    push byte 0
    push dword 0xd4
    jmp IDT_COMMON

IDT213:
    cli
    push byte 0
    push dword 0xd5
    jmp IDT_COMMON

IDT214:
    cli
    push byte 0
    push dword 0xd6
    jmp IDT_COMMON

IDT215:
    cli
    push byte 0
    push dword 0xd7
    jmp IDT_COMMON

IDT216:
    cli
    push byte 0
    push dword 0xd8
    jmp IDT_COMMON

IDT217:
    cli
    push byte 0
    push dword 0xd9
    jmp IDT_COMMON

IDT218:
    cli
    push byte 0
    push dword 0xda
    jmp IDT_COMMON

IDT219:
    cli
    push byte 0
    push dword 0xdb
    jmp IDT_COMMON

IDT220:
    cli
    push byte 0
    push dword 0xdc
    jmp IDT_COMMON

IDT221:
    cli
    push byte 0
    push dword 0xdd
    jmp IDT_COMMON

IDT222:
    cli
    push byte 0
    push dword 0xde
    jmp IDT_COMMON

IDT223:
    cli
    push byte 0
    push dword 0xdf
    jmp IDT_COMMON

IDT224:
    cli
    push byte 0
    push dword 0xe0
    jmp IDT_COMMON

IDT225:
    cli
    push byte 0
    push dword 0xe1
    jmp IDT_COMMON

IDT226:
    cli
    push byte 0
    push dword 0xe2
    jmp IDT_COMMON

IDT227:
    cli
    push byte 0
    push dword 0xe3
    jmp IDT_COMMON

IDT228:
    cli
    push byte 0
    push dword 0xe4
    jmp IDT_COMMON

IDT229:
    cli
    push byte 0
    push dword 0xe5
    jmp IDT_COMMON

IDT230:
    cli
    push byte 0
    push dword 0xe6
    jmp IDT_COMMON

IDT231:
    cli
    push byte 0
    push dword 0xe7
    jmp IDT_COMMON

IDT232:
    cli
    push byte 0
    push dword 0xe8
    jmp IDT_COMMON

IDT233:
    cli
    push byte 0
    push dword 0xe9
    jmp IDT_COMMON

IDT234:
    cli
    push byte 0
    push dword 0xea
    jmp IDT_COMMON

IDT235:
    cli
    push byte 0
    push dword 0xeb
    jmp IDT_COMMON

IDT236:
    cli
    push byte 0
    push dword 0xec
    jmp IDT_COMMON

IDT237:
    cli
    push byte 0
    push dword 0xed
    jmp IDT_COMMON

IDT238:
    cli
    push byte 0
    push dword 0xee
    jmp IDT_COMMON

IDT239:
    cli
    push byte 0
    push dword 0xef
    jmp IDT_COMMON

IDT240:
    cli
    push byte 0
    push dword 0xf0
    jmp IDT_COMMON

IDT241:
    cli
    push byte 0
    push dword 0xf1
    jmp IDT_COMMON

IDT242:
    cli
    push byte 0
    push dword 0xf2
    jmp IDT_COMMON

IDT243:
    cli
    push byte 0
    push dword 0xf3
    jmp IDT_COMMON

IDT244:
    cli
    push byte 0
    push dword 0xf4
    jmp IDT_COMMON

IDT245:
    cli
    push byte 0
    push dword 0xf5
    jmp IDT_COMMON

IDT246:
    cli
    push byte 0
    push dword 0xf6
    jmp IDT_COMMON

IDT247:
    cli
    push byte 0
    push dword 0xf7
    jmp IDT_COMMON

IDT248:
    cli
    push byte 0
    push dword 0xf8
    jmp IDT_COMMON

IDT249:
    cli
    push byte 0
    push dword 0xf9
    jmp IDT_COMMON

IDT250:
    cli
    push byte 0
    push dword 0xfa
    jmp IDT_COMMON

IDT251:
    cli
    push byte 0
    push dword 0xfb
    jmp IDT_COMMON

IDT252:
    cli
    push byte 0
    push dword 0xfc
    jmp IDT_COMMON

IDT253:
    cli
    push byte 0
    push dword 0xfd
    jmp IDT_COMMON

IDT254:
    cli
    push byte 0
    push dword 0xfe
    jmp IDT_COMMON

IDT255:
    cli
    push byte 0
    push dword 0xff
    jmp IDT_COMMON

IDT_COMMON:
    pusha
    push ds
    push es
    push fs
    push gs
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov eax, esp
    push eax
;xchg bx, bx
    mov eax, IDT_HANDLER
    call eax
    pop eax
    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp, 8
    iret
