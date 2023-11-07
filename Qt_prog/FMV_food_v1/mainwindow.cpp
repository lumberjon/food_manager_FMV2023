#include "mainwindow.h"
#include "ui_mainwindow.h"

/*
 *  barcodes gerados em
 *      https://www.barcode-generator.de/en/simple-generator-for-all-barcodes
 */

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->lE_ReadBarCode->setFocus();

    this->setWindowIcon( QIcon(":/fmv_logo.ico") );

    on_actionCarregar_Listagem_triggered();
}

MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * @brief MainWindow::on_lE_ReadBarCode_textEdited
 *  Nova leitura de Barcode
 * @param arg1
 */
void MainWindow::on_lE_ReadBarCode_textEdited(const QString &arg1)
{
    QString barcode = arg1;

    barcode.remove(QChar('\r'));
    barcode.remove(QChar('\n'));

    int IdxRefeicao = 0;

    if ( ui->rB_5J->isChecked() ) {
        IdxRefeicao = 0;
    } else if ( ui->rB_6A->isChecked() ) {
        IdxRefeicao = 1;
    } else if ( ui->rB_6J->isChecked() ) {
        IdxRefeicao = 2;
    } else if ( ui->rB_SA->isChecked() ) {
        IdxRefeicao = 3;
    } else if ( ui->rB_SJ->isChecked() ) {
        IdxRefeicao = 4;
    } else if ( ui->rB_DA->isChecked() ) {
        IdxRefeicao = 5;
    } else if ( ui->rB_DJ->isChecked() ) {
        IdxRefeicao = 6;
    }
    else {
        return;
    }

    /* So procura se tiver 4 digitos */
    if ( barcode.length() == 4 ) {

        if ( localizacao_ficheiro.isEmpty() || localizacao_ficheiro.isNull() ) {
            //QMessageBox::warning(this, "Procura de Voluntários", "Carregar a lista de voluntários primeiro!!!" );
            ui->l_result->setText( "<font color='red'>Carregar a lista de voluntários primeiro!!!</font>" );
            ui->lE_ReadBarCode->clear();
            return;
        }

        qDebug() << ">> Barcode: " << barcode;

        qDebug() << "\t>> Refeicao: " << IdxRefeicao;

        ui->l_result->clear();

        for ( int v = 0; v < VolunteerNumber.length(); v++ ) {
            if ( VolunteerNumber[v].toUpper() == barcode.toUpper() ) {

                /*#########################################################*/
                /*################  Voluntario encontrado  ################*/
                /*#########################################################*/

                // Preencher nome
                qDebug() << "\t\t>> Nome: " << VolunteerName[v];
                ui->lE_Nome->setText( VolunteerName[v] );

                // Preencher dieta
                qDebug() << "\t\t>> Dieta: " << tipo_dieta[v];
                ui->lE_Dieta->setText( tipo_dieta[v] );

                // Refeicao prevista?
                qDebug() << "\t\t>> Refeicao prevista: " << refeicoes_previstas[v][IdxRefeicao];
                if ( refeicoes_previstas[v][IdxRefeicao] != 1 ) {
                    // QMessageBox::warning(this, "Procura de Voluntários", "Refeição realizada às " + hora_refeicoes_realizadas[v][IdxRefeicao].toString() );
                    ui->l_result->setText( "<font color='red'>Refeição não Prevista</font>" );
                    //ui->l_result->setStyleSheet("{color:#2CAA2C}");
                    ui->lE_ReadBarCode->clear();
                    return;
                }

                // Refeicao repetida
                qDebug() << "\t\t>> Refeicao repetida: " << refeicoes_realizadas[v][IdxRefeicao];
                if ( refeicoes_realizadas[v][IdxRefeicao] == 1 ) {
                    // QMessageBox::warning(this, "Procura de Voluntários", "Refeição realizada às " + hora_refeicoes_realizadas[v][IdxRefeicao].toString() );
                    ui->l_result->setText( "<font color='red'>Refeição realizada às " + hora_refeicoes_realizadas[v][IdxRefeicao].toString() + "</font>" );
                    //ui->l_result->setStyleSheet("{color:#AA2C2C}");
                    ui->lE_ReadBarCode->clear();
                    return;
                }

                ui->l_result->setText( "<font color='green'>OK!</font>" );
                //ui->l_result->setStyleSheet("{color:#2CAA2C}");

                // Assinalar refeicao
                refeicoes_realizadas[v][IdxRefeicao] = 1;

                // Assinalar hora
                qDebug() << "\t\t>> Hora: " << QDateTime::currentDateTime();
                hora_refeicoes_realizadas[v][IdxRefeicao] = QDateTime::currentDateTime();

                // Calculo stats
                compute_everything();

                // Save dados
                save_data();

                ui->lE_ReadBarCode->clear();

                return;
            }
        }

        //QMessageBox::warning(this, "Procura de Voluntários", "Voluntário " + barcode + " não encontrado" );
        ui->l_result->setText( "<font color='red'>Voluntário " + barcode + " não encontrado</font>" );
        ui->lE_Nome->clear();
        ui->lE_Dieta->clear();

        ui->lE_ReadBarCode->clear();
    }
}


