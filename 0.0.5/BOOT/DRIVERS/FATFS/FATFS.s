00000000  60                pushad
00000001  85C0              test eax,eax
00000003  7426              jz 0x2b
00000005  83F801            cmp eax,byte +0x1
00000008  742C              jz 0x36
0000000A  83F802            cmp eax,byte +0x2
0000000D  7432              jz 0x41
0000000F  83F803            cmp eax,byte +0x3
00000012  743A              jz 0x4e
00000014  B801000000        mov eax,0x1
00000019  F8                clc
0000001A  A300B10000        mov [0xb100],eax
0000001F  85C0              test eax,eax
00000021  7401              jz 0x24
00000023  F9                stc
00000024  61                popad
00000025  A100B10000        mov eax,[0xb100]
0000002A  C3                ret
0000002B  56                push esi
0000002C  E8FF000000        call dword 0x130
00000031  83C404            add esp,byte +0x4
00000034  EBE3              jmp short 0x19
00000036  57                push edi
00000037  E8240E0000        call dword 0xe60
0000003C  83C404            add esp,byte +0x4
0000003F  EBD8              jmp short 0x19
00000041  51                push ecx
00000042  53                push ebx
00000043  56                push esi
00000044  E867100000        call dword 0x10b0
00000049  83C40C            add esp,byte +0xc
0000004C  EBCB              jmp short 0x19
0000004E  E86D100000        call dword 0x10c0
00000053  EBC4              jmp short 0x19
00000055  55                push ebp
00000056  89E5              mov ebp,esp
00000058  60                pushad
00000059  BF00050000        mov edi,0x500
0000005E  8B5D0C            mov ebx,[ebp+0xc]
00000061  8B4D10            mov ecx,[ebp+0x10]
00000064  66C7070C00        mov word [edi],0xc
00000069  66894F02          mov [edi+0x2],cx
0000006D  C7470400100000    mov dword [edi+0x4],0x1000
00000074  895F08            mov [edi+0x8],ebx
00000077  B001              mov al,0x1
00000079  BA83A00000        mov edx,0xa083
0000007E  E97DE7FFFF        jmp dword 0xffffe800
00000083  8A16              mov dl,[esi]
00000085  817BF6C2807419    cmp dword [ebx-0xa],0x197480c2
0000008C  F8                clc
0000008D  B441              mov ah,0x41
0000008F  BBAA55CD13        mov ebx,0x13cd55aa
00000094  720F              jc 0xa5
00000096  0FBAE100          bt ecx,0x0
0000009A  7309              jnc 0xa5
0000009C  B442              mov ah,0x42
0000009E  BE0005CD13        mov esi,0x13cd0500
000000A3  EB1A              jmp short 0xbf
000000A5  66678B4708        mov ax,[bx+0x8]
000000AA  E85200B402        call dword 0x2b40101
000000AF  B001              mov al,0x1
000000B1  8A2E              mov ch,[esi]
000000B3  06                push es
000000B4  B18A              mov cl,0x8a
000000B6  0E                push cs
000000B7  04B1              add al,0xb1
000000B9  8A36              mov dh,[esi]
000000BB  05B1CD1331        add eax,0x3113cdb1
000000C0  C0CD13            ror ch,byte 0x13
000000C3  B000              mov al,0x0
000000C5  66BACEA0          mov dx,0xa0ce
000000C9  0000              add [eax],al
000000CB  E932E78B7D        jmp dword 0x7d8be802
000000D0  08BE00100000      or [esi+0x1000],bh
000000D6  0FB7050B7C0000    movzx eax,word [dword 0x7c0b]
000000DD  0FB7D1            movzx edx,cx
000000E0  F7E2              mul edx
000000E2  89C1              mov ecx,eax
000000E4  F3A4              rep movsb
000000E6  A300B10000        mov [0xb100],eax
000000EB  61                popad
000000EC  A100B10000        mov eax,[0xb100]
000000F1  C70500B100000000  mov dword [dword 0xb100],0x0
         -0000
000000FB  89EC              mov esp,ebp
000000FD  5D                pop ebp
000000FE  C3                ret
000000FF  60                pushad
00000100  BD007C31D2        mov ebp,0xd2317c00
00000105  F7760D            div dword [esi+0xd]
00000108  FEC2              inc dl
0000010A  8816              mov [esi],dl
0000010C  04B1              add al,0xb1
0000010E  31D2              xor edx,edx
00000110  F7761A            div dword [esi+0x1a]
00000113  8816              mov [esi],dl
00000115  05B1A206B1        add eax,0xb106a2b1
0000011A  61                popad
0000011B  C3                ret
0000011C  66B80000          mov ax,0x0
00000120  0000              add [eax],al
00000122  C3                ret
00000123  662E0F1F84000000  nop word [cs:eax+eax+0x0]
         -0000
