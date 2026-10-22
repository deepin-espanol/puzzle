#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <DMainWindow>
#include <DLabel>

DWIDGET_USE_NAMESPACE

class PuzzleWidget;

class MainWindow : public DMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void newGame();
    void onPuzzleSolved();

private:
    void setupUI();
    void setupMenu();
    void updateStatus();
    int bestMoves() const;

    PuzzleWidget *m_puzzle;
    DLabel *m_movesLabel;
    int m_gridSize;
};

#endif // MAINWINDOW_H