/**
 * @brief MainWindow::on_actionCarregar_Listagem_triggered
 * Carregar a Listagem de voluntarios
 */
void MainWindow::on_actionCarregar_Listagem_triggered()
{
    qDebug() << ">> Carregar listagem";

    localizacao_ficheiro = QFileDialog::getOpenFileName(
        this,
        "Selecionar Listagem",
        "/",
        "*.csv");

    qDebug() << "\t>> " << localizacao_ficheiro;

    QFile file( localizacao_ficheiro );

    if (!file.open(QIODevice::ReadWrite)) {
        qDebug() << file.errorString();
        QMessageBox::warning(this, "FMV Manager 2023", "Ficheiro não carregado!" );
        return;
    }

    // Ignora a primeira linha
    header_ficheiro = file.readLine();

    // Limpeza vetores
    VolunteerNumber.empty();
    VolunteerName.empty();
    tipo_dieta.empty();
    refeicoes_previstas.empty();
    refeicoes_realizadas.empty();
    hora_refeicoes_realizadas.empty();
    cont_ref_previstas_p_dia.empty();
    cont_ref_realizadas_p_dia.empty();

    /*#############################################*/
    /*################  Fill data  ################*/
    /*#############################################*/
    while (!file.atEnd()) {
        QByteArray line = file.readLine();

        if ( line.split(',')[0].isEmpty() || line.split(',')[0].isNull() ) {
            qDebug() << "\t>> Fim do ficheiro";
            break;
        }

        // ler numero
        VolunteerNumber.append( line.split(',')[0] );
        qDebug() << "\n\n\tN: " << VolunteerNumber.last();

        // Ler nome
        VolunteerName.append( line.split(',')[1] );
        qDebug() << "\tNome: " << VolunteerName.last();

        // Ler dieta
        tipo_dieta.append( line.split(',')[2] );
        qDebug() << "\tDieta: " << tipo_dieta.last();

        // Ler refeicoes previstas
        int p5J = line.split(',')[3].toInt();
        qDebug() << "\t\tp5J " << p5J;
        int p6A = line.split(',')[4].toInt();
        qDebug() << "\t\tp6A " << p6A;
        int p6J = line.split(',')[5].toInt();
        qDebug() << "\t\tp6J " << p6J;
        int pSA = line.split(',')[6].toInt();
        qDebug() << "\t\tpSA " << pSA;
        int pSJ = line.split(',')[7].toInt();
        qDebug() << "\t\tpSJ " << pSJ;
        int pDA = line.split(',')[8].toInt();
        qDebug() << "\t\tpDA " << pDA;
        int pDJ = line.split(',')[9].toInt();
        qDebug() << "\t\tpDJ " << pDJ;
        QList <int> refeicoes_previstas_aux;
        refeicoes_previstas_aux.append( {p5J, p6A, p6J, pSA, pSJ, pDA, pDJ} );
        refeicoes_previstas.append( refeicoes_previstas_aux );

        // Ler refeicoes realizadas
        int r5J = line.split(',')[10].toInt();
        qDebug() << "\t\tr5J " << r5J;
        int r6A = line.split(',')[11].toInt();
        qDebug() << "\t\tr6A " << r6A;
        int r6J = line.split(',')[12].toInt();
        qDebug() << "\t\tr6J " << r6J;
        int rSA = line.split(',')[13].toInt();
        qDebug() << "\t\trSA " << rSA;
        int rSJ = line.split(',')[14].toInt();
        qDebug() << "\t\trSJ " << rSJ;
        int rDA = line.split(',')[15].toInt();
        qDebug() << "\t\trDA " << rDA;
        int rDJ = line.split(',')[16].toInt();
        qDebug() << "\t\trDJ " << rDJ;

        QList <int> refeicoes_realizadas_aux;
        refeicoes_realizadas_aux.append( {r5J, r6A, r6J, rSA, rSJ, rDA, rDJ} );
        refeicoes_realizadas.append( refeicoes_realizadas_aux );

        // Ler horas das refeicoes realizadas
        QDateTime h5J = QDateTime::fromString( line.split(',')[17] );
        qDebug() << "\t\th5J " << h5J;
        QDateTime h6A = QDateTime::fromString( line.split(',')[18] );
        qDebug() << "\t\th6A " << h6A;
        QDateTime h6J = QDateTime::fromString( line.split(',')[19] );
        qDebug() << "\t\th6J " << h6J;
        QDateTime hSA = QDateTime::fromString( line.split(',')[20] );
        qDebug() << "\t\thSA " << hSA;
        QDateTime hSJ = QDateTime::fromString( line.split(',')[21] );
        qDebug() << "\t\thSJ " << hSJ;
        QDateTime hDA = QDateTime::fromString( line.split(',')[22] );
        qDebug() << "\t\thDA " << hDA;
        QDateTime hDJ = QDateTime::fromString( line.split(',')[23] );
        qDebug() << "\t\thDJ " << hDJ;
        QList <QDateTime> hora_refeicoes_realizadas_aux;
        hora_refeicoes_realizadas_aux.append( {h5J, h6A, h6J, hSA, hSJ, hDA, hDJ} );
        hora_refeicoes_realizadas.append( hora_refeicoes_realizadas_aux );

        qDebug() << "\t>> " << VolunteerNumber.last() << "\t" << VolunteerName.last() << "\t" << tipo_dieta.last() << "\n\t\t" << refeicoes_previstas.last() << "\n\t\t" << refeicoes_realizadas.last() ;
    }
}


