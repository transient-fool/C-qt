#include "strategy.h"
#include <QMap>
#include <functional>

Strategy::Strategy(Player *player, const Cards &cards):
    m_player(player),
    m_cards(cards)
{
}

Cards Strategy::makeStrategy()
{
    // 得到出牌玩家对象以及打出的牌
    Player* pendPlayer = m_player->getPendPlayer();
    Cards pendCards = m_player->getPendCards();

    // 判断上次出牌的玩家是不是我自己
    if(pendPlayer == m_player || pendPlayer == nullptr)
    {
        // 直接出牌
        // 如果是我自己, 出牌没有限制
        return firstPlay();
    }
    else
    {
        // 如果不是我自己需要找比出牌玩家点数大的牌
        PlayHand type(pendCards);
        Cards beatCards = getGreaterCards(type);
        // 找到了点数大的牌需要考虑是否出牌
        bool shouldBeat = whetherToBeat(beatCards);
        if(shouldBeat)
        {
            return beatCards;
        }
        else
        {
            return Cards();
        }
    }
    return Cards();
}

Cards Strategy::firstPlay()
{
    // 判断玩家手中是否只剩单一的牌型
    PlayHand hand(m_cards);
    if(hand.getHandType() != PlayHand::Hand_Unknown)
    {
        return m_cards;
    }
    // 不是单一牌型
    // 判断玩家手中是否有顺子
    QVector<Cards> optimalSeq = pickOptimalSeqSingles();
    if(!optimalSeq.isEmpty())
    {
        // 得到单牌的数量
        int baseNum = findCardsByCount(1).size();
        // 把得到的顺子的集合从玩家手中删除
        Cards save = m_cards;
        save.remove(optimalSeq);
        int lastNum = Strategy(m_player, save).findCardsByCount(1).size();
        if(baseNum > lastNum)
        {
            return optimalSeq[0];
        }
    }

    bool hasPlane, hasTriple, hasPair;
    hasPair = hasTriple = hasPlane = false;
    Cards backup = m_cards;

    // 有没有炸弹
    QVector<Cards> bombArray = findCardType(PlayHand(PlayHand::Hand_Bomb, Card::Card_Begin, 0), false);
    backup.remove(bombArray);

    // 有没有飞机
    QVector<Cards> planeArray = Strategy(m_player, backup).findCardType(PlayHand(PlayHand::Hand_Plane, Card::Card_Begin, 0), false);
    if(!planeArray.isEmpty())
    {
        hasPlane = true;
        backup.remove(planeArray);
    }

    // 有没有三张点数相同的牌
    QVector<Cards> seqTripleArray = Strategy(m_player, backup).findCardType(PlayHand(PlayHand::Hand_Triple, Card::Card_Begin, 0), false);
    if(!seqTripleArray.isEmpty())
    {
        hasTriple = true;
        backup.remove(seqTripleArray);
    }

    // 有没有连对
    QVector<Cards> seqPairArray = Strategy(m_player, backup).findCardType(PlayHand(PlayHand::Hand_Seq_Pair, Card::Card_Begin, 0), false);
    if(!seqPairArray.isEmpty())
    {
        hasPair = true;
        backup.remove(seqPairArray);
    }

    if(hasPair)
    {
        Cards maxPair;
        for(int i=0; i<seqPairArray.size(); ++i)
        {
            if(seqPairArray[i].cardCount() > maxPair.cardCount())
            {
                maxPair = seqPairArray[i];
            }
        }
        return maxPair;
    }

    if(hasPlane)
    {
        // 1. 飞机带两个对儿
        bool twoPairFond = false;
        QVector<Cards> pairArray;
        for(Card::CardPoint point = Card::Card_3; point <= Card::Card_10; point = Card::CardPoint(point + 1))
        {
            Cards pair = Strategy(m_player, backup).findSamePointCards(point, 2);
            if(!pair.isEmpty())
            {
                pairArray.push_back(pair);
                if(pairArray.size() == 2)
                {
                    twoPairFond = true;
                    break;
                }
            }
        }
        if(twoPairFond)
        {
            Cards tmp = planeArray[0];
            tmp.add(pairArray);
            return tmp;
        }
        // 2. 飞机带两个单牌
        else
        {
            bool twoSingleFond = false;
            QVector<Cards> singleArray;
            for(Card::CardPoint point = Card::Card_3; point <= Card::Card_10; point = Card::CardPoint(point + 1))
            {
                if(backup.pointCount(point) == 1)
                {
                    Cards single = Strategy(m_player, backup).findSamePointCards(point, 1);
                    if(!single.isEmpty())
                    {
                        singleArray.push_back(single);
                        if(singleArray.size() == 2)
                        {
                            twoSingleFond = true;
                            break;
                        }
                    }
                }
            }
            if(twoSingleFond)
            {
                Cards tmp = planeArray[0];
                tmp.add(singleArray);
                return tmp;
            }
            else
            {
                // 3. 飞机
                return planeArray[0];
            }
        }
    }

    if(hasTriple)
    {
        if(PlayHand(seqTripleArray[0]).getCardPoint() < Card::Card_A)
        {
            for(Card::CardPoint point = Card::Card_3; point <= Card::Card_A; point = Card::CardPoint(point+1))
            {
                int pointCount = backup.pointCount(point);
                if(pointCount == 1)
                {
                    Cards single = Strategy(m_player, backup).findSamePointCards(point, 1);
                    Cards tmp = seqTripleArray[0];
                    tmp.add(single);
                    return tmp;
                }
                else if(pointCount == 2)
                {
                    Cards pair = Strategy(m_player, backup).findSamePointCards(point, 2);
                    Cards tmp = seqTripleArray[0];
                    tmp.add(pair);
                    return tmp;
                }
            }
        }
        // 不带副牌
        return seqTripleArray[0];
    }
    // 单牌或者对儿牌
    Player* nextPlayer = m_player->getNextPlayer();
    if(nextPlayer->getCards().cardCount() == 1 && m_player->getRole() != nextPlayer->getRole())
    {
        for(Card::CardPoint point = Card::CardPoint(Card::Card_End-1);
            point >= Card::Card_3; point = Card::CardPoint(point-1))
        {
            int pointCount = backup.pointCount(point);
            if(pointCount == 1)
            {
                Cards single = Strategy(m_player, backup).findSamePointCards(point, 1);
                return single;
            }
            else if(pointCount == 2)
            {
                Cards pair = Strategy(m_player, backup).findSamePointCards(point, 2);
                return pair;
            }
        }
    }
    else
    {
        for(Card::CardPoint point =  Card::Card_3;
            point < Card::Card_End; point = Card::CardPoint(point+1))
        {
            int pointCount = backup.pointCount(point);
            if(pointCount == 1)
            {
                Cards single = Strategy(m_player, backup).findSamePointCards(point, 1);
                return single;
            }
            else if(pointCount == 2)
            {
                Cards pair = Strategy(m_player, backup).findSamePointCards(point, 2);
                return pair;
            }
        }
    }
    return Cards();
}

