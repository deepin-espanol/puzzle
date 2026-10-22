#include "PuzzleWidget.h"

#include <QPainter>
#include <QMouseEvent>
#include <QRandomGenerator>
#include <QFont>
#include <QKeyEvent>

PuzzleWidget::PuzzleWidget(QWidget *parent)
    : QWidget(parent)
    , m_rows(4)
    , m_cols(4)
    , m_moves(0)
    , m_won(false)
{
    setFixedSize(400, 400);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    m_clock.setInterval(1000);
    connect(&m_clock, &QTimer::timeout, this, [this]() {
        ++m_seconds;
        emit secondsChanged(m_seconds);
    });
    newGame(4, 4);
}

void PuzzleWidget::newGame(int rows, int cols)
{
    m_rows = rows;
    m_cols = cols;
    m_moves = 0;
    m_won = false;
    m_seconds = 0;
    m_started = false;
    m_clock.stop();
    initBoard();
    // Barajar con movimientos validos garantiza que tiene solucion; se repite
    // si por azar quedara ya resuelto.
    do {
        shuffleBoard();
    } while (checkSolved());
    emit moveCountChanged(m_moves);
    emit secondsChanged(m_seconds);
    update();
}

void PuzzleWidget::initBoard()
{
    m_board.clear();
    int value = 1;

    for (int r = 0; r < m_rows; ++r) {
        QVector<int> row;
        for (int c = 0; c < m_cols; ++c) {
            if (r == m_rows - 1 && c == m_cols - 1) {
                row.append(0);
                m_emptyPos = QPoint(r, c);
            } else {
                row.append(value++);
            }
        }
        m_board.append(row);
    }
}

void PuzzleWidget::shuffleBoard()
{
    // Perform random valid moves to ensure solvability
    int totalTiles = m_rows * m_cols - 1;
    int shuffleMoves = totalTiles * 20;

    for (int i = 0; i < shuffleMoves; ++i) {
        QVector<QPoint> neighbors;

        if (m_emptyPos.x() > 0)
            neighbors.append(QPoint(m_emptyPos.x() - 1, m_emptyPos.y()));
        if (m_emptyPos.x() < m_rows - 1)
            neighbors.append(QPoint(m_emptyPos.x() + 1, m_emptyPos.y()));
        if (m_emptyPos.y() > 0)
            neighbors.append(QPoint(m_emptyPos.x(), m_emptyPos.y() - 1));
        if (m_emptyPos.y() < m_cols - 1)
            neighbors.append(QPoint(m_emptyPos.x(), m_emptyPos.y() + 1));

        int idx = QRandomGenerator::global()->bounded(neighbors.size());
        QPoint neighbor = neighbors[idx];

        // Swap empty with neighbor
        m_board[m_emptyPos.x()][m_emptyPos.y()] = m_board[neighbor.x()][neighbor.y()];
        m_board[neighbor.x()][neighbor.y()] = 0;
        m_emptyPos = neighbor;
    }

    m_moves = 0;
}

int PuzzleWidget::countInversions() const
{
    QVector<int> flat;
    for (int r = 0; r < m_rows; ++r) {
        for (int c = 0; c < m_cols; ++c) {
            if (m_board[r][c] != 0)
                flat.append(m_board[r][c]);
        }
    }

    int inversions = 0;
    for (int i = 0; i < flat.size(); ++i) {
        for (int j = i + 1; j < flat.size(); ++j) {
            if (flat[i] > flat[j])
                ++inversions;
        }
    }
    return inversions;
}

bool PuzzleWidget::isSolvable() const
{
    int inversions = countInversions();

    if (m_rows % 2 == 1) {
        // Odd grid: solvable if inversions are even
        return inversions % 2 == 0;
    } else {
        // Even grid: solvable if (inversions + empty row from bottom) is even
        int emptyRowFromBottom = m_rows - m_emptyPos.x();
        return (inversions + emptyRowFromBottom) % 2 == 0;
    }
}

QColor PuzzleWidget::tileColor(int value) const
{
    if (value == 0)
        return QColor(60, 60, 60);

    int total = m_rows * m_cols - 1;
    double t = static_cast<double>(value - 1) / (total - 1);

    int r, g, b;

    if (t < 0.25) {
        double lt = t / 0.25;
        r = static_cast<int>(30 + lt * 0);
        g = static_cast<int>(100 + lt * 55);
        b = static_cast<int>(200 + lt * 35);
    } else if (t < 0.5) {
        double lt = (t - 0.25) / 0.25;
        r = static_cast<int>(30 + lt * 50);
        g = static_cast<int>(155 + lt * 50);
        b = static_cast<int>(235 - lt * 50);
    } else if (t < 0.75) {
        double lt = (t - 0.5) / 0.25;
        r = static_cast<int>(80 + lt * 140);
        g = static_cast<int>(205 - lt * 20);
        b = static_cast<int>(185 - lt * 130);
    } else {
        double lt = (t - 0.75) / 0.25;
        r = static_cast<int>(220 + lt * 35);
        g = static_cast<int>(185 - lt * 110);
        b = static_cast<int>(55 - lt * 35);
    }

    return QColor(qBound(0, r, 255), qBound(0, g, 255), qBound(0, b, 255));
}

