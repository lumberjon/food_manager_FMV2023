#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QFile>
#include <QLineEdit>



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_lE_ReadBarCode_textEdited(const QString &arg1);

    void on_actionCarregar_Listagem_triggered();

    void compute_everything();

    void save_data();

    void on_rB_5J_clicked();

    void on_rB_6A_clicked();

    void on_rB_6J_clicked();

    void on_rB_SA_clicked();

    void on_rB_SJ_clicked();

    void on_rB_DA_clicked();

    void on_rB_DJ_clicked();

    void on_actionSobre_triggered();

private:
    Ui::MainWindow *ui;

    /*####  Constantes de leitura  ####*/

    QStringList VolunteerName;  // < 200

    QStringList VolunteerNumber;    // < 200

    QStringList tipo_dieta;

    // voluntarios - refeicoes previstas
    QList <QList <int>> refeicoes_previstas;

    QString localizacao_ficheiro;
    QString header_ficheiro;

    /*####  Variaveis para gravar  ####*/

    // voluntarios - refeicoes realizadas
    QList <QList <int>> refeicoes_realizadas;

    QList <QList <QDateTime>> hora_refeicoes_realizadas;

    /*####  Variaveis para apresentacao  ####*/

    QList <int> cont_ref_previstas_p_dia;   // contagem de refeicoes previstas por dia

    QList <int> cont_ref_realizadas_p_dia;  // contagem de refeicoes realizadas por dia

    int cont_ref_previstas_total;   // contagem de refeicoes previstas total

    int cont_ref_realizadas_total;  // contagem de refeicoes realizadas total
};



#endif // MAINWINDOW_H
