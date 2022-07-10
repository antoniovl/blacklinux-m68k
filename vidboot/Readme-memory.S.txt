[062498]:
  Size_memory() now works fine. :-)

[042798]:

Workaround to memory.S
----------------------

   Bad news: currently, memory.S won't compile with NeXT tools. This means
that in this way you won't have the size of the display memory.

   Depending on your hardware, the size_memory() function included in
memory.S can raise an "Exeption #2 (0x8)", a Bus Error. If so, and when
building with Linux, you have to edit memory.S and change the

#define MEM_END	0x0C000000

   to a correct value for your machine. But wich one will be my correct
value? It depends on the memory banks installed on your system. In example,
my NeXT Cube has 4 banks (of 4 simms = 16 slots for simms), but only has
memory installed on banks 1, 2 & 3. The first bank it's located at
0x04000000, the second bank it's located at 0x05000000, third bank it's
located at 0x06000000 and the fourth bank it's placed at 0x07000000. With
this on mind, my MEM_END value has to be set to 0x08000000, and that addr
it's where there are no more physically banks present. Note that on my
machine there aren't simms installed at 0x07000000, but the bank physically
exist on the board, so don't confuse this.

   The correct approach (currently ongoing) it's to replace the default
exeption #2 handler by our exeption handler, so when size_memory hits
unexistent memory addresses, it will launch our code and tell the function
that has reached the memory boundary.

Questions to:
 Randy Thelen, rthelen@ix.netcom.net
 Antonio Varela, antonio@naiki.com
