program   television2;
uses      crt;
const     page       :  byte = 0;
var       startmem   :  word;

procedure setmode (mode:word); assembler;
asm
 mov ax, mode
 int 10h
end;

procedure setmodex;
var       i          :  word;
begin
 asm
  mov ax, 13h
  int 10h
 end;
 portw[$3c4] := $604;
 portw[$3c4] := $f02;
 for i := 0 to 320*200 do mem[$a000:i] := 0;
 port[$3c2] := 163;     {10100011b. Konfigurerar Mode-X}
 portw[$3d4] := $14;
 portw[$3d4] := $e317;
 portw[$3d4] := $4009;  {320*400*256}
end;

procedure retrace;assembler;
asm
 mov dx,03dah
 @loop1:
 in al,dx
 test al,8
 jnz @loop1
 @loop2: in al,dx
 test al,8
 jz @loop2
end;


procedure pageflip; {Flippar mellan TVè "sidor" i VGA-minnet... }
var ofsadr : word;
begin
 ofsadr := page * 32000;
 port[$3d4] := $0d;
 port[$3d5] := lo(ofsadr);
 port[$3d4] := $0c;
 port[$3d5] := hi(ofsadr);
 page := 1 - page;
end;

procedure shiftbrus;
begin
 Asm
  mov     bx, startmem
  mov     ah,bh
  mov     al,0dh
  mov     dx,3d4h
  out     dx,ax
  mov     ah,bl
  inc     al
  out     dx,ax
  sub     startmem, 5442
 end;
end;

procedure putpixel (x,y:word;color:byte);
var       ofsadr : word; {Vilken offs, beroende pÜ "sida" i VGA-minnet man
                          ska skriva}
          temp   : word;
          xtmp   : word;
begin
 ofsadr := page * 32000;
 temp := y*80;            {Y * 320/4 (4 sidor)}
 xtmp := x shr 2;         {Samma som x div 4, men shr Ñr ju snabbare!}
 inc (temp, xtmp);        {temp variabeln Ñr den slutgiltiga offseten}
 x := x and 3;
 asm
  mov ax, xtmp
  mov ah, 1
  mov bx, x
  mov cl, bl
  shl ah, cl              {Se shr ovan!}
  mov al, 2
  mov xtmp, ax
 end;
 portw[$3c4] := xtmp;     {Vilken bitplans-address?}
 inc (temp,ofsadr+432);       {GÜ fram till rÑtt offs..}
 mem[$a000:temp] := color;{Klart!}
end;

procedure makebrus;
var       i      : word;
          j      : word;
          k      : byte;
begin
 for i := 0 to 320 do for j := 0 to 400 do begin
  k := random (2);
  if k <> 0 then k := 15;
  putpixel (i,j,k);
 end;
 pageflip;
 repeat
  retrace;
  shiftbrus;
 until keypressed;
end;

begin
 randomize;
 setmode ($13);
 setmodex;
 pageflip;
 makebrus;
 setmode ($03);
end.