void PuzzleWidget::drawTile(QPainter &painter, int row, int col, int value)
{
    int tileW = width() / m_cols;
    int tileH = height() / m_rows;

    int margin = 3;
    int x = col * tileW + margin;
    int y = row * tileH + margin;
    int w = tileW - margin * 2;
    int h = tileH - margin * 2;

    QColor color = tileColor(value);

    if (value != 0) {
        painter.setPen(Qt::NoPen);
        painter.setBrush(color);
        painter.drawRoundedRect(x, y, w, h, 8, 8);

        // Highlight
        QLinearGradient grad(x, y, x, y + h);
        grad.setColorAt(0, QColor(255, 255, 255, 60));
        grad.setColorAt(0.5, QColor(255, 255, 255, 0));
        grad.setColorAt(1, QColor(0, 0, 0, 30));
        painter.setBrush(grad);
        painter.drawRoundedRect(x, y, w, h, 8, 8);

        // Shadow bottom-right
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(0, 0, 0, 40));
        painter.drawRoundedRect(x + 2, y + 2, w, h, 8, 8);

        // Main tile again on top
        painter.setBrush(color);
        painter.drawRoundedRect(x, y - 1, w, h - 2, 8, 8);

        // Number text
        QFont font = painter.font();
        font.setPixelSize(qMin(w, h) / 2);
        font.setBold(true);
        painter.setFont(font);
        painter.setPen(QColor(255, 255, 255));
        painter.drawText(QRect(x, y, w, h), Qt::AlignCenter, QString::number(value));
    } else {
        // Empty tile
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(50, 50, 50));
        painter.drawRoundedRect(x, y, w, h, 8, 8);
    }
}

void PuzzleWidget::paintEvent(QPaintEvent * /*event*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.fillRect(rect(), QColor(40, 40, 40));

    for (int r = 0; r < m_rows; ++r) {
        for (int c = 0; c < m_cols; ++c) {
            drawTile(painter, r, c, m_board[r][c]);
        }
    }
}

bool PuzzleWidget::checkSolved() const
{
    int expected = 1;
    for (int r = 0; r < m_rows; ++r) {
        for (int c = 0; c < m_cols; ++c) {
            const bool last = (r == m_rows - 1 && c == m_cols - 1);
            if (m_board[r][c] != (last ? 0 : expected))
                return false;
            ++expected;
        }
    }
    return true;
}

// Desliza hacia el hueco todas las fichas entre (row,col) y el hueco, si
// estan en la misma fila o columna (antes solo se movia una ficha contigua).
bool PuzzleWidget::slideFrom(int row, int col)
{
    if (m_won || row < 0 || row >= m_rows || col < 0 || col >= m_cols)
        return false;
    const int er = m_emptyPos.x(), ec = m_emptyPos.y();
    if ((row != er && col != ec) || (row == er && col == ec))
        return false;

    const int dr = (row > er) - (row < er);
    const int dc = (col > ec) - (col < ec);
    while (m_emptyPos != QPoint(row, col)) {
        const int nr = m_emptyPos.x() + dr, nc = m_emptyPos.y() + dc;
        m_board[m_emptyPos.x()][m_emptyPos.y()] = m_board[nr][nc];
        m_board[nr][nc] = 0;
        m_emptyPos = QPoint(nr, nc);
        ++m_moves;
    }

    if (!m_started) {
        m_started = true;
        m_clock.start();
    }
    emit moveCountChanged(m_moves);
    if (checkSolved()) {
        m_won = true;
        m_clock.stop();
        emit puzzleSolved();
    }
    update();
    return true;
}

void PuzzleWidget::mousePressEvent(QMouseEvent *event)
{
    setFocus();
    const int tileW = width() / m_cols;
    const int tileH = height() / m_rows;
    slideFrom(int(event->position().y()) / tileH, int(event->position().x()) / tileW);
}

// Las flechas mueven la ficha que esta junto al hueco en esa direccion
void PuzzleWidget::keyPressEvent(QKeyEvent *event)
{
    const int er = m_emptyPos.x(), ec = m_emptyPos.y();
    switch (event->key()) {
    case Qt::Key_Left:  case Qt::Key_A: slideFrom(er, ec + 1); break;
    case Qt::Key_Right: case Qt::Key_D: slideFrom(er, ec - 1); break;
    case Qt::Key_Up:    case Qt::Key_W: slideFrom(er + 1, ec); break;
    case Qt::Key_Down:  case Qt::Key_S: slideFrom(er - 1, ec); break;
    default: QWidget::keyPressEvent(event);
    }
}

bool PuzzleWidget::isAdjacent(const QPoint &a, const QPoint &b) const
{
    int dr = qAbs(a.x() - b.x());
    int dc = qAbs(a.y() - b.y());
    return (dr + dc) == 1;
}
