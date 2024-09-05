#include "gamecontrol.h"
#include "playhand.h"
#include <QRandomGenerator>
#include <QTimer>
#include <QDebug>

GameControl::GameControl(QObject *parent) : QObject(parent)
{

}

void GameControl::playerInit()
{
    // 对象实例化
    m_robotLeft = new Robot("机器人A", this);
    m_robotRight = new Robot("机器人B", this);
    m_user = new UserPlayer("我自己", this);

    // 头像的显示
    m_robotLeft->setDirection(Player::Left);
    m_robotRight->setDirection(Player::Right);
    m_user->setDirection(Player::Right);

    // 性别
    Player::Sex sex;
    sex = (Player::Sex)QRandomGenerator::global()->bounded(2);
    m_robotLeft->setSex(sex);
    sex = (Player::Sex)QRandomGenerator::global()->bounded(2);
    m_robotRight->setSex(sex);
    sex = (Player::Sex)QRandomGenerator::global()->bounded(2);
    m_user->setSex(sex);

    // 出牌顺序
    // user
    m_user->setPrevPlayer(m_robotLeft);
    m_user->setNextPlayer(m_robotRight);

    // left robot
    m_robotLeft->setPrevPlayer(m_robotRight);
    m_robotLeft->setNextPlayer(m_user);

    // right robot
    m_robotRight->setPrevPlayer(m_user);
    m_robotRight->setNextPlayer(m_robotLeft);

    // 指定当前玩家
    m_currPlayer = m_user;

    // 处理玩家发射出的信号
    connect(m_user, &UserPlayer::notifyGrabLordBet, this, &GameControl::onGrabBet);
    connect(m_robotLeft, &UserPlayer::notifyGrabLordBet, this, &GameControl::onGrabBet);
    connect(m_robotRight, &UserPlayer::notifyGrabLordBet, this, &GameControl::onGrabBet);

    // 传递出牌玩家对象和玩家打出的牌
    connect(this, &GameControl::pendingInfo, m_robotLeft, &Robot::storePendingInfo);
    connect(this, &GameControl::pendingInfo, m_robotRight, &Robot::storePendingInfo);
    connect(this, &GameControl::pendingInfo, m_user, &Robot::storePendingInfo);

    // 处理玩家出牌
    connect(m_robotLeft, &Robot::notifyPlayHand, this, &GameControl::onPlayHand);
    connect(m_robotRight, &Robot::notifyPlayHand, this, &GameControl::onPlayHand);
    connect(m_user, &Robot::notifyPlayHand, this, &GameControl::onPlayHand);
}


Robot *GameControl::getLeftRobot()
{
    return m_robotLeft;
}

Robot *GameControl::getRightRobot()
{
    return m_robotRight;
}

UserPlayer *GameControl::getUserPlayer()
{
    return m_user;
}

void GameControl::setCurrentPlayer(Player *player)
{
    m_currPlayer = player;
}

Player *GameControl::getCurrentPlayer()
{
    return m_currPlayer;
}

Player *GameControl::getPendPlayer()
{
    return m_pendPlayer;
}

Cards GameControl::getPendCards()
{
    return m_pendCards;
}

void GameControl::initAllCards()
{
    m_allCards.clear();
    for(int p = Card::Card_Begin+1; p<Card::Card_SJ; ++p)
    {
        for(int s = Card::Suit_Begin+1; s<Card::Suit_End; ++s)
        {
            Card c((Card::CardPoint)p, (Card::CardSuit)s);
            m_allCards.add(c);
        }
    }
    m_allCards.add(Card(Card::Card_SJ, Card::Suit_Begin));
    m_allCards.add(Card(Card::Card_BJ, Card::Suit_Begin));
}

Card GameControl::takeOneCard()
{
#if 0
    // 测试飞机
    static bool flag = true;
    static Cards tmp;
    if(flag)
    {
        Card c1(Card::Card_10, Card::Club);
        Card c2(Card::Card_10, Card::Diamond);
        Card c3(Card::Card_10, Card::Heart);

        Card c4(Card::Card_J, Card::Club);
        Card c5(Card::Card_J, Card::Diamond);
        Card c6(Card::Card_J, Card::Heart);

        tmp << c1 << c2 << c3 << c4 << c5 << c6;
        m_allCards.remove(tmp);
        flag = false;
    }

    if(getCurrentPlayer() == m_user && !tmp.isEmpty())
    {
        return tmp.takeRandomCard();
    }
    else
    {
        return m_allCards.takeRandomCard();
    }
#else
    // not test code
    return m_allCards.takeRandomCard();
#endif
}

Cards GameControl::getSurplusCards()
{
    return m_allCards;
}

void GameControl::resetCardData()
{
    // 洗牌
    initAllCards();
    // 清空所有玩家的牌
    m_robotLeft->clearCards();
    m_robotRight->clearCards();
    m_user->clearCards();
    // 初始化出牌玩家和牌
    m_pendPlayer = nullptr;
    m_pendCards.clear();
}

