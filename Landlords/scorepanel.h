#ifndef SCOREPANEL_H
#define SCOREPANEL_H

#include <QLabel>
#include <QWidget>

namespace Ui {
class ScorePanel;
}

class ScorePanel : public QWidget
{
    Q_OBJECT

public:
    enum FontColor{Black, White, Red, Blue, Green};
    explicit ScorePanel(QWidget *parent = nullptr);
    ~ScorePanel();

    // 设置玩家的得分
    void setScores(int left, int right, int user);
    // 设置字体大小
    void setMyFontSize(int point);
    // 设置字体的颜色
    void setMyFontColor(FontColor color);

private:
    Ui::ScorePanel *ui;
    QVector<QLabel*> m_list;
};

#endif // SCOREPANEL_H