/**
 * @brief MainWindow::compute_everything
 * TODO - Calcular tudo e atualizar campos de apresentacao
 */
void MainWindow::compute_everything()
{

    qDebug() << "\t>> Contagem de refeicoes";

    // Contagem de refeicoes diarias
    cont_ref_previstas_p_dia.clear();
    cont_ref_previstas_p_dia.append(0);    // 5J
    cont_ref_previstas_p_dia.append(0);    // 6A
    cont_ref_previstas_p_dia.append(0);    // 6J
    cont_ref_previstas_p_dia.append(0);    // SA
    cont_ref_previstas_p_dia.append(0);    // SJ
    cont_ref_previstas_p_dia.append(0);    // DA
    cont_ref_previstas_p_dia.append(0);    // DJ

    cont_ref_realizadas_p_dia.clear();
    cont_ref_realizadas_p_dia.append(0);    // 5J
    cont_ref_realizadas_p_dia.append(0);    // 6A
    cont_ref_realizadas_p_dia.append(0);    // 6J
    cont_ref_realizadas_p_dia.append(0);    // SA
    cont_ref_realizadas_p_dia.append(0);    // SJ
    cont_ref_realizadas_p_dia.append(0);    // DA
    cont_ref_realizadas_p_dia.append(0);    // DJ

    // percorre todos os voluntarios
    for ( int v = 0; v < VolunteerNumber.length(); v++ ) {
        // percorre todas as refeicoes
        for ( int r = 0; r < 7; r++ ) {
            // contagem de refeicoes previstas
            if ( refeicoes_previstas[v][r] != 0 ) {
                cont_ref_previstas_p_dia[r]++;
            }

            // Contagem de refeicoes realizadas
            if ( refeicoes_realizadas[v][r] != 0 ) {
                cont_ref_realizadas_p_dia[r]++;
            }
        }
    }

    for ( int r = 0; r < 7; r++ ) {
        qDebug() << "\t\t>> R[" << r << "] previstas = " << cont_ref_previstas_p_dia[r];
        qDebug() << "\t\t>> R[" << r << "] realizadas = " << cont_ref_realizadas_p_dia[r];
    }
}


