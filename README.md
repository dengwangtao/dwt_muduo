# dwt_muduo

## 目录树

```
.
|-- CMakeLists.txt
|-- README.md
|-- bin             # test生成可执行文件
|-- build           # 构建目录
|-- lib             # my_muduo生成库
|-- muduo_use       # 使用muduo
|-- src             # 手撕muduo
`-- test            # 测试
```


## 开发中遇到的问题

1. 编译静态库没问题, 编译main_test对Socket测试时报错:
   ![image-20240605180800262](C:\docker_v\dev_01\project\dwt_muduo\README.assets\image-20240605180800262.png)
   原因: vscode 的CMake插件没反应过来, 对顶层CMakeLists.txt保存一下, 重新构建项目即可
   ![image-20240605181158978](C:\docker_v\dev_01\project\dwt_muduo\README.assets\image-20240605181158978.png)

2. ::accept 调用返回, errno=22

   > 原因: len没有赋值
   > 改正: socklen_t len = static_cast<socklen_t>(sizeof addr);
