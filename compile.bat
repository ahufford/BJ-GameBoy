del BJ.gb


C:\GBDEV\gbdk\bin\lcc -Wa-l -Wl-m -Wl-j -c -o tiles.o assets/neoBjTiles.c 
C:\GBDEV\gbdk\bin\lcc -Wa-l -Wl-m -Wl-j -c -o map.o assets/neoBjMap.c 
C:\GBDEV\gbdk\bin\lcc -Wa-l -Wl-m -Wl-j -c -o coin.o assets/coinTiles.c 
C:\GBDEV\gbdk\bin\lcc -Wa-l -Wl-m -Wl-j -c -o tb.o assets/trashBarrel.c 
C:\GBDEV\gbdk\bin\lcc -Wa-l -Wl-m -Wl-j -c -o main.o main.c 
C:\GBDEV\gbdk\bin\lcc -Wa-l -Wl-m -Wl-j -c -o beg.o beg.c 
C:\GBDEV\gbdk\bin\lcc -Wa-l -Wl-m -Wl-j -Wm-yc -o BJ.gb tiles.o map.o main.o beg.o coin.o tb.o 

del *.asm
del *.lst
del *.o
del *.sym
del *.map
del *.noi
del *.ihx