/**
 * @brief MainWindow::save_data
 * TODO - Gravar dados
 */
void MainWindow::save_data()
{
    QFile file( localizacao_ficheiro );

    if (!file.open(QIODevice::ReadWrite)) {
        qDebug() << file.errorString();
        return;
    }

    file.resize(0);

    QTextStream stream(&file);

    // header do ficheiro
    stream << header_ficheiro;

    // Dados dos voluntarios
    for ( int v = 0; v < VolunteerNumber.length(); v++ ) {

        stream <<
            VolunteerNumber[v] << "," <<
            VolunteerName[v] << "," <<
            tipo_dieta[v] << "," <<
            QString::number( refeicoes_previstas[v][0] ) << "," <<
            QString::number( refeicoes_previstas[v][1] ) << "," <<
            QString::number( refeicoes_previstas[v][2] ) << "," <<
            QString::number( refeicoes_previstas[v][3] ) << "," <<
            QString::number( refeicoes_previstas[v][4] ) << "," <<
            QString::number( refeicoes_previstas[v][5] ) << "," <<
            QString::number( refeicoes_previstas[v][6] ) << "," <<

            QString::number( refeicoes_realizadas[v][0] ) << "," <<
            QString::number( refeicoes_realizadas[v][1] ) << "," <<
            QString::number( refeicoes_realizadas[v][2] ) << "," <<
            QString::number( refeicoes_realizadas[v][3] ) << "," <<
            QString::number( refeicoes_realizadas[v][4] ) << "," <<
            QString::number( refeicoes_realizadas[v][5] ) << "," <<
            QString::number( refeicoes_realizadas[v][6] ) << "," <<

            hora_refeicoes_realizadas[v][0].toString() << "," <<
            hora_refeicoes_realizadas[v][1].toString() << "," <<
            hora_refeicoes_realizadas[v][2].toString() << "," <<
            hora_refeicoes_realizadas[v][3].toString() << "," <<
            hora_refeicoes_realizadas[v][4].toString() << "," <<
            hora_refeicoes_realizadas[v][5].toString() << "," <<
            hora_refeicoes_realizadas[v][6].toString() << "\r\n";
    }

    stream << ",,," <<
        QString::number( cont_ref_previstas_p_dia[0] ) << "," <<
        QString::number( cont_ref_previstas_p_dia[1] ) << "," <<
        QString::number( cont_ref_previstas_p_dia[2] ) << "," <<
        QString::number( cont_ref_previstas_p_dia[3] ) << "," <<
        QString::number( cont_ref_previstas_p_dia[4] ) << "," <<
        QString::number( cont_ref_previstas_p_dia[5] ) << "," <<
        QString::number( cont_ref_previstas_p_dia[6] ) << "," <<

        QString::number( cont_ref_realizadas_p_dia[0] ) << "," <<
        QString::number( cont_ref_realizadas_p_dia[1] ) << "," <<
        QString::number( cont_ref_realizadas_p_dia[2] ) << "," <<
        QString::number( cont_ref_realizadas_p_dia[3] ) << "," <<
        QString::number( cont_ref_realizadas_p_dia[4] ) << "," <<
        QString::number( cont_ref_realizadas_p_dia[5] ) << "," <<
        QString::number( cont_ref_realizadas_p_dia[6] ) << "\r\n";

    file.close();
}




void MainWindow::on_rB_5J_clicked()
{
    ui->lE_ReadBarCode->setFocus();
}


void MainWindow::on_rB_6A_clicked()
{
    ui->lE_ReadBarCode->setFocus();
}


void MainWindow::on_rB_6J_clicked()
{
    ui->lE_ReadBarCode->setFocus();
}


void MainWindow::on_rB_SA_clicked()
{
    ui->lE_ReadBarCode->setFocus();
}


void MainWindow::on_rB_SJ_clicked()
{
    ui->lE_ReadBarCode->setFocus();
}


void MainWindow::on_rB_DA_clicked()
{
    ui->lE_ReadBarCode->setFocus();
}


void MainWindow::on_rB_DJ_clicked()
{
    ui->lE_ReadBarCode->setFocus();
}


void MainWindow::on_actionSobre_triggered()
{
    QMessageBox::about(this, "FMV Manager 2023",
                                   "FMV Manager 2023\nJoão Freitas" );
}

