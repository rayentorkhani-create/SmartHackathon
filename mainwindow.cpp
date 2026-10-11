

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QInputDialog>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QLineEdit>
#include <QPushButton>
#include <QCoreApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Connexion à SQLite
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");

    QString cheminBase =
        QCoreApplication::applicationDirPath()
        + "/smart_hackathon.db";

    db.setDatabaseName(cheminBase);

    if (!db.open()) {
        QString drivers = QSqlDatabase::drivers().join(", ");

        QMessageBox::critical(
            this,
            "Erreur SQLite",
            "Impossible d'ouvrir la base de données.\n\n"
                + db.lastError().text()
                + "\n\nDrivers disponibles : "
                + (drivers.isEmpty() ? "aucun" : drivers)
            );

        return;
    }

    // Création de la table jury si elle n'existe pas
    QSqlQuery query;

    if (!query.exec(
            "CREATE TABLE IF NOT EXISTS jury ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "nom TEXT NOT NULL,"
            "email TEXT NOT NULL)")) {

        QMessageBox::critical(
            this,
            "Erreur SQL",
            query.lastError().text()
            );

        return;
    }

    // Connexion des boutons de l'interface
    connect(ui->boutonAjouter,
            &QPushButton::clicked,
            this,
            &MainWindow::ajouterJury);

    connect(ui->boutonModifier,
            &QPushButton::clicked,
            this,
            &MainWindow::modifierJury);

    connect(ui->boutonSupprimer,
            &QPushButton::clicked,
            this,
            &MainWindow::supprimerJury);

    connect(ui->boutonRechercher,
            &QPushButton::clicked,
            this,
            &MainWindow::rechercherJury);

    chargerJurys();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// ==================================
// AFFICHER LES JURYS
// ==================================
void MainWindow::chargerJurys()
{
    QSqlQuery query(
        "SELECT id, nom, email FROM jury ORDER BY nom"
        );

    if (!query.exec()) {
        QMessageBox::critical(
            this, "Erreur SQL",
            query.lastError().text()
            );
        return;
    }

    ui->tableauJurys->setColumnCount(3);

    ui->tableauJurys->setHorizontalHeaderLabels(
        {"ID", "Nom", "Email"}
        );

    ui->tableauJurys->setRowCount(0);

    while (query.next()) {
        int ligne = ui->tableauJurys->rowCount();

        ui->tableauJurys->insertRow(ligne);

        for (int col = 0; col < 3; ++col) {
            ui->tableauJurys->setItem(
                ligne,
                col,
                new QTableWidgetItem(
                    query.value(col).toString()
                    )
                );
        }
    }

    ui->tableauJurys->setColumnHidden(0, true);
    ui->tableauJurys->resizeColumnsToContents();
}

// ==================================
// AJOUTER UN JURY
// ==================================
void MainWindow::ajouterJury()
{
    bool okNom;
    bool okEmail;

    QString nom = QInputDialog::getText(
        this,
        "Ajouter un jury",
        "Nom :",
        QLineEdit::Normal,
        "",
        &okNom
        );

    if (!okNom)
        return;

    nom = nom.trimmed();

    if (nom.isEmpty()) {
        QMessageBox::warning(
            this, "Attention",
            "Le nom est obligatoire."
            );
        return;
    }

    QString email = QInputDialog::getText(
        this,
        "Ajouter un jury",
        "Email :",
        QLineEdit::Normal,
        "",
        &okEmail
        );

    if (!okEmail)
        return;

    email = email.trimmed();

    if (email.isEmpty()) {
        QMessageBox::warning(
            this, "Attention",
            "L'email est obligatoire."
            );
        return;
    }

    QSqlQuery query;

    query.prepare(
        "INSERT INTO jury (nom, email) "
        "VALUES (:nom, :email)"
        );

    query.bindValue(":nom", nom);
    query.bindValue(":email", email);

    if (!query.exec()) {
        QMessageBox::critical(
            this, "Erreur SQL",
            query.lastError().text()
            );
        return;
    }

    chargerJurys();

    QMessageBox::information(
        this, "Succès",
        "Jury ajouté avec succès."
        );
}