Cards Strategy::getGreaterCards(PlayHand type)
{
    //1. 出牌玩家和当前玩家不是一伙的
    Player* pendPlayer = m_player->getPendPlayer();
    if(pendPlayer != nullptr && pendPlayer->getRole() != m_player->getRole() && pendPlayer->getCards().cardCount() <= 3)
    {
        QVector<Cards> bombs = findCardsByCount(4);
        for(int i=0; i<bombs.size(); ++i)
        {
            if(PlayHand(bombs[i]).canBeat(type))
            {
                return bombs[i];
            }
        }
        // 搜索当前玩家手中有没有王炸
        Cards sj = findSamePointCards(Card::Card_SJ, 1);
        Cards bj = findSamePointCards(Card::Card_BJ, 1);
        if(!sj.isEmpty() && !bj.isEmpty())
        {
            Cards jokers;
            jokers << sj << bj;
            return jokers;
        }
    }
    //2. 当前玩家和下一个玩家不是一伙的
    Player* nextPlayer = m_player->getNextPlayer();
    // 将玩家手中的顺子剔除出去
    Cards remain = m_cards;
    remain.remove(Strategy(m_player, remain).pickOptimalSeqSingles());


    auto beatCard = std::bind([=](const Cards & cards){
        QVector<Cards> beatCardsArray = Strategy(m_player, cards).findCardType(type, true);
        if(!beatCardsArray.isEmpty())
        {
            if(m_player->getRole() != nextPlayer->getRole() && nextPlayer->getCards().cardCount() <= 2)
            {
                return beatCardsArray.back();
            }
            else
            {
                return beatCardsArray.front();
            }
        }
        return Cards();
    }, std::placeholders::_1);

    Cards cs;
    if(!(cs = beatCard(remain)).isEmpty())
    {
        return cs;
    }
    else
    {
        if(!(cs = beatCard(m_cards)).isEmpty()) return cs;
    }
    return Cards();
}

