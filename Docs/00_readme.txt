###########################电表源码工程管理纲要###########################

###电表源码分为4个层次：
驱动层（Devices、Libraries 硬件相关）
中间件（Middleware 文件系统等 部分硬件相关）
调度系统（Scheduler 硬件无关）
用户相关应用（Task 事件、负荷记录等 硬件无关）


###SVN管理
不同层次需要单独创建SVN路径
层次内部不同版本需要创建分支


###代码维护
驱动层         不同分支驱动代码接口需要保持不变
中间件         不同分支代码需要定期同步
系统级应用     不同分支代码需要定期同步
用户相关应用   分支管理


###根文件系统维护
根文件系统基于外置spi flash
电表出厂参数需要根文件系统支持
参数以文件形式上传到系统中，然后发送更新命令，电表加载并解析参数
根文件系统内文件模板也需要SVN来管理






###例子：
新项目 伊朗 
后付费 
硬件基于 印度表 有改动


###步骤一：获取各个层的主干源码
驱动层（硬件相关）
https://svn.meter.wsd.com/stm32f091/Drivers/trunk/Devices
https://svn.meter.wsd.com/stm32f091/Drivers/trunk/Libraries

中间件（部分硬件相关）
https://svn.meter.wsd.com/stm32f091/Middleware/trunk/Middleware

调度系统（硬件无关）
https://svn.meter.wsd.com/System/trunk/Scheduler

用户相关应用（硬件无关）
https://svn.meter.wsd.com/User/trunk/Tasks


###步骤二：硬件变化创建代码分支
https://svn.meter.wsd.com/stm32f091/Drivers/branch_iran/Devices
https://svn.meter.wsd.com/stm32f091/Drivers/branch_iran/Libraries


###步骤三：应用变化创建代码分支
https://svn.meter.wsd.com/User/branch_iran/Tasks


###步骤四：创建工程，修改分支，实现功能
一个工程对应多个分支，代码更新后需要分别提交




