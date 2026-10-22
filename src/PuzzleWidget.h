#ifndef PUZZLEWIDGET_H
#define PUZZLEWIDGET_H

#include <QWidget>
#include <QVector>
#include <QPoint>
#include <QTimer>
#include <QElapsedTimer>

class PuzzleWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PuzzleWidget(QWidget *parent = nullptr);

    void newGame(int rows = 4, int cols = 4);
    int moves() const { return m_moves; }
    bool isWon() const { return m_won; }
    int seconds() const { return m_seconds; }

signals:
    void moveCountChanged(int moves);
    void secondsChanged(int seconds);
    void puzzleSolved();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    void initBoard();
    void shuffleBoard();
    bool isSolvable() const;
    int countInversions() const;
    void drawTile(QPainter &painter, int row, int col, int value);
    QColor tileColor(int value) const;
    bool isAdjacent(const QPoint &a, const QPoint &b) const;
    bool slideFrom(int row, int col);
    bool checkSolved() const;

    int m_rows;
    int m_cols;
    QVector<QVector<int>> m_board;
    QPoint m_emptyPos;
    int m_moves;
    bool m_won;
    int m_seconds = 0;
    bool m_started = false;  // el reloj arranca con el primer movimiento
    QTimer m_clock;
};

#endif // PUZZLEWIDGET_H
