#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
// 出牌类型
typedef enum {
    PASS = 0,     // 过牌
    SINGLE,       // 单张
    PAIR,         // 对子
    STRAIGHT,     // 顺子
    BOMB,         // 炸弹（四张相同）
    ROCKET        // 王炸（大小王）
} PlayType;

// 表示一次出牌行为
typedef struct {
    PlayType type;
    int point;            // 主点数（如对子是5，顺子是起始点）
    int length;           // 长度（顺子用）
    int cardIndices[17];  // 选中的手牌索引
    int cardCount;        // 实际出几张牌
} Play;

// 牌种类枚举
typedef enum {
    CLUBS,      // 梅花
    DIAMONDS,       // 方片
    HEARTS,         // 红桃
    SPADES,         // 黑桃
    JOKER_SMALL,    // 小王
    JOKER_BIG       // 大王
} Suit;

// 点数枚举
typedef enum {
    POINT_3 = 0, POINT_4, POINT_5, POINT_6, POINT_7,
    POINT_8, POINT_9, POINT_10, POINT_J, POINT_Q,
    POINT_K, POINT_A, POINT_2,
    POINT_SMALL_JOKER, POINT_BIG_JOKER
} Point;

typedef struct {
    Suit suit;      // 花色
    Point point;    // 点数
    char name[8];   // 显示名称
} Card;

typedef struct {
    char name[20];              // 玩家名字
    Card hand[17];              // 手牌数组
    int cardCount;              // 当前手中牌的数量
    bool isLandlord;            // 是否是地主
} Player;

// 牌堆结构体
typedef struct {
    Card deck[54];
    int top;
} Deck;

// 初始化牌堆
void initializeDeck(Deck* deck) {
    int index = 0;
    const char* suitSymbols[] = { "梅", "方", "红", "黑" };
    const char* pointNames[] = { "3","4","5","6","7","8","9","10","J","Q","K","A","2" };

    for (int suit = CLUBS; suit <= SPADES; suit++) {
        for (int pointVal = 0; pointVal < 13; pointVal++) {
            deck->deck[index].suit = (Suit)suit;
            deck->deck[index].point = (Point)pointVal;
            sprintf(deck->deck[index].name, "%s%s", pointNames[pointVal], suitSymbols[suit]);
            index++;
        }
    }
    deck->deck[index++] = (Card){
        .name = "小王",
        .suit = JOKER_SMALL,
        .point = POINT_SMALL_JOKER
    };
    deck->deck[index++] = (Card){
        .name = "大王",
        .suit = JOKER_BIG,
        .point = POINT_BIG_JOKER
    };
}

// 洗牌
void shuffleDeck(Deck* deck) {
    for (int i = 54 - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Card temp = deck->deck[i];
        deck->deck[i] = deck->deck[j];
        deck->deck[j] = temp;
    }
}

// 发牌
void dealCards(Deck* deck, Player players[]) {
    for (int cardIdx = 0; cardIdx < 51; cardIdx++) {
        int playerIndex = cardIdx % 3;
        int targetSlot = players[playerIndex].cardCount;
        //targetSlot表示这张新牌应该插入到手牌数组的哪个位置（下标）
        players[playerIndex].hand[targetSlot] = deck->deck[cardIdx];
        players[playerIndex].cardCount++;
    }
    printf("发牌完成，每人获得 %d 张牌。\n", 17);
}

// 排序
void sortHandByPoint(Player* player) {
    for (int i = 0; i < player->cardCount - 1; i++) {
        for (int j = 0; j < player->cardCount - i - 1; j++) {
            if (player->hand[j].point > player->hand[j + 1].point) {
                Card temp = player->hand[j];
                player->hand[j] = player->hand[j + 1];
                player->hand[j + 1] = temp;
            }
        }
    }
}

// 打印我的所有手牌
void printPlayerHand(const Player* player) {
    printf("%s [%s] 的手牌 (%d张):\n",
        player->name,
        player->isLandlord ? "地主" : "农民",
        player->cardCount);
    for (int i = 0; i < player->cardCount; i++) {
        printf("[%d]%s\t", i, player->hand[i].name);
    }
    printf("\n");

}
// 让玩家选择一张牌打出（返回牌的索引，-1 表示过牌）
int getPlayerChoice(const Player* player, int lastPoint) {
    while (1) {
        printf("轮到你出牌！输入牌的编号（0-%d），或输入 -1 表示过牌: ", player->cardCount - 1);
        int choice;
        scanf("%d", &choice);

        if (choice == -1) {
            return -1;  // 过牌
        }

        if (choice >= 0 && choice < player->cardCount) {
            // 检查是否大于上家的牌
            if (lastPoint != -1 && player->hand[choice].point <= lastPoint) {
                printf("你出的牌太小了！必须大于上家的牌。\n");
                continue;
            }
            return choice;
        }
        else {
            printf("输入无效！请输入 0 到 %d 之间的数字。\n", player->cardCount - 1);
        }
    }
}

