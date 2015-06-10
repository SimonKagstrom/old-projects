program   pilearn;
uses      crt;
var       pifile  : file;
          piarray : array[0..2001] of char;
          errors  : integer;
          correct : integer;

procedure readfil;
var       i       : integer;
          temp    : char;
          tst     : integer;
begin
 assign (pifile, 'c:\bat\pi.txt');
 reset (pifile);
 i := 0;
 BlockRead(pifile, piarray, 2002, tst);
 close (pifile);
end;

procedure learn;
var       i,x,y : integer;
          key : char;
begin
 key := ' ';
 i := 0;
 y := 0;
 errors := 0;
 writeln ('3.');
 repeat
  gotoxy (1,50);
  clreol;
  write ('Antal fel: ',errors,' Antal decimaler: ',correct);
  key := readkey;
  while ((key <> piarray[correct]) and (key <> chr(27)) and (key <> 'h') and (key <> 'r')) do begin
   sound (400);
   delay (10);
   nosound;
   inc(errors);
   gotoxy (1,50);
   clreol;
   write ('Antal fel: ',errors,' Antal decimaler: ',correct);
   key := readkey;
  end;
  if ((i = 75) and (y = 0)) then begin
   i := 1;
   inc (y);
  end;
  if ((i = 78) and (y > 0)) then begin
   i := 1;
   inc (y);
  end;
  if (y = 0) then gotoxy (i+3,1+y) else gotoxy (i,1+y);
  if (key = 'h') then write (piarray[correct]) else write (key);
  inc (i);
  inc (correct);
  if (key = 'r') then begin
   i := 0;
   y := 0;
   errors := 0;
   correct := 0;
   clrscr;
   gotoxy (1,1);
   write ('3.');
  end;
 until key = chr(27);
end;

begin
 textmode (64+256);
 clrscr;
 readfil;
 learn;
 clrscr;
end.
