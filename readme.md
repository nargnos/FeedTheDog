﻿# 过度设计的喂狗程序  
最终功能是控制树莓派来喂狗，先过度设计一些有的没的    
黑喂狗！  
写来玩的不要使用  


写的东西大概是：  
AI  
看了本人工智能系统指南（入门感觉挺好的 isbn 978-7-111-38455-7）  
入门级，根据书中的描述用C++撸了个神经网络，书中只描述算法没例子，不知道自己写得对不对，执行勉强有结果  
相关计算用到GPU（amp），没做跟踪优化，只是写来玩的，pi也用不了  

LuaCpp  
cpp简单封装lua，之后打算直接传lua字节码到pi去执行  
与c交互的部分写了个控制栈的函数  
  
WaitGo  
未完成，雏形  
最终功能是让我可以在不同网络环境控制（或可通过邮件控制）家里的pi，pi再控制外部硬件，从而喂狗    
先用go实现，这样简单些  

命令控制
未完成  
连不上的时候可用邮件控制pi，不过会有一定的延时  
邮件部分看了一下协议文档长度...还是用现成的吧...
刚写命令部分，pi应该支持mef，用插件方式扩展功能  
一些功能是要控制pi的io输出的，只买了led没买小电机，等买了再完善  
  
  