Play analyzePlay(const Player* player, int selected[], int selectedCount) {
    Play play = { 0 };
    play.cardCount = selectedCount;

    if (selectedCount == 0) {
        play.type = PASS;
        return play;
    }

    // 提取点数并排序
    int points[17];
    for (int i = 0; i < selectedCount; i++) {
        points[i] = player->hand[selected[i]].point;
    }
    // 排序点数（从小到大）
    for (int i = 0; i < selectedCount - 1; i++) {
        for (int j = 0; j < selectedCount - i - 1; j++) {
            if (points[j] > points[j + 1]) {
                int temp = points[j];
                points[j] = points[j + 1];
                points[j + 1] = temp;
            }
        }
    }

    // 复制索引
    for (int i = 0; i < selectedCount; i++) {
        play.cardIndices[i] = selected[i];
    }

    // 先判断王炸
    if (selectedCount == 2) {
        if ((points[0] == POINT_SMALL_JOKER && points[1] == POINT_BIG_JOKER)) {
            play.type = ROCKET;
            play.point = POINT_BIG_JOKER;
            play.length = 2;
            return play;
        }
    }

    //再判断对子
    if (selectedCount == 2 && points[0] == points[1]) {
        play.type = PAIR;
        play.point = points[0];
        play.length = 2;
        return play;
    }

    // 单张
    if (selectedCount == 1) {
        play.type = SINGLE;
        play.point = points[0];
        play.length = 1;
        return play;
    }

    // 炸弹（四张相同）
    if (selectedCount == 4 && points[0] == points[3]) {
        play.type = BOMB;
        play.point = points[0];
        play.length = 4;
        return play;
    }

    // 顺子（5张及以上，连续，3~A）
    if (selectedCount >= 5) {
        bool isStraight = true;
        for (int i = 0; i < selectedCount - 1; i++) {
            if (points[i + 1] != points[i] + 1) {
                isStraight = false;
                break;
            }
        }
        if (isStraight && points[0] >= POINT_3 && points[selectedCount - 1] <= POINT_A) {
            play.type = STRAIGHT;
            play.point = points[0];
            play.length = selectedCount;
            return play;
        }
    }

    // 其他情况：非法牌型
    play.type = PASS;
    return play;
}
// 判断当前出的牌是否能打过上一家
bool canPlayBeat(const Play* current, const Play* last) {
    if (last->type == PASS) {
        return true;  // 上家没出牌（或过牌），任何合法牌都能出
    }

    // 1. 王炸最大，能打任何牌
    if (current->type == ROCKET) {
        return true;
    }
    if (last->type == ROCKET) {
        return false;  // 上家是王炸，你不是，打不过
    }

    // 2. 炸弹可以打除王炸外的任何牌
    if (current->type == BOMB && last->type != ROCKET) {
        // 但要比较点数（炸弹之间比点数）
        if (last->type == BOMB) {
            return current->point > last->point;
        }
        return true;  // 打非炸弹牌
    }
    if (last->type == BOMB && current->type != BOMB && current->type != ROCKET) {
        return false; // 上家是炸弹，你不是，打不过
    }

    // 3. 同类型比较
    if (current->type == last->type) {
        if (current->type == STRAIGHT) {
            // 顺子：长度必须相同，且起始点数更高
            if (current->length != last->length) {
                return false;
            }
            return current->point > last->point;
        }
        else {
            // 单张、对子、炸弹（已处理）等：比点数
            return current->point > last->point;
        }
    }

    // 其他情况：类型不同，不能打
    return false;
}

void initializeDeck(Deck* deck);
void shuffleDeck(Deck* deck);
void dealCards(Deck* deck, Player players[]);
void printPlayerHand(const Player* player);
void sortHandByPoint(Player* player);
const char* getCardName(const Card* card);
int getPlayerChoice(const Player* player, int lastPoint);
Play analyzePlay(const Player* player, int selected[], int selectedCount);
bool canPlayBeat(const Play* current, const Play* last);