0000012D  0F1F00            nop dword [eax]
00000130  55                push ebp
00000131  57                push edi
00000132  56                push esi
00000133  53                push ebx
00000134  83EC4C            sub esp,byte +0x4c
00000137  8B0D30B10000      mov ecx,[dword 0xb130]
0000013D  85C9              test ecx,ecx
0000013F  0F8491070000      jz dword 0x8d6
00000145  8B442460          mov eax,[esp+0x60]
00000149  803800            cmp byte [eax],0x0
0000014C  0F844B090000      jz dword 0xa9d
00000152  31ED              xor ebp,ebp
00000154  90                nop
00000155  90                nop
00000156  90                nop
00000157  90                nop
00000158  83C501            add ebp,byte +0x1
0000015B  803C2800          cmp byte [eax+ebp],0x0
0000015F  75F7              jnz 0x158
00000161  8B7C2460          mov edi,[esp+0x60]
00000165  31C9              xor ecx,ecx
00000167  31F6              xor esi,esi
00000169  C744243801000000  mov dword [esp+0x38],0x1
00000171  BBE9A28B2E        mov ebx,0x2e8ba2e9
00000176  EB55              jmp short 0x1cd
00000178  3C2E              cmp al,0x2e
0000017A  0F8418020000      jz dword 0x398
00000180  3CE5              cmp al,0xe5
00000182  0F8480020000      jz dword 0x408
00000188  8D509F            lea edx,[eax-0x61]
0000018B  80FA19            cmp dl,0x19
0000018E  0F8764020000      ja dword 0x3f8
00000194  83E820            sub eax,byte +0x20
00000197  83C101            add ecx,byte +0x1
0000019A  88813FB10000      mov [ecx+0xb13f],al
000001A0  89C8              mov eax,ecx
000001A2  F7EB              imul ebx
000001A4  89C8              mov eax,ecx
000001A6  C1F81F            sar eax,byte 0x1f
000001A9  D1FA              sar edx,1
000001AB  29C2              sub edx,eax
000001AD  8D0492            lea eax,[edx+edx*4]
000001B0  8D0442            lea eax,[edx+eax*2]
000001B3  39C1              cmp ecx,eax
000001B5  750C              jnz 0x1c3
000001B7  0FB64701          movzx eax,byte [edi+0x1]
000001BB  84C0              test al,al
000001BD  0F854D090000      jnz dword 0xb10
000001C3  83C601            add esi,byte +0x1
000001C6  83C701            add edi,byte +0x1
000001C9  39EE              cmp esi,ebp
000001CB  7443              jz 0x210
000001CD  0FB607            movzx eax,byte [edi]
000001D0  3C2F              cmp al,0x2f
000001D2  75A4              jnz 0x178
000001D4  85F6              test esi,esi
000001D6  7512              jnz 0x1ea
000001D8  EBE9              jmp short 0x1c3
000001DA  90                nop
000001DB  90                nop
000001DC  90                nop
000001DD  90                nop
000001DE  90                nop
000001DF  90                nop
000001E0  83C101            add ecx,byte +0x1
000001E3  C6813FB1000020    mov byte [ecx+0xb13f],0x20
000001EA  89C8              mov eax,ecx
000001EC  F7EB              imul ebx
000001EE  89C8              mov eax,ecx
000001F0  C1F81F            sar eax,byte 0x1f
000001F3  D1FA              sar edx,1
000001F5  29C2              sub edx,eax
000001F7  8D0492            lea eax,[edx+edx*4]
000001FA  8D0442            lea eax,[edx+eax*2]
000001FD  39C1              cmp ecx,eax
000001FF  75DF              jnz 0x1e0
00000201  83C601            add esi,byte +0x1
00000204  83C701            add edi,byte +0x1
00000207  8344243801        add dword [esp+0x38],byte +0x1
0000020C  39EE              cmp esi,ebp
0000020E  75BD              jnz 0x1cd
00000210  8DB140B10000      lea esi,[ecx+0xb140]
00000216  0FB61E            movzx ebx,byte [esi]
00000219  BABBFFFFFF        mov edx,0xffffffbb
0000021E  89D0              mov eax,edx
00000220  F6EB              imul bl
00000222  66C1E808          shr ax,byte 0x8
00000226  8D1403            lea edx,[ebx+eax]
00000229  89D8              mov eax,ebx
0000022B  C0F807            sar al,byte 0x7
0000022E  C0FA03            sar dl,byte 0x3
00000231  29C2              sub edx,eax
00000233  8D0492            lea eax,[edx+edx*4]
00000236  8D1442            lea edx,[edx+eax*2]
00000239  38D3              cmp bl,dl
0000023B  7435              jz 0x272
0000023D  81C141B10000      add ecx,0xb141
00000243  BFBBFFFFFF        mov edi,0xffffffbb
00000248  C60620            mov byte [esi],0x20
0000024B  89CE              mov esi,ecx
0000024D  89F8              mov eax,edi
0000024F  0FB61E            movzx ebx,byte [esi]
00000252  83C101            add ecx,byte +0x1
00000255  F6EB              imul bl
00000257  66C1E808          shr ax,byte 0x8
0000025B  8D1403            lea edx,[ebx+eax]
0000025E  89D8              mov eax,ebx
00000260  C0F807            sar al,byte 0x7
00000263  C0FA03            sar dl,byte 0x3
00000266  29C2              sub edx,eax
00000268  8D0492            lea eax,[edx+edx*4]
0000026B  8D1442            lea edx,[edx+eax*2]
0000026E  38D3              cmp bl,dl
00000270  75D6              jnz 0x248
00000272  C60600            mov byte [esi],0x0
00000275  C744243440B10000  mov dword [esp+0x34],0xb140
0000027D  C744243C00000000  mov dword [esp+0x3c],0x0
00000285  C744242C00000000  mov dword [esp+0x2c],0x0
0000028D  8B54242C          mov edx,[esp+0x2c]
00000291  85D2              test edx,edx
00000293  0F85E2010000      jnz dword 0x47b
00000299  0FB60540B10000    movzx eax,byte [dword 0xb140]
000002A0  BA00000200        mov edx,0x20000
000002A5  0FB62D46B10000    movzx ebp,byte [dword 0xb146]
000002AC  0FB61D49B10000    movzx ebx,byte [dword 0xb149]
000002B3  0FB63D47B10000    movzx edi,byte [dword 0xb147]
000002BA  88442431          mov [esp+0x31],al
000002BE  0FB60541B10000    movzx eax,byte [dword 0xb141]
000002C5  0FB63548B10000    movzx esi,byte [dword 0xb148]
000002CC  88442414          mov [esp+0x14],al
000002D0  0FB60542B10000    movzx eax,byte [dword 0xb142]
000002D7  88442418          mov [esp+0x18],al
000002DB  0FB60543B10000    movzx eax,byte [dword 0xb143]
000002E2  8844241C          mov [esp+0x1c],al
000002E6  0FB60544B10000    movzx eax,byte [dword 0xb144]
000002ED  88442420          mov [esp+0x20],al
000002F1  0FB60545B10000    movzx eax,byte [dword 0xb145]
000002F8  88442424          mov [esp+0x24],al
000002FC  0FB6054AB10000    movzx eax,byte [dword 0xb14a]
00000303  8844242B          mov [esp+0x2b],al
00000307  89E8              mov eax,ebp
00000309  89DD              mov ebp,ebx
0000030B  88442430          mov [esp+0x30],al
0000030F  EB6A              jmp short 0x37b
00000311  90                nop
00000312  90                nop
00000313  90                nop
00000314  90                nop
00000315  90                nop
00000316  90                nop
00000317  90                nop
00000318  0FB65C2418        movzx ebx,byte [esp+0x18]
0000031D  31C9              xor ecx,ecx
0000031F  385A02            cmp [edx+0x2],bl
00000322  0FB65C241C        movzx ebx,byte [esp+0x1c]
00000327  0F45C1            cmovnz eax,ecx
0000032A  385A03            cmp [edx+0x3],bl
0000032D  0FB65C2420        movzx ebx,byte [esp+0x20]
00000332  0F45C1            cmovnz eax,ecx
00000335  385A04            cmp [edx+0x4],bl
00000338  0FB65C2424        movzx ebx,byte [esp+0x24]
0000033D  0F45C1            cmovnz eax,ecx
00000340  385A05            cmp [edx+0x5],bl
00000343  0FB65C2430        movzx ebx,byte [esp+0x30]
00000348  0F45C1            cmovnz eax,ecx
0000034B  385A06            cmp [edx+0x6],bl
0000034E  89FB              mov ebx,edi
00000350  0F45C1            cmovnz eax,ecx
00000353  385A07            cmp [edx+0x7],bl
00000356  89F3              mov ebx,esi
00000358  0F45C1            cmovnz eax,ecx
0000035B  385A08            cmp [edx+0x8],bl
0000035E  0F45C1            cmovnz eax,ecx
00000361  89E9              mov ecx,ebp
00000363  384A09            cmp [edx+0x9],cl
00000366  0F84AC000000      jz dword 0x418
0000036C  83C220            add edx,byte +0x20
0000036F  81FA00000300      cmp edx,0x30000
00000375  0F84C8000000      jz dword 0x443
0000037B  0FB65C2414        movzx ebx,byte [esp+0x14]
00000380  31C0              xor eax,eax
00000382  385A01            cmp [edx+0x1],bl
00000385  0FB60A            movzx ecx,byte [edx]
00000388  758E              jnz 0x318
0000038A  31C0              xor eax,eax
0000038C  3A4C2431          cmp cl,[esp+0x31]
00000390  0F94C0            setz al
00000393  EB83              jmp short 0x318
00000395  90                nop
00000396  90                nop
00000397  90                nop
00000398  89C8              mov eax,ecx
0000039A  F7EB              imul ebx
0000039C  89C8              mov eax,ecx
0000039E  C1F81F            sar eax,byte 0x1f
000003A1  D1FA              sar edx,1
000003A3  29C2              sub edx,eax
000003A5  8D0492            lea eax,[edx+edx*4]
000003A8  8D0442            lea eax,[edx+eax*2]
000003AB  89CA              mov edx,ecx
000003AD  29C2              sub edx,eax
000003AF  83FA08            cmp edx,byte +0x8
000003B2  0F8F60070000      jg dword 0xb18
000003B8  83E207            and edx,byte +0x7
000003BB  0F8402FEFFFF      jz dword 0x1c3
000003C1  90                nop
000003C2  90                nop
000003C3  90                nop
000003C4  90                nop
000003C5  90                nop
000003C6  90                nop
000003C7  90                nop
000003C8  83C101            add ecx,byte +0x1
000003CB  89C8              mov eax,ecx
000003CD  F7EB              imul ebx
000003CF  89C8              mov eax,ecx
000003D1  C1F81F            sar eax,byte 0x1f
000003D4  C6813FB1000020    mov byte [ecx+0xb13f],0x20
000003DB  D1FA              sar edx,1
000003DD  29C2              sub edx,eax
000003DF  8D0492            lea eax,[edx+edx*4]
000003E2  8D0442            lea eax,[edx+eax*2]
000003E5  89CA              mov edx,ecx
000003E7  29C2              sub edx,eax
000003E9  89D0              mov eax,edx
000003EB  A807              test al,0x7
000003ED  75D9              jnz 0x3c8
000003EF  E9CFFDFFFF        jmp dword 0x1c3
000003F4  90                nop
000003F5  90                nop
000003F6  90                nop
000003F7  90                nop
000003F8  888140B10000      mov [ecx+0xb140],al
000003FE  83C101            add ecx,byte +0x1
00000401  E99AFDFFFF        jmp dword 0x1a0
00000406  90                nop
00000407  90                nop
00000408  C68140B1000005    mov byte [ecx+0xb140],0x5
0000040F  83C101            add ecx,byte +0x1
00000412  E9ACFDFFFF        jmp dword 0x1c3
00000417  90                nop
00000418  0FB64C242B        movzx ecx,byte [esp+0x2b]
0000041D  384A0A            cmp [edx+0xa],cl
00000420  0F8546FFFFFF      jnz dword 0x36c
00000426  85C0              test eax,eax
00000428  0F843EFFFFFF      jz dword 0x36c
0000042E  0FB74214          movzx eax,word [edx+0x14]
00000432  8944242C          mov [esp+0x2c],eax
00000436  0FB7421A          movzx eax,word [edx+0x1a]
0000043A  C164242C10        shl dword [esp+0x2c],byte 0x10
0000043F  0944242C          or [esp+0x2c],eax
00000443  8344243C01        add dword [esp+0x3c],byte +0x1
00000448  8B442438          mov eax,[esp+0x38]
0000044C  834424340B        add dword [esp+0x34],byte +0xb
00000451  3944243C          cmp [esp+0x3c],eax
00000455  0F8C32FEFFFF      jl dword 0x28d
0000045B  8B44242C          mov eax,[esp+0x2c]
0000045F  85C0              test eax,eax
00000461  0F8412020000      jz dword 0x679
00000467  A32CB10000        mov [0xb12c],eax
0000046C  A328B10000        mov [0xb128],eax
00000471  83C44C            add esp,byte +0x4c
00000474  31C0              xor eax,eax
00000476  5B                pop ebx
00000477  5E                pop esi
00000478  5F                pop edi
00000479  5D                pop ebp
0000047A  C3                ret
0000047B  8B44242C          mov eax,[esp+0x2c]
0000047F  31FF              xor edi,edi
00000481  31DB              xor ebx,ebx
00000483  0FB70D0B7C0000    movzx ecx,word [dword 0x7c0b]
0000048A  8D68FE            lea ebp,[eax-0x2]
0000048D  0FB6050D7C0000    movzx eax,byte [dword 0x7c0d]
00000494  0FAFE8            imul ebp,eax
00000497  89442414          mov [esp+0x14],eax
0000049B  0FAFC1            imul eax,ecx
0000049E  032D24B10000      add ebp,[dword 0xb124]
000004A4  8D90FF3F0000      lea edx,[eax+0x3fff]
000004AA  81FAFE7F0000      cmp edx,0x7ffe
000004B0  0F8722010000      ja dword 0x5d8
000004B6  25FF3F0000        and eax,0x3fff
000004BB  7425              jz 0x4e2
000004BD  99                cdq
000004BE  F7F9              idiv ecx
000004C0  C1E30E            shl ebx,byte 0xe
000004C3  89442408          mov [esp+0x8],eax
000004C7  89D8              mov eax,ebx
000004C9  81C300100000      add ebx,0x1000
000004CF  99                cdq
000004D0  F7F9              idiv ecx
000004D2  891C24            mov [esp],ebx
000004D5  01E8              add eax,ebp
000004D7  89442404          mov [esp+0x4],eax
000004DB  E875FBFFFF        call dword 0x55
000004E0  01C7              add edi,eax
000004E2  85FF              test edi,edi
000004E4  8D471F            lea eax,[edi+0x1f]
000004E7  0F48F8            cmovs edi,eax
000004EA  89FE              mov esi,edi
000004EC  C1FE05            sar esi,byte 0x5
000004EF  85F6              test esi,esi
000004F1  0F8E59010000      jng dword 0x650
000004F7  8B4C2434          mov ecx,[esp+0x34]
000004FB  31DB              xor ebx,ebx
000004FD  BA00100000        mov edx,0x1000
00000502  895C2414          mov [esp+0x14],ebx
00000506  0FB601            movzx eax,byte [ecx]
00000509  0FB67902          movzx edi,byte [ecx+0x2]
0000050D  0FB66903          movzx ebp,byte [ecx+0x3]
00000511  88442433          mov [esp+0x33],al
00000515  0FB64101          movzx eax,byte [ecx+0x1]
00000519  88442418          mov [esp+0x18],al
0000051D  0FB64104          movzx eax,byte [ecx+0x4]
00000521  8844241C          mov [esp+0x1c],al
00000525  0FB64105          movzx eax,byte [ecx+0x5]
00000529  88442420          mov [esp+0x20],al
0000052D  0FB64106          movzx eax,byte [ecx+0x6]
00000531  88442424          mov [esp+0x24],al
00000535  0FB64107          movzx eax,byte [ecx+0x7]
00000539  8844242B          mov [esp+0x2b],al
0000053D  0FB64108          movzx eax,byte [ecx+0x8]
00000541  88442430          mov [esp+0x30],al
00000545  0FB64109          movzx eax,byte [ecx+0x9]
00000549  88442431          mov [esp+0x31],al
0000054D  0FB6410A          movzx eax,byte [ecx+0xa]
00000551  88442432          mov [esp+0x32],al
00000555  EB66              jmp short 0x5bd
00000557  90                nop
00000558  31C9              xor ecx,ecx
0000055A  89FB              mov ebx,edi
0000055C  385A02            cmp [edx+0x2],bl
0000055F  89EB              mov ebx,ebp
00000561  0F45C1            cmovnz eax,ecx
00000564  385A03            cmp [edx+0x3],bl
00000567  0FB65C241C        movzx ebx,byte [esp+0x1c]
0000056C  0F45C1            cmovnz eax,ecx
0000056F  385A04            cmp [edx+0x4],bl
00000572  0FB65C2420        movzx ebx,byte [esp+0x20]
00000577  0F45C1            cmovnz eax,ecx
0000057A  385A05            cmp [edx+0x5],bl
0000057D  0FB65C2424        movzx ebx,byte [esp+0x24]
00000582  0F45C1            cmovnz eax,ecx
00000585  385A06            cmp [edx+0x6],bl
00000588  0FB65C242B        movzx ebx,byte [esp+0x2b]
0000058D  0F45C1            cmovnz eax,ecx
00000590  385A07            cmp [edx+0x7],bl
00000593  0FB65C2430        movzx ebx,byte [esp+0x30]
00000598  0F45C1            cmovnz eax,ecx
0000059B  385A08            cmp [edx+0x8],bl
0000059E  0F45C1            cmovnz eax,ecx
000005A1  0FB64C2431        movzx ecx,byte [esp+0x31]
000005A6  384A09            cmp [edx+0x9],cl
000005A9  747D              jz 0x628
000005AB  8344241401        add dword [esp+0x14],byte +0x1
000005B0  83C220            add edx,byte +0x20
000005B3  39742414          cmp [esp+0x14],esi
000005B7  0F8493000000      jz dword 0x650
000005BD  0FB65C2418        movzx ebx,byte [esp+0x18]
000005C2  31C0              xor eax,eax
000005C4  385A01            cmp [edx+0x1],bl
000005C7  0FB60A            movzx ecx,byte [edx]
000005CA  758C              jnz 0x558
000005CC  31C0              xor eax,eax
000005CE  3A4C2433          cmp cl,[esp+0x33]
000005D2  0F94C0            setz al
000005D5  EB81              jmp short 0x558
000005D7  90                nop
000005D8  B800400000        mov eax,0x4000
000005DD  89DE              mov esi,ebx
000005DF  99                cdq
000005E0  83C301            add ebx,byte +0x1
000005E3  F7F9              idiv ecx
000005E5  C1E60E            shl esi,byte 0xe
000005E8  89442408          mov [esp+0x8],eax
000005EC  89F0              mov eax,esi
000005EE  81C600100000      add esi,0x1000
000005F4  99                cdq
000005F5  F7F9              idiv ecx
000005F7  893424            mov [esp],esi
000005FA  01E8              add eax,ebp
000005FC  89442404          mov [esp+0x4],eax
00000600  E850FAFFFF        call dword 0x55
00000605  0FB70D0B7C0000    movzx ecx,word [dword 0x7c0b]
0000060C  01C7              add edi,eax
0000060E  8B442414          mov eax,[esp+0x14]
00000612  0FAFC1            imul eax,ecx
00000615  89C2              mov edx,eax
00000617  C1FA0E            sar edx,byte 0xe
0000061A  39D3              cmp ebx,edx
0000061C  7CBA              jl 0x5d8
0000061E  E993FEFFFF        jmp dword 0x4b6
00000623  90                nop
00000624  90                nop
00000625  90                nop
00000626  90                nop
00000627  90                nop
00000628  0FB64C2432        movzx ecx,byte [esp+0x32]
0000062D  384A0A            cmp [edx+0xa],cl
00000630  0F8575FFFFFF      jnz dword 0x5ab
00000636  85C0              test eax,eax
00000638  0F846DFFFFFF      jz dword 0x5ab
0000063E  0FB74214          movzx eax,word [edx+0x14]
00000642  0FB7521A          movzx edx,word [edx+0x1a]
00000646  C1E010            shl eax,byte 0x10
00000649  09D0              or eax,edx
0000064B  7539              jnz 0x686
0000064D  90                nop
0000064E  90                nop
0000064F  90                nop
00000650  0FB60520B10000    movzx eax,byte [dword 0xb120]
00000657  3C0C              cmp al,0xc
00000659  7435              jz 0x690
0000065B  3C10              cmp al,0x10
0000065D  8B44242C          mov eax,[esp+0x2c]
00000661  0F8441020000      jz dword 0x8a8
00000667  8B048500000100    mov eax,[eax*4+0x10000]
0000066E  3DF5FFFF0F        cmp eax,0xffffff5
00000673  8944242C          mov [esp+0x2c],eax
00000677  7647              jna 0x6c0
00000679  83C44C            add esp,byte +0x4c
0000067C  B806000000        mov eax,0x6
00000681  5B                pop ebx
00000682  5E                pop esi
00000683  5F                pop edi
00000684  5D                pop ebp
00000685  C3                ret
00000686  8944242C          mov [esp+0x2c],eax
0000068A  E9B4FDFFFF        jmp dword 0x443
0000068F  90                nop
00000690  8B7C242C          mov edi,[esp+0x2c]
00000694  89F8              mov eax,edi
00000696  D1E8              shr eax,1
00000698  01F8              add eax,edi
0000069A  83E701            and edi,byte +0x1
0000069D  0FB7840000000100  movzx eax,word [eax+eax+0x10000]
000006A5  0F841D020000      jz dword 0x8c8
000006AB  66C1E804          shr ax,byte 0x4
000006AF  0FB7C0            movzx eax,ax
000006B2  8944242C          mov [esp+0x2c],eax
000006B6  817C242CF50F0000  cmp dword [esp+0x2c],0xff5
000006BE  77B9              ja 0x679
000006C0  8B44242C          mov eax,[esp+0x2c]
000006C4  85C0              test eax,eax
000006C6  74B1              jz 0x679
000006C8  8B44242C          mov eax,[esp+0x2c]
000006CC  31ED              xor ebp,ebp
000006CE  31DB              xor ebx,ebx
000006D0  0FB70D0B7C0000    movzx ecx,word [dword 0x7c0b]
000006D7  8D78FE            lea edi,[eax-0x2]
000006DA  0FB6050D7C0000    movzx eax,byte [dword 0x7c0d]
000006E1  0FAFF8            imul edi,eax
000006E4  89442414          mov [esp+0x14],eax
000006E8  0FAFC1            imul eax,ecx
000006EB  033D24B10000      add edi,[dword 0xb124]
000006F1  8D90FF3F0000      lea edx,[eax+0x3fff]
000006F7  81FAFE7F0000      cmp edx,0x7ffe
000006FD  0F8725010000      ja dword 0x828
00000703  25FF3F0000        and eax,0x3fff
00000708  7425              jz 0x72f
0000070A  99                cdq
0000070B  F7F9              idiv ecx
0000070D  C1E30E            shl ebx,byte 0xe
00000710  89442408          mov [esp+0x8],eax
00000714  89D8              mov eax,ebx
00000716  81C300100000      add ebx,0x1000
0000071C  99                cdq
0000071D  F7F9              idiv ecx
0000071F  891C24            mov [esp],ebx
00000722  01F8              add eax,edi
00000724  89442404          mov [esp+0x4],eax
00000728  E828F9FFFF        call dword 0x55
0000072D  01C5              add ebp,eax
0000072F  85ED              test ebp,ebp
00000731  89EE              mov esi,ebp
00000733  8D451F            lea eax,[ebp+0x1f]
00000736  0F48F0            cmovs esi,eax
00000739  C1FE05            sar esi,byte 0x5
0000073C  85F6              test esi,esi
0000073E  0F8E0CFFFFFF      jng dword 0x650
00000744  8B4C2434          mov ecx,[esp+0x34]
00000748  31DB              xor ebx,ebx
0000074A  BA00100000        mov edx,0x1000
0000074F  895C2414          mov [esp+0x14],ebx
00000753  0FB601            movzx eax,byte [ecx]
00000756  0FB66903          movzx ebp,byte [ecx+0x3]
0000075A  0FB67904          movzx edi,byte [ecx+0x4]
0000075E  88442433          mov [esp+0x33],al
00000762  0FB64101          movzx eax,byte [ecx+0x1]
00000766  88442418          mov [esp+0x18],al
0000076A  0FB64102          movzx eax,byte [ecx+0x2]
0000076E  8844241C          mov [esp+0x1c],al
00000772  0FB64105          movzx eax,byte [ecx+0x5]
00000776  88442420          mov [esp+0x20],al
0000077A  0FB64106          movzx eax,byte [ecx+0x6]
0000077E  88442424          mov [esp+0x24],al
00000782  0FB64107          movzx eax,byte [ecx+0x7]
00000786  8844242B          mov [esp+0x2b],al
0000078A  0FB64108          movzx eax,byte [ecx+0x8]
0000078E  88442430          mov [esp+0x30],al
00000792  0FB64109          movzx eax,byte [ecx+0x9]
00000796  88442431          mov [esp+0x31],al
0000079A  0FB6410A          movzx eax,byte [ecx+0xa]
0000079E  88442432          mov [esp+0x32],al
000007A2  EB69              jmp short 0x80d
000007A4  90                nop
000007A5  90                nop
000007A6  90                nop
000007A7  90                nop
000007A8  0FB65C241C        movzx ebx,byte [esp+0x1c]
000007AD  31C9              xor ecx,ecx
000007AF  385A02            cmp [edx+0x2],bl
000007B2  89EB              mov ebx,ebp
000007B4  0F45C1            cmovnz eax,ecx
000007B7  385A03            cmp [edx+0x3],bl
000007BA  89FB              mov ebx,edi
000007BC  0F45C1            cmovnz eax,ecx
000007BF  385A04            cmp [edx+0x4],bl
000007C2  0FB65C2420        movzx ebx,byte [esp+0x20]
000007C7  0F45C1            cmovnz eax,ecx
000007CA  385A05            cmp [edx+0x5],bl
000007CD  0FB65C2424        movzx ebx,byte [esp+0x24]
000007D2  0F45C1            cmovnz eax,ecx
000007D5  385A06            cmp [edx+0x6],bl
000007D8  0FB65C242B        movzx ebx,byte [esp+0x2b]
000007DD  0F45C1            cmovnz eax,ecx
000007E0  385A07            cmp [edx+0x7],bl
000007E3  0FB65C2430        movzx ebx,byte [esp+0x30]
000007E8  0F45C1            cmovnz eax,ecx
000007EB  385A08            cmp [edx+0x8],bl
000007EE  0FB65C2431        movzx ebx,byte [esp+0x31]
000007F3  0F45C1            cmovnz eax,ecx
000007F6  385A09            cmp [edx+0x9],bl
000007F9  747D              jz 0x878
000007FB  8344241401        add dword [esp+0x14],byte +0x1
00000800  83C220            add edx,byte +0x20
00000803  39742414          cmp [esp+0x14],esi
00000807  0F8443FEFFFF      jz dword 0x650
0000080D  0FB65C2418        movzx ebx,byte [esp+0x18]
00000812  31C0              xor eax,eax
00000814  385A01            cmp [edx+0x1],bl
00000817  0FB60A            movzx ecx,byte [edx]
0000081A  758C              jnz 0x7a8
0000081C  31C0              xor eax,eax
0000081E  3A4C2433          cmp cl,[esp+0x33]
00000822  0F94C0            setz al
00000825  EB81              jmp short 0x7a8
00000827  90                nop
00000828  B800400000        mov eax,0x4000
0000082D  89DE              mov esi,ebx
0000082F  99                cdq
00000830  83C301            add ebx,byte +0x1
00000833  F7F9              idiv ecx
00000835  C1E60E            shl esi,byte 0xe
00000838  89442408          mov [esp+0x8],eax
0000083C  89F0              mov eax,esi
0000083E  81C600100000      add esi,0x1000
00000844  99                cdq
00000845  F7F9              idiv ecx
00000847  893424            mov [esp],esi
0000084A  01F8              add eax,edi
0000084C  89442404          mov [esp+0x4],eax
00000850  E800F8FFFF        call dword 0x55
00000855  0FB70D0B7C0000    movzx ecx,word [dword 0x7c0b]
0000085C  01C5              add ebp,eax
0000085E  8B442414          mov eax,[esp+0x14]
00000862  0FAFC1            imul eax,ecx
00000865  89C2              mov edx,eax
00000867  C1FA0E            sar edx,byte 0xe
0000086A  39D3              cmp ebx,edx
0000086C  7CBA              jl 0x828
0000086E  E990FEFFFF        jmp dword 0x703
00000873  90                nop
00000874  90                nop
00000875  90                nop
00000876  90                nop
00000877  90                nop
00000878  0FB65C2432        movzx ebx,byte [esp+0x32]
0000087D  385A0A            cmp [edx+0xa],bl
00000880  0F8575FFFFFF      jnz dword 0x7fb
00000886  85C0              test eax,eax
00000888  0F846DFFFFFF      jz dword 0x7fb
0000088E  0FB74214          movzx eax,word [edx+0x14]
00000892  0FB7521A          movzx edx,word [edx+0x1a]
00000896  C1E010            shl eax,byte 0x10
00000899  09D0              or eax,edx
0000089B  0F84AFFDFFFF      jz dword 0x650
000008A1  E9E0FDFFFF        jmp dword 0x686
000008A6  90                nop
000008A7  90                nop
000008A8  0FB7840000000100  movzx eax,word [eax+eax+0x10000]
000008B0  3DF5FF0000        cmp eax,0xfff5
000008B5  8944242C          mov [esp+0x2c],eax
000008B9  0F87BAFDFFFF      ja dword 0x679
000008BF  E9FCFDFFFF        jmp dword 0x6c0
000008C4  90                nop
000008C5  90                nop
000008C6  90                nop
000008C7  90                nop
000008C8  25FF0F0000        and eax,0xfff
000008CD  8944242C          mov [esp+0x2c],eax
000008D1  E9E0FDFFFF        jmp dword 0x6b6
000008D6  0FB71D117C0000    movzx ebx,word [dword 0x7c11]
000008DD  0FB70D0E7C0000    movzx ecx,word [dword 0x7c0e]
000008E4  030D1C7C0000      add ecx,[dword 0x7c1c]
000008EA  6685DB            test bx,bx
000008ED  0F84AD020000      jz dword 0xba0
000008F3  0FB715137C0000    movzx edx,word [dword 0x7c13]
000008FA  B810000000        mov eax,0x10
000008FF  6685D2            test dx,dx
00000902  741C              jz 0x920
00000904  0FB6350D7C0000    movzx esi,byte [dword 0x7c0d]
0000090B  0FB7C2            movzx eax,dx
0000090E  99                cdq
0000090F  F7FE              idiv esi
00000911  3DF10F0000        cmp eax,0xff1
00000916  0F9DC0            setnl al
00000919  8D04850C000000    lea eax,[eax*4+0xc]
00000920  A220B10000        mov [0xb120],al
00000925  0FB605107C0000    movzx eax,byte [dword 0x7c10]
0000092C  31D2              xor edx,edx
0000092E  0FB735167C0000    movzx esi,word [dword 0x7c16]
00000935  0FAFF0            imul esi,eax
00000938  89D8              mov eax,ebx
0000093A  01CE              add esi,ecx
0000093C  0FB70D0B7C0000    movzx ecx,word [dword 0x7c0b]
00000943  89CF              mov edi,ecx
00000945  66C1EF05          shr di,byte 0x5
00000949  66F7F7            div di
0000094C  0FB7C0            movzx eax,ax
0000094F  01F0              add eax,esi
00000951  A324B10000        mov [0xb124],eax
00000956  0FB7C3            movzx eax,bx
00000959  C1E005            shl eax,byte 0x5
0000095C  3D00000100        cmp eax,0x10000
00000961  0F8FBE010000      jg dword 0xb25
00000967  99                cdq
00000968  F7F9              idiv ecx
0000096A  89C7              mov edi,eax
0000096C  89C8              mov eax,ecx
0000096E  0FAFC7            imul eax,edi
00000971  8D90FF3F0000      lea edx,[eax+0x3fff]
00000977  81FAFE7F0000      cmp edx,0x7ffe
0000097D  7651              jna 0x9d0
0000097F  31DB              xor ebx,ebx
00000981  3DFF3F0000        cmp eax,0x3fff
00000986  0F8EB4040000      jng dword 0xe40
0000098C  B800400000        mov eax,0x4000
00000991  89DD              mov ebp,ebx
00000993  99                cdq
00000994  83C301            add ebx,byte +0x1
00000997  F7F9              idiv ecx
00000999  C1E50E            shl ebp,byte 0xe
0000099C  89442408          mov [esp+0x8],eax
000009A0  89E8              mov eax,ebp
000009A2  81C500000200      add ebp,0x20000
000009A8  99                cdq
000009A9  F7F9              idiv ecx
000009AB  892C24            mov [esp],ebp
000009AE  01F0              add eax,esi
000009B0  89442404          mov [esp+0x4],eax
000009B4  E89CF6FFFF        call dword 0x55
000009B9  0FB70D0B7C0000    movzx ecx,word [dword 0x7c0b]
000009C0  89F8              mov eax,edi
000009C2  0FAFC1            imul eax,ecx
000009C5  89C2              mov edx,eax
000009C7  C1FA0E            sar edx,byte 0xe
000009CA  39D3              cmp ebx,edx
000009CC  7CBE              jl 0x98c
000009CE  EB0E              jmp short 0x9de
000009D0  0FB70D0B7C0000    movzx ecx,word [dword 0x7c0b]
000009D7  89F8              mov eax,edi
000009D9  31DB              xor ebx,ebx
000009DB  0FAFC1            imul eax,ecx
000009DE  25FF3F0000        and eax,0x3fff
000009E3  0F85E4030000      jnz dword 0xdcd
000009E9  0FB71D167C0000    movzx ebx,word [dword 0x7c16]
000009F0  6685DB            test bx,bx
000009F3  7506              jnz 0x9fb
000009F5  8B1D247C0000      mov ebx,[dword 0x7c24]
000009FB  89D8              mov eax,ebx
000009FD  0FAFC1            imul eax,ecx
00000A00  3D00000100        cmp eax,0x10000
00000A05  7E0D              jng 0xa14
00000A07  B800000100        mov eax,0x10000
00000A0C  99                cdq
00000A0D  F7F9              idiv ecx
00000A0F  89C3              mov ebx,eax
00000A11  0FAFC1            imul eax,ecx
00000A14  C70528B100000000  mov dword [dword 0xb128],0x0
         -0000