bool Strategy::whetherToBeat(Cards &cs)
{
    // 没有找到能够击败对方的牌
    if(cs.isEmpty())
    {
        return false;
    }
    // 得到出牌玩家的对象
    Player* pendPlayer = m_player->getPendPlayer();
    if(m_player->getRole() == pendPlayer->getRole())
    {
        // 手里的牌所剩无几并且是一个完整的牌型
        Cards left = m_cards;
        left.remove(cs);
        if(PlayHand(left).getHandType() != PlayHand::Hand_Unknown)
        {
            return true;
        }
        // 如果cs对象中的牌的最小点数是2,大小王 --> 不出牌
        Card::CardPoint basePoint = PlayHand(cs).getCardPoint();
        if(basePoint == Card::Card_2 || basePoint == Card::Card_SJ || basePoint == Card::Card_BJ)
        {
            return false;
        }
    }
    else
    {
        PlayHand myHand(cs);
        // 如果是三个2带一,或者带一对, 不出牌(保存实力)
        if((myHand.getHandType() == PlayHand::Hand_Triple_Single || myHand.getHandType() == PlayHand::Hand_Triple_Pair)
                && myHand.getCardPoint() == Card::Card_2)
        {
            return false;
        }

        // 如果cs是对2, 并且出牌玩家手中的牌数量大于等于10 && 自己的牌的数量大于等于5, 暂时放弃出牌
        if(myHand.getHandType() == PlayHand::Hand_Pair && myHand.getCardPoint() == Card::Card_2
                && pendPlayer->getCards().cardCount() >= 10 && m_player->getCards().cardCount() >= 5)
        {
            return false;
        }
    }
    return true;
}

Cards Strategy::findSamePointCards(Card::CardPoint point, int count)
{
    if(count < 1 || count > 4)
    {
        return Cards();
    }
    // 大小王
    if(point == Card::Card_SJ || point == Card::Card_BJ)
    {
        if(count > 1)
        {
            return Cards();
        }

        Card card;
        card.setPoint(point);
        card.setSuit(Card::Suit_Begin);
        if(m_cards.contains(card))
        {
            Cards cards;
            cards.add(card);
            return cards;
        }
        return Cards();
    }

    // 不是大小王
    int findCount = 0;
    Cards findCards;
    for(int suit = Card::Suit_Begin+1; suit < Card::Suit_End; suit++)
    {
        Card card;
        card.setPoint(point);
        card.setSuit((Card::CardSuit)suit);
        if(m_cards.contains(card))
        {
            findCount ++;
            findCards.add(card);
            if(findCount == count)
            {
                return findCards;
            }
        }
    }
    return Cards();
}

QVector<Cards> Strategy::findCardsByCount(int count)
{
    if(count < 1 || count > 4)
    {
        return QVector<Cards>();
    }

    QVector<Cards> cardsArray;
    for(Card::CardPoint point = Card::Card_3; point < Card::Card_End; point = (Card::CardPoint)(point+1))
    {
        if(m_cards.pointCount(point) == count)
        {
            Cards cs;
            cs << findSamePointCards(point, count);
            cardsArray << cs;
        }
    }
    return cardsArray;
}

