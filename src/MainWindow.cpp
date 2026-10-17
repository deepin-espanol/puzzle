#include "MainWindow.h"
#include "PuzzleWidget.h"

#include <DTitlebar>
#include <QApplication>
#include <QMenu>
#include <QVBoxLayout>
#include <DWidget>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : DMainWindow(parent)
    , m_gridSize(4)
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
    layout->setSpacing(12);

    m_puzzle = new PuzzleWidget(this);
    m_puzzle->setFixedSize(400, 400);

    m_movesLabel = new DLabel(this);
    m_movesLabel->setAlignment(Qt::AlignCenter);
    m_movesLabel->setText(QApplication::translate("MainWindow", "Movimientos: 0"));

    layout->addStretch();
    layout->addWidget(m_puzzle, 0, Qt::AlignCenter);
    layout->addWidget(m_movesLabel);
    layout->addStretch();

    setCentralWidget(central);

    connect(m_puzzle, &PuzzleWidget::moveCountChanged, this, &MainWindow::onMoveCountChanged);
    connect(m_puzzle, &PuzzleWidget::puzzleSolved, this, &MainWindow::onPuzzleSolved);

    setFixedSize(480, 520);
    setWindowTitle(QApplication::translate("MainWindow", "Puzzle 15"));
}

void MainWindow::setupMenu()
{
    QMenu *menu = new QMenu(this);

    QAction *newAction = menu->addAction(QApplication::translate("MainWindow", "Nuevo juego"));
    connect(newAction, &QAction::triggered, this, &MainWindow::newGame);

    menu->addSeparator();

    QAction *action4x4 = menu->addAction(QApplication::translate("MainWindow", "4x4"));
    action4x4->setCheckable(true);
    action4x4->setChecked(true);
    connect(action4x4, &QAction::triggered, this, [this, action4x4]() {
        m_gridSize = 4;
        action4x4->setChecked(true);
        newGame();
    });

    QAction *action3x3 = menu->addAction(QApplication::translate("MainWindow", "3x3"));
    action3x3->setCheckable(true);
    connect(action3x3, &QAction::triggered, this, [this, action4x4, action3x3]() {
        m_gridSize = 3;
        action3x3->setChecked(true);
        action4x4->setChecked(false);
        newGame();
    });

    titlebar()->setMenu(menu);
}

void MainWindow::newGame()
{
    m_puzzle->newGame(m_gridSize, m_gridSize);

    int tileSize = 400 / m_gridSize;
    m_puzzle->setFixedSize(tileSize * m_gridSize, tileSize * m_gridSize);
}

void MainWindow::onMoveCountChanged(int moves)
{
    m_movesLabel->setText(QApplication::translate("MainWindow", "Movimientos: %1").arg(moves));
}

void MainWindow::onPuzzleSolved()
{
    QMessageBox::information(this,
                             QApplication::translate("MainWindow", "Felicidades"),
                             QApplication::translate("MainWindow", "Completaste el puzzle en %1 movimientos!")
                                 .arg(m_puzzle->moves()));
}
