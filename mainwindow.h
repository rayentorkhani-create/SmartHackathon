
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    // Ajouter un jury
    void ajouterJury();

    // Modifier un jury
    void modifierJury();

    // Supprimer un jury
    void supprimerJury();

    // Rechercher un jury
    void rechercherJury();

    // Afficher et actualiser les jurys
    void chargerJurys();

    // Affecter une equipe a un jury
    void affecterEquipe();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H