Cards Strategy::getRangeCards(Card::CardPoint begin, Card::CardPoint end)
{
    Cards rangeCards;
    for(Card::CardPoint point = begin; point < end; point = (Card::CardPoint)(point+1))
    {
        int count = m_cards.pointCount(point);
        Cards cs = findSamePointCards(point, count);
        rangeCards << cs;
    }
    return rangeCards;
}

QVector<Cards> Strategy::findCardType(PlayHand hand, bool beat)
{
    PlayHand::HandType type = hand.getHandType();
    Card::CardPoint point = hand.getCardPoint();
    int extra = hand.getExtra();

    // 确定起始点数
    Card::CardPoint beginPoint = beat ? Card::CardPoint(point + 1) : Card::Card_3;

    switch(type)
    {
    case PlayHand::Hand_Single:
        return getCards(beginPoint, 1);
    case PlayHand::Hand_Pair:
        return getCards(beginPoint, 2);
    case PlayHand::Hand_Triple:
        return getCards(beginPoint, 3);
    case PlayHand::Hand_Triple_Single:
        return getTripleSingleOrPair(beginPoint, PlayHand::Hand_Single);
    case PlayHand::Hand_Triple_Pair:
        return getTripleSingleOrPair(beginPoint, PlayHand::Hand_Pair);
    case PlayHand::Hand_Plane:
        return getPlane(beginPoint);
    case PlayHand::Hand_Plane_Two_Single:
        return getPlane2SingleOr2Pair(beginPoint, PlayHand::Hand_Single);
    case PlayHand::Hand_Plane_Two_Pair:
        return getPlane2SingleOr2Pair(beginPoint, PlayHand::Hand_Pair);
    case PlayHand::Hand_Seq_Pair:
    {
        CardInfo info;
        info.begin = beginPoint;
        info.end = Card::Card_Q;
        info.number = 2;
        info.base = 3;
        info.extra = extra;
        info.beat = beat;
        info.getSeq = &Strategy::getBaseSeqPair;
        return getSepPairOrSeqSingle(info);
    }
    case PlayHand::Hand_Seq_Single:
    {
        CardInfo info;
        info.begin = beginPoint;
        info.end = Card::Card_10;
        info.number = 1;
        info.base = 5;
        info.extra = extra;
        info.beat = beat;
        info.getSeq = &Strategy::getBaseSeqSingle;
        return getSepPairOrSeqSingle(info);
    }
    case PlayHand::Hand_Bomb:
        return getBomb(beginPoint);
    default:
        return QVector<Cards>();
    }
}

void Strategy::pickSeqSingles(QVector<QVector<Cards>> &allSeqRecord, const QVector<Cards> &seqSingle, const Cards &cards)
{
    // 1. 得到所有顺子的组合
    QVector<Cards> allSeq = Strategy(m_player, cards).findCardType(PlayHand(PlayHand::Hand_Seq_Single, Card::Card_Begin, 0), false);
    if(allSeq.isEmpty())
    {
        // 结束递归,将满足条件的顺子传递给调用者
        allSeqRecord << seqSingle;
    }
    else    // 2. 对顺子进行筛选
    {
        Cards saveCards = cards;
        // 遍历得到的所有的顺子
        for(int i=0; i<allSeq.size(); ++i)
        {
            // 将顺子取出
            Cards aScheme = allSeq.at(i);
            // 将顺子从用户手中删除
            Cards temp = saveCards;
            temp.remove(aScheme);

            QVector<Cards> seqArray = seqSingle;
            seqArray << aScheme;

            // 检测还有没有其他的顺子
            // seqArray 存储一轮for循环中多轮递归得到的所有的可用的顺子
            // allSeqRecord 存储多轮for循环中多轮递归得到的所有的可用的顺子
            pickSeqSingles(allSeqRecord, seqArray, temp);
        }
    }
}