00000A1E  8D90FF3F0000      lea edx,[eax+0x3fff]
00000A24  0FB73D0E7C0000    movzx edi,word [dword 0x7c0e]
00000A2B  033D1C7C0000      add edi,[dword 0x7c1c]
00000A31  81FAFE7F0000      cmp edx,0x7ffe
00000A37  7778              ja 0xab1
00000A39  0FB70D0B7C0000    movzx ecx,word [dword 0x7c0b]
00000A40  31F6              xor esi,esi
00000A42  0FAFD9            imul ebx,ecx
00000A45  89DA              mov edx,ebx
00000A47  F7C2FF3F0000      test edx,0x3fff
00000A4D  7437              jz 0xa86
00000A4F  89D0              mov eax,edx
00000A51  C1F81F            sar eax,byte 0x1f
00000A54  C1E812            shr eax,byte 0x12
00000A57  01C2              add edx,eax
00000A59  81E2FF3F0000      and edx,0x3fff
00000A5F  29C2              sub edx,eax
00000A61  89D0              mov eax,edx
00000A63  99                cdq
00000A64  F7F9              idiv ecx
00000A66  C1E60E            shl esi,byte 0xe
00000A69  89442408          mov [esp+0x8],eax
00000A6D  89F0              mov eax,esi
00000A6F  81C600000100      add esi,0x10000
00000A75  99                cdq
00000A76  F7F9              idiv ecx
00000A78  893424            mov [esp],esi
00000A7B  01F8              add eax,edi
00000A7D  89442404          mov [esp+0x4],eax
00000A81  E8CFF5FFFF        call dword 0x55
00000A86  8B442460          mov eax,[esp+0x60]
00000A8A  C70530B100000100  mov dword [dword 0xb130],0x1
         -0000