// ==================================
// MODIFIER UN JURY
// ==================================
void MainWindow::modifierJury()
{
    int ligne = ui->tableauJurys->currentRow();

    if (ligne < 0) {
        QMessageBox::warning(
            this, "Attention",
            "Sélectionne un jury dans le tableau."
            );
        return;
    }

    int id = ui->tableauJurys->item(ligne, 0)
                 ->text().toInt();

    QString ancienNom =
        ui->tableauJurys->item(ligne, 1)->text();

    QString ancienEmail =
        ui->tableauJurys->item(ligne, 2)->text();

    bool ok;

    QString nom = QInputDialog::getText(
        this, "Modifier le jury",
        "Nom :",
        QLineEdit::Normal,
        ancienNom,
        &ok
        );

    if (!ok)
        return;

    nom = nom.trimmed();

    if (nom.isEmpty()) {
        QMessageBox::warning(
            this, "Attention",
            "Le nom est obligatoire."
            );
        return;
    }

    QString email = QInputDialog::getText(
        this, "Modifier le jury",
        "Email :",
        QLineEdit::Normal,
        ancienEmail,
        &ok
        );

    if (!ok)
        return;

    email = email.trimmed();

    if (email.isEmpty()) {
        QMessageBox::warning(
            this, "Attention",
            "L'email est obligatoire."
            );
        return;
    }

    QSqlQuery query;

    query.prepare(
        "UPDATE jury SET nom = :nom, email = :email "
        "WHERE id = :id"
        );

    query.bindValue(":nom", nom);
    query.bindValue(":email", email);
    query.bindValue(":id", id);

    if (!query.exec()) {
        QMessageBox::critical(
            this, "Erreur SQL",
            query.lastError().text()
            );
        return;
    }

    chargerJurys();

    QMessageBox::information(
        this, "Succès",
        "Jury modifié avec succès."
        );
}

// ==================================
// SUPPRIMER UN JURY
// ==================================
void MainWindow::supprimerJury()
{
    int ligne = ui->tableauJurys->currentRow();

    if (ligne < 0) {
        QMessageBox::warning(
            this, "Attention",
            "Sélectionne un jury dans le tableau."
            );
        return;
    }

    int id = ui->tableauJurys->item(ligne, 0)
                 ->text().toInt();

    QMessageBox::StandardButton reponse =
        QMessageBox::question(
            this,
            "Confirmation",
            "Voulez-vous supprimer ce jury ?",
            QMessageBox::Yes | QMessageBox::No
            );

    if (reponse != QMessageBox::Yes)
        return;

    QSqlQuery query;

    query.prepare(
        "DELETE FROM jury WHERE id = :id"
        );

    query.bindValue(":id", id);

    if (!query.exec()) {
        QMessageBox::critical(
            this, "Erreur SQL",
            query.lastError().text()
            );
        return;
    }

    chargerJurys();

    QMessageBox::information(
        this, "Succès",
        "Jury supprimé avec succès."
        );
}

// ==================================
// RECHERCHER UN JURY
// ==================================
void MainWindow::rechercherJury()
{
    QString recherche =
        ui->champRecherche->text().trimmed();

    QSqlQuery query;

    query.prepare(
        "SELECT id, nom, email FROM jury "
        "WHERE nom LIKE :nom ORDER BY nom"
        );

    query.bindValue(":nom", "%" + recherche + "%");

    if (!query.exec()) {
        QMessageBox::critical(
            this, "Erreur SQL",
            query.lastError().text()
            );
        return;
    }

    ui->tableauJurys->setRowCount(0);

    while (query.next()) {
        int ligne = ui->tableauJurys->rowCount();

        ui->tableauJurys->insertRow(ligne);

        for (int col = 0; col < 3; ++col) {
            ui->tableauJurys->setItem(
                ligne,
                col,
                new QTableWidgetItem(
                    query.value(col).toString()
                    )
                );
        }
    }

    ui->tableauJurys->setColumnHidden(0, true);
    ui->tableauJurys->resizeColumnsToContents();
}

// ==================================
// AFFECTER UNE EQUIPE A UN JURY
// A COMPLETER SELON LA TABLE EQUIPE
// ==================================
void MainWindow::affecterEquipe()
{
    QMessageBox::information(
        this,
        "Affectation",
        "La fonction d'affectation sera ajoutée "
        "après vérification de la table des équipes."
        );
}