QVector<Cards> Strategy::pickOptimalSeqSingles()
{
    QVector<QVector<Cards>> seqRecord;
    QVector<Cards> seqSingles;
    Cards save = m_cards;
    save.remove(findCardsByCount(4));
    save.remove(findCardsByCount(3));
    pickSeqSingles(seqRecord, seqSingles, save);
    if(seqRecord.isEmpty())
    {
        return QVector<Cards>();
    }

    // 遍历容器
    QMap<int, int> seqMarks;
    for(int i=0; i<seqRecord.size(); ++i)
    {
        Cards backupCards = m_cards;
        QVector<Cards> seqArray = seqRecord[i];
        backupCards.remove(seqArray);

        // 判断剩下的单牌是数量, 数量越少,顺子的组合就越合理
        QVector<Cards> singleArray = Strategy(m_player, backupCards).findCardsByCount(1);

        CardList cardList;
        for(int j=0; j<singleArray.size(); ++j)
        {
            cardList << singleArray[j].toCardList();
        }
        // 找点数相对较大一点顺子
        int mark = 0;
        for(int j=0; j<cardList.size(); ++j)
        {
            mark += cardList[j].point() + 15;
        }
        seqMarks.insert(i, mark);
    }

    // 遍历map
    int value = 0;
    int comMark = 1000;
    auto it = seqMarks.constBegin();
    for(;it!=seqMarks.constEnd(); ++it)
    {
        if(it.value() < comMark)
        {
            comMark = it.value();
            value = it.key();
        }
    }

    return seqRecord[value];
}

QVector<Cards> Strategy::getCards(Card::CardPoint point, int number)
{
    QVector<Cards> findCardsArray;
    for(Card::CardPoint pt=point; pt < Card::Card_End; pt = (Card::CardPoint)(pt + 1))
    {
        // 目的是尽量不拆分别的牌型
        if(m_cards.pointCount(pt) == number)
        {
            Cards cs = findSamePointCards(pt, number);
            findCardsArray << cs;
        }
    }
    return findCardsArray;
}

QVector<Cards> Strategy::getTripleSingleOrPair(Card::CardPoint begin, PlayHand::HandType type)
{
    // 找到点数相同的三张牌
    QVector<Cards> findCardArray = getCards(begin, 3);
    if(!findCardArray.isEmpty())
    {
        // 将找到的牌从用户手中删除
        Cards remainCards = m_cards;
        remainCards.remove(findCardArray);
        // 搜索牌型
        // 搜索单牌或者成对的牌
        Strategy st(m_player, remainCards);
        QVector<Cards> cardsArray = st.findCardType(PlayHand(type, Card::Card_Begin, 0), false);
        if(!cardsArray.isEmpty())
        {
            // 将找到的牌和三张点数相同的牌进行组合
            for(int i=0; i<findCardArray.size(); ++i)
            {
                findCardArray[i].add(cardsArray.at(i));
            }
        }
        else
        {
            findCardArray.clear();
        }
    }
    // 将最终结果返回给函数调用者
    return findCardArray;
}

QVector<Cards> Strategy::getPlane(Card::CardPoint begin)
{
    QVector<Cards> findCardArray;
    for(Card::CardPoint point = begin; point <= Card::Card_K; point = (Card::CardPoint)(point+1))
    {
        // 根据点数和数量进行搜索
        Cards prevCards = findSamePointCards(point, 3);
        Cards nextCards = findSamePointCards((Card::CardPoint)(point+1), 3);
        if(!prevCards.isEmpty() && !nextCards.isEmpty())
        {
            Cards tmp;
            tmp << prevCards << nextCards;
            findCardArray << tmp;
        }
    }
    return findCardArray;
}

QVector<Cards> Strategy::getPlane2SingleOr2Pair(Card::CardPoint begin, PlayHand::HandType type)
{
    // 找飞机
    QVector<Cards> findCardArray = getPlane(begin);
    if(!findCardArray.isEmpty())
    {
        // 将找到的牌从用户手中删除
        Cards remainCards = m_cards;
        remainCards.remove(findCardArray);
        // 搜索牌型
        // 搜索单牌或者成对的牌
        Strategy st(m_player, remainCards);
        QVector<Cards> cardsArray = st.findCardType(PlayHand(type, Card::Card_Begin, 0), false);
        if(cardsArray.size() >= 2)
        {
            // 找到了, 将其添加到飞机组合中
            for(int i=0; i<findCardArray.size(); ++i)
            {
                Cards tmp;
                tmp << cardsArray[0] << cardsArray[1];
                findCardArray[i].add(tmp);
            }
        }
        else
        {
            findCardArray.clear();
        }
    }
    return findCardArray;
}


