#include "MainWindow.h"
#include "PuzzleWidget.h"

#include <DTitlebar>
#include <QActionGroup>
#include <QMenu>
#include <QMessageBox>
#include <QSettings>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : DMainWindow(parent)
    , m_gridSize(qBound(3, QSettings().value("tamano", 4).toInt(), 5))
{
    setupUI();
    setupMenu();
    newGame();
}

void MainWindow::setupUI()
{
    QWidget *central = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(central);
    layout->setAlignment(Qt::AlignCenter);
    layout->setContentsMargins(20, 12, 20, 12);
    layout->setSpacing(12);

    m_puzzle = new PuzzleWidget(this);

    m_movesLabel = new DLabel(this);
    m_movesLabel->setAlignment(Qt::AlignCenter);
    QFont f = m_movesLabel->font();
    f.setBold(true);
    m_movesLabel->setFont(f);

    layout->addWidget(m_puzzle, 0, Qt::AlignCenter);
    layout->addWidget(m_movesLabel);

    setCentralWidget(central);

    connect(m_puzzle, &PuzzleWidget::moveCountChanged, this, &MainWindow::updateStatus);
    connect(m_puzzle, &PuzzleWidget::secondsChanged, this, &MainWindow::updateStatus);
    connect(m_puzzle, &PuzzleWidget::puzzleSolved, this, &MainWindow::onPuzzleSolved);

    // El tamano de la ventana incluye la barra de titulo de DTK
    setFixedSize(440, 400 + 12 * 3 + m_movesLabel->sizeHint().height() + titlebar()->sizeHint().height());
    setWindowTitle(tr("Puzzle 15"));
}

void MainWindow::setupMenu()
{
    QMenu *menu = new QMenu(this);

    QAction *newAction = menu->addAction(tr("Nuevo juego"));
    newAction->setShortcut(QKeySequence("Ctrl+N"));
    connect(newAction, &QAction::triggered, this, &MainWindow::newGame);

    menu->addSeparator();

    // Grupo exclusivo: antes, al volver de 3x3 a 4x4 quedaban las dos marcadas
    auto *sizes = new QActionGroup(this);
    sizes->setExclusive(true);
    for (int n : {3, 4, 5}) {
        QAction *a = menu->addAction(tr("%1x%1").arg(n));
        a->setCheckable(true);
        a->setChecked(n == m_gridSize);
        sizes->addAction(a);
        connect(a, &QAction::triggered, this, [this, n]() {
            m_gridSize = n;
            QSettings().setValue("tamano", n);
            newGame();
        });
    }

    menu->addSeparator();
    QAction *quitAction = menu->addAction(tr("Salir"));
    quitAction->setShortcut(QKeySequence("Ctrl+Q"));
    connect(quitAction, &QAction::triggered, this, &QWidget::close);

    titlebar()->setMenu(menu);
    // Los atajos del menu de la barra de titulo solo funcionan con el menu abierto
    addAction(newAction);
    addAction(quitAction);
}

int MainWindow::bestMoves() const
{
    return QSettings().value(QStringLiteral("record_%1").arg(m_gridSize), 0).toInt();
}

void MainWindow::newGame()
{
    m_puzzle->newGame(m_gridSize, m_gridSize);
    m_puzzle->setFocus();
    updateStatus();
}

void MainWindow::updateStatus()
{
    const int s = m_puzzle->seconds();
    const QString tiempo = QStringLiteral("%1:%2").arg(s / 60).arg(s % 60, 2, 10, QLatin1Char('0'));
    const int best = bestMoves();
    QString texto = tr("Movimientos: %1  |  Tiempo: %2").arg(m_puzzle->moves()).arg(tiempo);
    if (best > 0)
        texto += tr("  |  Récord: %1").arg(best);
    m_movesLabel->setText(texto);
}

void MainWindow::onPuzzleSolved()
{
    const int moves = m_puzzle->moves();
    const int best = bestMoves();
    const bool record = best == 0 || moves < best;
    if (record)
        QSettings().setValue(QStringLiteral("record_%1").arg(m_gridSize), moves);
    updateStatus();

    const int s = m_puzzle->seconds();
    QString msg = tr("Completaste el puzzle en %1 movimientos y %2:%3.")
                      .arg(moves).arg(s / 60).arg(s % 60, 2, 10, QLatin1Char('0'));
    if (record)
        msg += QLatin1Char('\n') + tr("¡Nuevo récord para %1x%1!").arg(m_gridSize);
    QMessageBox::information(this, tr("Felicidades"), msg);
}
