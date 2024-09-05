#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include "cards.h"

class Player : public QObject
{
    Q_OBJECT
public:
    enum Role{Lord, Farmer};    // 角色
    enum Sex{Man, Woman};       // 性别
    enum Direction{Left, Right};    // 头像的显示方位
    enum Type{Robot, User, UnKnow}; // 玩家的类型
    explicit Player(QObject *parent = nullptr);
    explicit Player(QString name, QObject *parent = nullptr);

    // 名字
    void setName(QString name);
    QString getName();

    // 角色
    void setRole(Role role);
    Role getRole();

    // 性别
    void setSex(Sex sex);
    Sex getSex();

    // 方位
    void setDirection(Direction direction);
    Direction getDirection();

    // 玩家类型
    void setType(Type type);
    Type getType();

    // 玩家的分数
    void setScore(int score);
    int getScore();

    // 游戏结果
    void setWin(bool flag);
    bool isWin();

    // 提供当前对象的上家/下家对象
    void setPrevPlayer(Player* player);
    void setNextPlayer(Player* player);
    Player* getPrevPlayer();
    Player* getNextPlayer();

    // 叫地主/抢地主
    void grabLordBet(int point);

    // 存储扑克牌(发牌的时候得到的)
    void storeDispatchCard(const Card& card);
    void storeDispatchCard(const Cards& cards);

    // 得到所有的牌
    Cards getCards();
    // 清空玩家手中所有的牌
    void clearCards();
    // 出牌
    void playHand(Cards& cards);

    // 获取待出牌玩家对象以及这个玩家打出的牌
    Player* getPendPlayer();
    Cards getPendCards();

    // 存储出牌玩家对象和打出的牌
    void storePendingInfo(Player* player, const Cards& cards);

    // 虚函数
    virtual void prepareCallLord();
    virtual void preparePlayHand();
    virtual void thinkCallLord();
    virtual void thinkPlayHand();



signals:
    // 通知已经叫地主下注
    void notifyGrabLordBet(Player* player, int bet);
    // 通知已经出牌
    void notifyPlayHand(Player* player, Cards& card);
    // 通知已经发牌了
    void notifyPickCards(Player* player, const Cards& cards);

protected:
    int m_score = 0;
    QString m_name;
    Role m_role;
    Sex m_sex;
    Direction m_direction;
    Type m_type;
    bool m_isWin = false;
    Player* m_prev = nullptr;
    Player* m_next = nullptr;
    Cards m_cards;  // 存储多张扑克牌(玩家手中的牌)
    Cards m_pendCards;
    Player* m_pendPlayer = nullptr;
};

#endif // PLAYER_H