00000A94  803800            cmp byte [eax],0x0
00000A97  0F85B5F6FFFF      jnz dword 0x152
00000A9D  BE40B10000        mov esi,0xb140
00000AA2  31C9              xor ecx,ecx
00000AA4  C744243801000000  mov dword [esp+0x38],0x1
00000AAC  E965F7FFFF        jmp dword 0x216
00000AB1  31F6              xor esi,esi
00000AB3  3DFF3F0000        cmp eax,0x3fff
00000AB8  0F8E71030000      jng dword 0xe2f
00000ABE  90                nop
00000ABF  90                nop
00000AC0  B800400000        mov eax,0x4000
00000AC5  89F5              mov ebp,esi
00000AC7  99                cdq
00000AC8  83C601            add esi,byte +0x1
00000ACB  F7F9              idiv ecx
00000ACD  C1E50E            shl ebp,byte 0xe
00000AD0  89442408          mov [esp+0x8],eax
00000AD4  89E8              mov eax,ebp
00000AD6  81C500000100      add ebp,0x10000
00000ADC  99                cdq
00000ADD  F7F9              idiv ecx
00000ADF  892C24            mov [esp],ebp
00000AE2  01F8              add eax,edi
00000AE4  89442404          mov [esp+0x4],eax
00000AE8  E868F5FFFF        call dword 0x55
00000AED  0FB70D0B7C0000    movzx ecx,word [dword 0x7c0b]
00000AF4  89DA              mov edx,ebx
00000AF6  0FAFD1            imul edx,ecx
00000AF9  85D2              test edx,edx
00000AFB  8D82FF3F0000      lea eax,[edx+0x3fff]
00000B01  0F49C2            cmovns eax,edx
00000B04  C1F80E            sar eax,byte 0xe
00000B07  39C6              cmp esi,eax
00000B09  7CB5              jl 0xac0
00000B0B  E937FFFFFF        jmp dword 0xa47
00000B10  3C2F              cmp al,0x2f
00000B12  0F84ABF6FFFF      jz dword 0x1c3
00000B18  83C44C            add esp,byte +0x4c
00000B1B  B802000000        mov eax,0x2
00000B20  5B                pop ebx
00000B21  5E                pop esi
00000B22  5F                pop edi
00000B23  5D                pop ebp
00000B24  C3                ret
00000B25  B800000100        mov eax,0x10000
00000B2A  99                cdq
00000B2B  F7F9              idiv ecx
00000B2D  89C7              mov edi,eax
00000B2F  89C8              mov eax,ecx
00000B31  0FAFC7            imul eax,edi
00000B34  8D90FF3F0000      lea edx,[eax+0x3fff]
00000B3A  81FAFE7F0000      cmp edx,0x7ffe
00000B40  0F868AFEFFFF      jna dword 0x9d0
00000B46  31DB              xor ebx,ebx
00000B48  3DFF3F0000        cmp eax,0x3fff
00000B4D  0F8EED020000      jng dword 0xe40
00000B53  B800400000        mov eax,0x4000
00000B58  89DD              mov ebp,ebx
00000B5A  99                cdq
00000B5B  83C301            add ebx,byte +0x1
00000B5E  F7F9              idiv ecx
00000B60  C1E50E            shl ebp,byte 0xe
00000B63  89442408          mov [esp+0x8],eax
00000B67  89E8              mov eax,ebp
00000B69  81C500000200      add ebp,0x20000
00000B6F  99                cdq
00000B70  F7F9              idiv ecx
00000B72  892C24            mov [esp],ebp
00000B75  01F0              add eax,esi
00000B77  89442404          mov [esp+0x4],eax
00000B7B  E8D5F4FFFF        call dword 0x55
00000B80  0FB70D0B7C0000    movzx ecx,word [dword 0x7c0b]
00000B87  89F8              mov eax,edi
00000B89  0FAFC1            imul eax,ecx
00000B8C  89C2              mov edx,eax
00000B8E  C1FA0E            sar edx,byte 0xe
00000B91  39D3              cmp ebx,edx
00000B93  7CBE              jl 0xb53
00000B95  E944FEFFFF        jmp dword 0x9de
00000B9A  90                nop
00000B9B  90                nop
00000B9C  90                nop
00000B9D  90                nop
00000B9E  90                nop
00000B9F  90                nop
00000BA0  C60520B1000020    mov byte [dword 0xb120],0x20
00000BA7  0FB605107C0000    movzx eax,byte [dword 0x7c10]
00000BAE  0FAF05247C0000    imul eax,[dword 0x7c24]
00000BB5  01C8              add eax,ecx
00000BB7  A324B10000        mov [0xb124],eax
00000BBC  8B352C7C0000      mov esi,[dword 0x7c2c]
00000BC2  0FB63D0D7C0000    movzx edi,byte [dword 0x7c0d]
00000BC9  0FB7150B7C0000    movzx edx,word [dword 0x7c0b]
00000BD0  C744241400000000  mov dword [esp+0x14],0x0
00000BD8  8D6EFE            lea ebp,[esi-0x2]
00000BDB  0FAFEF            imul ebp,edi
00000BDE  0FB7CA            movzx ecx,dx
00000BE1  01C5              add ebp,eax
00000BE3  89F8              mov eax,edi
00000BE5  0FAFC1            imul eax,ecx
00000BE8  8D98FF3F0000      lea ebx,[eax+0x3fff]
00000BEE  81FBFE7F0000      cmp ebx,0x7ffe
00000BF4  BB00000000        mov ebx,0x0
00000BF9  773F              ja 0xc3a
00000BFB  25FF3F0000        and eax,0x3fff
00000C00  0F85F6010000      jnz dword 0xdfc
00000C06  8B2D2C7C0000      mov ebp,[dword 0x7c2c]
00000C0C  90                nop
00000C0D  90                nop
00000C0E  90                nop
00000C0F  90                nop
00000C10  0FB60520B10000    movzx eax,byte [dword 0xb120]
00000C17  3C0C              cmp al,0xc
00000C19  746D              jz 0xc88
00000C1B  3C10              cmp al,0x10
00000C1D  0F8484010000      jz dword 0xda7
00000C23  8B2CAD00000100    mov ebp,[ebp*4+0x10000]
00000C2A  81FDF5FFFF0F      cmp ebp,0xffffff5
00000C30  767C              jna 0xcae
00000C32  0FB7CA            movzx ecx,dx
00000C35  E9AFFDFFFF        jmp dword 0x9e9
00000C3A  B800400000        mov eax,0x4000
00000C3F  89DE              mov esi,ebx
00000C41  99                cdq
00000C42  83C301            add ebx,byte +0x1
00000C45  F7F9              idiv ecx
00000C47  C1E60E            shl esi,byte 0xe
00000C4A  89442408          mov [esp+0x8],eax
00000C4E  89F0              mov eax,esi
00000C50  81C600000200      add esi,0x20000
00000C56  99                cdq
00000C57  F7F9              idiv ecx
00000C59  893424            mov [esp],esi
00000C5C  01E8              add eax,ebp
00000C5E  89442404          mov [esp+0x4],eax
00000C62  E8EEF3FFFF        call dword 0x55
00000C67  0FB7150B7C0000    movzx edx,word [dword 0x7c0b]
00000C6E  01442414          add [esp+0x14],eax
00000C72  89F8              mov eax,edi
00000C74  0FB7CA            movzx ecx,dx
00000C77  0FAFC1            imul eax,ecx
00000C7A  89C6              mov esi,eax
00000C7C  C1FE0E            sar esi,byte 0xe
00000C7F  39F3              cmp ebx,esi
00000C81  7CB7              jl 0xc3a
00000C83  E973FFFFFF        jmp dword 0xbfb
00000C88  89E8              mov eax,ebp
00000C8A  D1E8              shr eax,1
00000C8C  01E8              add eax,ebp
00000C8E  83E501            and ebp,byte +0x1
00000C91  0FB7840000000100  movzx eax,word [eax+eax+0x10000]
00000C99  0F8421010000      jz dword 0xdc0
00000C9F  66C1E804          shr ax,byte 0x4
00000CA3  0FB7E8            movzx ebp,ax
00000CA6  81FDF50F0000      cmp ebp,0xff5
00000CAC  7784              ja 0xc32
00000CAE  85ED              test ebp,ebp
00000CB0  7480              jz 0xc32
00000CB2  817C2414FFFF0000  cmp dword [esp+0x14],0xffff
00000CBA  0F8F72FFFFFF      jg dword 0xc32
00000CC0  8B442414          mov eax,[esp+0x14]
00000CC4  0FB7CA            movzx ecx,dx
00000CC7  0500000200        add eax,0x20000
00000CCC  89442420          mov [esp+0x20],eax
00000CD0  8D45FE            lea eax,[ebp-0x2]
00000CD3  89C7              mov edi,eax
00000CD5  0FB6050D7C0000    movzx eax,byte [dword 0x7c0d]
00000CDC  0FAFF8            imul edi,eax
00000CDF  8944241C          mov [esp+0x1c],eax
00000CE3  0FAFC1            imul eax,ecx
00000CE6  897C2418          mov [esp+0x18],edi
00000CEA  8B3D24B10000      mov edi,[dword 0xb124]
00000CF0  8D98FF3F0000      lea ebx,[eax+0x3fff]
00000CF6  017C2418          add [esp+0x18],edi
00000CFA  81FBFE7F0000      cmp ebx,0x7ffe
00000D00  0F869B000000      jna dword 0xda1
00000D06  8B742420          mov esi,[esp+0x20]
00000D0A  31FF              xor edi,edi
00000D0C  31DB              xor ebx,ebx
00000D0E  896C2424          mov [esp+0x24],ebp
00000D12  90                nop
00000D13  90                nop
00000D14  90                nop
00000D15  90                nop
00000D16  90                nop
00000D17  90                nop
00000D18  B800400000        mov eax,0x4000
00000D1D  99                cdq
00000D1E  F7F9              idiv ecx
00000D20  893424            mov [esp],esi
00000D23  81C600400000      add esi,0x4000
00000D29  89442408          mov [esp+0x8],eax
00000D2D  89D8              mov eax,ebx
00000D2F  83C301            add ebx,byte +0x1
00000D32  C1E00E            shl eax,byte 0xe
00000D35  99                cdq
00000D36  F7F9              idiv ecx
00000D38  03442418          add eax,[esp+0x18]
00000D3C  89442404          mov [esp+0x4],eax
00000D40  E810F3FFFF        call dword 0x55
00000D45  0FB7150B7C0000    movzx edx,word [dword 0x7c0b]
00000D4C  0FB7CA            movzx ecx,dx
00000D4F  01C7              add edi,eax
00000D51  8B44241C          mov eax,[esp+0x1c]
00000D55  0FAFC1            imul eax,ecx
00000D58  89C5              mov ebp,eax
00000D5A  C1FD0E            sar ebp,byte 0xe
00000D5D  39EB              cmp ebx,ebp
00000D5F  7CB7              jl 0xd18
00000D61  8B6C2424          mov ebp,[esp+0x24]
00000D65  25FF3F0000        and eax,0x3fff
00000D6A  742C              jz 0xd98
00000D6C  99                cdq
00000D6D  F7F9              idiv ecx
00000D6F  C1E30E            shl ebx,byte 0xe
00000D72  89442408          mov [esp+0x8],eax
00000D76  89D8              mov eax,ebx
00000D78  99                cdq
00000D79  F7F9              idiv ecx
00000D7B  035C2420          add ebx,[esp+0x20]
00000D7F  891C24            mov [esp],ebx
00000D82  03442418          add eax,[esp+0x18]
00000D86  89442404          mov [esp+0x4],eax
00000D8A  E8C6F2FFFF        call dword 0x55
00000D8F  0FB7150B7C0000    movzx edx,word [dword 0x7c0b]
00000D96  01C7              add edi,eax
00000D98  017C2414          add [esp+0x14],edi
00000D9C  E96FFEFFFF        jmp dword 0xc10
00000DA1  31FF              xor edi,edi
00000DA3  31DB              xor ebx,ebx
00000DA5  EBBE              jmp short 0xd65
00000DA7  0FB7AC2D00000100  movzx ebp,word [ebp+ebp+0x10000]
00000DAF  81FDF5FF0000      cmp ebp,0xfff5
00000DB5  0F8777FEFFFF      ja dword 0xc32
00000DBB  E9EEFEFFFF        jmp dword 0xcae
00000DC0  89C5              mov ebp,eax
00000DC2  81E5FF0F0000      and ebp,0xfff
00000DC8  E9D9FEFFFF        jmp dword 0xca6
00000DCD  99                cdq
00000DCE  F7F9              idiv ecx
00000DD0  C1E30E            shl ebx,byte 0xe
00000DD3  89442408          mov [esp+0x8],eax
00000DD7  89D8              mov eax,ebx
00000DD9  81C300000200      add ebx,0x20000
00000DDF  99                cdq
00000DE0  F7F9              idiv ecx
00000DE2  891C24            mov [esp],ebx
00000DE5  01F0              add eax,esi
00000DE7  89442404          mov [esp+0x4],eax
00000DEB  E865F2FFFF        call dword 0x55
00000DF0  0FB70D0B7C0000    movzx ecx,word [dword 0x7c0b]
00000DF7  E9EDFBFFFF        jmp dword 0x9e9
00000DFC  99                cdq
00000DFD  F7F9              idiv ecx
00000DFF  C1E30E            shl ebx,byte 0xe
00000E02  89442408          mov [esp+0x8],eax
00000E06  89D8              mov eax,ebx
00000E08  81C300000200      add ebx,0x20000
00000E0E  99                cdq
00000E0F  F7F9              idiv ecx
00000E11  891C24            mov [esp],ebx
00000E14  01E8              add eax,ebp
00000E16  89442404          mov [esp+0x4],eax
00000E1A  E836F2FFFF        call dword 0x55
00000E1F  0FB7150B7C0000    movzx edx,word [dword 0x7c0b]
00000E26  01442414          add [esp+0x14],eax
00000E2A  E9D7FDFFFF        jmp dword 0xc06
00000E2F  0FB70D0B7C0000    movzx ecx,word [dword 0x7c0b]
00000E36  0FAFD9            imul ebx,ecx
00000E39  89DA              mov edx,ebx
00000E3B  E907FCFFFF        jmp dword 0xa47
00000E40  0FB70D0B7C0000    movzx ecx,word [dword 0x7c0b]
00000E47  89F8              mov eax,edi
00000E49  0FAFC1            imul eax,ecx
00000E4C  E98DFBFFFF        jmp dword 0x9de
00000E51  EB0D              jmp short 0xe60
00000E53  90                nop
00000E54  90                nop
00000E55  90                nop
00000E56  90                nop
00000E57  90                nop
00000E58  90                nop
00000E59  90                nop
00000E5A  90                nop
00000E5B  90                nop
00000E5C  90                nop
00000E5D  90                nop
00000E5E  90                nop
00000E5F  90                nop
00000E60  55                push ebp
00000E61  B805000000        mov eax,0x5
00000E66  57                push edi
00000E67  56                push esi
00000E68  53                push ebx
00000E69  83EC2C            sub esp,byte +0x2c
00000E6C  8B352CB10000      mov esi,[dword 0xb12c]
00000E72  85F6              test esi,esi
00000E74  750A              jnz 0xe80
00000E76  83C42C            add esp,byte +0x2c
00000E79  5B                pop ebx
00000E7A  5E                pop esi
00000E7B  5F                pop edi
00000E7C  5D                pop ebp
00000E7D  C3                ret
00000E7E  90                nop
00000E7F  90                nop
00000E80  893528B10000      mov [dword 0xb128],esi
00000E86  0FB62D0D7C0000    movzx ebp,byte [dword 0x7c0d]
00000E8D  83EE02            sub esi,byte +0x2
00000E90  0FB70D0B7C0000    movzx ecx,word [dword 0x7c0b]
00000E97  89E8              mov eax,ebp
00000E99  0FAFC1            imul eax,ecx
00000E9C  0FAFF5            imul esi,ebp
00000E9F  033524B10000      add esi,[dword 0xb124]
00000EA5  8D90FF3F0000      lea edx,[eax+0x3fff]
00000EAB  81FAFE7F0000      cmp edx,0x7ffe
00000EB1  0F86EA010000      jna dword 0x10a1
00000EB7  8B7C2440          mov edi,[esp+0x40]
00000EBB  31C0              xor eax,eax
00000EBD  31DB              xor ebx,ebx
00000EBF  896C2414          mov [esp+0x14],ebp
00000EC3  89C5              mov ebp,eax
00000EC5  90                nop
00000EC6  90                nop
00000EC7  90                nop
00000EC8  B800400000        mov eax,0x4000
00000ECD  99                cdq
00000ECE  F7F9              idiv ecx
00000ED0  893C24            mov [esp],edi
00000ED3  81C700400000      add edi,0x4000
00000ED9  89442408          mov [esp+0x8],eax
00000EDD  89D8              mov eax,ebx
00000EDF  83C301            add ebx,byte +0x1
00000EE2  C1E00E            shl eax,byte 0xe
00000EE5  99                cdq
00000EE6  F7F9              idiv ecx
00000EE8  01F0              add eax,esi
00000EEA  89442404          mov [esp+0x4],eax
00000EEE  E862F1FFFF        call dword 0x55
00000EF3  0FB70D0B7C0000    movzx ecx,word [dword 0x7c0b]
00000EFA  01C5              add ebp,eax
00000EFC  8B442414          mov eax,[esp+0x14]
00000F00  0FAFC1            imul eax,ecx
00000F03  89C2              mov edx,eax
00000F05  C1FA0E            sar edx,byte 0xe
00000F08  39D3              cmp ebx,edx
00000F0A  7CBC              jl 0xec8
00000F0C  896C2418          mov [esp+0x18],ebp
00000F10  25FF3F0000        and eax,0x3fff
00000F15  7429              jz 0xf40
00000F17  99                cdq
00000F18  F7F9              idiv ecx
00000F1A  C1E30E            shl ebx,byte 0xe
00000F1D  89442408          mov [esp+0x8],eax
00000F21  89D8              mov eax,ebx
00000F23  99                cdq
00000F24  F7F9              idiv ecx
00000F26  035C2440          add ebx,[esp+0x40]
00000F2A  891C24            mov [esp],ebx
00000F2D  01F0              add eax,esi
00000F2F  89442404          mov [esp+0x4],eax
00000F33  E81DF1FFFF        call dword 0x55
00000F38  01442418          add [esp+0x18],eax
00000F3C  90                nop
00000F3D  90                nop
00000F3E  90                nop
00000F3F  90                nop
00000F40  0FB60520B10000    movzx eax,byte [dword 0xb120]
00000F47  8B1528B10000      mov edx,[dword 0xb128]
00000F4D  3C0C              cmp al,0xc
00000F4F  0F841B010000      jz dword 0x1070
00000F55  3C10              cmp al,0x10
00000F57  742F              jz 0xf88
00000F59  8B049500000100    mov eax,[edx*4+0x10000]
00000F60  3DF5FFFF0F        cmp eax,0xffffff5
00000F65  7636              jna 0xf9d
00000F67  C70528B100000000  mov dword [dword 0xb128],0x0
         -0000
