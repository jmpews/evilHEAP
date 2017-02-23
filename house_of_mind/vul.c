#include <stdio.h>
#include <stdlib.h>

int fvuln (void)
{
  char *ptr  = malloc(1024);
  char *ptr2;
  int heap = (int)ptr & 0xFFF00000, i;
  // 利用局部变量伪造 `av->mutex`, 处于低地址
  int ret = 0;

  printf("ptr found at %p\n", ptr);

  // i == 2 because this is my second chunk to allocate
  for (i = 2; i < 1024; i++)
  {
    // 不断分配内存, 来让 heap_info 落到可控的内存.
    // 这里需要注意下 0x100000, 否则 heap_info, 是无法处于 ptr 之后.
    if (((int)(ptr2 = malloc(1024)) & 0xFFF00000) == (heap + 0x100000))
    {
      printf("good heap alignment found on malloc() %i (%p)\n", i, ptr2);
      break;
    }
  }

  // 接收 payload
  fread (ptr, 1024 * 1024, 1, stdin);

  // 没有具体作用
  free(ptr);
  // 实现任意地址写, 具体是修改返回地址为 ptr2
  free(ptr2);

  return ret;
}

int main(void)
{
  fvuln();

  return 0;
}