void GameControl::startLordCard()
{
    m_currPlayer->prepareCallLord();
    emit playerStatusChanged(m_currPlayer, ThinkingForCallLord);
}

void GameControl::becomeLord(Player *player, int bet)
{
    m_curBet = bet;
    player->setRole(Player::Lord);
    player->getPrevPlayer()->setRole(Player::Farmer);
    player->getNextPlayer()->setRole(Player::Farmer);

    m_currPlayer = player;
    player->storeDispatchCard(m_allCards);

    QTimer::singleShot(1000, this, [=]()
    {
        emit gameStatusChanged(PlayingHand);
        emit playerStatusChanged(player, ThinkingForPlayHand);
        m_currPlayer->preparePlayHand();
    });
}

void GameControl::clearPlayerScore()
{
    m_robotLeft->setScore(0);
    m_robotRight->setScore(0);
    m_user->setScore(0);
}

int GameControl::getPlayerMaxBet()
{
    return m_betRecord.bet;
}

void GameControl::onGrabBet(Player *player, int bet)
{
    // 1. 通知主界面玩家叫地主了(更新信息提示)
    if(bet == 0 || m_betRecord.bet >= bet)
    {
        emit notifyGrabLordBet(player, 0, false);
    }
    else if(bet > 0 && m_betRecord.bet == 0)
    {
        // 第一个抢地主的玩家
        emit notifyGrabLordBet(player, bet, true);
    }
    else
    {
        // 第2,3个抢地主的玩家
        emit notifyGrabLordBet(player, bet, false);
    }

    qDebug() << "curent player name: " << player->getName() << ", 下注分数: "
            << bet<< ", m_betRecord.times: " << m_betRecord.times;


    // 2. 判断玩家下注是不是3分, 如果是抢地主结束
    if(bet == 3)
    {
        // 玩家成为地主
        becomeLord(player, bet);
        // 清空数据
        m_betRecord.reset();
        return;
    }
    // 3. 下注不够3分, 对玩家的分数进行比较, 分数高的是地主
    if(m_betRecord.bet < bet)
    {
        m_betRecord.bet = bet;
        m_betRecord.player = player;
    }
    m_betRecord.times ++;
    // 如果每个玩家都抢过一次地主, 抢地主结束
    if(m_betRecord.times == 3)
    {
        if(m_betRecord.bet == 0)
        {
            emit gameStatusChanged(DispatchCard);
        }
        else
        {
            becomeLord(m_betRecord.player, m_betRecord.bet);
        }
        m_betRecord.reset();
        return;
    }
    // 4. 切换玩家, 通知下一个玩家继续抢地主
    m_currPlayer = player->getNextPlayer();
    // 发送信号给主界面, 告知当前状态为抢地主
    emit playerStatusChanged(m_currPlayer, ThinkingForCallLord);
    m_currPlayer->prepareCallLord();
}

void GameControl::onPlayHand(Player *player, Cards &card)
{
    // 1. 将玩家出牌的信号转发给主界面
    emit notifyPlayHand(player, card);
    // 2. 如果不是空牌, 给其他玩家发送信号, 保存出牌玩家对象和打出的牌
    if(!card.isEmpty())
    {
        m_pendCards = card;
        m_pendPlayer = player;
        emit pendingInfo(player, card);
    }
    // 如果有炸弹, 低分翻倍
    PlayHand::HandType type = PlayHand(card).getHandType();
    if(type == PlayHand::Hand_Bomb || type == PlayHand::Hand_Bomb_Jokers)
    {
        m_curBet = m_curBet * 2;
    }

    // 3. 如果玩家的牌出完了, 计算本局游戏的总分
    if(player->getCards().isEmpty())
    {
        Player* prev = player->getPrevPlayer();
        Player* next = player->getNextPlayer();
        if(player->getRole() == Player::Lord)
        {
            player->setScore(player->getScore() + 2 * m_curBet);
            prev->setScore(prev->getScore() - m_curBet);
            next->setScore(next->getScore() - m_curBet);
            player->setWin(true);
            prev->setWin(false);
            next->setWin(false);
        }
        else
        {
            player->setWin(true);
            player->setScore(player->getScore() + m_curBet);
            if(prev->getRole() == Player::Lord)
            {
                prev->setScore(prev->getScore() - 2 * m_curBet);
                next->setScore(next->getScore() + m_curBet);
                prev->setWin(false);
                next->setWin(true);
            }
            else
            {
                next->setScore(next->getScore() - 2 * m_curBet);
                prev->setScore(prev->getScore() + m_curBet);
                next->setWin(false);
                prev->setWin(true);
            }
        }
        emit playerStatusChanged(player, GameControl::Winning);
        return;
    }
    // 4. 牌没有出完, 下一个玩家继续出牌
    m_currPlayer = player->getNextPlayer();
    m_currPlayer->preparePlayHand();
    emit playerStatusChanged(m_currPlayer, GameControl::ThinkingForPlayHand);
}