QVector<Cards> Strategy::getSepPairOrSeqSingle(CardInfo &info)
{
    QVector<Cards> findCardsArray;
    if(info.beat)
    {
        // 最少3个, 最大A
        for(Card::CardPoint point = info.begin; point <= info.end; point = (Card::CardPoint)(point+1))
        {
            bool found = true;
            Cards seqCards;
            for(int i=0; i<info.extra; ++i)
            {
                // 基于点数和数量进行牌的搜索
                Cards cards = findSamePointCards((Card::CardPoint)(point + i), info.number);
                if(cards.isEmpty() || (point + info.extra >= Card::Card_2))
                {
                    found = false;
                    seqCards.clear();
                    break;
                }
                else
                {
                    seqCards << cards;
                }
            }
            if(found)
            {
                findCardsArray << seqCards;
                return findCardsArray;
            }
        }
    }
    else
    {
        for(Card::CardPoint point = info.begin; point <= info.end; point = (Card::CardPoint)(point+1))
        {
            // 将找到的这个基础连对存储起来
            Cards baseSeq = (this->*info.getSeq)(point);
            if(baseSeq.isEmpty())
            {
                continue;
            }
            // 连对存储到容器中
            findCardsArray << baseSeq;

            int followed = info.base;
            Cards alreadyFollowedCards; // 存储后续找到的满足条件的连对

            while(true)
            {
                // 新的起始点数
                Card::CardPoint followedPoint = Card::CardPoint(point + followed);
                // 判断是否超出了上限
                if(followedPoint >= Card::Card_2)
                {
                    break;
                }
                Cards follwedCards = findSamePointCards(followedPoint, info.number);
                if(follwedCards.isEmpty())
                {
                    break;
                }
                else
                {
                    alreadyFollowedCards << follwedCards;
                    Cards newSeq = baseSeq;
                    newSeq << alreadyFollowedCards;
                    findCardsArray << newSeq;
                    followed++;
                }
            }
        }
    }
    return findCardsArray;
}

Cards Strategy::getBaseSeqPair(Card::CardPoint point)
{
    Cards cards0 = findSamePointCards(point, 2);
    Cards cards1 = findSamePointCards((Card::CardPoint)(point+1), 2);
    Cards cards2 = findSamePointCards((Card::CardPoint)(point+2), 2);
    Cards baseSeq;
    if(!cards0.isEmpty() && !cards1.isEmpty() && !cards2.isEmpty())
    {
        baseSeq << cards0 << cards1 << cards2;
    }
    return baseSeq;
}

Cards Strategy::getBaseSeqSingle(Card::CardPoint point)
{
    Cards cards0 = findSamePointCards(point, 1);
    Cards cards1 = findSamePointCards((Card::CardPoint)(point+1), 1);
    Cards cards2 = findSamePointCards((Card::CardPoint)(point+2), 1);
    Cards cards3 = findSamePointCards((Card::CardPoint)(point+3), 1);
    Cards cards4 = findSamePointCards((Card::CardPoint)(point+4), 1);
    Cards baseSeq;
    if(!cards0.isEmpty() && !cards1.isEmpty() && !cards2.isEmpty() && !cards3.isEmpty() && !cards4.isEmpty())
    {
        baseSeq << cards0 << cards1 << cards2 << cards3 << cards4;
    }
    return baseSeq;
}

QVector<Cards> Strategy::getBomb(Card::CardPoint begin)
{
    QVector<Cards> findcardsArray;
    for(Card::CardPoint point = begin; point < Card::Card_End; point = (Card::CardPoint)(point + 1))
    {
        Cards cs = findSamePointCards(point, 4);
        if(!cs.isEmpty())
        {
            findcardsArray << cs;
        }
    }
    return findcardsArray;
}