00000F71  837C241801        cmp dword [esp+0x18],byte +0x1
00000F76  19C0              sbb eax,eax
00000F78  83C42C            add esp,byte +0x2c
00000F7B  5B                pop ebx
00000F7C  83E003            and eax,byte +0x3
00000F7F  5E                pop esi
00000F80  5F                pop edi
00000F81  5D                pop ebp
00000F82  C3                ret
00000F83  90                nop
00000F84  90                nop
00000F85  90                nop
00000F86  90                nop
00000F87  90                nop
00000F88  0FB7841200000100  movzx eax,word [edx+edx+0x10000]
00000F90  BE00000000        mov esi,0x0
00000F95  3DF6FF0000        cmp eax,0xfff6
00000F9A  0F43C6            cmovnc eax,esi
00000F9D  85C0              test eax,eax
00000F9F  A328B10000        mov [0xb128],eax
00000FA4  74CB              jz 0xf71
00000FA6  8B7C2418          mov edi,[esp+0x18]
00000FAA  037C2440          add edi,[esp+0x40]
00000FAE  0FB70D0B7C0000    movzx ecx,word [dword 0x7c0b]
00000FB5  897C241C          mov [esp+0x1c],edi
00000FB9  8D78FE            lea edi,[eax-0x2]
00000FBC  0FB6050D7C0000    movzx eax,byte [dword 0x7c0d]
00000FC3  0FAFF8            imul edi,eax
00000FC6  89442414          mov [esp+0x14],eax
00000FCA  0FAFC1            imul eax,ecx
00000FCD  033D24B10000      add edi,[dword 0xb124]
00000FD3  8D90FF3F0000      lea edx,[eax+0x3fff]
00000FD9  81FAFE7F0000      cmp edx,0x7ffe
00000FDF  0F8683000000      jna dword 0x1068
00000FE5  8B6C241C          mov ebp,[esp+0x1c]
00000FE9  31F6              xor esi,esi
00000FEB  31DB              xor ebx,ebx
00000FED  90                nop
00000FEE  90                nop
00000FEF  90                nop
00000FF0  B800400000        mov eax,0x4000
00000FF5  99                cdq
00000FF6  F7F9              idiv ecx
00000FF8  892C24            mov [esp],ebp
00000FFB  81C500400000      add ebp,0x4000
00001001  89442408          mov [esp+0x8],eax
00001005  89D8              mov eax,ebx
00001007  83C301            add ebx,byte +0x1
0000100A  C1E00E            shl eax,byte 0xe
0000100D  99                cdq
0000100E  F7F9              idiv ecx
00001010  01F8              add eax,edi
00001012  89442404          mov [esp+0x4],eax
00001016  E83AF0FFFF        call dword 0x55
0000101B  0FB70D0B7C0000    movzx ecx,word [dword 0x7c0b]
00001022  01C6              add esi,eax
00001024  8B442414          mov eax,[esp+0x14]
00001028  0FAFC1            imul eax,ecx
0000102B  89C2              mov edx,eax
0000102D  C1FA0E            sar edx,byte 0xe
00001030  39D3              cmp ebx,edx
00001032  7CBC              jl 0xff0
00001034  25FF3F0000        and eax,0x3fff
00001039  7423              jz 0x105e
0000103B  99                cdq
0000103C  F7F9              idiv ecx
0000103E  C1E30E            shl ebx,byte 0xe
00001041  89442408          mov [esp+0x8],eax
00001045  89D8              mov eax,ebx
00001047  99                cdq
00001048  F7F9              idiv ecx
0000104A  035C241C          add ebx,[esp+0x1c]
0000104E  891C24            mov [esp],ebx
00001051  01F8              add eax,edi
00001053  89442404          mov [esp+0x4],eax
00001057  E8F9EFFFFF        call dword 0x55
0000105C  01C6              add esi,eax
0000105E  01742418          add [esp+0x18],esi
00001062  E9D9FEFFFF        jmp dword 0xf40
00001067  90                nop
00001068  31F6              xor esi,esi
0000106A  31DB              xor ebx,ebx
0000106C  EBC6              jmp short 0x1034
0000106E  90                nop
0000106F  90                nop
00001070  89D0              mov eax,edx
00001072  D1E8              shr eax,1
00001074  01D0              add eax,edx
00001076  0FB78C0000000100  movzx ecx,word [eax+eax+0x10000]
0000107E  89C8              mov eax,ecx
00001080  25FF0F0000        and eax,0xfff
00001085  83E201            and edx,byte +0x1
00001088  7407              jz 0x1091
0000108A  66C1E904          shr cx,byte 0x4
0000108E  0FB7C1            movzx eax,cx
00001091  3DF50F0000        cmp eax,0xff5
00001096  0F87CBFEFFFF      ja dword 0xf67
0000109C  E9FCFEFFFF        jmp dword 0xf9d
000010A1  C744241800000000  mov dword [esp+0x18],0x0
000010A9  31DB              xor ebx,ebx
000010AB  E960FEFFFF        jmp dword 0xf10
000010B0  833D2CB1000001    cmp dword [dword 0xb12c],byte +0x1
000010B7  19C0              sbb eax,eax
000010B9  83E002            and eax,byte +0x2
000010BC  83C003            add eax,byte +0x3
000010BF  C3                ret
000010C0  8B152CB10000      mov edx,[dword 0xb12c]
000010C6  B805000000        mov eax,0x5
000010CB  85D2              test edx,edx
000010CD  7509              jnz 0x10d8
000010CF  F3C3              rep ret
000010D1  90                nop
000010D2  90                nop
000010D3  90                nop
000010D4  90                nop
000010D5  90                nop
000010D6  90                nop
000010D7  90                nop
000010D8  C7052CB100000000  mov dword [dword 0xb12c],0x0
         -0000
