**#** **bypass-Kaspersky-360**

进程镂空bypass-卡巴斯基-360等安全软件

![image](https://github.com/user-attachments/assets/a6cc70c8-c965-45ab-9996-ce61c0792145)

![image](https://github.com/user-attachments/assets/e32062d5-3f7e-41b3-b675-e4144fdf1f91)

即是进行了一些敏感操作造成我们的马子被删除进程仍然是存在的

![image](https://github.com/user-attachments/assets/e9391282-924d-4aa4-8160-b4a9edcee47b)

卡巴只会报毒，也不影响我们正常执行命令

![image](https://github.com/user-attachments/assets/0ddc7b40-ac47-4891-8256-5674fa062f08)

添加用户也是没问题的

![image](https://github.com/user-attachments/assets/9a58d778-a9e0-4982-9667-796c2a058a17)

这里放的a.exe是编译好的能过360的exe，如果360报202就加一下资源。
.c是能过360的代码，如果过卡巴需要加一个加解密。

使用方法参考（当然注入其他进程也是可以的）：.\a.exe .\payload_x64.bin "C:\Windows\System32\winlogon.exe"



参考项目：https://github.com/adamhlt/Process-Hollowing
