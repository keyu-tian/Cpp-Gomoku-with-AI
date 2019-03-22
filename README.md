# Gobang_by_Kevin

![截图](./images/screenshot.png)

<br>


# 简介

通过 Qt 5.11.1 (C++ 11) 开发，AI最大搜索深度可达 18 层，棋力可观。

目前还谈不上严格的 AI。下一步会用 ResNet+RL+MCTS（部分参考Alpha Zero）来做一个真正意义上的棋类AI（用PyTorch或者自搭网络）。

<br>


# 体验

将 headers 文件夹下所有文件、sources 文件夹下所有文件以及 gobang_by_Kevin.pro 三部分放在同一目录下，使用Qt打开 gobang_by_Kevin.pro，编译运行后即可体验，通过鼠标操作：
- 落子：鼠标直接点击（待落子点有提示标记）
- 与AI对弈时切换先后手并重新开始游戏：new 按钮
- 重新开始游戏：restart 按钮
- 悔棋：retract 按钮
- 切换人机对弈模式与人人对弈模式：change 按钮

- 难度：可在 aithread_algo.cpp 最上方更改搜索深度
- BASE_DEPTH 基础深度 2-6、KILL_DEPTH 浅层算杀深度 8-14、RES_DEPTH 深层算杀深度 12-18
- （后续更新会增设配置设置界面）

（出现真香警告时，说明 AI 搜到了某一方的必胜情况~）

<br>


# 涉及算法/技术

- 状态压缩

- 极大极小搜索 + alpha-beta 剪枝 + 特殊剪枝（响应必杀棋）

- A*优化（启发式搜索）

- 迭代加深

- 棋类算杀模块

- 棋型映射表

- Zobrist置换表（testing）

- 多线程

<br>


# 关于重构

- 预计在 2019 夏季对代码进行大范围重构

- 目的在于进行更彻底的解耦，以及简化部分逻辑，以大幅提高可读性（将诸多奇技淫巧进行二次封装）

- 可能会改变状压方式（并利用__gnu_pbds::gp_hash_table进行更高效的棋型识别）



