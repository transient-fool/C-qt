#ifndef CARDS_H
#define CARDS_H
#include "card.h"

#include <QSet>

class Cards
{
public:
    enum SortType{Asc, Desc, NoSort};
    Cards();
    explicit Cards(const Card& card);

    // 添加扑克牌
    void add(const Card& card);
    void add(const Cards& cards);
    void add(const QVector<Cards>& cards);


    // 一次性插入多个数据 (操作符重载 << )
    Cards& operator <<(const Card& card);
    Cards& operator <<(const Cards& cards);

    // 删除扑克牌
    void remove(const Card& card);
    void remove(const Cards& cards);
    void remove(const QVector<Cards>& cards);

    // 扑克牌的数量
    int cardCount();
    // 是否为空
    bool isEmpty();
    void clear();

    // 最大点数
    Card::CardPoint maxPoint();
    // 最小点数
    Card::CardPoint minPoint();
    // 指定点数的牌的数量
    int pointCount(Card::CardPoint point);
    // 某张牌是否在集合中
    bool contains(const Card& card);
    bool contains(const Cards& cards);

    // 随机取出一张扑克牌
    Card takeRandomCard();

    // QVector<Card>
    // QSet -> QVector
    CardList toCardList(SortType type = Desc);

    // 测试函数, 打印所有的卡牌信息
    void printAllCardInfo();

private:
    QSet<Card> m_cards;
};

#endif // CARDS_H