int main() {
    srand((unsigned)time(NULL));

    // 初始化
    Deck gameDeck = { 0 };
    gameDeck.top = 0;
    initializeDeck(&gameDeck);
    shuffleDeck(&gameDeck);

    Player players[3] = { 0 };
    strcpy(players[0].name, "我");
    strcpy(players[1].name, "电脑1");
    strcpy(players[2].name, "电脑2");

    for (int i = 0; i < 3; i++) {
        players[i].cardCount = 0;
        players[i].isLandlord = false;
    }

    dealCards(&gameDeck, players);

    // 随机选地主
    int landlordIdx = rand() % 3;
    players[landlordIdx].isLandlord = true;
    printf("\n【地主】: %s\n", players[landlordIdx].name);

    // 所有玩家排序手牌
    for (int i = 0; i < 3; i++) {
        sortHandByPoint(&players[i]);
    }

    // 游戏状态
    int passCount = 0;
    int lastPlayer = -1;  // 上一个出牌的人
    Play lastPlay = { 0 };
    lastPlay.type = PASS;

    int round = 1;

    while (1) {
        bool hasWinner = false;
        for (int i = 0; i < 3; i++) {
            if (players[i].cardCount == 0) {
                printf("\n 游戏结束！%s 赢了！\n", players[i].name);
                if (players[i].isLandlord) {
                    printf("【地主胜利】\n");
                }
                else {
                    printf("【农民胜利】\n");
                }
                hasWinner = true;
                break;
            }
        }
        if (hasWinner) break;

        printf("\n==================== 回合 %d ====================\n", round);
        printPlayerHand(&players[0]);

        bool playedThisRound = false;

        for (int i = 0; i < 3; i++) {
            if (players[i].cardCount == 0) continue;

            // 判断是否轮到当前玩家
            if (lastPlayer != -1 && i != (lastPlayer + 1) % 3) {
                continue;
            }

            printf("\n轮到 %s 出牌\n", players[i].name);

            // 重置本轮出牌标志
            playedThisRound = false;

            Play currentPlay = { 0 };
            bool isPass = false;
            int selected[17];
            int selectedCount = 0;

            // === 人类玩家出牌 ===
            if (i == 0) {
                while (1) {
                    printf("输入要出的牌编号（空格分隔），-1 表示过牌: ");
                    int input;
                    selectedCount = 0;
                    while (scanf("%d", &input) == 1) {
                        if (input == -1) break;
                        if (input >= 0 && input < players[i].cardCount) {
                            selected[selectedCount++] = input;
                        }
                        else {
                            printf("无效编号 %d，忽略。\n", input);
                        }
                        int c = getchar();
                        if (c == '\n' || c == EOF) break;
                    }

                    if (selectedCount == 0) {
                        printf("%s 过牌。\n", players[i].name);
                        isPass = true;
                        break;
                    }

                    currentPlay = analyzePlay(&players[i], selected, selectedCount);
                    if (currentPlay.type == PASS) {
                        printf("❌ 牌型不合法！请重新选择。\n");
                        continue;
                    }

                    if (!canPlayBeat(&currentPlay, &lastPlay)) {
                        printf("❌ 你出的牌打不过上家！请重新选择。\n");
                        continue;
                    }

                    // 合法且能打过
                    break;
                }
            }
            // === 电脑玩家 AI 出牌（简化版：能出就出最小合法牌）===
            else {
                // 简单 AI：遍历所有牌，找第一个能打过上家的合法牌型
                bool found = false;

                // 先尝试单张（最小点数）
                if (lastPlay.type == PASS || lastPlay.type == SINGLE) {
                    for (int idx = 0; idx < players[i].cardCount; idx++) {
                        if (lastPlay.type == PASS || players[i].hand[idx].point > lastPlay.point) {
                            selected[0] = idx;
                            selectedCount = 1;
                            currentPlay = analyzePlay(&players[i], selected, 1);
                            if (currentPlay.type == SINGLE) {
                                found = true;
                                break;
                            }
                        }
                    }
                }


                if (!found) {
                    printf("%s 过牌。\n", players[i].name);
                    isPass = true;
                }
                else {
                    printf("%s 打出了: ", players[i].name);
                    for (int k = 0; k < selectedCount; k++) {
                        int idx = selected[k];
                        printf("%s ", players[i].hand[idx].name);
                    }
                    printf("\n");
                }
            }

            // === 处理出牌结果 ===
            if (!isPass) {
                // 更新 lastPlay
                lastPlay = currentPlay;
                lastPlayer = i;
                passCount = 0;  // 重置过牌计数

                // 从手牌中移除
                for (int k = selectedCount - 1; k >= 0; k--) {
                    int idx = selected[k];
                    for (int j = idx; j < players[i].cardCount - 1; j++) {
                        players[i].hand[j] = players[i].hand[j + 1];
                    }
                    players[i].cardCount--;
                }

                playedThisRound = true;
            }
            else {
                passCount++;
                // 三家都过？重置出牌权
                if (passCount >= 2) {
                    printf("【三家都过，新一轮开始】\n");
                    lastPlay.type = PASS;
                    lastPlayer = -1;
                    passCount = 0;
                }
                lastPlayer = i;  // 更新轮到谁过牌
            }

            if (playedThisRound) break;  // 有人出牌，本轮结束
        }

        round++;
        if (round > 50) {
            printf("游戏超时，强制结束。\n");
            break;
        }
    }

    return 0;
}