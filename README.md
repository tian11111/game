<img width="80" height="20" alt="image" src="https://github.com/user-attachments/assets/dbcc3a9a-fd17-41e2-a927-7cca898a6a13" />
<img width="82" height="20" alt="image" src="https://github.com/user-attachments/assets/e558502f-a55b-4ffa-a236-3688c9a3aa11" />
<img width="158" height="20" alt="image" src="https://github.com/user-attachments/assets/df99210e-ac9c-4b11-b9db-c7b415e97d4e" />

🎯 游戏简介

这是一个简易但完整的斗地主（Fight the Landlord） 纸牌游戏实现。游戏遵循经典斗地主规则：

使用一副54张扑克牌（含大小王）
三名玩家：一名地主，两名农民
地主先出牌，农民合作对抗地主
谁先出完手牌即为胜利者
本项目完全使用 C 语言 编写，不依赖任何图形库，运行于命令行终端，适合作为 C 语言学习、数据结构练习或小型游戏开发参考。

🧩 功能特性

✅ 标准牌型识别

支持以下合法出牌类型：

单张（SINGLE）
对子（PAIR）
顺子（STRAIGHT，5张及以上连续点数）
炸弹（BOMB，四张相同）
王炸（ROCKET，大小王）
过牌（PASS）
✅ 出牌合法性校验

自动判断：

牌型是否合法
是否能“压住”上一家的牌
王炸最大，炸弹可炸非王炸牌型
✅ 智能比较机制

根据斗地主规则进行出牌比较：

同类型比点数（如对5 vs 对6）
顺子要求长度一致且起始点更高
炸弹之间比较点数
王炸无敌
✅ 简单AI对手

电脑玩家采用基础策略：

尝试打出最小可压制牌（如单张、对子）
无法出牌则选择“过牌”
支持连续过牌后重置出牌权
✅ 游戏流程完整

洗牌 → 发牌 → 随机定地主 → 轮流出牌 → 判断胜负

🛠️ 编译与运行
环境要求
支持 C99 或以上标准的编译器（如 gcc, clang, MSVC）
Windows / Linux / macOS

📂 项目结构
landlords-c/
├── main.c              # 主游戏逻辑
├── README.md           # 项目说明
└── LICENSE             # MIT 许可证

核心模块说明
模块	说明
Card / Suit / Point	定义扑克牌结构与花色点数

Player / Deck	玩家与牌堆结构体

PlayType / Play	出牌类型与出牌行为封装

analyzePlay()	分析玩家选择的牌是否构成合法牌型

canPlayBeat()	判断当前出牌能否压制上一家

shuffleDeck()	Fisher-Yates 洗牌算法

dealCards()	给三位玩家发牌

🤖 AI策略说明（简化版）
当前 AI 实现为贪心最小出牌策略：
🉑优先尝试出单张能压制的最小牌
🉑不支持组合牌、拆牌、炸弹保留等高级策略
🉑可扩展为更智能的AI（欢迎贡献！）

📢 待改进 & 扩展建议
 
 🔳支持更多复合牌型（如三带一、飞机等）
 
 🔳添加音效与颜色输出（ANSI）
 
 🔳实现图形界面（可选 SDL/ncurses）
 
 🔳多轮游戏与积分系统
 
 🔳更强的AI策略（记忆、合作判断）
 
 🔳网络对战支持（socket 编程）
 

 📜 许可证

本项目采用 MIT License，欢迎学习、修改与分发。

🙌 致谢

感谢你查看这个项目！如果你觉得有用，欢迎：

⭐ 点个 Star
🐞 提交 Issue 报告 Bug
💡 提出功能建议
🤝 提交 Pull Request 参与开发