000010E2  30C0              xor al,al
000010E4  C3                ret
000010E5  662E0F1F84000000  nop word [cs:eax+eax+0x0]
         -0000
000010EF  662E0F1F84000000  nop word [cs:eax+eax+0x0]
         -0000
000010F9  0F1F8000000000    nop dword [eax+0x0]
00001100  0000              add [eax],al
00001102  0000              add [eax],al
00001104  0000              add [eax],al
00001106  0000              add [eax],al
00001108  0000              add [eax],al
0000110A  0000              add [eax],al
0000110C  0000              add [eax],al
0000110E  0000              add [eax],al
00001110  0000              add [eax],al
00001112  0000              add [eax],al
00001114  0000              add [eax],al
00001116  0000              add [eax],al
00001118  0000              add [eax],al
0000111A  0000              add [eax],al
0000111C  0000              add [eax],al
0000111E  0000              add [eax],al
00001120  0000              add [eax],al
00001122  0000              add [eax],al
00001124  0000              add [eax],al
00001126  0000              add [eax],al
00001128  0000              add [eax],al
0000112A  0000              add [eax],al
0000112C  0000              add [eax],al
0000112E  0000              add [eax],al
00001130  0000              add [eax],al
00001132  0000              add [eax],al
00001134  0000              add [eax],al
00001136  0000              add [eax],al
00001138  0000              add [eax],al
0000113A  0000              add [eax],al
0000113C  0000              add [eax],al
0000113E  0000              add [eax],al
00001140  0000              add [eax],al
00001142  0000              add [eax],al
00001144  0000              add [eax],al
00001146  0000              add [eax],al
00001148  0000              add [eax],al
0000114A  0000              add [eax],al
0000114C  0000              add [eax],al
0000114E  0000              add [eax],al
00001150  0000              add [eax],al
00001152  0000              add [eax],al
00001154  0000              add [eax],al
00001156  0000              add [eax],al
00001158  0000              add [eax],al
0000115A  0000              add [eax],al
0000115C  0000              add [eax],al
0000115E  0000              add [eax],al
00001160  0000              add [eax],al
00001162  0000              add [eax],al
00001164  0000              add [eax],al
00001166  0000              add [eax],al
00001168  0000              add [eax],al
0000116A  0000              add [eax],al
0000116C  0000              add [eax],al
0000116E  0000              add [eax],al
00001170  0000              add [eax],al
00001172  0000              add [eax],al
00001174  0000              add [eax],al
00001176  0000              add [eax],al
00001178  0000              add [eax],al
0000117A  0000              add [eax],al
0000117C  0000              add [eax],al
0000117E  0000              add [eax],al
00001180  0000              add [eax],al
00001182  0000              add [eax],al
00001184  0000              add [eax],al
00001186  0000              add [eax],al
00001188  0000              add [eax],al
0000118A  0000              add [eax],al
0000118C  0000              add [eax],al
0000118E  0000              add [eax],al
00001190  0000              add [eax],al
00001192  0000              add [eax],al
00001194  0000              add [eax],al
00001196  0000              add [eax],al
00001198  0000              add [eax],al
0000119A  0000              add [eax],al
0000119C  0000              add [eax],al
0000119E  0000              add [eax],al
000011A0  0000              add [eax],al
000011A2  0000              add [eax],al
000011A4  0000              add [eax],al
000011A6  0000              add [eax],al
000011A8  0000              add [eax],al
000011AA  0000              add [eax],al
000011AC  0000              add [eax],al
000011AE  0000              add [eax],al
000011B0  0000              add [eax],al
000011B2  0000              add [eax],al
000011B4  0000              add [eax],al
000011B6  0000              add [eax],al
000011B8  0000              add [eax],al
000011BA  0000              add [eax],al
000011BC  0000              add [eax],al
000011BE  0000              add [eax],al
000011C0  0000              add [eax],al
000011C2  0000              add [eax],al
000011C4  0000              add [eax],al
000011C6  0000              add [eax],al
000011C8  0000              add [eax],al
000011CA  0000              add [eax],al
000011CC  0000              add [eax],al
000011CE  0000              add [eax],al
000011D0  0000              add [eax],al
000011D2  0000              add [eax],al
000011D4  0000              add [eax],al
000011D6  0000              add [eax],al
000011D8  0000              add [eax],al
000011DA  0000              add [eax],al
000011DC  0000              add [eax],al
000011DE  0000              add [eax],al
000011E0  0000              add [eax],al
000011E2  0000              add [eax],al
000011E4  0000              add [eax],al
000011E6  0000              add [eax],al
000011E8  0000              add [eax],al
000011EA  0000              add [eax],al
000011EC  0000              add [eax],al
000011EE  0000              add [eax],al
000011F0  0000              add [eax],al
000011F2  0000              add [eax],al
000011F4  0000              add [eax],al
000011F6  0000              add [eax],al
000011F8  0000              add [eax],al
000011FA  0000              add [eax],al
000011FC  0000              add [eax],al
000011FE  0000              add [eax],al
00001200  0000              add [eax],al
00001202  0000              add [eax],al
00001204  0000              add [eax],al
00001206  0000              add [eax],al
00001208  0000              add [eax],al
0000120A  0000              add [eax],al
0000120C  0000              add [eax],al
0000120E  0000              add [eax],al
00001210  0000              add [eax],al
00001212  0000              add [eax],al
00001214  0000              add [eax],al
00001216  0000              add [eax],al
00001218  0000              add [eax],al
0000121A  0000              add [eax],al
0000121C  0000              add [eax],al
0000121E  0000              add [eax],al
00001220  0000              add [eax],al
00001222  0000              add [eax],al
00001224  0000              add [eax],al
00001226  0000              add [eax],al
00001228  0000              add [eax],al
0000122A  0000              add [eax],al
0000122C  0000              add [eax],al
0000122E  0000              add [eax],al
00001230  0000              add [eax],al
00001232  0000              add [eax],al
00001234  0000              add [eax],al
00001236  0000              add [eax],al
00001238  0000              add [eax],al
0000123A  0000              add [eax],al
0000123C  0000              add [eax],al
0000123E  0000              add [eax],al
00001240  0000              add [eax],al
00001242  0000              add [eax],al
00001244  0000              add [eax],al
00001246  0000              add [eax],al
00001248  0000              add [eax],al
0000124A  0000              add [eax],al
0000124C  0000              add [eax],al
0000124E  0000              add [eax],al
00001250  0000              add [eax],al
00001252  0000              add [eax],al
00001254  0000              add [eax],al
00001256  0000              add [eax],al
00001258  0000              add [eax],al
0000125A  0000              add [eax],al
0000125C  0000              add [eax],al
0000125E  0000              add [eax],al
00001260  0000              add [eax],al
00001262  0000              add [eax],al
00001264  0000              add [eax],al
00001266  0000              add [eax],al
00001268  0000              add [eax],al
0000126A  0000              add [eax],al
0000126C  0000              add [eax],al
0000126E  0000              add [eax],al
00001270  0000              add [eax],al
00001272  0000              add [eax],al
00001274  0000              add [eax],al
00001276  0000              add [eax],al
00001278  0000              add [eax],al
0000127A  0000              add [eax],al
0000127C  0000              add [eax],al
0000127E  0000              add [eax],al
00001280  0000              add [eax],al
00001282  0000              add [eax],al
00001284  0000              add [eax],al
00001286  0000              add [eax],al
00001288  0000              add [eax],al
0000128A  0000              add [eax],al
0000128C  0000              add [eax],al
0000128E  0000              add [eax],al
00001290  0000              add [eax],al
00001292  0000              add [eax],al
00001294  0000              add [eax],al
00001296  0000              add [eax],al
00001298  0000              add [eax],al
0000129A  0000              add [eax],al
0000129C  0000              add [eax],al
0000129E  0000              add [eax],al
000012A0  0000              add [eax],al
000012A2  0000              add [eax],al
000012A4  0000              add [eax],al
000012A6  0000              add [eax],al
000012A8  0000              add [eax],al
000012AA  0000              add [eax],al
000012AC  0000              add [eax],al
000012AE  0000              add [eax],al
000012B0  0000              add [eax],al
000012B2  0000              add [eax],al
000012B4  0000              add [eax],al
000012B6  0000              add [eax],al
000012B8  0000              add [eax],al
000012BA  0000              add [eax],al
000012BC  0000              add [eax],al
000012BE  0000              add [eax],al
000012C0  0000              add [eax],al
000012C2  0000              add [eax],al
000012C4  0000              add [eax],al
000012C6  0000              add [eax],al
000012C8  0000              add [eax],al
000012CA  0000              add [eax],al
000012CC  0000              add [eax],al
000012CE  0000              add [eax],al
000012D0  0000              add [eax],al
000012D2  0000              add [eax],al
000012D4  0000              add [eax],al
000012D6  0000              add [eax],al
000012D8  0000              add [eax],al
000012DA  0000              add [eax],al
000012DC  0000              add [eax],al
000012DE  0000              add [eax],al
000012E0  0000              add [eax],al
000012E2  0000              add [eax],al
000012E4  0000              add [eax],al
000012E6  0000              add [eax],al
000012E8  0000              add [eax],al
000012EA  0000              add [eax],al
000012EC  0000              add [eax],al
000012EE  0000              add [eax],al
000012F0  0000              add [eax],al
000012F2  0000              add [eax],al
000012F4  0000              add [eax],al
000012F6  0000              add [eax],al
000012F8  0000              add [eax],al
000012FA  0000              add [eax],al
000012FC  0000              add [eax],al
000012FE  0000              add [eax],al
00001300  0000              add [eax],al
00001302  0000              add [eax],al
00001304  0000              add [eax],al
00001306  0000              add [eax],al
00001308  0000              add [eax],al
0000130A  0000              add [eax],al
0000130C  0000              add [eax],al
0000130E  0000              add [eax],al
00001310  0000              add [eax],al
00001312  0000              add [eax],al
00001314  0000              add [eax],al
00001316  0000              add [eax],al
00001318  0000              add [eax],al
0000131A  0000              add [eax],al
0000131C  0000              add [eax],al
0000131E  0000              add [eax],al
00001320  0000              add [eax],al
00001322  0000              add [eax],al
00001324  0000              add [eax],al
00001326  0000              add [eax],al
00001328  0000              add [eax],al
0000132A  0000              add [eax],al
0000132C  0000              add [eax],al
0000132E  0000              add [eax],al
00001330  0000              add [eax],al
00001332  0000              add [eax],al
00001334  0000              add [eax],al
00001336  0000              add [eax],al
00001338  0000              add [eax],al
0000133A  0000              add [eax],al
0000133C  0000              add [eax],al
0000133E  0000              add [eax],al
00001340  0000              add [eax],al
00001342  0000              add [eax],al
00001344  0000              add [eax],al
00001346  0000              add [eax],al
00001348  0000              add [eax],al
0000134A  0000              add [eax],al
0000134C  0000              add [eax],al
0000134E  0000              add [eax],al
00001350  0000              add [eax],al
00001352  0000              add [eax],al
00001354  0000              add [eax],al
00001356  0000              add [eax],al
00001358  0000              add [eax],al
0000135A  0000              add [eax],al
0000135C  0000              add [eax],al
0000135E  0000              add [eax],al
00001360  0000              add [eax],al
00001362  0000              add [eax],al
00001364  0000              add [eax],al
00001366  0000              add [eax],al
00001368  0000              add [eax],al
0000136A  0000              add [eax],al
0000136C  0000              add [eax],al
0000136E  0000              add [eax],al
00001370  0000              add [eax],al
00001372  0000              add [eax],al
00001374  0000              add [eax],al
00001376  0000              add [eax],al
00001378  0000              add [eax],al
0000137A  0000              add [eax],al
0000137C  0000              add [eax],al
0000137E  0000              add [eax],al
00001380  0000              add [eax],al
00001382  0000              add [eax],al
00001384  0000              add [eax],al
00001386  0000              add [eax],al
00001388  0000              add [eax],al
0000138A  0000              add [eax],al
0000138C  0000              add [eax],al
0000138E  0000              add [eax],al
00001390  0000              add [eax],al
00001392  0000              add [eax],al
00001394  0000              add [eax],al
00001396  0000              add [eax],al
00001398  0000              add [eax],al
0000139A  0000              add [eax],al
0000139C  0000              add [eax],al
0000139E  0000              add [eax],al
000013A0  0000              add [eax],al
000013A2  0000              add [eax],al
000013A4  0000              add [eax],al
000013A6  0000              add [eax],al
000013A8  0000              add [eax],al
000013AA  0000              add [eax],al
000013AC  0000              add [eax],al
000013AE  0000              add [eax],al
000013B0  0000              add [eax],al
000013B2  0000              add [eax],al
000013B4  0000              add [eax],al
000013B6  0000              add [eax],al
000013B8  0000              add [eax],al
000013BA  0000              add [eax],al
000013BC  0000              add [eax],al
000013BE  0000              add [eax],al
000013C0  0000              add [eax],al
000013C2  0000              add [eax],al
000013C4  0000              add [eax],al
000013C6  0000              add [eax],al
000013C8  0000              add [eax],al
000013CA  0000              add [eax],al
000013CC  0000              add [eax],al
000013CE  0000              add [eax],al
000013D0  0000              add [eax],al
000013D2  0000              add [eax],al
000013D4  0000              add [eax],al
000013D6  0000              add [eax],al
000013D8  0000              add [eax],al
000013DA  0000              add [eax],al
000013DC  0000              add [eax],al
000013DE  0000              add [eax],al
000013E0  0000              add [eax],al
000013E2  0000              add [eax],al
000013E4  0000              add [eax],al
000013E6  0000              add [eax],al
000013E8  0000              add [eax],al
000013EA  0000              add [eax],al
000013EC  0000              add [eax],al
000013EE  0000              add [eax],al
000013F0  0000              add [eax],al
000013F2  0000              add [eax],al
000013F4  0000              add [eax],al
000013F6  0000              add [eax],al
000013F8  0000              add [eax],al
000013FA  0000              add [eax],al
000013FC  0000              add [eax],al
000013FE  0000              add [eax],al
00001400  0000              add [eax],al
00001402  0000              add [eax],al
00001404  0000              add [eax],al
00001406  0000              add [eax],al
00001408  0000              add [eax],al
0000140A  0000              add [eax],al
0000140C  0000              add [eax],al
0000140E  0000              add [eax],al
00001410  0000              add [eax],al
00001412  0000              add [eax],al
00001414  0000              add [eax],al
00001416  0000              add [eax],al
00001418  0000              add [eax],al
0000141A  0000              add [eax],al
0000141C  0000              add [eax],al
0000141E  0000              add [eax],al
00001420  0000              add [eax],al
00001422  0000              add [eax],al
00001424  0000              add [eax],al
00001426  0000              add [eax],al
00001428  0000              add [eax],al
0000142A  0000              add [eax],al
0000142C  0000              add [eax],al
0000142E  0000              add [eax],al
00001430  0000              add [eax],al
00001432  0000              add [eax],al
00001434  0000              add [eax],al
00001436  0000              add [eax],al
00001438  0000              add [eax],al
0000143A  0000              add [eax],al
0000143C  0000              add [eax],al
0000143E  0000              add [eax],al
00001440  0000              add [eax],al
00001442  0000              add [eax],al
00001444  0000              add [eax],al
00001446  0000              add [eax],al
00001448  0000              add [eax],al
0000144A  0000              add [eax],al
0000144C  0000              add [eax],al
0000144E  0000              add [eax],al
00001450  0000              add [eax],al
00001452  0000              add [eax],al
00001454  0000              add [eax],al
00001456  0000              add [eax],al
00001458  0000              add [eax],al
0000145A  0000              add [eax],al
0000145C  0000              add [eax],al
0000145E  0000              add [eax],al
00001460  0000              add [eax],al
00001462  0000              add [eax],al
00001464  0000              add [eax],al
00001466  0000              add [eax],al
00001468  0000              add [eax],al
0000146A  0000              add [eax],al
0000146C  0000              add [eax],al
0000146E  0000              add [eax],al
00001470  0000              add [eax],al
00001472  0000              add [eax],al
00001474  0000              add [eax],al
00001476  0000              add [eax],al
00001478  0000              add [eax],al
0000147A  0000              add [eax],al
0000147C  0000              add [eax],al
0000147E  0000              add [eax],al
00001480  0000              add [eax],al
00001482  0000              add [eax],al
00001484  0000              add [eax],al
00001486  0000              add [eax],al
00001488  0000              add [eax],al
0000148A  0000              add [eax],al
0000148C  0000              add [eax],al
0000148E  0000              add [eax],al
00001490  0000              add [eax],al
00001492  0000              add [eax],al
00001494  0000              add [eax],al
00001496  0000              add [eax],al
00001498  0000              add [eax],al
0000149A  0000              add [eax],al
0000149C  0000              add [eax],al
0000149E  0000              add [eax],al
000014A0  0000              add [eax],al
000014A2  0000              add [eax],al
000014A4  0000              add [eax],al
000014A6  0000              add [eax],al
000014A8  0000              add [eax],al
000014AA  0000              add [eax],al
000014AC  0000              add [eax],al
000014AE  0000              add [eax],al
000014B0  0000              add [eax],al
000014B2  0000              add [eax],al
000014B4  0000              add [eax],al
000014B6  0000              add [eax],al
000014B8  0000              add [eax],al
000014BA  0000              add [eax],al
000014BC  0000              add [eax],al
000014BE  0000              add [eax],al
000014C0  0000              add [eax],al
000014C2  0000              add [eax],al
000014C4  0000              add [eax],al
000014C6  0000              add [eax],al
000014C8  0000              add [eax],al
000014CA  0000              add [eax],al
000014CC  0000              add [eax],al
000014CE  0000              add [eax],al
000014D0  0000              add [eax],al
000014D2  0000              add [eax],al
000014D4  0000              add [eax],al
000014D6  0000              add [eax],al
000014D8  0000              add [eax],al
000014DA  0000              add [eax],al
000014DC  0000              add [eax],al
000014DE  0000              add [eax],al
000014E0  0000              add [eax],al
000014E2  0000              add [eax],al
000014E4  0000              add [eax],al
000014E6  0000              add [eax],al
000014E8  0000              add [eax],al
000014EA  0000              add [eax],al
000014EC  0000              add [eax],al
000014EE  0000              add [eax],al
000014F0  0000              add [eax],al
000014F2  0000              add [eax],al
000014F4  0000              add [eax],al
000014F6  0000              add [eax],al
000014F8  0000              add [eax],al
000014FA  0000              add [eax],al
000014FC  0000              add [eax],al
000014FE  0000              add [eax],al
00001500  0000              add [eax],al
00001502  0000              add [eax],al
00001504  0000              add [eax],al
00001506  0000              add [eax],al
00001508  0000              add [eax],al
0000150A  0000              add [eax],al
0000150C  0000              add [eax],al
0000150E  0000              add [eax],al
00001510  0000              add [eax],al
00001512  0000              add [eax],al
00001514  0000              add [eax],al
00001516  0000              add [eax],al
00001518  0000              add [eax],al
0000151A  0000              add [eax],al
0000151C  0000              add [eax],al
0000151E  0000              add [eax],al
00001520  0000              add [eax],al
00001522  0000              add [eax],al
00001524  0000              add [eax],al
00001526  0000              add [eax],al
00001528  0000              add [eax],al
0000152A  0000              add [eax],al
0000152C  0000              add [eax],al
0000152E  0000              add [eax],al
00001530  0000              add [eax],al
00001532  0000              add [eax],al
00001534  0000              add [eax],al
00001536  0000              add [eax],al
00001538  0000              add [eax],al
0000153A  0000              add [eax],al
0000153C  0000              add [eax],al
0000153E  0000              add [eax],al
00001540  0000              add [eax],al
00001542  0000              add [eax],al
00001544  0000              add [eax],al
00001546  0000              add [eax],al
00001548  0000              add [eax],al
0000154A  0000              add [eax],al
0000154C  0000              add [eax],al
0000154E  0000              add [eax],al
00001550  0000              add [eax],al
00001552  0000              add [eax],al
00001554  0000              add [eax],al
00001556  0000              add [eax],al
00001558  0000              add [eax],al
0000155A  0000              add [eax],al
0000155C  0000              add [eax],al
0000155E  0000              add [eax],al
00001560  0000              add [eax],al
00001562  0000              add [eax],al
00001564  0000              add [eax],al
00001566  0000              add [eax],al
00001568  0000              add [eax],al
0000156A  0000              add [eax],al
0000156C  0000              add [eax],al
0000156E  0000              add [eax],al
00001570  0000              add [eax],al
00001572  0000              add [eax],al
00001574  0000              add [eax],al
00001576  0000              add [eax],al
00001578  0000              add [eax],al
0000157A  0000              add [eax],al
0000157C  0000              add [eax],al
0000157E  0000              add [eax],al
00001580  0000              add [eax],al
00001582  0000              add [eax],al
00001584  0000              add [eax],al
00001586  0000              add [eax],al
00001588  0000              add [eax],al
0000158A  0000              add [eax],al
0000158C  0000              add [eax],al
0000158E  0000              add [eax],al
00001590  0000              add [eax],al
00001592  0000              add [eax],al
00001594  0000              add [eax],al
00001596  0000              add [eax],al
00001598  0000              add [eax],al
0000159A  0000              add [eax],al
0000159C  0000              add [eax],al
0000159E  0000              add [eax],al
000015A0  0000              add [eax],al
000015A2  0000              add [eax],al
000015A4  0000              add [eax],al
000015A6  0000              add [eax],al
000015A8  0000              add [eax],al
000015AA  0000              add [eax],al
000015AC  0000              add [eax],al
000015AE  0000              add [eax],al
000015B0  0000              add [eax],al
000015B2  0000              add [eax],al
000015B4  0000              add [eax],al
000015B6  0000              add [eax],al
000015B8  0000              add [eax],al
000015BA  0000              add [eax],al
000015BC  0000              add [eax],al
000015BE  0000              add [eax],al
000015C0  0000              add [eax],al
000015C2  0000              add [eax],al
000015C4  0000              add [eax],al
000015C6  0000              add [eax],al
000015C8  0000              add [eax],al
000015CA  0000              add [eax],al
000015CC  0000              add [eax],al
000015CE  0000              add [eax],al
000015D0  0000              add [eax],al
000015D2  0000              add [eax],al
000015D4  0000              add [eax],al
000015D6  0000              add [eax],al
000015D8  0000              add [eax],al
000015DA  0000              add [eax],al
000015DC  0000              add [eax],al
000015DE  0000              add [eax],al
000015E0  0000              add [eax],al
000015E2  0000              add [eax],al
000015E4  0000              add [eax],al
000015E6  0000              add [eax],al
000015E8  0000              add [eax],al
000015EA  0000              add [eax],al
000015EC  0000              add [eax],al
000015EE  0000              add [eax],al
000015F0  0000              add [eax],al
000015F2  0000              add [eax],al
000015F4  0000              add [eax],al
000015F6  0000              add [eax],al
000015F8  0000              add [eax],al
000015FA  0000              add [eax],al
000015FC  0000              add [eax],al
000015FE  0000              add [eax],al
