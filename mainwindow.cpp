#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QFileDialog>
#include <QPrinter>
#include <QPixmap>

QSound *timerSound;
QTimer *timer;


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow){

    ui->setupUi(this);
    QSqlDatabase db;
    db=QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setPort(3306);
    db.setDatabaseName("estacionamiento");
    db.setUserName("root");
    db.setPassword("");
    if(!db.open()){ qDebug()<<"Error "; }
    else{ qDebug()<<"Conectada "; }

    const QIcon user("://Imagenes/user.png");
    const QIcon contra("://Imagenes/contra.png");
    QIcon piso("://Imagenes/piso1.png");
    QIcon piso1("://Imagenes/piso2.png");
    QIcon piso2("://Imagenes/piso3.png");
    ui->Pass->addAction(contra, QLineEdit::LeadingPosition);
    ui->User->addAction(user, QLineEdit::LeadingPosition);
    ui->tabWidget->setTabIcon(0, piso);
    ui->tabWidget->setTabIcon(1, piso1);
    ui->tabWidget->setTabIcon(2, piso2);
    Cajones();

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::Tiempo);
    timer->start(1000);
    timerSound = new QSound(QString("://Imagenes/sound1.wav"), this);

    f= -1;

    ui->img_oferta->setStyleSheet("border-image: url(:/Imagenes/normal.jpg);");


}

MainWindow::~MainWindow(){
    delete ui;
}

/*----------------------------------INTERFAZ DE INICIO DE SESION------------------------------------------*/

void MainWindow::on_IniciarSesion_clicked(){
    QSqlQuery sql;
    QString clave;
    int contador = 0;
    username = ui ->User -> text();
    password = ui ->Pass -> text();

    qDebug()<< username;
    qDebug()<< password;


    iniciar="select contrasena from Usuario where usuario='" +username+"'";
    sql.exec(iniciar);
    while (sql.next()) {
        contador++;
        clave = sql.value(0).toString();
    }

    if(ui->User->text()=="Admin" && ui->Pass->text()=="admin" ){
            QMessageBox::about(this, "Bienvenido", "Bienvenido Administrador");
            ui->stackedWidget->setCurrentIndex(12);
            qDebug()<<"Valor de in: "+in ;
            ui ->User ->clear();
            ui ->Pass->clear();
            return;
     }

    if(contador == 0){
        QMessageBox::about(this, "Error", "El usuario ingresado no ha sido registrado");
        ui ->User ->clear();
        ui ->Pass->clear();
    }

    else if (contador == 1) {
        if(password == clave){
            QMessageBox::about(this, "Bienvenido", "Bienvenido Cliente");
            ui->stackedWidget->setCurrentIndex(4);
            username = ui ->User-> text();
            ui ->User ->clear();
            ui ->Pass->clear();
            qDebug()<<"Valor de in: "+in ;

           }
        else {
           QMessageBox::about(this, "Error", "Contraseña incorrecta");
        }
    }
}
void MainWindow::on_registro_clicked(){
    ui->stackedWidget->setCurrentIndex(1);
}
void MainWindow::on_OContra_clicked(){
    ui->stackedWidget->setCurrentIndex(3);
}
/******************************************************************************************************************/


/*------------------------------------INTERFAZ DE REGISTRO DE DATOS PERSONALES---------------------------------------*/
void MainWindow::on_regresar_clicked(){
    ui->stackedWidget->setCurrentIndex(0);
}
void MainWindow::on_Siguiente_clicked(){
    QString correo, contra, contra2;
    QRegExp regex ("\\b[A-Z0-9._%+-]+@[A-Z0-9.-]+\\.[A-Z]{2,4}\\b");
    correo=ui->CorreoReg->text();
    contra=ui->ContraReg->text();
    contra2=ui->ContraReg_2->text();
    regex.setCaseSensitivity(Qt::CaseInsensitive);
    regex.setPatternSyntax(QRegExp::RegExp);
    bool regMat = regex.exactMatch(correo);
    if(regMat == false){
        QMessageBox::information(this, "Error", "El formato del correo no es correcto");
    }
    else{
        if(contra == contra2){
            ui->stackedWidget->setCurrentIndex(2);
        }
        else{
            QMessageBox::information(this, "Error", "Las contraseñas no son iguales");
        }

    }
}
/************************************************************************************************************************/

/*------------------------------------------INTERFAZ DE REGISTRO DE TARJETA----------------------------------------------*/

void MainWindow::on_regresar_2_clicked(){
    ui->stackedWidget->setCurrentIndex(1);
}
void MainWindow::on_Crear_clicked(){
    QString nombre, apellidom, apellidop, usuario, email, contra, tel,resp,num,mes, agno,cvv, banco, checkExist, existente, id, id2;
    QSqlQuery sql, sql2, sql3, sql4, sql5, query;
    nombre=ui->NombreReg->text();
    apellidom=ui->ApellidoReg->text();
    apellidop=ui->ApellidoReg_2->text();
    usuario=ui->UsuarioReg->text();
    tel=ui->TelReg->text();
    email=ui->CorreoReg->text();
    contra=ui->ContraReg->text();
    resp=ui->dueno->text();
    num=ui->num_tarjeta->text();
    mes=ui->mes->text();
    agno=ui->agno->text();
    cvv=ui->cvv->text();
    banco = ui->comboBox->currentText();

    existente = "select * from usuario where usuario = '"+ usuario +"'";
    if(nombre.isEmpty()||apellidom.isEmpty()||apellidop.isEmpty()||email.isEmpty()||contra.isEmpty()||usuario.isEmpty()|| tel.isEmpty()){
        QMessageBox::about(this, "Error", "Campos Vacios");
    }
    else{
        if(sql.exec(existente)){
            if(sql.next() == true){
                QMessageBox::information(this, "Error al registrar", "Nombre de usuario ya existe");
            }
            else{
                if (sql2.exec("insert into Usuario(usuario,  contrasena) values ('" + usuario+ "','" +contra+ "');")){

                    id= "select MAX(idUsuario) from Usuario";
                    QSqlQuery insert2;
                    insert2.prepare(id);
                    insert2.exec();
                    while ( insert2.next()){
                        id=insert2.value(0).toString();
                    }

                    qDebug()<<id;

                    if(sql3.exec("insert into Cliente(Nombre, Apellido_p, Apellido_m, correo, telefono, idUsuario) values('" +nombre+ "','" +apellidop+ "','" +apellidom+ "','"  +email+ "','" +tel+ "','" +id+ "');")){
                        if(sql4.exec("insert into Tarjeta(num, tipo, nombre, agno, mes, cvv) values('" +num+ "','" + banco + "','" +resp+ "','"  +agno+ "','" +mes+ "','" +cvv+ "');")){
                            id2= "select MAX(idTarjeta) from Tarjeta";
                            QSqlQuery insert;
                            insert.prepare(id2);
                            insert.exec();
                            while ( insert.next()){
                                id2=insert.value(0).toString();
                            }

                            if (sql5.exec("update Cliente set idTarjeta = '"+id2+"' where idCliente = '"+id+"';")){

                                QMessageBox::about(this, "Registro", "Registro Exitoso");
                                ui->stackedWidget->setCurrentIndex(0);
                            }

                            else{
                                QMessageBox::about(this, "Registro", "No ha podido registrarse");
                            }
                        }
                    }
                }
                ui->NombreReg->clear();
                ui->ApellidoReg->clear();
                ui->ApellidoReg_2->clear();
                ui->UsuarioReg->clear();
                ui->TelReg->clear();
                ui->CorreoReg->clear();
                ui->ContraReg->clear();
                ui->dueno->clear();
                ui->num_tarjeta->clear();
                ui->mes->clear();
                ui->agno->clear();
                ui->cvv->clear();
                ui->comboBox->clear();
            }
        }
    }
}
/***********************************************************************************************************************/

/*------------------------------------INTERFAZ OLVIDASTE TU CONTRASEÑA--------------------------------------------------*/
void MainWindow::on_regresar_3_clicked(){
    ui->stackedWidget->setCurrentIndex(0);
}
void MainWindow::on_cambiarC_clicked(){
    QString username=ui->user->text();
        QString password=ui->CPass->text();
        QString password2=ui->CPass2->text();

        QSqlQuery query;


        if((QString::compare(password,password2, Qt::CaseInsensitive))!=0)
        {
            QMessageBox::about(this,"Contraseña", "Las contraseñas no coinciden.");
            return;
        }
        if (query.exec("update Usuario set contrasena = '"+password2+"' where usuario like '"+username+"';")){
            QMessageBox::about(this, "Contraseña", "La contraseña se ha cambiado exitosamente");
            ui->stackedWidget->setCurrentIndex(0);

            ui->User->clear();
            ui->Pass->clear();
        }
}
/************************************************************************************************************************/

/*------------------------------------------INTERFAZ PRINCIPAL----------------------------------------------------------*/
void MainWindow::TablaReservacion(){

    QString obtidusuario = "select * from usuario where usuario = '"+ username +"'";
    QString idReserva;
    int numeroColumnas=ui->tableWidget_1->rowCount();

    QSqlQuery obtidus;
    obtidus.exec(obtidusuario);
    obtidus.next();

    QString obtidusua = obtidus.value("idUsuario").toString();//Obtenemos el id del usuario y lo almacenamos

    qDebug()<< "Id usuario: " << obtidusua;

    QString reserv = "SELECT inicio, fin, ((time_to_sec(timediff(fin,inicio))/3600) * pagoxhora) as precio, liquidado  FROM Reservacion "
                     "INNER JOIN Transaccion ON Reservacion.idReservacion=Transaccion.idReservacion "
                     "WHERE Reservacion.idCliente = '" + obtidusua + "'";

    QList<int> numeros;
    for(int i=1; i<=numeroColumnas; i++){
        numeros.append(i);
        ui->tableWidget_1->removeRow(numeros.value(i));
    }

    QSqlQuery qry;
    qry.exec(reserv);

    while(qry.next()){
        //QString IDT = qry.value("idReservacion").toString();
         //idReserva=qry.value("idReservacion").toString();
        QString init = qry.value("inicio").toString();
        QString fint = qry.value("fin").toString();
        QString prec = "$" + qry.value("precio").toString();
        QString estado= qry.value("liquidado").toString();

        qDebug()<< "Precio: " << prec;

        if((QString::compare(prec,"$10"))==0)
        {estado="Cancelada";}
        else if(estado=="0"){
        estado="----------";
        }else{
         estado="Pagado";
         }

        ui->tableWidget_1->insertRow(ui->tableWidget_1->rowCount());

        int fila = ui->tableWidget_1->rowCount()-1;
        //ui->tableWidget_1->setItem(fila,0,new QTableWidgetItem(IDT));
        ui->tableWidget_1->setItem(fila,0,new QTableWidgetItem(init));
        ui->tableWidget_1->setItem(fila,1,new QTableWidgetItem(fint));
       ui->tableWidget_1->setItem(fila,2,new QTableWidgetItem(prec));
       ui->tableWidget_1->setItem(fila,3,new QTableWidgetItem(estado));

    }


    //cargar tabla pensiones
    QString pensn = "SELECT P.fecha_inicio,P.fecha_fin, T.total_a_pagar, T.liquidado FROM pension as P INNER JOIN transaccion as T "
                    "ON P.idPension=T.idPension WHERE P.idCliente = '" + obtidusua + "'";
    QList<int> numeros2;
    for(int i=1; i<=numeroColumnas; i++){
        numeros2.append(i);
        ui->tableWidget_2->removeRow(numeros2.value(i));
    }

    QSqlQuery qry2;
    QString estadoP;
    qry2.exec(pensn);

    while(qry2.next()){
        QString iniP = qry2.value("fecha_inicio").toString();
        QString finP = qry2.value("fecha_fin").toString();
        QString tPaga = "$" + qry2.value("total_a_Pagar").toString();
        QString tliqui = qry2.value("liquidado").toString();

        if((QString::compare(tliqui,"0"))==0)
        {estadoP="--------";
        }else{
         estadoP="Pagado";
        }


        ui->tableWidget_2->insertRow(ui->tableWidget_2->rowCount());

        int filap = ui->tableWidget_2->rowCount()-1;
        ui->tableWidget_2->setItem(filap,0,new QTableWidgetItem(iniP));
        ui->tableWidget_2->setItem(filap,1,new QTableWidgetItem(finP));
        ui->tableWidget_2->setItem(filap,2,new QTableWidgetItem(tPaga));
        ui->tableWidget_2->setItem(filap,3,new QTableWidgetItem(estadoP));
    }

    ui->stackedWidget->setCurrentIndex(11);
}
void MainWindow::on_estacionamiento_clicked(){
    Cajones();
    QSqlQuery query, query2;
    QMessageBox messageBox;
    messageBox.setText("Eliga el tipo de reservación");
    QPushButton  *Em = messageBox.addButton("Emergencia", QMessageBox::YesRole );
    QPushButton   *Normal = messageBox.addButton("Normal", QMessageBox::AcceptRole);
    QPushButton   *Premium = messageBox.addButton("Premium", QMessageBox::RejectRole);
    Em->setStyleSheet("color: white; background-color: #1a237e; font-size:14px;  border: 1px solid #1a237e;  padding: 2px 4px;");
    Normal->setStyleSheet("color: white; background-color: #1a237e; font-size:14px;  border: 1px solid #1a237e;  padding: 2px 4px;");
    Premium->setStyleSheet("color: white; background-color: #1a237e; font-size:14px;  border: 1px solid #1a237e; padding: 2px 4px;");
    messageBox.exec();

    if(messageBox.clickedButton() == Normal){
        ui->stackedWidget->setCurrentIndex(5);
        ui->tabWidget->setTabEnabled(0,true);
        ui->tabWidget->setTabEnabled(1,true);
        ui->tabWidget->setTabEnabled(2, true);
       Tipo = "Normal";
    }


    if(messageBox.clickedButton() == Premium){
        ui->stackedWidget->setCurrentIndex(5);
        ui->tabWidget->setTabEnabled(0,true);
        ui->tabWidget->setTabEnabled(1,true);
        ui->tabWidget->setTabEnabled(2, true);
        Tipo = "Premium";
    }

    if(messageBox.clickedButton() == Em){
        ui->tabWidget->setTabEnabled(0,true);
        ui->tabWidget->setTabEnabled(1, false);
        ui->tabWidget->setTabEnabled(2, false);
        Tipo = "Emergencia";
    }
    ui->stackedWidget->setCurrentIndex(5);
}
void MainWindow::on_perfil_clicked(){
    Datos();
    DatosFoto();
    DatosDireccion();
    ui->stackedWidget->setCurrentIndex(9);
}
void MainWindow::on_reservacion_clicked(){
    ui->stackedWidget->setCurrentIndex(10);
    Foto();
    Ofer();
    DatosFoto();
    TablaReservacion();
    DatosTarjeta();
    Piso();
    Tiempo();
}
void MainWindow::on_salir_clicked(){
    ui->stackedWidget->setCurrentIndex(0);
}
/****************************************************************************************************************************/


/*--------------------------------------- INTERFAZ DEL ESTACIONAMIENTO GENERAL --------------------------------------------*/

void MainWindow::on_estacionamiento_2_clicked(){
    Cajones();
    QString tipo;
    QMessageBox messageBox;
    QPushButton  *Em = messageBox.addButton("Emergencia", QMessageBox::YesRole );
    QPushButton   *Normal = messageBox.addButton("Normal", QMessageBox::AcceptRole);
    QPushButton   *Premium = messageBox.addButton("Premium", QMessageBox::RejectRole);
    Em->setStyleSheet("color: white; background-color: #1a237e; font-size:14px;  border: 1px solid #1a237e;  padding: 2px 4px;");
    Normal->setStyleSheet("color: white; background-color: #1a237e; font-size:14px;  border: 1px solid #1a237e;  padding: 2px 4px;");
    Premium->setStyleSheet("color: white; background-color: #1a237e; font-size:14px;  border: 1px solid #1a237e; padding: 2px 4px;");
    messageBox.exec();

    if(messageBox.clickedButton() == Normal){
        ui->stackedWidget->setCurrentIndex(5);
        ui->tabWidget->setTabEnabled(0,true);
        ui->tabWidget->setTabEnabled(1,true);
        ui->tabWidget->setTabEnabled(2, true);
        Tipo = "Normal";
    }

    if(messageBox.clickedButton() == Premium){
        ui->stackedWidget->setCurrentIndex(5);
        ui->tabWidget->setTabEnabled(0,true);
        ui->tabWidget->setTabEnabled(1,true);
        ui->tabWidget->setTabEnabled(2, true);
        Tipo = "Premium";
    }

    if(messageBox.clickedButton() == Em){
        ui->tabWidget->setTabEnabled(0,true);
        ui->tabWidget->setTabEnabled(1, false);
        ui->tabWidget->setTabEnabled(2, false);
        Tipo = "Emergencia";
    }
    ui->stackedWidget->setCurrentIndex(5);
}
void MainWindow::on_perfil_2_clicked(){
    Datos();
    DatosFoto();
    DatosDireccion();
    ui->stackedWidget->setCurrentIndex(9);
}
void MainWindow::on_reservacion_2_clicked(){
    ui->stackedWidget->setCurrentIndex(10);
    Foto();
    Ofer();
    TablaReservacion();
    DatosTarjeta();
    Piso();
    Tiempo();
}
void MainWindow::on_salir_2_clicked(){
    ui->stackedWidget->setCurrentIndex(0);
}
/*******************************************************************************************************************************/

void MainWindow::Cajones(){
    QString id, id2, id3, cajon, cajon2, cajon3;
    id= "SELECT idCajon FROM cajon where lleno='1' and idPiso = '1';";    
    id2 = "SELECT idCajon FROM cajon where lleno='1' and idPiso = '2';";
    id3= "SELECT idCajon FROM cajon where lleno='1' and idPiso = '3';";
    QSqlQuery sql, sql2, sql3;

    if(sql.exec(id)){
        while(sql.next()){
            cajon= sql.value(0).toString();
            if(cajon=="1"){

                /*obtener si hay reservacion*/
                QString sifechentre = "SELECT inicio,fin,estado FROM reservacion WHERE idCajon = " + cajon;

                QSqlQuery sifechen;
                sifechen.exec(sifechentre);

                while(sifechen.next()){
                    QString iniR = sifechen.value("inicio").toString();
                    QString finiR = sifechen.value("fin").toString();
                    int estate = sifechen.value("estado").toInt();

                    qDebug() << "Estado: " << estate;

                    QDateTime dateIni = QLocale(QLocale::English).toDateTime(iniR,"yyyy-MM-ddThh:mm:ss.zzz");
                    QDateTime dateFin = QLocale(QLocale::English).toDateTime(finiR,"yyyy-MM-ddThh:mm:ss.zzz");
                    QDateTime actual = QDateTime::currentDateTime();

                    bool withinRange = (actual >= dateIni && actual <= dateFin);

                    if(estate==0){
                        if(withinRange==true){
                            ui->numero1->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
                        }
                    }
                }

                /*obtener si hay pension*/
                QString sifechentre2 = "SELECT fecha_inicio,fecha_fin FROM pension WHERE idCajon = " + cajon;

                QSqlQuery sifechen2;
                sifechen2.exec(sifechentre2);

                while(sifechen2.next()){
                    QString iniR2 = sifechen2.value("fecha_inicio").toString();
                    QString finiR2 = sifechen2.value("fecha_fin").toString();

                    QDate dateIni2 = QDate::fromString(iniR2,"yyyy-MM-dd");
                    QDate dateFin2 = QDate::fromString(finiR2,"yyyy-MM-dd");
                    QDate actual2 = QDate::currentDate();

                    bool withinRange2 = (actual2 >= dateIni2 && actual2 <= dateFin2);

                    if(withinRange2==true){
                        ui->numero1->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
                    }
                }

            }
            if(cajon=="2"){

                /*obtener si hay reservacion*/
                QString sifechentre = "SELECT inicio,fin,estado FROM reservacion WHERE idCajon = " + cajon;

                QSqlQuery sifechen;
                sifechen.exec(sifechentre);

                while(sifechen.next()){
                    QString iniR = sifechen.value("inicio").toString();
                    QString finiR = sifechen.value("fin").toString();
                    int estate = sifechen.value("estado").toInt();

                    qDebug() << "Estado: " << estate;

                    QDateTime dateIni = QLocale(QLocale::English).toDateTime(iniR,"yyyy-MM-ddThh:mm:ss.zzz");
                    QDateTime dateFin = QLocale(QLocale::English).toDateTime(finiR,"yyyy-MM-ddThh:mm:ss.zzz");
                    QDateTime actual = QDateTime::currentDateTime();

                    bool withinRange = (actual >= dateIni && actual <= dateFin);

                    if(estate==0){
                        if(withinRange==true){
                            ui->numero2->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
                        }
                    }
                }

                /*obtener si hay pension*/
                QString sifechentre2 = "SELECT fecha_inicio,fecha_fin FROM pension WHERE idCajon = " + cajon;

                QSqlQuery sifechen2;
                sifechen2.exec(sifechentre2);

                while(sifechen2.next()){
                    QString iniR2 = sifechen2.value("fecha_inicio").toString();
                    QString finiR2 = sifechen2.value("fecha_fin").toString();

                    QDate dateIni2 = QDate::fromString(iniR2,"yyyy-MM-dd");
                    QDate dateFin2 = QDate::fromString(finiR2,"yyyy-MM-dd");
                    QDate actual2 = QDate::currentDate();

                    bool withinRange2 = (actual2 >= dateIni2 && actual2 <= dateFin2);

                    if(withinRange2==true){
                        ui->numero2->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
                    }
                }

            }
            if(cajon=="3"){

                /*obtener si hay reservacion*/
                QString sifechentre = "SELECT inicio,fin,estado FROM reservacion WHERE idCajon = " + cajon;

                QSqlQuery sifechen;
                sifechen.exec(sifechentre);

                while(sifechen.next()){
                    QString iniR = sifechen.value("inicio").toString();
                    QString finiR = sifechen.value("fin").toString();
                    int estate = sifechen.value("estado").toInt();

                    qDebug() << "Estado: " << estate;

                    QDateTime dateIni = QLocale(QLocale::English).toDateTime(iniR,"yyyy-MM-ddThh:mm:ss.zzz");
                    QDateTime dateFin = QLocale(QLocale::English).toDateTime(finiR,"yyyy-MM-ddThh:mm:ss.zzz");
                    QDateTime actual = QDateTime::currentDateTime();

                    bool withinRange = (actual >= dateIni && actual <= dateFin);

                    if(estate==0){
                        if(withinRange==true){
                            ui->numero3->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
                        }
                    }
                }

                /*obtener si hay pension*/
                QString sifechentre2 = "SELECT fecha_inicio,fecha_fin FROM pension WHERE idCajon = " + cajon;

                QSqlQuery sifechen2;
                sifechen2.exec(sifechentre2);

                while(sifechen2.next()){
                    QString iniR2 = sifechen2.value("fecha_inicio").toString();
                    QString finiR2 = sifechen2.value("fecha_fin").toString();

                    QDate dateIni2 = QDate::fromString(iniR2,"yyyy-MM-dd");
                    QDate dateFin2 = QDate::fromString(finiR2,"yyyy-MM-dd");
                    QDate actual2 = QDate::currentDate();

                    bool withinRange2 = (actual2 >= dateIni2 && actual2 <= dateFin2);

                    if(withinRange2==true){
                        ui->numero3->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
                    }
                }

            }
            if(cajon=="4"){

                /*obtener si hay reservacion*/
                QString sifechentre = "SELECT inicio,fin,estado FROM reservacion WHERE idCajon = " + cajon;

                QSqlQuery sifechen;
                sifechen.exec(sifechentre);

                while(sifechen.next()){
                    QString iniR = sifechen.value("inicio").toString();
                    QString finiR = sifechen.value("fin").toString();
                    int estate = sifechen.value("estado").toInt();

                    qDebug() << "Estado: " << estate;

                    QDateTime dateIni = QLocale(QLocale::English).toDateTime(iniR,"yyyy-MM-ddThh:mm:ss.zzz");
                    QDateTime dateFin = QLocale(QLocale::English).toDateTime(finiR,"yyyy-MM-ddThh:mm:ss.zzz");
                    QDateTime actual = QDateTime::currentDateTime();

                    bool withinRange = (actual >= dateIni && actual <= dateFin);

                    if(estate==0){
                        if(withinRange==true){
                            ui->numero4->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
                        }
                    }
                }

                /*obtener si hay pension*/
                QString sifechentre2 = "SELECT fecha_inicio,fecha_fin FROM pension WHERE idCajon = " + cajon;

                QSqlQuery sifechen2;
                sifechen2.exec(sifechentre2);

                while(sifechen2.next()){
                    QString iniR2 = sifechen2.value("fecha_inicio").toString();
                    QString finiR2 = sifechen2.value("fecha_fin").toString();

                    QDate dateIni2 = QDate::fromString(iniR2,"yyyy-MM-dd");
                    QDate dateFin2 = QDate::fromString(finiR2,"yyyy-MM-dd");
                    QDate actual2 = QDate::currentDate();

                    bool withinRange2 = (actual2 >= dateIni2 && actual2 <= dateFin2);

                    if(withinRange2==true){
                        ui->numero4->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
                    }
                }

            }
            if(cajon=="5"){

                /*obtener si hay reservacion*/
                QString sifechentre = "SELECT inicio,fin,estado FROM reservacion WHERE idCajon = " + cajon;

                QSqlQuery sifechen;
                sifechen.exec(sifechentre);

                while(sifechen.next()){
                    QString iniR = sifechen.value("inicio").toString();
                    QString finiR = sifechen.value("fin").toString();
                    int estate = sifechen.value("estado").toInt();

                    qDebug() << "Estado: " << estate;

                    QDateTime dateIni = QLocale(QLocale::English).toDateTime(iniR,"yyyy-MM-ddThh:mm:ss.zzz");
                    QDateTime dateFin = QLocale(QLocale::English).toDateTime(finiR,"yyyy-MM-ddThh:mm:ss.zzz");
                    QDateTime actual = QDateTime::currentDateTime();

                    bool withinRange = (actual >= dateIni && actual <= dateFin);

                    if(estate==0){
                        if(withinRange==true){
                            ui->numero5->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
                        }
                    }
                }

                /*obtener si hay pension*/
                QString sifechentre2 = "SELECT fecha_inicio,fecha_fin FROM pension WHERE idCajon = " + cajon;

                QSqlQuery sifechen2;
                sifechen2.exec(sifechentre2);

                while(sifechen2.next()){
                    QString iniR2 = sifechen2.value("fecha_inicio").toString();
                    QString finiR2 = sifechen2.value("fecha_fin").toString();

                    QDate dateIni2 = QDate::fromString(iniR2,"yyyy-MM-dd");
                    QDate dateFin2 = QDate::fromString(finiR2,"yyyy-MM-dd");
                    QDate actual2 = QDate::currentDate();

                    bool withinRange2 = (actual2 >= dateIni2 && actual2 <= dateFin2);

                    if(withinRange2==true){
                        ui->numero5->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
                    }
                }

            }
            if(cajon=="6"){

                /*obtener si hay reservacion*/
                QString sifechentre = "SELECT inicio,fin,estado FROM reservacion WHERE idCajon = " + cajon;

                QSqlQuery sifechen;
                sifechen.exec(sifechentre);

                while(sifechen.next()){
                    QString iniR = sifechen.value("inicio").toString();
                    QString finiR = sifechen.value("fin").toString();
                    int estate = sifechen.value("estado").toInt();

                    qDebug() << "Estado: " << estate;

                    QDateTime dateIni = QLocale(QLocale::English).toDateTime(iniR,"yyyy-MM-ddThh:mm:ss.zzz");
                    QDateTime dateFin = QLocale(QLocale::English).toDateTime(finiR,"yyyy-MM-ddThh:mm:ss.zzz");
                    QDateTime actual = QDateTime::currentDateTime();

                    bool withinRange = (actual >= dateIni && actual <= dateFin);

                    if(estate==0){
                        if(withinRange==true){
                            ui->numero6->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
                        }
                    }
                }

                /*obtener si hay pension*/
                QString sifechentre2 = "SELECT fecha_inicio,fecha_fin FROM pension WHERE idCajon = " + cajon;

                QSqlQuery sifechen2;
                sifechen2.exec(sifechentre2);

                while(sifechen2.next()){
                    QString iniR2 = sifechen2.value("fecha_inicio").toString();
                    QString finiR2 = sifechen2.value("fecha_fin").toString();

                    QDate dateIni2 = QDate::fromString(iniR2,"yyyy-MM-dd");
                    QDate dateFin2 = QDate::fromString(finiR2,"yyyy-MM-dd");
                    QDate actual2 = QDate::currentDate();

                    bool withinRange2 = (actual2 >= dateIni2 && actual2 <= dateFin2);

                    if(withinRange2==true){
                        ui->numero6->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
                    }
                }

            }
            if(cajon=="7"){

                /*obtener si hay reservacion*/
                QString sifechentre = "SELECT inicio,fin,estado FROM reservacion WHERE idCajon = " + cajon;

                QSqlQuery sifechen;
                sifechen.exec(sifechentre);

                while(sifechen.next()){
                    QString iniR = sifechen.value("inicio").toString();
                    QString finiR = sifechen.value("fin").toString();
                    int estate = sifechen.value("estado").toInt();

                    qDebug() << "Estado: " << estate;

                    QDateTime dateIni = QLocale(QLocale::English).toDateTime(iniR,"yyyy-MM-ddThh:mm:ss.zzz");
                    QDateTime dateFin = QLocale(QLocale::English).toDateTime(finiR,"yyyy-MM-ddThh:mm:ss.zzz");
                    QDateTime actual = QDateTime::currentDateTime();

                    bool withinRange = (actual >= dateIni && actual <= dateFin);

                    if(estate==0){
                        if(withinRange==true){
                            ui->numero7->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
                        }
                    }
                }

                /*obtener si hay pension*/
                QString sifechentre2 = "SELECT fecha_inicio,fecha_fin FROM pension WHERE idCajon = " + cajon;

                QSqlQuery sifechen2;
                sifechen2.exec(sifechentre2);

                while(sifechen2.next()){
                    QString iniR2 = sifechen2.value("fecha_inicio").toString();
                    QString finiR2 = sifechen2.value("fecha_fin").toString();

                    QDate dateIni2 = QDate::fromString(iniR2,"yyyy-MM-dd");
                    QDate dateFin2 = QDate::fromString(finiR2,"yyyy-MM-dd");
                    QDate actual2 = QDate::currentDate();

                    bool withinRange2 = (actual2 >= dateIni2 && actual2 <= dateFin2);

                    if(withinRange2==true){
                        ui->numero7->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
                    }
                }

            }
            if(cajon=="8"){

                /*obtener si hay reservacion*/
                QString sifechentre = "SELECT inicio,fin,estado FROM reservacion WHERE idCajon = " + cajon;

                QSqlQuery sifechen;
                sifechen.exec(sifechentre);

                while(sifechen.next()){
                    QString iniR = sifechen.value("inicio").toString();
                    QString finiR = sifechen.value("fin").toString();
                    int estate = sifechen.value("estado").toInt();

                    qDebug() << "Estado: " << estate;

                    QDateTime dateIni = QLocale(QLocale::English).toDateTime(iniR,"yyyy-MM-ddThh:mm:ss.zzz");
                    QDateTime dateFin = QLocale(QLocale::English).toDateTime(finiR,"yyyy-MM-ddThh:mm:ss.zzz");
                    QDateTime actual = QDateTime::currentDateTime();

                    bool withinRange = (actual >= dateIni && actual <= dateFin);

                    if(estate==0){
                        if(withinRange==true){
                            ui->numero8->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
                        }
                    }
                }

                /*obtener si hay pension*/
                QString sifechentre2 = "SELECT fecha_inicio,fecha_fin FROM pension WHERE idCajon = " + cajon;

                QSqlQuery sifechen2;
                sifechen2.exec(sifechentre2);

                while(sifechen2.next()){
                    QString iniR2 = sifechen2.value("fecha_inicio").toString();
                    QString finiR2 = sifechen2.value("fecha_fin").toString();

                    QDate dateIni2 = QDate::fromString(iniR2,"yyyy-MM-dd");
                    QDate dateFin2 = QDate::fromString(finiR2,"yyyy-MM-dd");
                    QDate actual2 = QDate::currentDate();

                    bool withinRange2 = (actual2 >= dateIni2 && actual2 <= dateFin2);

                    if(withinRange2==true){
                        ui->numero8->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
                    }
                }

            }
            if(cajon=="9"){

                /*obtener si hay reservacion*/
                QString sifechentre = "SELECT inicio,fin,estado FROM reservacion WHERE idCajon = " + cajon;

                QSqlQuery sifechen;
                sifechen.exec(sifechentre);

                while(sifechen.next()){
                    QString iniR = sifechen.value("inicio").toString();
                    QString finiR = sifechen.value("fin").toString();
                    int estate = sifechen.value("estado").toInt();

                    qDebug() << "Estado: " << estate;

                    QDateTime dateIni = QLocale(QLocale::English).toDateTime(iniR,"yyyy-MM-ddThh:mm:ss.zzz");
                    QDateTime dateFin = QLocale(QLocale::English).toDateTime(finiR,"yyyy-MM-ddThh:mm:ss.zzz");
                    QDateTime actual = QDateTime::currentDateTime();

                    bool withinRange = (actual >= dateIni && actual <= dateFin);

                    if(estate==0){
                        if(withinRange==true){
                            ui->numero9->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
                        }
                    }
                }

                /*obtener si hay pension*/
                QString sifechentre2 = "SELECT fecha_inicio,fecha_fin FROM pension WHERE idCajon = " + cajon;

                QSqlQuery sifechen2;
                sifechen2.exec(sifechentre2);

                while(sifechen2.next()){
                    QString iniR2 = sifechen2.value("fecha_inicio").toString();
                    QString finiR2 = sifechen2.value("fecha_fin").toString();

                    QDate dateIni2 = QDate::fromString(iniR2,"yyyy-MM-dd");
                    QDate dateFin2 = QDate::fromString(finiR2,"yyyy-MM-dd");
                    QDate actual2 = QDate::currentDate();

                    bool withinRange2 = (actual2 >= dateIni2 && actual2 <= dateFin2);

                    if(withinRange2==true){
                        ui->numero9->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
                    }
                }

            }
            if(cajon=="10"){

                /*obtener si hay reservacion*/
                QString sifechentre = "SELECT inicio,fin,estado FROM reservacion WHERE idCajon = " + cajon;

                QSqlQuery sifechen;
                sifechen.exec(sifechentre);

                while(sifechen.next()){
                    QString iniR = sifechen.value("inicio").toString();
                    QString finiR = sifechen.value("fin").toString();
                    int estate = sifechen.value("estado").toInt();

                    qDebug() << "Estado: " << estate;

                    QDateTime dateIni = QLocale(QLocale::English).toDateTime(iniR,"yyyy-MM-ddThh:mm:ss.zzz");
                    QDateTime dateFin = QLocale(QLocale::English).toDateTime(finiR,"yyyy-MM-ddThh:mm:ss.zzz");
                    QDateTime actual = QDateTime::currentDateTime();

                    bool withinRange = (actual >= dateIni && actual <= dateFin);

                    if(estate==0){
                        if(withinRange==true){
                            ui->numero10->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
                        }
                    }
                }

                /*obtener si hay pension*/
                QString sifechentre2 = "SELECT fecha_inicio,fecha_fin FROM pension WHERE idCajon = " + cajon;

                QSqlQuery sifechen2;
                sifechen2.exec(sifechentre2);

                while(sifechen2.next()){
                    QString iniR2 = sifechen2.value("fecha_inicio").toString();
                    QString finiR2 = sifechen2.value("fecha_fin").toString();

                    QDate dateIni2 = QDate::fromString(iniR2,"yyyy-MM-dd");
                    QDate dateFin2 = QDate::fromString(finiR2,"yyyy-MM-dd");
                    QDate actual2 = QDate::currentDate();

                    bool withinRange2 = (actual2 >= dateIni2 && actual2 <= dateFin2);

                    if(withinRange2==true){
                        ui->numero10->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
                    }
                }

            }
        }
    }


    if(sql2.exec(id2)){
        while(sql2.next()){
            cajon2= sql2.value(0).toString();
            qDebug()<<"Numero de cajones"+cajon2;


            if(cajon2=="11"){

                /*obtener si hay reservacion*/
                QString sifechentre = "SELECT inicio,fin,estado FROM reservacion WHERE idCajon = " + cajon;

                QSqlQuery sifechen;
                sifechen.exec(sifechentre);

                while(sifechen.next()){
                    QString iniR = sifechen.value("inicio").toString();
                    QString finiR = sifechen.value("fin").toString();
                    int estate = sifechen.value("estado").toInt();

                    qDebug() << "Estado: " << estate;

                    QDateTime dateIni = QLocale(QLocale::English).toDateTime(iniR,"yyyy-MM-ddThh:mm:ss.zzz");
                    QDateTime dateFin = QLocale(QLocale::English).toDateTime(finiR,"yyyy-MM-ddThh:mm:ss.zzz");
                    QDateTime actual = QDateTime::currentDateTime();

                    bool withinRange = (actual >= dateIni && actual <= dateFin);

                    if(estate==0){
                        if(withinRange==true){
                            ui->Piso2num1->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
                        }
                    }
                }

                /*obtener si hay pension*/
                QString sifechentre2 = "SELECT fecha_inicio,fecha_fin FROM pension WHERE idCajon = " + cajon;

                QSqlQuery sifechen2;
                sifechen2.exec(sifechentre2);

                while(sifechen2.next()){
                    QString iniR2 = sifechen2.value("fecha_inicio").toString();
                    QString finiR2 = sifechen2.value("fecha_fin").toString();

                    QDate dateIni2 = QDate::fromString(iniR2,"yyyy-MM-dd");
                    QDate dateFin2 = QDate::fromString(finiR2,"yyyy-MM-dd");
                    QDate actual2 = QDate::currentDate();

                    bool withinRange2 = (actual2 >= dateIni2 && actual2 <= dateFin2);

                    if(withinRange2==true){
                        ui->Piso2num1->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
                    }
                }

            }
            if(cajon2=="12"){

                /*obtener si hay reservacion*/
                QString sifechentre = "SELECT inicio,fin,estado FROM reservacion WHERE idCajon = " + cajon;

                QSqlQuery sifechen;
                sifechen.exec(sifechentre);

                while(sifechen.next()){
                    QString iniR = sifechen.value("inicio").toString();
                    QString finiR = sifechen.value("fin").toString();
                    int estate = sifechen.value("estado").toInt();

                    qDebug() << "Estado: " << estate;

                    QDateTime dateIni = QLocale(QLocale::English).toDateTime(iniR,"yyyy-MM-ddThh:mm:ss.zzz");
                    QDateTime dateFin = QLocale(QLocale::English).toDateTime(finiR,"yyyy-MM-ddThh:mm:ss.zzz");
                    QDateTime actual = QDateTime::currentDateTime();

                    bool withinRange = (actual >= dateIni && actual <= dateFin);

                    if(estate==0){
                        if(withinRange==true){
                            ui->Piso2num2->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
                        }
                    }
                }

                /*obtener si hay pension*/
                QString sifechentre2 = "SELECT fecha_inicio,fecha_fin FROM pension WHERE idCajon = " + cajon;

                QSqlQuery sifechen2;
                sifechen2.exec(sifechentre2);

                while(sifechen2.next()){
                    QString iniR2 = sifechen2.value("fecha_inicio").toString();
                    QString finiR2 = sifechen2.value("fecha_fin").toString();

                    QDate dateIni2 = QDate::fromString(iniR2,"yyyy-MM-dd");
                    QDate dateFin2 = QDate::fromString(finiR2,"yyyy-MM-dd");
                    QDate actual2 = QDate::currentDate();

                    bool withinRange2 = (actual2 >= dateIni2 && actual2 <= dateFin2);

                    if(withinRange2==true){
                        ui->Piso2num2->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
                    }
                }

            }
            if(cajon2=="13"){

                /*obtener si hay reservacion*/
                QString sifechentre = "SELECT inicio,fin,estado FROM reservacion WHERE idCajon = " + cajon;

                QSqlQuery sifechen;
                sifechen.exec(sifechentre);

                while(sifechen.next()){
                    QString iniR = sifechen.value("inicio").toString();
                    QString finiR = sifechen.value("fin").toString();
                    int estate = sifechen.value("estado").toInt();

                    qDebug() << "Estado: " << estate;

                    QDateTime dateIni = QLocale(QLocale::English).toDateTime(iniR,"yyyy-MM-ddThh:mm:ss.zzz");
                    QDateTime dateFin = QLocale(QLocale::English).toDateTime(finiR,"yyyy-MM-ddThh:mm:ss.zzz");
                    QDateTime actual = QDateTime::currentDateTime();

                    bool withinRange = (actual >= dateIni && actual <= dateFin);

                    if(estate==0){
                        if(withinRange==true){
                            ui->Piso2num3->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
                        }
                    }
                }

                /*obtener si hay pension*/
                QString sifechentre2 = "SELECT fecha_inicio,fecha_fin FROM pension WHERE idCajon = " + cajon;

                QSqlQuery sifechen2;
                sifechen2.exec(sifechentre2);

                while(sifechen2.next()){
                    QString iniR2 = sifechen2.value("fecha_inicio").toString();
                    QString finiR2 = sifechen2.value("fecha_fin").toString();

                    QDate dateIni2 = QDate::fromString(iniR2,"yyyy-MM-dd");
                    QDate dateFin2 = QDate::fromString(finiR2,"yyyy-MM-dd");
                    QDate actual2 = QDate::currentDate();

                    bool withinRange2 = (actual2 >= dateIni2 && actual2 <= dateFin2);

                    if(withinRange2==true){
                        ui->Piso2num3->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
                    }
                }

            }
            if(cajon2=="14"){

                /*obtener si hay reservacion*/
                QString sifechentre = "SELECT inicio,fin,estado FROM reservacion WHERE idCajon = " + cajon;

                QSqlQuery sifechen;
                sifechen.exec(sifechentre);

                while(sifechen.next()){
                    QString iniR = sifechen.value("inicio").toString();
                    QString finiR = sifechen.value("fin").toString();
                    int estate = sifechen.value("estado").toInt();

                    qDebug() << "Estado: " << estate;

                    QDateTime dateIni = QLocale(QLocale::English).toDateTime(iniR,"yyyy-MM-ddThh:mm:ss.zzz");
                    QDateTime dateFin = QLocale(QLocale::English).toDateTime(finiR,"yyyy-MM-ddThh:mm:ss.zzz");
                    QDateTime actual = QDateTime::currentDateTime();

                    bool withinRange = (actual >= dateIni && actual <= dateFin);

                    if(estate==0){
                        if(withinRange==true){
                            ui->Piso2num4->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
                        }
                    }
                }

                /*obtener si hay pension*/
                QString sifechentre2 = "SELECT fecha_inicio,fecha_fin FROM pension WHERE idCajon = " + cajon;

                QSqlQuery sifechen2;
                sifechen2.exec(sifechentre2);

                while(sifechen2.next()){
                    QString iniR2 = sifechen2.value("fecha_inicio").toString();
                    QString finiR2 = sifechen2.value("fecha_fin").toString();

                    QDate dateIni2 = QDate::fromString(iniR2,"yyyy-MM-dd");
                    QDate dateFin2 = QDate::fromString(finiR2,"yyyy-MM-dd");
                    QDate actual2 = QDate::currentDate();

                    bool withinRange2 = (actual2 >= dateIni2 && actual2 <= dateFin2);

                    if(withinRange2==true){
                        ui->Piso2num4->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
                    }
                }

            }
            if(cajon2=="15"){

                /*obtener si hay reservacion*/
                QString sifechentre = "SELECT inicio,fin,estado FROM reservacion WHERE idCajon = " + cajon;

                QSqlQuery sifechen;
                sifechen.exec(sifechentre);

                while(sifechen.next()){
                    QString iniR = sifechen.value("inicio").toString();
                    QString finiR = sifechen.value("fin").toString();
                    int estate = sifechen.value("estado").toInt();

                    qDebug() << "Estado: " << estate;

                    QDateTime dateIni = QLocale(QLocale::English).toDateTime(iniR,"yyyy-MM-ddThh:mm:ss.zzz");
                    QDateTime dateFin = QLocale(QLocale::English).toDateTime(finiR,"yyyy-MM-ddThh:mm:ss.zzz");
                    QDateTime actual = QDateTime::currentDateTime();

                    bool withinRange = (actual >= dateIni && actual <= dateFin);

                    if(estate==0){
                        if(withinRange==true){
                            ui->Piso2num5->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
                        }
                    }
                }

                /*obtener si hay pension*/
                QString sifechentre2 = "SELECT fecha_inicio,fecha_fin FROM pension WHERE idCajon = " + cajon;

                QSqlQuery sifechen2;
                sifechen2.exec(sifechentre2);

                while(sifechen2.next()){
                    QString iniR2 = sifechen2.value("fecha_inicio").toString();
                    QString finiR2 = sifechen2.value("fecha_fin").toString();

                    QDate dateIni2 = QDate::fromString(iniR2,"yyyy-MM-dd");
                    QDate dateFin2 = QDate::fromString(finiR2,"yyyy-MM-dd");
                    QDate actual2 = QDate::currentDate();

                    bool withinRange2 = (actual2 >= dateIni2 && actual2 <= dateFin2);

                    if(withinRange2==true){
                        ui->Piso2num5->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
                    }
                }

            }
            if(cajon2=="16"){

                /*obtener si hay reservacion*/
                QString sifechentre = "SELECT inicio,fin,estado FROM reservacion WHERE idCajon = " + cajon;

                QSqlQuery sifechen;
                sifechen.exec(sifechentre);

                while(sifechen.next()){
                    QString iniR = sifechen.value("inicio").toString();
                    QString finiR = sifechen.value("fin").toString();
                    int estate = sifechen.value("estado").toInt();

                    qDebug() << "Estado: " << estate;

                    QDateTime dateIni = QLocale(QLocale::English).toDateTime(iniR,"yyyy-MM-ddThh:mm:ss.zzz");
                    QDateTime dateFin = QLocale(QLocale::English).toDateTime(finiR,"yyyy-MM-ddThh:mm:ss.zzz");
                    QDateTime actual = QDateTime::currentDateTime();

                    bool withinRange = (actual >= dateIni && actual <= dateFin);

                    if(estate==0){
                        if(withinRange==true){
                            ui->Piso2num6->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
                        }
                    }
                }

                /*obtener si hay pension*/
                QString sifechentre2 = "SELECT fecha_inicio,fecha_fin FROM pension WHERE idCajon = " + cajon;

                QSqlQuery sifechen2;
                sifechen2.exec(sifechentre2);

                while(sifechen2.next()){
                    QString iniR2 = sifechen2.value("fecha_inicio").toString();
                    QString finiR2 = sifechen2.value("fecha_fin").toString();

                    QDate dateIni2 = QDate::fromString(iniR2,"yyyy-MM-dd");
                    QDate dateFin2 = QDate::fromString(finiR2,"yyyy-MM-dd");
                    QDate actual2 = QDate::currentDate();

                    bool withinRange2 = (actual2 >= dateIni2 && actual2 <= dateFin2);

                    if(withinRange2==true){
                        ui->Piso2num6->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
                    }
                }

            }
            if(cajon2=="17"){

                /*obtener si hay reservacion*/
                QString sifechentre = "SELECT inicio,fin,estado FROM reservacion WHERE idCajon = " + cajon;

                QSqlQuery sifechen;
                sifechen.exec(sifechentre);

                while(sifechen.next()){
                    QString iniR = sifechen.value("inicio").toString();
                    QString finiR = sifechen.value("fin").toString();
                    int estate = sifechen.value("estado").toInt();

                    qDebug() << "Estado: " << estate;

                    QDateTime dateIni = QLocale(QLocale::English).toDateTime(iniR,"yyyy-MM-ddThh:mm:ss.zzz");
                    QDateTime dateFin = QLocale(QLocale::English).toDateTime(finiR,"yyyy-MM-ddThh:mm:ss.zzz");
                    QDateTime actual = QDateTime::currentDateTime();

                    bool withinRange = (actual >= dateIni && actual <= dateFin);

                    if(estate==0){
                        if(withinRange==true){
                            ui->Piso2num7->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
                        }
                    }
                }

                /*obtener si hay pension*/
                QString sifechentre2 = "SELECT fecha_inicio,fecha_fin FROM pension WHERE idCajon = " + cajon;

                QSqlQuery sifechen2;
                sifechen2.exec(sifechentre2);

                while(sifechen2.next()){
                    QString iniR2 = sifechen2.value("fecha_inicio").toString();
                    QString finiR2 = sifechen2.value("fecha_fin").toString();

                    QDate dateIni2 = QDate::fromString(iniR2,"yyyy-MM-dd");
                    QDate dateFin2 = QDate::fromString(finiR2,"yyyy-MM-dd");
                    QDate actual2 = QDate::currentDate();

                    bool withinRange2 = (actual2 >= dateIni2 && actual2 <= dateFin2);

                    if(withinRange2==true){
                        ui->Piso2num7->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
                    }
                }

            }
            if(cajon2=="18"){

                /*obtener si hay reservacion*/
                QString sifechentre = "SELECT inicio,fin,estado FROM reservacion WHERE idCajon = " + cajon;

                QSqlQuery sifechen;
                sifechen.exec(sifechentre);

                while(sifechen.next()){
                    QString iniR = sifechen.value("inicio").toString();
                    QString finiR = sifechen.value("fin").toString();
                    int estate = sifechen.value("estado").toInt();

                    qDebug() << "Estado: " << estate;

                    QDateTime dateIni = QLocale(QLocale::English).toDateTime(iniR,"yyyy-MM-ddThh:mm:ss.zzz");
                    QDateTime dateFin = QLocale(QLocale::English).toDateTime(finiR,"yyyy-MM-ddThh:mm:ss.zzz");
                    QDateTime actual = QDateTime::currentDateTime();

                    bool withinRange = (actual >= dateIni && actual <= dateFin);

                    if(estate==0){
                        if(withinRange==true){
                            ui->Piso2num8->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
                        }
                    }
                }

                /*obtener si hay pension*/
                QString sifechentre2 = "SELECT fecha_inicio,fecha_fin FROM pension WHERE idCajon = " + cajon;

                QSqlQuery sifechen2;
                sifechen2.exec(sifechentre2);

                while(sifechen2.next()){
                    QString iniR2 = sifechen2.value("fecha_inicio").toString();
                    QString finiR2 = sifechen2.value("fecha_fin").toString();

                    QDate dateIni2 = QDate::fromString(iniR2,"yyyy-MM-dd");
                    QDate dateFin2 = QDate::fromString(finiR2,"yyyy-MM-dd");
                    QDate actual2 = QDate::currentDate();

                    bool withinRange2 = (actual2 >= dateIni2 && actual2 <= dateFin2);

                    if(withinRange2==true){
                        ui->Piso2num8->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
                    }
                }

            }
            if(cajon2=="19"){

                /*obtener si hay reservacion*/
                QString sifechentre = "SELECT inicio,fin,estado FROM reservacion WHERE idCajon = " + cajon;

                QSqlQuery sifechen;
                sifechen.exec(sifechentre);

                while(sifechen.next()){
                    QString iniR = sifechen.value("inicio").toString();
                    QString finiR = sifechen.value("fin").toString();
                    int estate = sifechen.value("estado").toInt();

                    qDebug() << "Estado: " << estate;

                    QDateTime dateIni = QLocale(QLocale::English).toDateTime(iniR,"yyyy-MM-ddThh:mm:ss.zzz");
                    QDateTime dateFin = QLocale(QLocale::English).toDateTime(finiR,"yyyy-MM-ddThh:mm:ss.zzz");
                    QDateTime actual = QDateTime::currentDateTime();

                    bool withinRange = (actual >= dateIni && actual <= dateFin);

                    if(estate==0){
                        if(withinRange==true){
                            ui->Piso2num9->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
                        }
                    }
                }

                /*obtener si hay pension*/
                QString sifechentre2 = "SELECT fecha_inicio,fecha_fin FROM pension WHERE idCajon = " + cajon;

                QSqlQuery sifechen2;
                sifechen2.exec(sifechentre2);

                while(sifechen2.next()){
                    QString iniR2 = sifechen2.value("fecha_inicio").toString();
                    QString finiR2 = sifechen2.value("fecha_fin").toString();

                    QDate dateIni2 = QDate::fromString(iniR2,"yyyy-MM-dd");
                    QDate dateFin2 = QDate::fromString(finiR2,"yyyy-MM-dd");
                    QDate actual2 = QDate::currentDate();

                    bool withinRange2 = (actual2 >= dateIni2 && actual2 <= dateFin2);

                    if(withinRange2==true){
                        ui->Piso2num9->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
                    }
                }

            }
            if(cajon2=="20"){

                /*obtener si hay reservacion*/
                QString sifechentre = "SELECT inicio,fin,estado FROM reservacion WHERE idCajon = " + cajon;

                QSqlQuery sifechen;
                sifechen.exec(sifechentre);

                while(sifechen.next()){
                    QString iniR = sifechen.value("inicio").toString();
                    QString finiR = sifechen.value("fin").toString();
                    int estate = sifechen.value("estado").toInt();

                    qDebug() << "Estado: " << estate;

                    QDateTime dateIni = QLocale(QLocale::English).toDateTime(iniR,"yyyy-MM-ddThh:mm:ss.zzz");
                    QDateTime dateFin = QLocale(QLocale::English).toDateTime(finiR,"yyyy-MM-ddThh:mm:ss.zzz");
                    QDateTime actual = QDateTime::currentDateTime();

                    bool withinRange = (actual >= dateIni && actual <= dateFin);

                    if(estate==0){
                        if(withinRange==true){
                            ui->Piso2num10->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
                        }
                    }
                }

                /*obtener si hay pension*/
                QString sifechentre2 = "SELECT fecha_inicio,fecha_fin FROM pension WHERE idCajon = " + cajon;

                QSqlQuery sifechen2;
                sifechen2.exec(sifechentre2);

                while(sifechen2.next()){
                    QString iniR2 = sifechen2.value("fecha_inicio").toString();
                    QString finiR2 = sifechen2.value("fecha_fin").toString();

                    QDate dateIni2 = QDate::fromString(iniR2,"yyyy-MM-dd");
                    QDate dateFin2 = QDate::fromString(finiR2,"yyyy-MM-dd");
                    QDate actual2 = QDate::currentDate();

                    bool withinRange2 = (actual2 >= dateIni2 && actual2 <= dateFin2);

                    if(withinRange2==true){
                        ui->Piso2num10->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
                    }
                }

            }
        }
    }


    if(sql3.exec(id3)){
        while(sql3.next()){
            cajon3= sql3.value(0).toString();
            if(cajon3=="21"){

                /*obtener si hay reservacion*/
                QString sifechentre = "SELECT inicio,fin,estado FROM reservacion WHERE idCajon = " + cajon;

                QSqlQuery sifechen;
                sifechen.exec(sifechentre);

                while(sifechen.next()){
                    QString iniR = sifechen.value("inicio").toString();
                    QString finiR = sifechen.value("fin").toString();
                    int estate = sifechen.value("estado").toInt();

                    qDebug() << "Estado: " << estate;

                    QDateTime dateIni = QLocale(QLocale::English).toDateTime(iniR,"yyyy-MM-ddThh:mm:ss.zzz");
                    QDateTime dateFin = QLocale(QLocale::English).toDateTime(finiR,"yyyy-MM-ddThh:mm:ss.zzz");
                    QDateTime actual = QDateTime::currentDateTime();

                    bool withinRange = (actual >= dateIni && actual <= dateFin);

                    if(estate==0){
                        if(withinRange==true){
                            ui->Piso3num1->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
                        }
                    }
                }

                /*obtener si hay pension*/
                QString sifechentre2 = "SELECT fecha_inicio,fecha_fin FROM pension WHERE idCajon = " + cajon;

                QSqlQuery sifechen2;
                sifechen2.exec(sifechentre2);

                while(sifechen2.next()){
                    QString iniR2 = sifechen2.value("fecha_inicio").toString();
                    QString finiR2 = sifechen2.value("fecha_fin").toString();

                    QDate dateIni2 = QDate::fromString(iniR2,"yyyy-MM-dd");
                    QDate dateFin2 = QDate::fromString(finiR2,"yyyy-MM-dd");
                    QDate actual2 = QDate::currentDate();

                    bool withinRange2 = (actual2 >= dateIni2 && actual2 <= dateFin2);

                    if(withinRange2==true){
                        ui->Piso3num1->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
                    }
                }

            }
            if(cajon3=="22"){

                /*obtener si hay reservacion*/
                QString sifechentre = "SELECT inicio,fin,estado FROM reservacion WHERE idCajon = " + cajon;

                QSqlQuery sifechen;
                sifechen.exec(sifechentre);

                while(sifechen.next()){
                    QString iniR = sifechen.value("inicio").toString();
                    QString finiR = sifechen.value("fin").toString();
                    int estate = sifechen.value("estado").toInt();

                    qDebug() << "Estado: " << estate;

                    QDateTime dateIni = QLocale(QLocale::English).toDateTime(iniR,"yyyy-MM-ddThh:mm:ss.zzz");
                    QDateTime dateFin = QLocale(QLocale::English).toDateTime(finiR,"yyyy-MM-ddThh:mm:ss.zzz");
                    QDateTime actual = QDateTime::currentDateTime();

                    bool withinRange = (actual >= dateIni && actual <= dateFin);

                    if(estate==0){
                        if(withinRange==true){
                            ui->Piso3num2->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
                        }
                    }
                }

                /*obtener si hay pension*/
                QString sifechentre2 = "SELECT fecha_inicio,fecha_fin FROM pension WHERE idCajon = " + cajon;

                QSqlQuery sifechen2;
                sifechen2.exec(sifechentre2);

                while(sifechen2.next()){
                    QString iniR2 = sifechen2.value("fecha_inicio").toString();
                    QString finiR2 = sifechen2.value("fecha_fin").toString();

                    QDate dateIni2 = QDate::fromString(iniR2,"yyyy-MM-dd");
                    QDate dateFin2 = QDate::fromString(finiR2,"yyyy-MM-dd");
                    QDate actual2 = QDate::currentDate();

                    bool withinRange2 = (actual2 >= dateIni2 && actual2 <= dateFin2);

                    if(withinRange2==true){
                        ui->Piso3num2->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
                    }
                }

            }
            if(cajon3=="23"){

                /*obtener si hay reservacion*/
                QString sifechentre = "SELECT inicio,fin,estado FROM reservacion WHERE idCajon = " + cajon;

                QSqlQuery sifechen;
                sifechen.exec(sifechentre);

                while(sifechen.next()){
                    QString iniR = sifechen.value("inicio").toString();
                    QString finiR = sifechen.value("fin").toString();
                    int estate = sifechen.value("estado").toInt();

                    qDebug() << "Estado: " << estate;

                    QDateTime dateIni = QLocale(QLocale::English).toDateTime(iniR,"yyyy-MM-ddThh:mm:ss.zzz");
                    QDateTime dateFin = QLocale(QLocale::English).toDateTime(finiR,"yyyy-MM-ddThh:mm:ss.zzz");
                    QDateTime actual = QDateTime::currentDateTime();

                    bool withinRange = (actual >= dateIni && actual <= dateFin);

                    if(estate==0){
                        if(withinRange==true){
                            ui->Piso3num3->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
                        }
                    }
                }

                /*obtener si hay pension*/
                QString sifechentre2 = "SELECT fecha_inicio,fecha_fin FROM pension WHERE idCajon = " + cajon;

                QSqlQuery sifechen2;
                sifechen2.exec(sifechentre2);

                while(sifechen2.next()){
                    QString iniR2 = sifechen2.value("fecha_inicio").toString();
                    QString finiR2 = sifechen2.value("fecha_fin").toString();

                    QDate dateIni2 = QDate::fromString(iniR2,"yyyy-MM-dd");
                    QDate dateFin2 = QDate::fromString(finiR2,"yyyy-MM-dd");
                    QDate actual2 = QDate::currentDate();

                    bool withinRange2 = (actual2 >= dateIni2 && actual2 <= dateFin2);

                    if(withinRange2==true){
                        ui->Piso3num3->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
                    }
                }

            }
            if(cajon3=="24"){

                /*obtener si hay reservacion*/
                QString sifechentre = "SELECT inicio,fin,estado FROM reservacion WHERE idCajon = " + cajon;

                QSqlQuery sifechen;
                sifechen.exec(sifechentre);

                while(sifechen.next()){
                    QString iniR = sifechen.value("inicio").toString();
                    QString finiR = sifechen.value("fin").toString();
                    int estate = sifechen.value("estado").toInt();

                    qDebug() << "Estado: " << estate;

                    QDateTime dateIni = QLocale(QLocale::English).toDateTime(iniR,"yyyy-MM-ddThh:mm:ss.zzz");
                    QDateTime dateFin = QLocale(QLocale::English).toDateTime(finiR,"yyyy-MM-ddThh:mm:ss.zzz");
                    QDateTime actual = QDateTime::currentDateTime();

                    bool withinRange = (actual >= dateIni && actual <= dateFin);

                    if(estate==0){
                        if(withinRange==true){
                            ui->Piso3num4->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
                        }
                    }
                }

                /*obtener si hay pension*/
                QString sifechentre2 = "SELECT fecha_inicio,fecha_fin FROM pension WHERE idCajon = " + cajon;

                QSqlQuery sifechen2;
                sifechen2.exec(sifechentre2);

                while(sifechen2.next()){
                    QString iniR2 = sifechen2.value("fecha_inicio").toString();
                    QString finiR2 = sifechen2.value("fecha_fin").toString();

                    QDate dateIni2 = QDate::fromString(iniR2,"yyyy-MM-dd");
                    QDate dateFin2 = QDate::fromString(finiR2,"yyyy-MM-dd");
                    QDate actual2 = QDate::currentDate();

                    bool withinRange2 = (actual2 >= dateIni2 && actual2 <= dateFin2);

                    if(withinRange2==true){
                        ui->Piso3num4->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
                    }
                }

            }
            if(cajon3=="25"){

                /*obtener si hay reservacion*/
                QString sifechentre = "SELECT inicio,fin,estado FROM reservacion WHERE idCajon = " + cajon;

                QSqlQuery sifechen;
                sifechen.exec(sifechentre);

                while(sifechen.next()){
                    QString iniR = sifechen.value("inicio").toString();
                    QString finiR = sifechen.value("fin").toString();
                    int estate = sifechen.value("estado").toInt();

                    qDebug() << "Estado: " << estate;

                    QDateTime dateIni = QLocale(QLocale::English).toDateTime(iniR,"yyyy-MM-ddThh:mm:ss.zzz");
                    QDateTime dateFin = QLocale(QLocale::English).toDateTime(finiR,"yyyy-MM-ddThh:mm:ss.zzz");
                    QDateTime actual = QDateTime::currentDateTime();

                    bool withinRange = (actual >= dateIni && actual <= dateFin);

                    if(estate==0){
                        if(withinRange==true){
                            ui->Piso3num5->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
                        }
                    }
                }

                /*obtener si hay pension*/
                QString sifechentre2 = "SELECT fecha_inicio,fecha_fin FROM pension WHERE idCajon = " + cajon;

                QSqlQuery sifechen2;
                sifechen2.exec(sifechentre2);

                while(sifechen2.next()){
                    QString iniR2 = sifechen2.value("fecha_inicio").toString();
                    QString finiR2 = sifechen2.value("fecha_fin").toString();

                    QDate dateIni2 = QDate::fromString(iniR2,"yyyy-MM-dd");
                    QDate dateFin2 = QDate::fromString(finiR2,"yyyy-MM-dd");
                    QDate actual2 = QDate::currentDate();

                    bool withinRange2 = (actual2 >= dateIni2 && actual2 <= dateFin2);

                    if(withinRange2==true){
                        ui->Piso3num5->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
                    }
                }

            }
            if(cajon3=="26"){

                /*obtener si hay reservacion*/
                QString sifechentre = "SELECT inicio,fin,estado FROM reservacion WHERE idCajon = " + cajon;

                QSqlQuery sifechen;
                sifechen.exec(sifechentre);

                while(sifechen.next()){
                    QString iniR = sifechen.value("inicio").toString();
                    QString finiR = sifechen.value("fin").toString();
                    int estate = sifechen.value("estado").toInt();

                    qDebug() << "Estado: " << estate;

                    QDateTime dateIni = QLocale(QLocale::English).toDateTime(iniR,"yyyy-MM-ddThh:mm:ss.zzz");
                    QDateTime dateFin = QLocale(QLocale::English).toDateTime(finiR,"yyyy-MM-ddThh:mm:ss.zzz");
                    QDateTime actual = QDateTime::currentDateTime();

                    bool withinRange = (actual >= dateIni && actual <= dateFin);

                    if(estate==0){
                        if(withinRange==true){
                            ui->Piso3num6->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
                        }
                    }
                }

                /*obtener si hay pension*/
                QString sifechentre2 = "SELECT fecha_inicio,fecha_fin FROM pension WHERE idCajon = " + cajon;

                QSqlQuery sifechen2;
                sifechen2.exec(sifechentre2);

                while(sifechen2.next()){
                    QString iniR2 = sifechen2.value("fecha_inicio").toString();
                    QString finiR2 = sifechen2.value("fecha_fin").toString();

                    QDate dateIni2 = QDate::fromString(iniR2,"yyyy-MM-dd");
                    QDate dateFin2 = QDate::fromString(finiR2,"yyyy-MM-dd");
                    QDate actual2 = QDate::currentDate();

                    bool withinRange2 = (actual2 >= dateIni2 && actual2 <= dateFin2);

                    if(withinRange2==true){
                        ui->Piso3num6->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
                    }
                }

            }
            if(cajon3=="27"){

                /*obtener si hay reservacion*/
                QString sifechentre = "SELECT inicio,fin,estado FROM reservacion WHERE idCajon = " + cajon;

                QSqlQuery sifechen;
                sifechen.exec(sifechentre);

                while(sifechen.next()){
                    QString iniR = sifechen.value("inicio").toString();
                    QString finiR = sifechen.value("fin").toString();
                    int estate = sifechen.value("estado").toInt();

                    qDebug() << "Estado: " << estate;

                    QDateTime dateIni = QLocale(QLocale::English).toDateTime(iniR,"yyyy-MM-ddThh:mm:ss.zzz");
                    QDateTime dateFin = QLocale(QLocale::English).toDateTime(finiR,"yyyy-MM-ddThh:mm:ss.zzz");
                    QDateTime actual = QDateTime::currentDateTime();

                    bool withinRange = (actual >= dateIni && actual <= dateFin);

                    if(estate==0){
                        if(withinRange==true){
                            ui->Piso3num7->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
                        }
                    }
                }

                /*obtener si hay pension*/
                QString sifechentre2 = "SELECT fecha_inicio,fecha_fin FROM pension WHERE idCajon = " + cajon;

                QSqlQuery sifechen2;
                sifechen2.exec(sifechentre2);

                while(sifechen2.next()){
                    QString iniR2 = sifechen2.value("fecha_inicio").toString();
                    QString finiR2 = sifechen2.value("fecha_fin").toString();

                    QDate dateIni2 = QDate::fromString(iniR2,"yyyy-MM-dd");
                    QDate dateFin2 = QDate::fromString(finiR2,"yyyy-MM-dd");
                    QDate actual2 = QDate::currentDate();

                    bool withinRange2 = (actual2 >= dateIni2 && actual2 <= dateFin2);

                    if(withinRange2==true){
                        ui->Piso3num7->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
                    }
                }

            }
            if(cajon3=="28"){

                /*obtener si hay reservacion*/
                QString sifechentre = "SELECT inicio,fin,estado FROM reservacion WHERE idCajon = " + cajon;

                QSqlQuery sifechen;
                sifechen.exec(sifechentre);

                while(sifechen.next()){
                    QString iniR = sifechen.value("inicio").toString();
                    QString finiR = sifechen.value("fin").toString();
                    int estate = sifechen.value("estado").toInt();

                    qDebug() << "Estado: " << estate;

                    QDateTime dateIni = QLocale(QLocale::English).toDateTime(iniR,"yyyy-MM-ddThh:mm:ss.zzz");
                    QDateTime dateFin = QLocale(QLocale::English).toDateTime(finiR,"yyyy-MM-ddThh:mm:ss.zzz");
                    QDateTime actual = QDateTime::currentDateTime();

                    bool withinRange = (actual >= dateIni && actual <= dateFin);

                    if(estate==0){
                        if(withinRange==true){
                            ui->Piso3num8->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
                        }
                    }
                }

                /*obtener si hay pension*/
                QString sifechentre2 = "SELECT fecha_inicio,fecha_fin FROM pension WHERE idCajon = " + cajon;

                QSqlQuery sifechen2;
                sifechen2.exec(sifechentre2);

                while(sifechen2.next()){
                    QString iniR2 = sifechen2.value("fecha_inicio").toString();
                    QString finiR2 = sifechen2.value("fecha_fin").toString();

                    QDate dateIni2 = QDate::fromString(iniR2,"yyyy-MM-dd");
                    QDate dateFin2 = QDate::fromString(finiR2,"yyyy-MM-dd");
                    QDate actual2 = QDate::currentDate();

                    bool withinRange2 = (actual2 >= dateIni2 && actual2 <= dateFin2);

                    if(withinRange2==true){
                        ui->Piso3num8->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
                    }
                }

            }
            if(cajon3=="29"){

                /*obtener si hay reservacion*/
                QString sifechentre = "SELECT inicio,fin,estado FROM reservacion WHERE idCajon = " + cajon;

                QSqlQuery sifechen;
                sifechen.exec(sifechentre);

                while(sifechen.next()){
                    QString iniR = sifechen.value("inicio").toString();
                    QString finiR = sifechen.value("fin").toString();
                    int estate = sifechen.value("estado").toInt();

                    qDebug() << "Estado: " << estate;

                    QDateTime dateIni = QLocale(QLocale::English).toDateTime(iniR,"yyyy-MM-ddThh:mm:ss.zzz");
                    QDateTime dateFin = QLocale(QLocale::English).toDateTime(finiR,"yyyy-MM-ddThh:mm:ss.zzz");
                    QDateTime actual = QDateTime::currentDateTime();

                    bool withinRange = (actual >= dateIni && actual <= dateFin);

                    if(estate==0){
                        if(withinRange==true){
                            ui->Piso3num9->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
                        }
                    }
                }

                /*obtener si hay pension*/
                QString sifechentre2 = "SELECT fecha_inicio,fecha_fin FROM pension WHERE idCajon = " + cajon;

                QSqlQuery sifechen2;
                sifechen2.exec(sifechentre2);

                while(sifechen2.next()){
                    QString iniR2 = sifechen2.value("fecha_inicio").toString();
                    QString finiR2 = sifechen2.value("fecha_fin").toString();

                    QDate dateIni2 = QDate::fromString(iniR2,"yyyy-MM-dd");
                    QDate dateFin2 = QDate::fromString(finiR2,"yyyy-MM-dd");
                    QDate actual2 = QDate::currentDate();

                    bool withinRange2 = (actual2 >= dateIni2 && actual2 <= dateFin2);

                    if(withinRange2==true){
                        ui->Piso3num9->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
                    }
                }

            }
            if(cajon3=="30"){

                /*obtener si hay reservacion*/
                QString sifechentre = "SELECT inicio,fin,estado FROM reservacion WHERE idCajon = " + cajon;

                QSqlQuery sifechen;
                sifechen.exec(sifechentre);

                while(sifechen.next()){
                    QString iniR = sifechen.value("inicio").toString();
                    QString finiR = sifechen.value("fin").toString();
                    int estate = sifechen.value("estado").toInt();

                    qDebug() << "Estado: " << estate;

                    QDateTime dateIni = QLocale(QLocale::English).toDateTime(iniR,"yyyy-MM-ddThh:mm:ss.zzz");
                    QDateTime dateFin = QLocale(QLocale::English).toDateTime(finiR,"yyyy-MM-ddThh:mm:ss.zzz");
                    QDateTime actual = QDateTime::currentDateTime();

                    bool withinRange = (actual >= dateIni && actual <= dateFin);

                    if(estate==0){
                        if(withinRange==true){
                            ui->Piso3num10->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
                        }
                    }
                }

                /*obtener si hay pension*/
                QString sifechentre2 = "SELECT fecha_inicio,fecha_fin FROM pension WHERE idCajon = " + cajon;

                QSqlQuery sifechen2;
                sifechen2.exec(sifechentre2);

                while(sifechen2.next()){
                    QString iniR2 = sifechen2.value("fecha_inicio").toString();
                    QString finiR2 = sifechen2.value("fecha_fin").toString();

                    QDate dateIni2 = QDate::fromString(iniR2,"yyyy-MM-dd");
                    QDate dateFin2 = QDate::fromString(finiR2,"yyyy-MM-dd");
                    QDate actual2 = QDate::currentDate();

                    bool withinRange2 = (actual2 >= dateIni2 && actual2 <= dateFin2);

                    if(withinRange2==true){
                        ui->Piso3num10->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
                    }
                }

            }
        }
    }

}


/*---------------------------------------- INTERFAZ ESTACIONAMIENTO PRIMER PISO -----------------------------------------------*/

void MainWindow::on_numero1_clicked(){
    QString ocupado;
    piso = "1";
    cajon = "1";

    //consultar si el cajon está ocupado

    QSqlQuery consultcaj;
    QString idCaj=obtenerIDCajon();
    qDebug() << "ID Cajon: " << idCaj;
    QString concaj = "SELECT lleno FROM cajon WHERE idCajon = '" + idCaj + "'";
    consultcaj.exec(concaj);
    consultcaj.next();
    int llenocaj = consultcaj.value("lleno").toInt();

    if(llenocaj==1){
        /*obtener si hay reservacion*/
        QString sifechentre = "SELECT inicio,fin,estado FROM reservacion WHERE idCajon = " + idCaj;

        QSqlQuery sifechen;
        sifechen.exec(sifechentre);

        while(sifechen.next()){
            QString iniR = sifechen.value("inicio").toString();
            QString finiR = sifechen.value("fin").toString();
            int estate = sifechen.value("estado").toInt();

            qDebug() << "Estado: " << estate;

            QDateTime dateIni = QLocale(QLocale::English).toDateTime(iniR,"yyyy-MM-ddThh:mm:ss.zzz");
            QDateTime dateFin = QLocale(QLocale::English).toDateTime(finiR,"yyyy-MM-ddThh:mm:ss.zzz");
            QDateTime actual = QDateTime::currentDateTime();

            bool withinRange = (actual >= dateIni && actual <= dateFin);

            if(estate==0){
                if(withinRange==true){
                    QMessageBox::about(this, "Advertencia", "Cajon no disponible");
                    return;
                }
            }
        }

        /*obtener si hay pension*/
        QString sifechentre2 = "SELECT fecha_inicio,fecha_fin FROM pension WHERE idCajon = " + idCaj;

        QSqlQuery sifechen2;
        sifechen2.exec(sifechentre2);

        while(sifechen2.next()){
            QString iniR2 = sifechen2.value("fecha_inicio").toString();
            QString finiR2 = sifechen2.value("fecha_fin").toString();

            QDate dateIni2 = QDate::fromString(iniR2,"yyyy-MM-dd");
            QDate dateFin2 = QDate::fromString(finiR2,"yyyy-MM-dd");
            QDate actual2 = QDate::currentDate();

            bool withinRange2 = (actual2 >= dateIni2 && actual2 <= dateFin2);

            if(withinRange2==true){
                QMessageBox::about(this, "Advertencia", "Cajon no disponible");
                return;
            }
        }
    }

    /*-------------------------------------------------------------------------------------*/

    /*----------------------------------------------ANIMACION---------------------------------------*/
    Animacion(140);
    ui->numero1->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
    Descansar();
    /*----------------------------------------------------------------------------------------------*/


    QString reservacion;
    reservacion = Tipo;
    if(reservacion== "Normal"){
        ui->stackedWidget->setCurrentIndex(7);

        ui->Matricula->clear();
        ui->Modelo->clear();
        ui->descripcion->clear();
        ui->fotouser_2->clear();
    }
    if(reservacion== "Premium"){
        ui->stackedWidget->setCurrentIndex(8);
    }
    if(reservacion== "Emergencia"){
        ui->stackedWidget->setCurrentIndex(6);

        ui->Matricula_3->clear();
        ui->Modelo_3->clear();
        ui->descripcion_3->clear();
        ui->fotouser_4->clear();
    }



    ui->numero1->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:none;");

}
void MainWindow::on_numero2_clicked(){
    piso = "1";
    cajon = "2";

    //consultar si el cajon está ocupado

    QSqlQuery consultcaj;
    QString idCaj=obtenerIDCajon();
    qDebug() << "ID Cajon: " << idCaj;
    QString concaj = "SELECT lleno FROM cajon WHERE idCajon = '" + idCaj + "'";
    consultcaj.exec(concaj);
    consultcaj.next();
    int llenocaj = consultcaj.value("lleno").toInt();

    if(llenocaj==1){
        /*obtener si hay reservacion*/
        QString sifechentre = "SELECT inicio,fin,estado FROM reservacion WHERE idCajon = " + idCaj;

        QSqlQuery sifechen;
        sifechen.exec(sifechentre);

        while(sifechen.next()){
            QString iniR = sifechen.value("inicio").toString();
            QString finiR = sifechen.value("fin").toString();
            int estate = sifechen.value("estado").toInt();

            qDebug() << "Estado: " << estate;

            QDateTime dateIni = QLocale(QLocale::English).toDateTime(iniR,"yyyy-MM-ddThh:mm:ss.zzz");
            QDateTime dateFin = QLocale(QLocale::English).toDateTime(finiR,"yyyy-MM-ddThh:mm:ss.zzz");
            QDateTime actual = QDateTime::currentDateTime();

            bool withinRange = (actual >= dateIni && actual <= dateFin);

            if(estate==0){
                if(withinRange==true){
                    QMessageBox::about(this, "Advertencia", "Cajon no disponible");
                    return;
                }
            }
        }

        /*obtener si hay pension*/
        QString sifechentre2 = "SELECT fecha_inicio,fecha_fin FROM pension WHERE idCajon = " + idCaj;

        QSqlQuery sifechen2;
        sifechen2.exec(sifechentre2);

        while(sifechen2.next()){
            QString iniR2 = sifechen2.value("fecha_inicio").toString();
            QString finiR2 = sifechen2.value("fecha_fin").toString();

            QDate dateIni2 = QDate::fromString(iniR2,"yyyy-MM-dd");
            QDate dateFin2 = QDate::fromString(finiR2,"yyyy-MM-dd");
            QDate actual2 = QDate::currentDate();

            bool withinRange2 = (actual2 >= dateIni2 && actual2 <= dateFin2);

            if(withinRange2==true){
                QMessageBox::about(this, "Advertencia", "Cajon no disponible");
                return;
            }
        }
    }

    /*-------------------------------------------------------------------------------------*/

    /*----------------------------------------------ANIMACION---------------------------------------*/
    Animacion(250);
    ui->numero2->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
    Descansar();
    /*----------------------------------------------------------------------------------------------*/

    QString reservacion;
    reservacion = Tipo;
    if(reservacion== "Normal"){
        ui->stackedWidget->setCurrentIndex(7);

        ui->Matricula->clear();
        ui->Modelo->clear();
        ui->descripcion->clear();
        ui->fotouser_2->clear();
    }
    if(reservacion== "Premium"){
        ui->stackedWidget->setCurrentIndex(8);
    }
    if(reservacion== "Emergencia"){
        ui->stackedWidget->setCurrentIndex(6);

        ui->Matricula_3->clear();
        ui->Modelo_3->clear();
        ui->descripcion_3->clear();
        ui->fotouser_4->clear();
    }

    ui->numero2->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:none;");
}
void MainWindow::on_numero3_clicked(){
    piso = "1";
    cajon = "3";

    //consultar si el cajon está ocupado

    QSqlQuery consultcaj;
    QString idCaj=obtenerIDCajon();
    qDebug() << "ID Cajon: " << idCaj;
    QString concaj = "SELECT lleno FROM cajon WHERE idCajon = '" + idCaj + "'";
    consultcaj.exec(concaj);
    consultcaj.next();
    int llenocaj = consultcaj.value("lleno").toInt();

    if(llenocaj==1){
        /*obtener si hay reservacion*/
        QString sifechentre = "SELECT inicio,fin,estado FROM reservacion WHERE idCajon = " + idCaj;

        QSqlQuery sifechen;
        sifechen.exec(sifechentre);

        while(sifechen.next()){
            QString iniR = sifechen.value("inicio").toString();
            QString finiR = sifechen.value("fin").toString();
            int estate = sifechen.value("estado").toInt();

            qDebug() << "Estado: " << estate;

            QDateTime dateIni = QLocale(QLocale::English).toDateTime(iniR,"yyyy-MM-ddThh:mm:ss.zzz");
            QDateTime dateFin = QLocale(QLocale::English).toDateTime(finiR,"yyyy-MM-ddThh:mm:ss.zzz");
            QDateTime actual = QDateTime::currentDateTime();

            bool withinRange = (actual >= dateIni && actual <= dateFin);

            if(estate==0){
                if(withinRange==true){
                    QMessageBox::about(this, "Advertencia", "Cajon no disponible");
                    return;
                }
            }
        }

        /*obtener si hay pension*/
        QString sifechentre2 = "SELECT fecha_inicio,fecha_fin FROM pension WHERE idCajon = " + idCaj;

        QSqlQuery sifechen2;
        sifechen2.exec(sifechentre2);

        while(sifechen2.next()){
            QString iniR2 = sifechen2.value("fecha_inicio").toString();
            QString finiR2 = sifechen2.value("fecha_fin").toString();

            QDate dateIni2 = QDate::fromString(iniR2,"yyyy-MM-dd");
            QDate dateFin2 = QDate::fromString(finiR2,"yyyy-MM-dd");
            QDate actual2 = QDate::currentDate();

            bool withinRange2 = (actual2 >= dateIni2 && actual2 <= dateFin2);

            if(withinRange2==true){
                QMessageBox::about(this, "Advertencia", "Cajon no disponible");
                return;
            }
        }
    }

    /*----------------------------------------------ANIMACION---------------------------------------*/
    Animacion(360);
    ui->numero3->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
    Descansar();
    /*----------------------------------------------------------------------------------------------*/

    QString reservacion;
    reservacion = Tipo;
    if(reservacion== "Normal"){
        ui->stackedWidget->setCurrentIndex(7);

        ui->Matricula->clear();
        ui->Modelo->clear();
        ui->descripcion->clear();
        ui->fotouser_2->clear();
    }
    if(reservacion== "Premium"){
        ui->stackedWidget->setCurrentIndex(8);
    }
    if(reservacion== "Emergencia"){
        ui->stackedWidget->setCurrentIndex(6);

        ui->Matricula_3->clear();
        ui->Modelo_3->clear();
        ui->descripcion_3->clear();
        ui->fotouser_4->clear();
    }

    ui->numero3->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:none;");
}
void MainWindow::on_numero4_clicked(){
    piso = "1";
    cajon = "4";

    //consultar si el cajon está ocupado

    QSqlQuery consultcaj;
    QString idCaj=obtenerIDCajon();
    qDebug() << "ID Cajon: " << idCaj;
    QString concaj = "SELECT lleno FROM cajon WHERE idCajon = '" + idCaj + "'";
    consultcaj.exec(concaj);
    consultcaj.next();
    int llenocaj = consultcaj.value("lleno").toInt();

    if(llenocaj==1){
        /*obtener si hay reservacion*/
        QString sifechentre = "SELECT inicio,fin,estado FROM reservacion WHERE idCajon = " + idCaj;

        QSqlQuery sifechen;
        sifechen.exec(sifechentre);

        while(sifechen.next()){
            QString iniR = sifechen.value("inicio").toString();
            QString finiR = sifechen.value("fin").toString();
            int estate = sifechen.value("estado").toInt();

            qDebug() << "Estado: " << estate;

            QDateTime dateIni = QLocale(QLocale::English).toDateTime(iniR,"yyyy-MM-ddThh:mm:ss.zzz");
            QDateTime dateFin = QLocale(QLocale::English).toDateTime(finiR,"yyyy-MM-ddThh:mm:ss.zzz");
            QDateTime actual = QDateTime::currentDateTime();

            bool withinRange = (actual >= dateIni && actual <= dateFin);

            if(estate==0){
                if(withinRange==true){
                    QMessageBox::about(this, "Advertencia", "Cajon no disponible");
                    return;
                }
            }
        }

        /*obtener si hay pension*/
        QString sifechentre2 = "SELECT fecha_inicio,fecha_fin FROM pension WHERE idCajon = " + idCaj;

        QSqlQuery sifechen2;
        sifechen2.exec(sifechentre2);

        while(sifechen2.next()){
            QString iniR2 = sifechen2.value("fecha_inicio").toString();
            QString finiR2 = sifechen2.value("fecha_fin").toString();

            QDate dateIni2 = QDate::fromString(iniR2,"yyyy-MM-dd");
            QDate dateFin2 = QDate::fromString(finiR2,"yyyy-MM-dd");
            QDate actual2 = QDate::currentDate();

            bool withinRange2 = (actual2 >= dateIni2 && actual2 <= dateFin2);

            if(withinRange2==true){
                QMessageBox::about(this, "Advertencia", "Cajon no disponible");
                return;
            }
        }
    }


    /*----------------------------------------------ANIMACION---------------------------------------*/
    Animacion(470);
    ui->numero4->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
    Descansar();
    /*----------------------------------------------------------------------------------------------*/
    QString reservacion;
    reservacion = Tipo;
    if(reservacion== "Normal"){
        ui->stackedWidget->setCurrentIndex(7);

        ui->Matricula->clear();
        ui->Modelo->clear();
        ui->descripcion->clear();
        ui->fotouser_2->clear();
    }
    if(reservacion== "Premium"){
        ui->stackedWidget->setCurrentIndex(8);
    }
    if(reservacion== "Emergencia"){
        ui->stackedWidget->setCurrentIndex(6);

        ui->Matricula_3->clear();
        ui->Modelo_3->clear();
        ui->descripcion_3->clear();
        ui->fotouser_4->clear();
    }

    ui->numero4->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:none;");

}
void MainWindow::on_numero5_clicked(){
    piso = "1";
    cajon = "5";

    //consultar si el cajon está ocupado

    QSqlQuery consultcaj;
    QString idCaj=obtenerIDCajon();
    qDebug() << "ID Cajon: " << idCaj;
    QString concaj = "SELECT lleno FROM cajon WHERE idCajon = '" + idCaj + "'";
    consultcaj.exec(concaj);
    consultcaj.next();
    int llenocaj = consultcaj.value("lleno").toInt();

    if(llenocaj==1){
        /*obtener si hay reservacion*/
        QString sifechentre = "SELECT inicio,fin,estado FROM reservacion WHERE idCajon = " + idCaj;

        QSqlQuery sifechen;
        sifechen.exec(sifechentre);

        while(sifechen.next()){
            QString iniR = sifechen.value("inicio").toString();
            QString finiR = sifechen.value("fin").toString();
            int estate = sifechen.value("estado").toInt();

            qDebug() << "Estado: " << estate;

            QDateTime dateIni = QLocale(QLocale::English).toDateTime(iniR,"yyyy-MM-ddThh:mm:ss.zzz");
            QDateTime dateFin = QLocale(QLocale::English).toDateTime(finiR,"yyyy-MM-ddThh:mm:ss.zzz");
            QDateTime actual = QDateTime::currentDateTime();

            bool withinRange = (actual >= dateIni && actual <= dateFin);

            if(estate==0){
                if(withinRange==true){
                    QMessageBox::about(this, "Advertencia", "Cajon no disponible");
                    return;
                }
            }
        }

        /*obtener si hay pension*/
        QString sifechentre2 = "SELECT fecha_inicio,fecha_fin FROM pension WHERE idCajon = " + idCaj;

        QSqlQuery sifechen2;
        sifechen2.exec(sifechentre2);

        while(sifechen2.next()){
            QString iniR2 = sifechen2.value("fecha_inicio").toString();
            QString finiR2 = sifechen2.value("fecha_fin").toString();

            QDate dateIni2 = QDate::fromString(iniR2,"yyyy-MM-dd");
            QDate dateFin2 = QDate::fromString(finiR2,"yyyy-MM-dd");
            QDate actual2 = QDate::currentDate();

            bool withinRange2 = (actual2 >= dateIni2 && actual2 <= dateFin2);

            if(withinRange2==true){
                QMessageBox::about(this, "Advertencia", "Cajon no disponible");
                return;
            }
        }
    }


    /*----------------------------------------------ANIMACION---------------------------------------*/
    Animacion(580);
    ui->numero5->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
    Descansar();
    /*----------------------------------------------------------------------------------------------*/

    QString reservacion;
    reservacion = Tipo;
    if(reservacion== "Normal"){
        ui->stackedWidget->setCurrentIndex(7);

        ui->Matricula->clear();
        ui->Modelo->clear();
        ui->descripcion->clear();
        ui->fotouser_2->clear();
    }
    if(reservacion== "Premium"){
        ui->stackedWidget->setCurrentIndex(8);
    }
    if(reservacion== "Emergencia"){
        ui->stackedWidget->setCurrentIndex(6);

        ui->Matricula_3->clear();
        ui->Modelo_3->clear();
        ui->descripcion_3->clear();
        ui->fotouser_4->clear();
    }

    ui->numero5->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:none;");

}
void MainWindow::on_numero6_clicked(){
    piso = "1";
    cajon = "6";

    //consultar si el cajon está ocupado

    QSqlQuery consultcaj;
    QString idCaj=obtenerIDCajon();
    qDebug() << "ID Cajon: " << idCaj;
    QString concaj = "SELECT lleno FROM cajon WHERE idCajon = '" + idCaj + "'";
    consultcaj.exec(concaj);
    consultcaj.next();
    int llenocaj = consultcaj.value("lleno").toInt();

    if(llenocaj==1){
        /*obtener si hay reservacion*/
        QString sifechentre = "SELECT inicio,fin,estado FROM reservacion WHERE idCajon = " + idCaj;

        QSqlQuery sifechen;
        sifechen.exec(sifechentre);

        while(sifechen.next()){
            QString iniR = sifechen.value("inicio").toString();
            QString finiR = sifechen.value("fin").toString();
            int estate = sifechen.value("estado").toInt();

            qDebug() << "Estado: " << estate;

            QDateTime dateIni = QLocale(QLocale::English).toDateTime(iniR,"yyyy-MM-ddThh:mm:ss.zzz");
            QDateTime dateFin = QLocale(QLocale::English).toDateTime(finiR,"yyyy-MM-ddThh:mm:ss.zzz");
            QDateTime actual = QDateTime::currentDateTime();

            bool withinRange = (actual >= dateIni && actual <= dateFin);

            if(estate==0){
                if(withinRange==true){
                    QMessageBox::about(this, "Advertencia", "Cajon no disponible");
                    return;
                }
            }
        }

        /*obtener si hay pension*/
        QString sifechentre2 = "SELECT fecha_inicio,fecha_fin FROM pension WHERE idCajon = " + idCaj;

        QSqlQuery sifechen2;
        sifechen2.exec(sifechentre2);

        while(sifechen2.next()){
            QString iniR2 = sifechen2.value("fecha_inicio").toString();
            QString finiR2 = sifechen2.value("fecha_fin").toString();

            QDate dateIni2 = QDate::fromString(iniR2,"yyyy-MM-dd");
            QDate dateFin2 = QDate::fromString(finiR2,"yyyy-MM-dd");
            QDate actual2 = QDate::currentDate();

            bool withinRange2 = (actual2 >= dateIni2 && actual2 <= dateFin2);

            if(withinRange2==true){
                QMessageBox::about(this, "Advertencia", "Cajon no disponible");
                return;
            }
        }
    }


    /*----------------------------------------------ANIMACION---------------------------------------*/
    Animacion(140);
    ui->numero6->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/abajo.png);");
    Descansar();
    /*----------------------------------------------------------------------------------------------*/


    QString reservacion;
    reservacion = Tipo;
    if(reservacion== "Normal"){
        ui->stackedWidget->setCurrentIndex(7);

        ui->Matricula->clear();
        ui->Modelo->clear();
        ui->descripcion->clear();
        ui->fotouser_2->clear();
    }
    if(reservacion== "Premium"){
        ui->stackedWidget->setCurrentIndex(8);
    }
    if(reservacion== "Emergencia"){
        ui->stackedWidget->setCurrentIndex(6);

        ui->Matricula_3->clear();
        ui->Modelo_3->clear();
        ui->descripcion_3->clear();
        ui->fotouser_4->clear();
    }

    ui->numero6->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:none;");

}
void MainWindow::on_numero7_clicked(){
    piso = "1";
    cajon = "7";

    //consultar si el cajon está ocupado

    QSqlQuery consultcaj;
    QString idCaj=obtenerIDCajon();
    qDebug() << "ID Cajon: " << idCaj;
    QString concaj = "SELECT lleno FROM cajon WHERE idCajon = '" + idCaj + "'";
    consultcaj.exec(concaj);
    consultcaj.next();
    int llenocaj = consultcaj.value("lleno").toInt();

    if(llenocaj==1){
        /*obtener si hay reservacion*/
        QString sifechentre = "SELECT inicio,fin,estado FROM reservacion WHERE idCajon = " + idCaj;

        QSqlQuery sifechen;
        sifechen.exec(sifechentre);

        while(sifechen.next()){
            QString iniR = sifechen.value("inicio").toString();
            QString finiR = sifechen.value("fin").toString();
            int estate = sifechen.value("estado").toInt();

            qDebug() << "Estado: " << estate;

            QDateTime dateIni = QLocale(QLocale::English).toDateTime(iniR,"yyyy-MM-ddThh:mm:ss.zzz");
            QDateTime dateFin = QLocale(QLocale::English).toDateTime(finiR,"yyyy-MM-ddThh:mm:ss.zzz");
            QDateTime actual = QDateTime::currentDateTime();

            bool withinRange = (actual >= dateIni && actual <= dateFin);

            if(estate==0){
                if(withinRange==true){
                    QMessageBox::about(this, "Advertencia", "Cajon no disponible");
                    return;
                }
            }
        }

        /*obtener si hay pension*/
        QString sifechentre2 = "SELECT fecha_inicio,fecha_fin FROM pension WHERE idCajon = " + idCaj;

        QSqlQuery sifechen2;
        sifechen2.exec(sifechentre2);

        while(sifechen2.next()){
            QString iniR2 = sifechen2.value("fecha_inicio").toString();
            QString finiR2 = sifechen2.value("fecha_fin").toString();

            QDate dateIni2 = QDate::fromString(iniR2,"yyyy-MM-dd");
            QDate dateFin2 = QDate::fromString(finiR2,"yyyy-MM-dd");
            QDate actual2 = QDate::currentDate();

            bool withinRange2 = (actual2 >= dateIni2 && actual2 <= dateFin2);

            if(withinRange2==true){
                QMessageBox::about(this, "Advertencia", "Cajon no disponible");
                return;
            }
        }
    }

    /*----------------------------------------------ANIMACION---------------------------------------*/
    Animacion(250);
    ui->numero7->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/abajo.png);");
    Descansar();
    /*----------------------------------------------------------------------------------------------*/

    QString reservacion;
    reservacion = Tipo;
    if(reservacion== "Normal"){
        ui->stackedWidget->setCurrentIndex(7);

        ui->Matricula->clear();
        ui->Modelo->clear();
        ui->descripcion->clear();
        ui->fotouser_2->clear();
    }
    if(reservacion== "Premium"){
        ui->stackedWidget->setCurrentIndex(8);
    }
    if(reservacion== "Emergencia"){
        ui->stackedWidget->setCurrentIndex(6);

        ui->Matricula_3->clear();
        ui->Modelo_3->clear();
        ui->descripcion_3->clear();
        ui->fotouser_4->clear();
    }

    ui->numero7->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:none;");

}
void MainWindow::on_numero8_clicked(){
    piso = "1";
    cajon = "8";

    //consultar si el cajon está ocupado

    QSqlQuery consultcaj;
    QString idCaj=obtenerIDCajon();
    qDebug() << "ID Cajon: " << idCaj;
    QString concaj = "SELECT lleno FROM cajon WHERE idCajon = '" + idCaj + "'";
    consultcaj.exec(concaj);
    consultcaj.next();
    int llenocaj = consultcaj.value("lleno").toInt();

    if(llenocaj==1){
        /*obtener si hay reservacion*/
        QString sifechentre = "SELECT inicio,fin,estado FROM reservacion WHERE idCajon = " + idCaj;

        QSqlQuery sifechen;
        sifechen.exec(sifechentre);

        while(sifechen.next()){
            QString iniR = sifechen.value("inicio").toString();
            QString finiR = sifechen.value("fin").toString();
            int estate = sifechen.value("estado").toInt();

            qDebug() << "Estado: " << estate;

            QDateTime dateIni = QLocale(QLocale::English).toDateTime(iniR,"yyyy-MM-ddThh:mm:ss.zzz");
            QDateTime dateFin = QLocale(QLocale::English).toDateTime(finiR,"yyyy-MM-ddThh:mm:ss.zzz");
            QDateTime actual = QDateTime::currentDateTime();

            bool withinRange = (actual >= dateIni && actual <= dateFin);

            if(estate==0){
                if(withinRange==true){
                    QMessageBox::about(this, "Advertencia", "Cajon no disponible");
                    return;
                }
            }
        }

        /*obtener si hay pension*/
        QString sifechentre2 = "SELECT fecha_inicio,fecha_fin FROM pension WHERE idCajon = " + idCaj;

        QSqlQuery sifechen2;
        sifechen2.exec(sifechentre2);

        while(sifechen2.next()){
            QString iniR2 = sifechen2.value("fecha_inicio").toString();
            QString finiR2 = sifechen2.value("fecha_fin").toString();

            QDate dateIni2 = QDate::fromString(iniR2,"yyyy-MM-dd");
            QDate dateFin2 = QDate::fromString(finiR2,"yyyy-MM-dd");
            QDate actual2 = QDate::currentDate();

            bool withinRange2 = (actual2 >= dateIni2 && actual2 <= dateFin2);

            if(withinRange2==true){
                QMessageBox::about(this, "Advertencia", "Cajon no disponible");
                return;
            }
        }
    }

    /*----------------------------------------------ANIMACION---------------------------------------*/
    Animacion(360);
    ui->numero8->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/abajo.png);");
    Descansar();
    /*----------------------------------------------------------------------------------------------*/

    QString reservacion;
    reservacion = Tipo;
    if(reservacion== "Normal"){
        ui->stackedWidget->setCurrentIndex(7);

        ui->Matricula->clear();
        ui->Modelo->clear();
        ui->descripcion->clear();
        ui->fotouser_2->clear();
    }
    if(reservacion== "Premium"){
        ui->stackedWidget->setCurrentIndex(8);
    }
    if(reservacion== "Emergencia"){
        ui->stackedWidget->setCurrentIndex(6);

        ui->Matricula_3->clear();
        ui->Modelo_3->clear();
        ui->descripcion_3->clear();
        ui->fotouser_4->clear();
    }

    ui->numero8->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:none;");

}
void MainWindow::on_numero9_clicked(){
    piso = "1";
    cajon = "9";

    //consultar si el cajon está ocupado

    QSqlQuery consultcaj;
    QString idCaj=obtenerIDCajon();
    qDebug() << "ID Cajon: " << idCaj;
    QString concaj = "SELECT lleno FROM cajon WHERE idCajon = '" + idCaj + "'";
    consultcaj.exec(concaj);
    consultcaj.next();
    int llenocaj = consultcaj.value("lleno").toInt();

    if(llenocaj==1){
        /*obtener si hay reservacion*/
        QString sifechentre = "SELECT inicio,fin,estado FROM reservacion WHERE idCajon = " + idCaj;

        QSqlQuery sifechen;
        sifechen.exec(sifechentre);

        while(sifechen.next()){
            QString iniR = sifechen.value("inicio").toString();
            QString finiR = sifechen.value("fin").toString();
            int estate = sifechen.value("estado").toInt();

            qDebug() << "Estado: " << estate;

            QDateTime dateIni = QLocale(QLocale::English).toDateTime(iniR,"yyyy-MM-ddThh:mm:ss.zzz");
            QDateTime dateFin = QLocale(QLocale::English).toDateTime(finiR,"yyyy-MM-ddThh:mm:ss.zzz");
            QDateTime actual = QDateTime::currentDateTime();

            bool withinRange = (actual >= dateIni && actual <= dateFin);

            if(estate==0){
                if(withinRange==true){
                    QMessageBox::about(this, "Advertencia", "Cajon no disponible");
                    return;
                }
            }
        }

        /*obtener si hay pension*/
        QString sifechentre2 = "SELECT fecha_inicio,fecha_fin FROM pension WHERE idCajon = " + idCaj;

        QSqlQuery sifechen2;
        sifechen2.exec(sifechentre2);

        while(sifechen2.next()){
            QString iniR2 = sifechen2.value("fecha_inicio").toString();
            QString finiR2 = sifechen2.value("fecha_fin").toString();

            QDate dateIni2 = QDate::fromString(iniR2,"yyyy-MM-dd");
            QDate dateFin2 = QDate::fromString(finiR2,"yyyy-MM-dd");
            QDate actual2 = QDate::currentDate();

            bool withinRange2 = (actual2 >= dateIni2 && actual2 <= dateFin2);

            if(withinRange2==true){
                QMessageBox::about(this, "Advertencia", "Cajon no disponible");
                return;
            }
        }
    }

    /*----------------------------------------------ANIMACION---------------------------------------*/
    Animacion(470);
    ui->numero9->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/abajo.png);");
    Descansar();
    /*----------------------------------------------------------------------------------------------*/

    QString reservacion;
    reservacion = Tipo;
    if(reservacion== "Normal"){
        ui->stackedWidget->setCurrentIndex(7);

        ui->Matricula->clear();
        ui->Modelo->clear();
        ui->descripcion->clear();
        ui->fotouser_2->clear();
    }
    if(reservacion== "Premium"){
        ui->stackedWidget->setCurrentIndex(8);
    }
    if(reservacion== "Emergencia"){
        ui->stackedWidget->setCurrentIndex(6);

        ui->Matricula_3->clear();
        ui->Modelo_3->clear();
        ui->descripcion_3->clear();
        ui->fotouser_4->clear();
    }
    ui->numero9->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:none;");

}
void MainWindow::on_numero10_clicked(){
    piso = "1";
    cajon = "10";

    //consultar si el cajon está ocupado

    QSqlQuery consultcaj;
    QString idCaj=obtenerIDCajon();
    qDebug() << "ID Cajon: " << idCaj;
    QString concaj = "SELECT lleno FROM cajon WHERE idCajon = '" + idCaj + "'";
    consultcaj.exec(concaj);
    consultcaj.next();
    int llenocaj = consultcaj.value("lleno").toInt();

    if(llenocaj==1){
        /*obtener si hay reservacion*/
        QString sifechentre = "SELECT inicio,fin,estado FROM reservacion WHERE idCajon = " + idCaj;

        QSqlQuery sifechen;
        sifechen.exec(sifechentre);

        while(sifechen.next()){
            QString iniR = sifechen.value("inicio").toString();
            QString finiR = sifechen.value("fin").toString();
            int estate = sifechen.value("estado").toInt();

            qDebug() << "Estado: " << estate;

            QDateTime dateIni = QLocale(QLocale::English).toDateTime(iniR,"yyyy-MM-ddThh:mm:ss.zzz");
            QDateTime dateFin = QLocale(QLocale::English).toDateTime(finiR,"yyyy-MM-ddThh:mm:ss.zzz");
            QDateTime actual = QDateTime::currentDateTime();

            bool withinRange = (actual >= dateIni && actual <= dateFin);

            if(estate==0){
                if(withinRange==true){
                    QMessageBox::about(this, "Advertencia", "Cajon no disponible");
                    return;
                }
            }
        }

        /*obtener si hay pension*/
        QString sifechentre2 = "SELECT fecha_inicio,fecha_fin FROM pension WHERE idCajon = " + idCaj;

        QSqlQuery sifechen2;
        sifechen2.exec(sifechentre2);

        while(sifechen2.next()){
            QString iniR2 = sifechen2.value("fecha_inicio").toString();
            QString finiR2 = sifechen2.value("fecha_fin").toString();

            QDate dateIni2 = QDate::fromString(iniR2,"yyyy-MM-dd");
            QDate dateFin2 = QDate::fromString(finiR2,"yyyy-MM-dd");
            QDate actual2 = QDate::currentDate();

            bool withinRange2 = (actual2 >= dateIni2 && actual2 <= dateFin2);

            if(withinRange2==true){
                QMessageBox::about(this, "Advertencia", "Cajon no disponible");
                return;
            }
        }
    }

    /*----------------------------------------------ANIMACION---------------------------------------*/
    Animacion(580);
    ui->numero10->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/abajo.png);");
    Descansar();
    /*----------------------------------------------------------------------------------------------*/

    QString reservacion;
    reservacion = Tipo;
    if(reservacion== "Normal"){
        ui->stackedWidget->setCurrentIndex(7);

        ui->Matricula->clear();
        ui->Modelo->clear();
        ui->descripcion->clear();
        ui->fotouser_2->clear();
    }
    if(reservacion== "Premium"){
        ui->stackedWidget->setCurrentIndex(8);
    }
    if(reservacion== "Emergencia"){
        ui->stackedWidget->setCurrentIndex(6);

        ui->Matricula_3->clear();
        ui->Modelo_3->clear();
        ui->descripcion_3->clear();
        ui->fotouser_4->clear();
    }
    ui->numero10->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:none;");

}
/********************************************************************************************************************************/

/*---------------------------------------- INTERFAZ DEL ESTACIONAMIENTO SEGUNDO PISO--------------------------------------------*/

void MainWindow::on_Piso2num1_clicked(){
    QString reservacion;
    reservacion = Tipo;

    piso = "2";
    cajon = "1";


    //consultar si el cajon está ocupado

    QSqlQuery consultcaj;
    QString idCaj=obtenerIDCajon();
    qDebug() << "ID Cajon: " << idCaj;
    QString concaj = "SELECT lleno FROM cajon WHERE idCajon = '" + idCaj + "'";
    consultcaj.exec(concaj);
    consultcaj.next();
    int llenocaj = consultcaj.value("lleno").toInt();

    if(llenocaj==1){
        /*obtener si hay reservacion*/
        QString sifechentre = "SELECT inicio,fin,estado FROM reservacion WHERE idCajon = " + idCaj;

        QSqlQuery sifechen;
        sifechen.exec(sifechentre);

        while(sifechen.next()){
            QString iniR = sifechen.value("inicio").toString();
            QString finiR = sifechen.value("fin").toString();
            int estate = sifechen.value("estado").toInt();

            qDebug() << "Estado: " << estate;

            QDateTime dateIni = QLocale(QLocale::English).toDateTime(iniR,"yyyy-MM-ddThh:mm:ss.zzz");
            QDateTime dateFin = QLocale(QLocale::English).toDateTime(finiR,"yyyy-MM-ddThh:mm:ss.zzz");
            QDateTime actual = QDateTime::currentDateTime();

            bool withinRange = (actual >= dateIni && actual <= dateFin);

            if(estate==0){
                if(withinRange==true){
                    QMessageBox::about(this, "Advertencia", "Cajon no disponible");
                    return;
                }
            }
        }

        /*obtener si hay pension*/
        QString sifechentre2 = "SELECT fecha_inicio,fecha_fin FROM pension WHERE idCajon = " + idCaj;

        QSqlQuery sifechen2;
        sifechen2.exec(sifechentre2);

        while(sifechen2.next()){
            QString iniR2 = sifechen2.value("fecha_inicio").toString();
            QString finiR2 = sifechen2.value("fecha_fin").toString();

            QDate dateIni2 = QDate::fromString(iniR2,"yyyy-MM-dd");
            QDate dateFin2 = QDate::fromString(finiR2,"yyyy-MM-dd");
            QDate actual2 = QDate::currentDate();

            bool withinRange2 = (actual2 >= dateIni2 && actual2 <= dateFin2);

            if(withinRange2==true){
                QMessageBox::about(this, "Advertencia", "Cajon no disponible");
                return;
            }
        }
    }

    /*----------------------------------------------ANIMACION---------------------------------------*/
    Animacion2(140);
    ui->Piso2num1->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
    Descansar();
    /*----------------------------------------------------------------------------------------------*/


    if(reservacion== "Normal"){
        ui->stackedWidget->setCurrentIndex(7);
    }
    if(reservacion== "Premium"){
        ui->stackedWidget->setCurrentIndex(8);
    }
    if(reservacion== "Emergencia"){
        ui->stackedWidget->setCurrentIndex(6);
    }

    ui->Piso2num1->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:none;");

}
void MainWindow::on_Piso2num2_clicked(){
    QString reservacion;
    reservacion = Tipo;

    piso = "2";
    cajon = "2";

    //consultar si el cajon está ocupado

    QSqlQuery consultcaj;
    QString idCaj=obtenerIDCajon();
    qDebug() << "ID Cajon: " << idCaj;
    QString concaj = "SELECT lleno FROM cajon WHERE idCajon = '" + idCaj + "'";
    consultcaj.exec(concaj);
    consultcaj.next();
    int llenocaj = consultcaj.value("lleno").toInt();

    if(llenocaj==1){
        /*obtener si hay reservacion*/
        QString sifechentre = "SELECT inicio,fin,estado FROM reservacion WHERE idCajon = " + idCaj;

        QSqlQuery sifechen;
        sifechen.exec(sifechentre);

        while(sifechen.next()){
            QString iniR = sifechen.value("inicio").toString();
            QString finiR = sifechen.value("fin").toString();
            int estate = sifechen.value("estado").toInt();

            qDebug() << "Estado: " << estate;

            QDateTime dateIni = QLocale(QLocale::English).toDateTime(iniR,"yyyy-MM-ddThh:mm:ss.zzz");
            QDateTime dateFin = QLocale(QLocale::English).toDateTime(finiR,"yyyy-MM-ddThh:mm:ss.zzz");
            QDateTime actual = QDateTime::currentDateTime();

            bool withinRange = (actual >= dateIni && actual <= dateFin);

            if(estate==0){
                if(withinRange==true){
                    QMessageBox::about(this, "Advertencia", "Cajon no disponible");
                    return;
                }
            }
        }

        /*obtener si hay pension*/
        QString sifechentre2 = "SELECT fecha_inicio,fecha_fin FROM pension WHERE idCajon = " + idCaj;

        QSqlQuery sifechen2;
        sifechen2.exec(sifechentre2);

        while(sifechen2.next()){
            QString iniR2 = sifechen2.value("fecha_inicio").toString();
            QString finiR2 = sifechen2.value("fecha_fin").toString();

            QDate dateIni2 = QDate::fromString(iniR2,"yyyy-MM-dd");
            QDate dateFin2 = QDate::fromString(finiR2,"yyyy-MM-dd");
            QDate actual2 = QDate::currentDate();

            bool withinRange2 = (actual2 >= dateIni2 && actual2 <= dateFin2);

            if(withinRange2==true){
                QMessageBox::about(this, "Advertencia", "Cajon no disponible");
                return;
            }
        }
    }

    /*----------------------------------------------ANIMACION---------------------------------------*/
    Animacion2(250);
    ui->Piso2num2->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
    Descansar();
    /*----------------------------------------------------------------------------------------------*/


    if(reservacion== "Normal"){
        ui->stackedWidget->setCurrentIndex(7);
    }
    if(reservacion== "Premium"){
        ui->stackedWidget->setCurrentIndex(8);
    }
    if(reservacion== "Emergencia"){
        ui->stackedWidget->setCurrentIndex(6);
    }
    ui->Piso2num2->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:none;");

}
void MainWindow::on_Piso2num3_clicked(){
    QString reservacion;
    reservacion = Tipo;

    piso = "2";
    cajon = "3";

    //consultar si el cajon está ocupado

    QSqlQuery consultcaj;
    QString idCaj=obtenerIDCajon();
    qDebug() << "ID Cajon: " << idCaj;
    QString concaj = "SELECT lleno FROM cajon WHERE idCajon = '" + idCaj + "'";
    consultcaj.exec(concaj);
    consultcaj.next();
    int llenocaj = consultcaj.value("lleno").toInt();

    if(llenocaj==1){
        /*obtener si hay reservacion*/
        QString sifechentre = "SELECT inicio,fin,estado FROM reservacion WHERE idCajon = " + idCaj;

        QSqlQuery sifechen;
        sifechen.exec(sifechentre);

        while(sifechen.next()){
            QString iniR = sifechen.value("inicio").toString();
            QString finiR = sifechen.value("fin").toString();
            int estate = sifechen.value("estado").toInt();

            qDebug() << "Estado: " << estate;

            QDateTime dateIni = QLocale(QLocale::English).toDateTime(iniR,"yyyy-MM-ddThh:mm:ss.zzz");
            QDateTime dateFin = QLocale(QLocale::English).toDateTime(finiR,"yyyy-MM-ddThh:mm:ss.zzz");
            QDateTime actual = QDateTime::currentDateTime();

            bool withinRange = (actual >= dateIni && actual <= dateFin);

            if(estate==0){
                if(withinRange==true){
                    QMessageBox::about(this, "Advertencia", "Cajon no disponible");
                    return;
                }
            }
        }

        /*obtener si hay pension*/
        QString sifechentre2 = "SELECT fecha_inicio,fecha_fin FROM pension WHERE idCajon = " + idCaj;

        QSqlQuery sifechen2;
        sifechen2.exec(sifechentre2);

        while(sifechen2.next()){
            QString iniR2 = sifechen2.value("fecha_inicio").toString();
            QString finiR2 = sifechen2.value("fecha_fin").toString();

            QDate dateIni2 = QDate::fromString(iniR2,"yyyy-MM-dd");
            QDate dateFin2 = QDate::fromString(finiR2,"yyyy-MM-dd");
            QDate actual2 = QDate::currentDate();

            bool withinRange2 = (actual2 >= dateIni2 && actual2 <= dateFin2);

            if(withinRange2==true){
                QMessageBox::about(this, "Advertencia", "Cajon no disponible");
                return;
            }
        }
    }

    /*----------------------------------------------ANIMACION---------------------------------------*/
    Animacion2(360);
    ui->Piso2num3->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
    Descansar();
    /*----------------------------------------------------------------------------------------------*/

    if(reservacion== "Normal"){
        ui->stackedWidget->setCurrentIndex(7);
    }
    if(reservacion== "Premium"){
        ui->stackedWidget->setCurrentIndex(8);
    }
    if(reservacion== "Emergencia"){
        ui->stackedWidget->setCurrentIndex(6);
    }

    ui->Piso2num3->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:none;");

}
void MainWindow::on_Piso2num4_clicked(){
    QString reservacion;
    reservacion = Tipo;

    piso = "2";
    cajon = "4";

    //consultar si el cajon está ocupado

    QSqlQuery consultcaj;
    QString idCaj=obtenerIDCajon();
    qDebug() << "ID Cajon: " << idCaj;
    QString concaj = "SELECT lleno FROM cajon WHERE idCajon = '" + idCaj + "'";
    consultcaj.exec(concaj);
    consultcaj.next();
    int llenocaj = consultcaj.value("lleno").toInt();

    if(llenocaj==1){
        /*obtener si hay reservacion*/
        QString sifechentre = "SELECT inicio,fin,estado FROM reservacion WHERE idCajon = " + idCaj;

        QSqlQuery sifechen;
        sifechen.exec(sifechentre);

        while(sifechen.next()){
            QString iniR = sifechen.value("inicio").toString();
            QString finiR = sifechen.value("fin").toString();
            int estate = sifechen.value("estado").toInt();

            qDebug() << "Estado: " << estate;

            QDateTime dateIni = QLocale(QLocale::English).toDateTime(iniR,"yyyy-MM-ddThh:mm:ss.zzz");
            QDateTime dateFin = QLocale(QLocale::English).toDateTime(finiR,"yyyy-MM-ddThh:mm:ss.zzz");
            QDateTime actual = QDateTime::currentDateTime();

            bool withinRange = (actual >= dateIni && actual <= dateFin);

            if(estate==0){
                if(withinRange==true){
                    QMessageBox::about(this, "Advertencia", "Cajon no disponible");
                    return;
                }
            }
        }

        /*obtener si hay pension*/
        QString sifechentre2 = "SELECT fecha_inicio,fecha_fin FROM pension WHERE idCajon = " + idCaj;

        QSqlQuery sifechen2;
        sifechen2.exec(sifechentre2);

        while(sifechen2.next()){
            QString iniR2 = sifechen2.value("fecha_inicio").toString();
            QString finiR2 = sifechen2.value("fecha_fin").toString();

            QDate dateIni2 = QDate::fromString(iniR2,"yyyy-MM-dd");
            QDate dateFin2 = QDate::fromString(finiR2,"yyyy-MM-dd");
            QDate actual2 = QDate::currentDate();

            bool withinRange2 = (actual2 >= dateIni2 && actual2 <= dateFin2);

            if(withinRange2==true){
                QMessageBox::about(this, "Advertencia", "Cajon no disponible");
                return;
            }
        }
    }

    /*----------------------------------------------ANIMACION---------------------------------------*/
    Animacion2(470);
    ui->Piso2num3->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
    Descansar();
    /*----------------------------------------------------------------------------------------------*/
    reservacion = Tipo;
    if(reservacion== "Normal"){
        ui->stackedWidget->setCurrentIndex(7);
    }
    if(reservacion== "Premium"){
        ui->stackedWidget->setCurrentIndex(8);
    }
    if(reservacion== "Emergencia"){
        ui->stackedWidget->setCurrentIndex(6);
    }
    ui->Piso2num4->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:none;");

}
void MainWindow::on_Piso2num5_clicked(){
    QString reservacion;
    reservacion = Tipo;

    piso = "2";
    cajon = "5";

    //consultar si el cajon está ocupado

    QSqlQuery consultcaj;
    QString idCaj=obtenerIDCajon();
    qDebug() << "ID Cajon: " << idCaj;
    QString concaj = "SELECT lleno FROM cajon WHERE idCajon = '" + idCaj + "'";
    consultcaj.exec(concaj);
    consultcaj.next();
    int llenocaj = consultcaj.value("lleno").toInt();

    if(llenocaj==1){
        /*obtener si hay reservacion*/
        QString sifechentre = "SELECT inicio,fin,estado FROM reservacion WHERE idCajon = " + idCaj;

        QSqlQuery sifechen;
        sifechen.exec(sifechentre);

        while(sifechen.next()){
            QString iniR = sifechen.value("inicio").toString();
            QString finiR = sifechen.value("fin").toString();
            int estate = sifechen.value("estado").toInt();

            qDebug() << "Estado: " << estate;

            QDateTime dateIni = QLocale(QLocale::English).toDateTime(iniR,"yyyy-MM-ddThh:mm:ss.zzz");
            QDateTime dateFin = QLocale(QLocale::English).toDateTime(finiR,"yyyy-MM-ddThh:mm:ss.zzz");
            QDateTime actual = QDateTime::currentDateTime();

            bool withinRange = (actual >= dateIni && actual <= dateFin);

            if(estate==0){
                if(withinRange==true){
                    QMessageBox::about(this, "Advertencia", "Cajon no disponible");
                    return;
                }
            }
        }

        /*obtener si hay pension*/
        QString sifechentre2 = "SELECT fecha_inicio,fecha_fin FROM pension WHERE idCajon = " + idCaj;

        QSqlQuery sifechen2;
        sifechen2.exec(sifechentre2);

        while(sifechen2.next()){
            QString iniR2 = sifechen2.value("fecha_inicio").toString();
            QString finiR2 = sifechen2.value("fecha_fin").toString();

            QDate dateIni2 = QDate::fromString(iniR2,"yyyy-MM-dd");
            QDate dateFin2 = QDate::fromString(finiR2,"yyyy-MM-dd");
            QDate actual2 = QDate::currentDate();

            bool withinRange2 = (actual2 >= dateIni2 && actual2 <= dateFin2);

            if(withinRange2==true){
                QMessageBox::about(this, "Advertencia", "Cajon no disponible");
                return;
            }
        }
    }

    /*----------------------------------------------ANIMACION---------------------------------------*/
    Animacion2(580);
    ui->Piso2num5->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
    Descansar();
    /*----------------------------------------------------------------------------------------------*/


    if(reservacion== "Normal"){
        ui->stackedWidget->setCurrentIndex(7);
    }
    if(reservacion== "Premium"){
        ui->stackedWidget->setCurrentIndex(8);
    }
    if(reservacion== "Emergencia"){
        ui->stackedWidget->setCurrentIndex(6);
    }
    ui->Piso2num5->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:none;");

}
void MainWindow::on_Piso2num6_clicked(){
    QString reservacion;
    reservacion = Tipo;

    piso = "2";
    cajon = "6";

    //consultar si el cajon está ocupado

    QSqlQuery consultcaj;
    QString idCaj=obtenerIDCajon();
    qDebug() << "ID Cajon: " << idCaj;
    QString concaj = "SELECT lleno FROM cajon WHERE idCajon = '" + idCaj + "'";
    consultcaj.exec(concaj);
    consultcaj.next();
    int llenocaj = consultcaj.value("lleno").toInt();

    if(llenocaj==1){
        /*obtener si hay reservacion*/
        QString sifechentre = "SELECT inicio,fin,estado FROM reservacion WHERE idCajon = " + idCaj;

        QSqlQuery sifechen;
        sifechen.exec(sifechentre);

        while(sifechen.next()){
            QString iniR = sifechen.value("inicio").toString();
            QString finiR = sifechen.value("fin").toString();
            int estate = sifechen.value("estado").toInt();

            qDebug() << "Estado: " << estate;

            QDateTime dateIni = QLocale(QLocale::English).toDateTime(iniR,"yyyy-MM-ddThh:mm:ss.zzz");
            QDateTime dateFin = QLocale(QLocale::English).toDateTime(finiR,"yyyy-MM-ddThh:mm:ss.zzz");
            QDateTime actual = QDateTime::currentDateTime();

            bool withinRange = (actual >= dateIni && actual <= dateFin);

            if(estate==0){
                if(withinRange==true){
                    QMessageBox::about(this, "Advertencia", "Cajon no disponible");
                    return;
                }
            }
        }

        /*obtener si hay pension*/
        QString sifechentre2 = "SELECT fecha_inicio,fecha_fin FROM pension WHERE idCajon = " + idCaj;

        QSqlQuery sifechen2;
        sifechen2.exec(sifechentre2);

        while(sifechen2.next()){
            QString iniR2 = sifechen2.value("fecha_inicio").toString();
            QString finiR2 = sifechen2.value("fecha_fin").toString();

            QDate dateIni2 = QDate::fromString(iniR2,"yyyy-MM-dd");
            QDate dateFin2 = QDate::fromString(finiR2,"yyyy-MM-dd");
            QDate actual2 = QDate::currentDate();

            bool withinRange2 = (actual2 >= dateIni2 && actual2 <= dateFin2);

            if(withinRange2==true){
                QMessageBox::about(this, "Advertencia", "Cajon no disponible");
                return;
            }
        }
    }

    /*----------------------------------------------ANIMACION---------------------------------------*/
    Animacion2(140);
    ui->Piso2num6->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/abajo.png);");
    Descansar();
    /*----------------------------------------------------------------------------------------------*/

    if(reservacion== "Normal"){
        ui->stackedWidget->setCurrentIndex(7);
    }
    if(reservacion== "Premium"){
        ui->stackedWidget->setCurrentIndex(8);
    }
    if(reservacion== "Emergencia"){
        ui->stackedWidget->setCurrentIndex(6);
    }

    ui->Piso2num6->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:none;");

}
void MainWindow::on_Piso2num7_clicked(){
    QString reservacion;
    reservacion = Tipo;

    piso = "2";
    cajon = "7";

    //consultar si el cajon está ocupado

    QSqlQuery consultcaj;
    QString idCaj=obtenerIDCajon();
    qDebug() << "ID Cajon: " << idCaj;
    QString concaj = "SELECT lleno FROM cajon WHERE idCajon = '" + idCaj + "'";
    consultcaj.exec(concaj);
    consultcaj.next();
    int llenocaj = consultcaj.value("lleno").toInt();

    if(llenocaj==1){
        /*obtener si hay reservacion*/
        QString sifechentre = "SELECT inicio,fin,estado FROM reservacion WHERE idCajon = " + idCaj;

        QSqlQuery sifechen;
        sifechen.exec(sifechentre);

        while(sifechen.next()){
            QString iniR = sifechen.value("inicio").toString();
            QString finiR = sifechen.value("fin").toString();
            int estate = sifechen.value("estado").toInt();

            qDebug() << "Estado: " << estate;

            QDateTime dateIni = QLocale(QLocale::English).toDateTime(iniR,"yyyy-MM-ddThh:mm:ss.zzz");
            QDateTime dateFin = QLocale(QLocale::English).toDateTime(finiR,"yyyy-MM-ddThh:mm:ss.zzz");
            QDateTime actual = QDateTime::currentDateTime();

            bool withinRange = (actual >= dateIni && actual <= dateFin);

            if(estate==0){
                if(withinRange==true){
                    QMessageBox::about(this, "Advertencia", "Cajon no disponible");
                    return;
                }
            }
        }

        /*obtener si hay pension*/
        QString sifechentre2 = "SELECT fecha_inicio,fecha_fin FROM pension WHERE idCajon = " + idCaj;

        QSqlQuery sifechen2;
        sifechen2.exec(sifechentre2);

        while(sifechen2.next()){
            QString iniR2 = sifechen2.value("fecha_inicio").toString();
            QString finiR2 = sifechen2.value("fecha_fin").toString();

            QDate dateIni2 = QDate::fromString(iniR2,"yyyy-MM-dd");
            QDate dateFin2 = QDate::fromString(finiR2,"yyyy-MM-dd");
            QDate actual2 = QDate::currentDate();

            bool withinRange2 = (actual2 >= dateIni2 && actual2 <= dateFin2);

            if(withinRange2==true){
                QMessageBox::about(this, "Advertencia", "Cajon no disponible");
                return;
            }
        }
    }

    /*----------------------------------------------ANIMACION---------------------------------------*/
    Animacion2(250);
    ui->Piso2num7->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/abajo.png);");
    Descansar();
    /*----------------------------------------------------------------------------------------------*/

    if(reservacion== "Normal"){
        ui->stackedWidget->setCurrentIndex(7);
    }
    if(reservacion== "Premium"){
        ui->stackedWidget->setCurrentIndex(8);
    }
    if(reservacion== "Emergencia"){
        ui->stackedWidget->setCurrentIndex(6);
    }

    ui->Piso2num7->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:none;");

}
void MainWindow::on_Piso2num8_clicked(){
    QString reservacion;
    reservacion = Tipo;

    piso = "2";
    cajon = "8";

    //consultar si el cajon está ocupado

    QSqlQuery consultcaj;
    QString idCaj=obtenerIDCajon();
    qDebug() << "ID Cajon: " << idCaj;
    QString concaj = "SELECT lleno FROM cajon WHERE idCajon = '" + idCaj + "'";
    consultcaj.exec(concaj);
    consultcaj.next();
    int llenocaj = consultcaj.value("lleno").toInt();

    if(llenocaj==1){
        /*obtener si hay reservacion*/
        QString sifechentre = "SELECT inicio,fin,estado FROM reservacion WHERE idCajon = " + idCaj;

        QSqlQuery sifechen;
        sifechen.exec(sifechentre);

        while(sifechen.next()){
            QString iniR = sifechen.value("inicio").toString();
            QString finiR = sifechen.value("fin").toString();
            int estate = sifechen.value("estado").toInt();

            qDebug() << "Estado: " << estate;

            QDateTime dateIni = QLocale(QLocale::English).toDateTime(iniR,"yyyy-MM-ddThh:mm:ss.zzz");
            QDateTime dateFin = QLocale(QLocale::English).toDateTime(finiR,"yyyy-MM-ddThh:mm:ss.zzz");
            QDateTime actual = QDateTime::currentDateTime();

            bool withinRange = (actual >= dateIni && actual <= dateFin);

            if(estate==0){
                if(withinRange==true){
                    QMessageBox::about(this, "Advertencia", "Cajon no disponible");
                    return;
                }
            }
        }

        /*obtener si hay pension*/
        QString sifechentre2 = "SELECT fecha_inicio,fecha_fin FROM pension WHERE idCajon = " + idCaj;

        QSqlQuery sifechen2;
        sifechen2.exec(sifechentre2);

        while(sifechen2.next()){
            QString iniR2 = sifechen2.value("fecha_inicio").toString();
            QString finiR2 = sifechen2.value("fecha_fin").toString();

            QDate dateIni2 = QDate::fromString(iniR2,"yyyy-MM-dd");
            QDate dateFin2 = QDate::fromString(finiR2,"yyyy-MM-dd");
            QDate actual2 = QDate::currentDate();

            bool withinRange2 = (actual2 >= dateIni2 && actual2 <= dateFin2);

            if(withinRange2==true){
                QMessageBox::about(this, "Advertencia", "Cajon no disponible");
                return;
            }
        }
    }

    /*----------------------------------------------ANIMACION---------------------------------------*/
    Animacion2(360);
    ui->Piso2num8->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/abajo.png);");
    Descansar();
    /*----------------------------------------------------------------------------------------------*/

    if(reservacion== "Normal"){
        ui->stackedWidget->setCurrentIndex(7);
    }
    if(reservacion== "Premium"){
        ui->stackedWidget->setCurrentIndex(8);
    }
    if(reservacion== "Emergencia"){
        ui->stackedWidget->setCurrentIndex(6);
    }
    ui->Piso2num8->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:none;");

}
void MainWindow::on_Piso2num9_clicked(){
    QString reservacion;
    reservacion = Tipo;

    piso = "2";
    cajon = "9";

    //consultar si el cajon está ocupado

    QSqlQuery consultcaj;
    QString idCaj=obtenerIDCajon();
    qDebug() << "ID Cajon: " << idCaj;
    QString concaj = "SELECT lleno FROM cajon WHERE idCajon = '" + idCaj + "'";
    consultcaj.exec(concaj);
    consultcaj.next();
    int llenocaj = consultcaj.value("lleno").toInt();

    if(llenocaj==1){
        /*obtener si hay reservacion*/
        QString sifechentre = "SELECT inicio,fin,estado FROM reservacion WHERE idCajon = " + idCaj;

        QSqlQuery sifechen;
        sifechen.exec(sifechentre);

        while(sifechen.next()){
            QString iniR = sifechen.value("inicio").toString();
            QString finiR = sifechen.value("fin").toString();
            int estate = sifechen.value("estado").toInt();

            qDebug() << "Estado: " << estate;

            QDateTime dateIni = QLocale(QLocale::English).toDateTime(iniR,"yyyy-MM-ddThh:mm:ss.zzz");
            QDateTime dateFin = QLocale(QLocale::English).toDateTime(finiR,"yyyy-MM-ddThh:mm:ss.zzz");
            QDateTime actual = QDateTime::currentDateTime();

            bool withinRange = (actual >= dateIni && actual <= dateFin);

            if(estate==0){
                if(withinRange==true){
                    QMessageBox::about(this, "Advertencia", "Cajon no disponible");
                    return;
                }
            }
        }

        /*obtener si hay pension*/
        QString sifechentre2 = "SELECT fecha_inicio,fecha_fin FROM pension WHERE idCajon = " + idCaj;

        QSqlQuery sifechen2;
        sifechen2.exec(sifechentre2);

        while(sifechen2.next()){
            QString iniR2 = sifechen2.value("fecha_inicio").toString();
            QString finiR2 = sifechen2.value("fecha_fin").toString();

            QDate dateIni2 = QDate::fromString(iniR2,"yyyy-MM-dd");
            QDate dateFin2 = QDate::fromString(finiR2,"yyyy-MM-dd");
            QDate actual2 = QDate::currentDate();

            bool withinRange2 = (actual2 >= dateIni2 && actual2 <= dateFin2);

            if(withinRange2==true){
                QMessageBox::about(this, "Advertencia", "Cajon no disponible");
                return;
            }
        }
    }

    /*----------------------------------------------ANIMACION---------------------------------------*/
    Animacion2(470);
    ui->Piso2num9->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/abajo.png);");
    Descansar();
    /*----------------------------------------------------------------------------------------------*/

    if(reservacion== "Normal"){
        ui->stackedWidget->setCurrentIndex(7);
    }
    if(reservacion== "Premium"){
        ui->stackedWidget->setCurrentIndex(8);
    }
    if(reservacion== "Emergencia"){
        ui->stackedWidget->setCurrentIndex(6);
    }

    ui->Piso2num9->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:none;");

}
void MainWindow::on_Piso2num10_clicked(){
    QString reservacion;
    reservacion = Tipo;

    piso = "2";
    cajon = "10";

    //consultar si el cajon está ocupado

    QSqlQuery consultcaj;
    QString idCaj=obtenerIDCajon();
    qDebug() << "ID Cajon: " << idCaj;
    QString concaj = "SELECT lleno FROM cajon WHERE idCajon = '" + idCaj + "'";
    consultcaj.exec(concaj);
    consultcaj.next();
    int llenocaj = consultcaj.value("lleno").toInt();

    if(llenocaj==1){
        /*obtener si hay reservacion*/
        QString sifechentre = "SELECT inicio,fin,estado FROM reservacion WHERE idCajon = " + idCaj;

        QSqlQuery sifechen;
        sifechen.exec(sifechentre);

        while(sifechen.next()){
            QString iniR = sifechen.value("inicio").toString();
            QString finiR = sifechen.value("fin").toString();
            int estate = sifechen.value("estado").toInt();

            qDebug() << "Estado: " << estate;

            QDateTime dateIni = QLocale(QLocale::English).toDateTime(iniR,"yyyy-MM-ddThh:mm:ss.zzz");
            QDateTime dateFin = QLocale(QLocale::English).toDateTime(finiR,"yyyy-MM-ddThh:mm:ss.zzz");
            QDateTime actual = QDateTime::currentDateTime();

            bool withinRange = (actual >= dateIni && actual <= dateFin);

            if(estate==0){
                if(withinRange==true){
                    QMessageBox::about(this, "Advertencia", "Cajon no disponible");
                    return;
                }
            }
        }

        /*obtener si hay pension*/
        QString sifechentre2 = "SELECT fecha_inicio,fecha_fin FROM pension WHERE idCajon = " + idCaj;

        QSqlQuery sifechen2;
        sifechen2.exec(sifechentre2);

        while(sifechen2.next()){
            QString iniR2 = sifechen2.value("fecha_inicio").toString();
            QString finiR2 = sifechen2.value("fecha_fin").toString();

            QDate dateIni2 = QDate::fromString(iniR2,"yyyy-MM-dd");
            QDate dateFin2 = QDate::fromString(finiR2,"yyyy-MM-dd");
            QDate actual2 = QDate::currentDate();

            bool withinRange2 = (actual2 >= dateIni2 && actual2 <= dateFin2);

            if(withinRange2==true){
                QMessageBox::about(this, "Advertencia", "Cajon no disponible");
                return;
            }
        }
    }

    /*----------------------------------------------ANIMACION---------------------------------------*/
    Animacion2(580);
    ui->Piso2num10->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/abajo.png);");
    Descansar();
    /*----------------------------------------------------------------------------------------------*/

    if(reservacion== "Normal"){
        ui->stackedWidget->setCurrentIndex(7);
    }
    if(reservacion== "Premium"){
        ui->stackedWidget->setCurrentIndex(8);
    }
    if(reservacion== "Emergencia"){
        ui->stackedWidget->setCurrentIndex(6);
    }
    ui->Piso2num10->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:none;");

}
/*****************************************************************************************************************************/

/*------------------------------------------ INTERFAZ ESTACIONAMIENTO TERCER PISO--------------------------------------------*/
void MainWindow::on_Piso3num1_clicked(){
    QString reservacion;
    reservacion = Tipo;

    piso = "3";
    cajon = "1";

    //consultar si el cajon está ocupado

    QSqlQuery consultcaj;
    QString idCaj=obtenerIDCajon();
    qDebug() << "ID Cajon: " << idCaj;
    QString concaj = "SELECT lleno FROM cajon WHERE idCajon = '" + idCaj + "'";
    consultcaj.exec(concaj);
    consultcaj.next();
    int llenocaj = consultcaj.value("lleno").toInt();

    if(llenocaj==1){
        /*obtener si hay reservacion*/
        QString sifechentre = "SELECT inicio,fin,estado FROM reservacion WHERE idCajon = " + idCaj;

        QSqlQuery sifechen;
        sifechen.exec(sifechentre);

        while(sifechen.next()){
            QString iniR = sifechen.value("inicio").toString();
            QString finiR = sifechen.value("fin").toString();
            int estate = sifechen.value("estado").toInt();

            qDebug() << "Estado: " << estate;

            QDateTime dateIni = QLocale(QLocale::English).toDateTime(iniR,"yyyy-MM-ddThh:mm:ss.zzz");
            QDateTime dateFin = QLocale(QLocale::English).toDateTime(finiR,"yyyy-MM-ddThh:mm:ss.zzz");
            QDateTime actual = QDateTime::currentDateTime();

            bool withinRange = (actual >= dateIni && actual <= dateFin);

            if(estate==0){
                if(withinRange==true){
                    QMessageBox::about(this, "Advertencia", "Cajon no disponible");
                    return;
                }
            }
        }

        /*obtener si hay pension*/
        QString sifechentre2 = "SELECT fecha_inicio,fecha_fin FROM pension WHERE idCajon = " + idCaj;

        QSqlQuery sifechen2;
        sifechen2.exec(sifechentre2);

        while(sifechen2.next()){
            QString iniR2 = sifechen2.value("fecha_inicio").toString();
            QString finiR2 = sifechen2.value("fecha_fin").toString();

            QDate dateIni2 = QDate::fromString(iniR2,"yyyy-MM-dd");
            QDate dateFin2 = QDate::fromString(finiR2,"yyyy-MM-dd");
            QDate actual2 = QDate::currentDate();

            bool withinRange2 = (actual2 >= dateIni2 && actual2 <= dateFin2);

            if(withinRange2==true){
                QMessageBox::about(this, "Advertencia", "Cajon no disponible");
                return;
            }
        }
    }

    /*----------------------------------------------ANIMACION---------------------------------------*/
    Animacion3(140);
    ui->Piso3num1->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
    Descansar();
    /*----------------------------------------------------------------------------------------------*/

    if(reservacion== "Normal"){
        ui->stackedWidget->setCurrentIndex(7);
    }
    if(reservacion== "Premium"){
        ui->stackedWidget->setCurrentIndex(8);
    }
    if(reservacion== "Emergencia"){
        ui->stackedWidget->setCurrentIndex(6);
    }
    ui->Piso3num1->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:none;");

}
void MainWindow::on_Piso3num2_clicked(){
    QString reservacion;
    reservacion = Tipo;

    piso = "3";
    cajon = "2";

    //consultar si el cajon está ocupado

    QSqlQuery consultcaj;
    QString idCaj=obtenerIDCajon();
    qDebug() << "ID Cajon: " << idCaj;
    QString concaj = "SELECT lleno FROM cajon WHERE idCajon = '" + idCaj + "'";
    consultcaj.exec(concaj);
    consultcaj.next();
    int llenocaj = consultcaj.value("lleno").toInt();

    if(llenocaj==1){
        /*obtener si hay reservacion*/
        QString sifechentre = "SELECT inicio,fin,estado FROM reservacion WHERE idCajon = " + idCaj;

        QSqlQuery sifechen;
        sifechen.exec(sifechentre);

        while(sifechen.next()){
            QString iniR = sifechen.value("inicio").toString();
            QString finiR = sifechen.value("fin").toString();
            int estate = sifechen.value("estado").toInt();

            qDebug() << "Estado: " << estate;

            QDateTime dateIni = QLocale(QLocale::English).toDateTime(iniR,"yyyy-MM-ddThh:mm:ss.zzz");
            QDateTime dateFin = QLocale(QLocale::English).toDateTime(finiR,"yyyy-MM-ddThh:mm:ss.zzz");
            QDateTime actual = QDateTime::currentDateTime();

            bool withinRange = (actual >= dateIni && actual <= dateFin);

            if(estate==0){
                if(withinRange==true){
                    QMessageBox::about(this, "Advertencia", "Cajon no disponible");
                    return;
                }
            }
        }

        /*obtener si hay pension*/
        QString sifechentre2 = "SELECT fecha_inicio,fecha_fin FROM pension WHERE idCajon = " + idCaj;

        QSqlQuery sifechen2;
        sifechen2.exec(sifechentre2);

        while(sifechen2.next()){
            QString iniR2 = sifechen2.value("fecha_inicio").toString();
            QString finiR2 = sifechen2.value("fecha_fin").toString();

            QDate dateIni2 = QDate::fromString(iniR2,"yyyy-MM-dd");
            QDate dateFin2 = QDate::fromString(finiR2,"yyyy-MM-dd");
            QDate actual2 = QDate::currentDate();

            bool withinRange2 = (actual2 >= dateIni2 && actual2 <= dateFin2);

            if(withinRange2==true){
                QMessageBox::about(this, "Advertencia", "Cajon no disponible");
                return;
            }
        }
    }

    /*----------------------------------------------ANIMACION---------------------------------------*/
    Animacion3(250);
    ui->Piso3num2->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
    Descansar();
    /*----------------------------------------------------------------------------------------------*/

    if(reservacion== "Normal"){
        ui->stackedWidget->setCurrentIndex(7);
    }
    if(reservacion== "Premium"){
        ui->stackedWidget->setCurrentIndex(8);
    }
    if(reservacion== "Emergencia"){
        ui->stackedWidget->setCurrentIndex(6);
    }
    ui->Piso3num2->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:none;");

}
void MainWindow::on_Piso3num3_clicked(){
    QString reservacion;
    reservacion = Tipo;

    piso = "3";
    cajon = "3";

    //consultar si el cajon está ocupado

    QSqlQuery consultcaj;
    QString idCaj=obtenerIDCajon();
    qDebug() << "ID Cajon: " << idCaj;
    QString concaj = "SELECT lleno FROM cajon WHERE idCajon = '" + idCaj + "'";
    consultcaj.exec(concaj);
    consultcaj.next();
    int llenocaj = consultcaj.value("lleno").toInt();

    if(llenocaj==1){
        /*obtener si hay reservacion*/
        QString sifechentre = "SELECT inicio,fin,estado FROM reservacion WHERE idCajon = " + idCaj;

        QSqlQuery sifechen;
        sifechen.exec(sifechentre);

        while(sifechen.next()){
            QString iniR = sifechen.value("inicio").toString();
            QString finiR = sifechen.value("fin").toString();
            int estate = sifechen.value("estado").toInt();

            qDebug() << "Estado: " << estate;

            QDateTime dateIni = QLocale(QLocale::English).toDateTime(iniR,"yyyy-MM-ddThh:mm:ss.zzz");
            QDateTime dateFin = QLocale(QLocale::English).toDateTime(finiR,"yyyy-MM-ddThh:mm:ss.zzz");
            QDateTime actual = QDateTime::currentDateTime();

            bool withinRange = (actual >= dateIni && actual <= dateFin);

            if(estate==0){
                if(withinRange==true){
                    QMessageBox::about(this, "Advertencia", "Cajon no disponible");
                    return;
                }
            }
        }

        /*obtener si hay pension*/
        QString sifechentre2 = "SELECT fecha_inicio,fecha_fin FROM pension WHERE idCajon = " + idCaj;

        QSqlQuery sifechen2;
        sifechen2.exec(sifechentre2);

        while(sifechen2.next()){
            QString iniR2 = sifechen2.value("fecha_inicio").toString();
            QString finiR2 = sifechen2.value("fecha_fin").toString();

            QDate dateIni2 = QDate::fromString(iniR2,"yyyy-MM-dd");
            QDate dateFin2 = QDate::fromString(finiR2,"yyyy-MM-dd");
            QDate actual2 = QDate::currentDate();

            bool withinRange2 = (actual2 >= dateIni2 && actual2 <= dateFin2);

            if(withinRange2==true){
                QMessageBox::about(this, "Advertencia", "Cajon no disponible");
                return;
            }
        }
    }

    /*----------------------------------------------ANIMACION---------------------------------------*/
    Animacion3(360);
    ui->Piso3num3->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
    Descansar();
    /*----------------------------------------------------------------------------------------------*/
    if(reservacion== "Normal"){
        ui->stackedWidget->setCurrentIndex(7);
    }
    if(reservacion== "Premium"){
        ui->stackedWidget->setCurrentIndex(8);
    }
    if(reservacion== "Emergencia"){
        ui->stackedWidget->setCurrentIndex(6);
    }
    ui->Piso3num3->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:none;");

}
void MainWindow::on_Piso3num4_clicked(){
    QString reservacion;
    reservacion = Tipo;

    piso = "3";
    cajon = "4";

    //consultar si el cajon está ocupado

    QSqlQuery consultcaj;
    QString idCaj=obtenerIDCajon();
    qDebug() << "ID Cajon: " << idCaj;
    QString concaj = "SELECT lleno FROM cajon WHERE idCajon = '" + idCaj + "'";
    consultcaj.exec(concaj);
    consultcaj.next();
    int llenocaj = consultcaj.value("lleno").toInt();

    if(llenocaj==1){
        /*obtener si hay reservacion*/
        QString sifechentre = "SELECT inicio,fin,estado FROM reservacion WHERE idCajon = " + idCaj;

        QSqlQuery sifechen;
        sifechen.exec(sifechentre);

        while(sifechen.next()){
            QString iniR = sifechen.value("inicio").toString();
            QString finiR = sifechen.value("fin").toString();
            int estate = sifechen.value("estado").toInt();

            qDebug() << "Estado: " << estate;

            QDateTime dateIni = QLocale(QLocale::English).toDateTime(iniR,"yyyy-MM-ddThh:mm:ss.zzz");
            QDateTime dateFin = QLocale(QLocale::English).toDateTime(finiR,"yyyy-MM-ddThh:mm:ss.zzz");
            QDateTime actual = QDateTime::currentDateTime();

            bool withinRange = (actual >= dateIni && actual <= dateFin);

            if(estate==0){
                if(withinRange==true){
                    QMessageBox::about(this, "Advertencia", "Cajon no disponible");
                    return;
                }
            }
        }

        /*obtener si hay pension*/
        QString sifechentre2 = "SELECT fecha_inicio,fecha_fin FROM pension WHERE idCajon = " + idCaj;

        QSqlQuery sifechen2;
        sifechen2.exec(sifechentre2);

        while(sifechen2.next()){
            QString iniR2 = sifechen2.value("fecha_inicio").toString();
            QString finiR2 = sifechen2.value("fecha_fin").toString();

            QDate dateIni2 = QDate::fromString(iniR2,"yyyy-MM-dd");
            QDate dateFin2 = QDate::fromString(finiR2,"yyyy-MM-dd");
            QDate actual2 = QDate::currentDate();

            bool withinRange2 = (actual2 >= dateIni2 && actual2 <= dateFin2);

            if(withinRange2==true){
                QMessageBox::about(this, "Advertencia", "Cajon no disponible");
                return;
            }
        }
    }

    /*----------------------------------------------ANIMACION---------------------------------------*/
    Animacion3(470);
    ui->Piso3num4->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
    Descansar();
    /*----------------------------------------------------------------------------------------------*/
    if(reservacion== "Normal"){
        ui->stackedWidget->setCurrentIndex(7);
    }
    if(reservacion== "Premium"){
        ui->stackedWidget->setCurrentIndex(8);
    }
    if(reservacion== "Emergencia"){
        ui->stackedWidget->setCurrentIndex(6);
    }
    ui->Piso3num4->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:none;");

}
void MainWindow::on_Piso3num5_clicked(){
    QString reservacion;
    reservacion = Tipo;

    piso = "3";
    cajon = "5";

    //consultar si el cajon está ocupado

    QSqlQuery consultcaj;
    QString idCaj=obtenerIDCajon();
    qDebug() << "ID Cajon: " << idCaj;
    QString concaj = "SELECT lleno FROM cajon WHERE idCajon = '" + idCaj + "'";
    consultcaj.exec(concaj);
    consultcaj.next();
    int llenocaj = consultcaj.value("lleno").toInt();

    if(llenocaj==1){
        /*obtener si hay reservacion*/
        QString sifechentre = "SELECT inicio,fin,estado FROM reservacion WHERE idCajon = " + idCaj;

        QSqlQuery sifechen;
        sifechen.exec(sifechentre);

        while(sifechen.next()){
            QString iniR = sifechen.value("inicio").toString();
            QString finiR = sifechen.value("fin").toString();
            int estate = sifechen.value("estado").toInt();

            qDebug() << "Estado: " << estate;

            QDateTime dateIni = QLocale(QLocale::English).toDateTime(iniR,"yyyy-MM-ddThh:mm:ss.zzz");
            QDateTime dateFin = QLocale(QLocale::English).toDateTime(finiR,"yyyy-MM-ddThh:mm:ss.zzz");
            QDateTime actual = QDateTime::currentDateTime();

            bool withinRange = (actual >= dateIni && actual <= dateFin);

            if(estate==0){
                if(withinRange==true){
                    QMessageBox::about(this, "Advertencia", "Cajon no disponible");
                    return;
                }
            }
        }

        /*obtener si hay pension*/
        QString sifechentre2 = "SELECT fecha_inicio,fecha_fin FROM pension WHERE idCajon = " + idCaj;

        QSqlQuery sifechen2;
        sifechen2.exec(sifechentre2);

        while(sifechen2.next()){
            QString iniR2 = sifechen2.value("fecha_inicio").toString();
            QString finiR2 = sifechen2.value("fecha_fin").toString();

            QDate dateIni2 = QDate::fromString(iniR2,"yyyy-MM-dd");
            QDate dateFin2 = QDate::fromString(finiR2,"yyyy-MM-dd");
            QDate actual2 = QDate::currentDate();

            bool withinRange2 = (actual2 >= dateIni2 && actual2 <= dateFin2);

            if(withinRange2==true){
                QMessageBox::about(this, "Advertencia", "Cajon no disponible");
                return;
            }
        }
    }

    /*----------------------------------------------ANIMACION---------------------------------------*/
    Animacion3(580);
    ui->Piso3num5->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/arriba.png);");
    Descansar();
    /*----------------------------------------------------------------------------------------------*/
    if(reservacion== "Normal"){
        ui->stackedWidget->setCurrentIndex(7);
    }
    if(reservacion== "Premium"){
        ui->stackedWidget->setCurrentIndex(8);
    }
    if(reservacion== "Emergencia"){
        ui->stackedWidget->setCurrentIndex(6);
    }
    ui->Piso3num5->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:none;");
}
void MainWindow::on_Piso3num6_clicked(){
    QString reservacion;
    reservacion = Tipo;

    piso = "3";
    cajon = "6";

    //consultar si el cajon está ocupado

    QSqlQuery consultcaj;
    QString idCaj=obtenerIDCajon();
    qDebug() << "ID Cajon: " << idCaj;
    QString concaj = "SELECT lleno FROM cajon WHERE idCajon = '" + idCaj + "'";
    consultcaj.exec(concaj);
    consultcaj.next();
    int llenocaj = consultcaj.value("lleno").toInt();

    if(llenocaj==1){
        /*obtener si hay reservacion*/
        QString sifechentre = "SELECT inicio,fin,estado FROM reservacion WHERE idCajon = " + idCaj;

        QSqlQuery sifechen;
        sifechen.exec(sifechentre);

        while(sifechen.next()){
            QString iniR = sifechen.value("inicio").toString();
            QString finiR = sifechen.value("fin").toString();
            int estate = sifechen.value("estado").toInt();

            qDebug() << "Estado: " << estate;

            QDateTime dateIni = QLocale(QLocale::English).toDateTime(iniR,"yyyy-MM-ddThh:mm:ss.zzz");
            QDateTime dateFin = QLocale(QLocale::English).toDateTime(finiR,"yyyy-MM-ddThh:mm:ss.zzz");
            QDateTime actual = QDateTime::currentDateTime();

            bool withinRange = (actual >= dateIni && actual <= dateFin);

            if(estate==0){
                if(withinRange==true){
                    QMessageBox::about(this, "Advertencia", "Cajon no disponible");
                    return;
                }
            }
        }

        /*obtener si hay pension*/
        QString sifechentre2 = "SELECT fecha_inicio,fecha_fin FROM pension WHERE idCajon = " + idCaj;

        QSqlQuery sifechen2;
        sifechen2.exec(sifechentre2);

        while(sifechen2.next()){
            QString iniR2 = sifechen2.value("fecha_inicio").toString();
            QString finiR2 = sifechen2.value("fecha_fin").toString();

            QDate dateIni2 = QDate::fromString(iniR2,"yyyy-MM-dd");
            QDate dateFin2 = QDate::fromString(finiR2,"yyyy-MM-dd");
            QDate actual2 = QDate::currentDate();

            bool withinRange2 = (actual2 >= dateIni2 && actual2 <= dateFin2);

            if(withinRange2==true){
                QMessageBox::about(this, "Advertencia", "Cajon no disponible");
                return;
            }
        }
    }

    /*----------------------------------------------ANIMACION---------------------------------------*/
    Animacion3(140);
    ui->Piso3num6->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/abajo.png);");
    Descansar();
    /*----------------------------------------------------------------------------------------------*/
    if(reservacion== "Normal"){
        ui->stackedWidget->setCurrentIndex(7);
    }
    if(reservacion== "Premium"){
        ui->stackedWidget->setCurrentIndex(8);
    }
    if(reservacion== "Emergencia"){
        ui->stackedWidget->setCurrentIndex(6);
    }
    ui->Piso3num6->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:none;");
}
void MainWindow::on_Piso3num7_clicked(){
    QString reservacion;
    reservacion = Tipo;

    piso = "3";
    cajon = "7";

    //consultar si el cajon está ocupado

    QSqlQuery consultcaj;
    QString idCaj=obtenerIDCajon();
    qDebug() << "ID Cajon: " << idCaj;
    QString concaj = "SELECT lleno FROM cajon WHERE idCajon = '" + idCaj + "'";
    consultcaj.exec(concaj);
    consultcaj.next();
    int llenocaj = consultcaj.value("lleno").toInt();

    if(llenocaj==1){
        /*obtener si hay reservacion*/
        QString sifechentre = "SELECT inicio,fin,estado FROM reservacion WHERE idCajon = " + idCaj;

        QSqlQuery sifechen;
        sifechen.exec(sifechentre);

        while(sifechen.next()){
            QString iniR = sifechen.value("inicio").toString();
            QString finiR = sifechen.value("fin").toString();
            int estate = sifechen.value("estado").toInt();

            qDebug() << "Estado: " << estate;

            QDateTime dateIni = QLocale(QLocale::English).toDateTime(iniR,"yyyy-MM-ddThh:mm:ss.zzz");
            QDateTime dateFin = QLocale(QLocale::English).toDateTime(finiR,"yyyy-MM-ddThh:mm:ss.zzz");
            QDateTime actual = QDateTime::currentDateTime();

            bool withinRange = (actual >= dateIni && actual <= dateFin);

            if(estate==0){
                if(withinRange==true){
                    QMessageBox::about(this, "Advertencia", "Cajon no disponible");
                    return;
                }
            }
        }

        /*obtener si hay pension*/
        QString sifechentre2 = "SELECT fecha_inicio,fecha_fin FROM pension WHERE idCajon = " + idCaj;

        QSqlQuery sifechen2;
        sifechen2.exec(sifechentre2);

        while(sifechen2.next()){
            QString iniR2 = sifechen2.value("fecha_inicio").toString();
            QString finiR2 = sifechen2.value("fecha_fin").toString();

            QDate dateIni2 = QDate::fromString(iniR2,"yyyy-MM-dd");
            QDate dateFin2 = QDate::fromString(finiR2,"yyyy-MM-dd");
            QDate actual2 = QDate::currentDate();

            bool withinRange2 = (actual2 >= dateIni2 && actual2 <= dateFin2);

            if(withinRange2==true){
                QMessageBox::about(this, "Advertencia", "Cajon no disponible");
                return;
            }
        }
    }

    /*----------------------------------------------ANIMACION---------------------------------------*/
    Animacion3(250);
    ui->Piso3num7->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/abajo.png);");
    Descansar();
    /*----------------------------------------------------------------------------------------------*/
    if(reservacion== "Normal"){
        ui->stackedWidget->setCurrentIndex(7);
    }
    if(reservacion== "Premium"){
        ui->stackedWidget->setCurrentIndex(8);
    }
    if(reservacion== "Emergencia"){
        ui->stackedWidget->setCurrentIndex(6);
    }
    ui->Piso3num7->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:none;");

}
void MainWindow::on_Piso3num8_clicked(){
    QString reservacion;
    reservacion = Tipo;

    piso = "3";
    cajon = "8";

    //consultar si el cajon está ocupado

    QSqlQuery consultcaj;
    QString idCaj=obtenerIDCajon();
    qDebug() << "ID Cajon: " << idCaj;
    QString concaj = "SELECT lleno FROM cajon WHERE idCajon = '" + idCaj + "'";
    consultcaj.exec(concaj);
    consultcaj.next();
    int llenocaj = consultcaj.value("lleno").toInt();

    if(llenocaj==1){
        /*obtener si hay reservacion*/
        QString sifechentre = "SELECT inicio,fin,estado FROM reservacion WHERE idCajon = " + idCaj;

        QSqlQuery sifechen;
        sifechen.exec(sifechentre);

        while(sifechen.next()){
            QString iniR = sifechen.value("inicio").toString();
            QString finiR = sifechen.value("fin").toString();
            int estate = sifechen.value("estado").toInt();

            qDebug() << "Estado: " << estate;

            QDateTime dateIni = QLocale(QLocale::English).toDateTime(iniR,"yyyy-MM-ddThh:mm:ss.zzz");
            QDateTime dateFin = QLocale(QLocale::English).toDateTime(finiR,"yyyy-MM-ddThh:mm:ss.zzz");
            QDateTime actual = QDateTime::currentDateTime();

            bool withinRange = (actual >= dateIni && actual <= dateFin);

            if(estate==0){
                if(withinRange==true){
                    QMessageBox::about(this, "Advertencia", "Cajon no disponible");
                    return;
                }
            }
        }

        /*obtener si hay pension*/
        QString sifechentre2 = "SELECT fecha_inicio,fecha_fin FROM pension WHERE idCajon = " + idCaj;

        QSqlQuery sifechen2;
        sifechen2.exec(sifechentre2);

        while(sifechen2.next()){
            QString iniR2 = sifechen2.value("fecha_inicio").toString();
            QString finiR2 = sifechen2.value("fecha_fin").toString();

            QDate dateIni2 = QDate::fromString(iniR2,"yyyy-MM-dd");
            QDate dateFin2 = QDate::fromString(finiR2,"yyyy-MM-dd");
            QDate actual2 = QDate::currentDate();

            bool withinRange2 = (actual2 >= dateIni2 && actual2 <= dateFin2);

            if(withinRange2==true){
                QMessageBox::about(this, "Advertencia", "Cajon no disponible");
                return;
            }
        }
    }

    /*----------------------------------------------ANIMACION---------------------------------------*/
    Animacion3(360);
    ui->Piso3num8->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/abajo.png);");
    Descansar();
    /*----------------------------------------------------------------------------------------------*/
    if(reservacion== "Normal"){
        ui->stackedWidget->setCurrentIndex(7);
    }
    if(reservacion== "Premium"){
        ui->stackedWidget->setCurrentIndex(8);
    }
    if(reservacion== "Emergencia"){
        ui->stackedWidget->setCurrentIndex(6);
    }
    ui->Piso3num8->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:none;");

}
void MainWindow::on_Piso3num9_clicked(){
    QString reservacion;
    reservacion = Tipo;

    piso = "3";
    cajon = "9";

    //consultar si el cajon está ocupado

    QSqlQuery consultcaj;
    QString idCaj=obtenerIDCajon();
    qDebug() << "ID Cajon: " << idCaj;
    QString concaj = "SELECT lleno FROM cajon WHERE idCajon = '" + idCaj + "'";
    consultcaj.exec(concaj);
    consultcaj.next();
    int llenocaj = consultcaj.value("lleno").toInt();

    if(llenocaj==1){
        /*obtener si hay reservacion*/
        QString sifechentre = "SELECT inicio,fin,estado FROM reservacion WHERE idCajon = " + idCaj;

        QSqlQuery sifechen;
        sifechen.exec(sifechentre);

        while(sifechen.next()){
            QString iniR = sifechen.value("inicio").toString();
            QString finiR = sifechen.value("fin").toString();
            int estate = sifechen.value("estado").toInt();

            qDebug() << "Estado: " << estate;

            QDateTime dateIni = QLocale(QLocale::English).toDateTime(iniR,"yyyy-MM-ddThh:mm:ss.zzz");
            QDateTime dateFin = QLocale(QLocale::English).toDateTime(finiR,"yyyy-MM-ddThh:mm:ss.zzz");
            QDateTime actual = QDateTime::currentDateTime();

            bool withinRange = (actual >= dateIni && actual <= dateFin);

            if(estate==0){
                if(withinRange==true){
                    QMessageBox::about(this, "Advertencia", "Cajon no disponible");
                    return;
                }
            }
        }

        /*obtener si hay pension*/
        QString sifechentre2 = "SELECT fecha_inicio,fecha_fin FROM pension WHERE idCajon = " + idCaj;

        QSqlQuery sifechen2;
        sifechen2.exec(sifechentre2);

        while(sifechen2.next()){
            QString iniR2 = sifechen2.value("fecha_inicio").toString();
            QString finiR2 = sifechen2.value("fecha_fin").toString();

            QDate dateIni2 = QDate::fromString(iniR2,"yyyy-MM-dd");
            QDate dateFin2 = QDate::fromString(finiR2,"yyyy-MM-dd");
            QDate actual2 = QDate::currentDate();

            bool withinRange2 = (actual2 >= dateIni2 && actual2 <= dateFin2);

            if(withinRange2==true){
                QMessageBox::about(this, "Advertencia", "Cajon no disponible");
                return;
            }
        }
    }

    /*----------------------------------------------ANIMACION---------------------------------------*/
    Animacion3(470);
    ui->Piso3num9->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/abajo.png);");
    Descansar();
    /*----------------------------------------------------------------------------------------------*/
    if(reservacion== "Normal"){
        ui->stackedWidget->setCurrentIndex(7);
    }
    if(reservacion== "Premium"){
        ui->stackedWidget->setCurrentIndex(8);
    }
    if(reservacion== "Emergencia"){
        ui->stackedWidget->setCurrentIndex(6);
    }
    ui->Piso3num9->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:none;");

}
void MainWindow::on_Piso3num10_clicked(){
    QString reservacion;
    reservacion = Tipo;

    piso = "3";
    cajon = "10";

    //consultar si el cajon está ocupado

    QSqlQuery consultcaj;
    QString idCaj=obtenerIDCajon();
    qDebug() << "ID Cajon: " << idCaj;
    QString concaj = "SELECT lleno FROM cajon WHERE idCajon = '" + idCaj + "'";
    consultcaj.exec(concaj);
    consultcaj.next();
    int llenocaj = consultcaj.value("lleno").toInt();

    if(llenocaj==1){
        /*obtener si hay reservacion*/
        QString sifechentre = "SELECT inicio,fin,estado FROM reservacion WHERE idCajon = " + idCaj;

        QSqlQuery sifechen;
        sifechen.exec(sifechentre);

        while(sifechen.next()){
            QString iniR = sifechen.value("inicio").toString();
            QString finiR = sifechen.value("fin").toString();
            int estate = sifechen.value("estado").toInt();

            qDebug() << "Estado: " << estate;

            QDateTime dateIni = QLocale(QLocale::English).toDateTime(iniR,"yyyy-MM-ddThh:mm:ss.zzz");
            QDateTime dateFin = QLocale(QLocale::English).toDateTime(finiR,"yyyy-MM-ddThh:mm:ss.zzz");
            QDateTime actual = QDateTime::currentDateTime();

            bool withinRange = (actual >= dateIni && actual <= dateFin);

            if(estate==0){
                if(withinRange==true){
                    QMessageBox::about(this, "Advertencia", "Cajon no disponible");
                    return;
                }
            }
        }

        /*obtener si hay pension*/
        QString sifechentre2 = "SELECT fecha_inicio,fecha_fin FROM pension WHERE idCajon = " + idCaj;

        QSqlQuery sifechen2;
        sifechen2.exec(sifechentre2);

        while(sifechen2.next()){
            QString iniR2 = sifechen2.value("fecha_inicio").toString();
            QString finiR2 = sifechen2.value("fecha_fin").toString();

            QDate dateIni2 = QDate::fromString(iniR2,"yyyy-MM-dd");
            QDate dateFin2 = QDate::fromString(finiR2,"yyyy-MM-dd");
            QDate actual2 = QDate::currentDate();

            bool withinRange2 = (actual2 >= dateIni2 && actual2 <= dateFin2);

            if(withinRange2==true){
                QMessageBox::about(this, "Advertencia", "Cajon no disponible");
                return;
            }
        }
    }

    /*----------------------------------------------ANIMACION---------------------------------------*/
    Animacion3(580);
    ui->Piso3num10->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:url(:/Imagenes/abajo.png);");
    Descansar();
    /*----------------------------------------------------------------------------------------------*/
    if(reservacion== "Normal"){
        ui->stackedWidget->setCurrentIndex(7);
    }
    if(reservacion== "Premium"){
        ui->stackedWidget->setCurrentIndex(8);
    }
    if(reservacion== "Emergencia"){
        ui->stackedWidget->setCurrentIndex(6);
    }
    ui->Piso3num10->setStyleSheet("font-size:28px; background-color: #757575; border: 2px solid black; image:none;");

}
/********************************************************************************************************************************/


/*----------------------------------------- INTERFAZ RESERVACION DE EMERGENCIA--------------------------------------------------*/

void MainWindow::on_regresar_4_clicked(){
    ui->stackedWidget->setCurrentIndex(5);
}
void MainWindow::on_autofoto_clicked(){
    auto nombreArchivo = QFileDialog::getOpenFileName(this,"abrir imagen",QDir::rootPath(),"imagenes(.png *.jpg *.jpeg);; cualquier archivo(.*)");
     QString ruta=nombreArchivo;
     QFile archivo(ruta);
     if(!archivo.open(QIODevice::ReadOnly))
     {
         qDebug() << "Imagen no reconocida";
         return;
     }
     QByteArray mostrar=archivo.readAll();
     QPixmap imag;
     imag.loadFromData(mostrar);
     ui->fotouser_4->setPixmap(imag.scaled(180,150));


     ruta2=ruta;
     qDebug() << ruta2;
}
void MainWindow::on_reservarcajon_clicked(){
    qDebug() << ruta2;
    QString inic,finic,descauto,matauto,rutimgauto,modelauto;

    rutimgauto = ruta2;
    matauto = ui->Matricula_3->text();
    modelauto = ui->Modelo_3->text();
    descauto = ui->descripcion_3->toPlainText();

    /*------------------------Verificamos que los campos no esten vacios---------------------*/
    if(matauto.isEmpty() || modelauto.isEmpty() || descauto.isEmpty() || rutimgauto.isEmpty()){
        QMessageBox::about(this, "Error", "Campos Vacios");
        return;
    }
    /*---------------------------------------------------------------------------------------*/

    /*-------------Verificamos que no se exceda el tiempo limite de la reserva---------------*/
    QTime tem1 = ui->hora_inicio->time();
    QTime tem2 = ui->hora_fin->time();

    QDateTime fech3 = QDateTime::currentDateTime();

    int seguns = tem1.secsTo(tem2);
    qDebug() << "Segundos entre: " << seguns;

    if(seguns<3600 || seguns > 18000){
        QMessageBox::about(this, "Error", "La reservacion excede el tiempo");
        return;
    }
    /*---------------------------------------------------------------------------------------*/

    QDateTime fech1(QDate::currentDate(),tem1);
    QDateTime fech2(QDate::currentDate(),tem2);

    /*-------------Verificamos que no choque con otra reserva--------------------------------*/

    /*Obtener si el tiempo de la reservacion no interrumpe con otra*/

    QString idInterrumpCaj =obtenerIDCajon();
    qDebug() << "ID Cajon: " << idInterrumpCaj;

    QSqlQuery consultacaj111;
    QString concaj111 = "SELECT lleno FROM cajon WHERE idCajon = '" + idInterrumpCaj + "'";
    consultacaj111.exec(concaj111);
    consultacaj111.next();

    int llenocaj111 = consultacaj111.value("lleno").toInt();

    if(llenocaj111 == 1){
        /*obtener si interrumpe una reservacion*/
        QString sifechentre000 = "SELECT inicio,fin,estado FROM reservacion WHERE idCajon = " + idInterrumpCaj;

        QSqlQuery sifechen00;
        sifechen00.exec(sifechentre000);

        while(sifechen00.next()){
            QString iniR00 = sifechen00.value("inicio").toString();
            QString finiR00 = sifechen00.value("fin").toString();

            int estate = sifechen00.value("estado").toInt();

            qDebug() << "Estado: " << estate;

            QDateTime dateIni00 = QLocale(QLocale::English).toDateTime(iniR00,"yyyy-MM-ddThh:mm:ss.zzz");
            QDateTime dateFin00 = QLocale(QLocale::English).toDateTime(finiR00,"yyyy-MM-ddThh:mm:ss.zzz");

            bool withinRange00 = (fech1 >= dateIni00 && fech1 <= dateFin00);
            bool withinRange000 = (fech2 >= dateIni00 && fech2 <= dateFin00);
            bool withinRange0000 = (dateIni00 >= fech1 && dateIni00 <= fech2);
            bool withinRange00000 = (dateFin00 >= fech1 && dateFin00 <= fech2);

            if(estate==0){
                if(withinRange00==true || withinRange000==true || withinRange0000==true || withinRange00000==true){
                    QMessageBox::about(this, "Advertencia", "Cajon no disponible durante ese rango de tiempo");
                    return;
                }
            }
        }

        /*obtener si interrumpe una pension*/
        QDate inicio000 = QDate::currentDate();
        QDate fin000 = QDate::currentDate();

        QString sifechentre2000 = "SELECT fecha_inicio,fecha_fin FROM pension WHERE idCajon = " + idInterrumpCaj;

        QSqlQuery sifechen2000;
        sifechen2000.exec(sifechentre2000);

        while(sifechen2000.next()){
            QString iniR200 = sifechen2000.value("fecha_inicio").toString();
            QString finiR200 = sifechen2000.value("fecha_fin").toString();

            QDate dateIni200 = QDate::fromString(iniR200,"yyyy-MM-dd");
            QDate dateFin200 = QDate::fromString(finiR200,"yyyy-MM-dd");

            bool withinRange200 = (inicio000 >= dateIni200 && inicio000 <= dateFin200);
            bool withinRange2000 = (fin000 >= dateIni200 && fin000 <= dateFin200);
            bool withinRange20000 = (dateIni200 >= inicio000 && dateIni200 <= fin000);
            bool withinRange200000 = (dateFin200 >= inicio000 && dateFin200 <= fin000);

            if(withinRange200==true || withinRange2000==true || withinRange20000 == true || withinRange200000 == true){
                QMessageBox::about(this, "Advertencia", "Cajon no disponible durante ese rango de tiempo");
                return;
            }
        }
    }

    /*---------------------------------------------------------------------------------------*/

    /*-----------------------------------obtener fecha inicio------------------------------- */
    int diainic = fech3.date().day();
    QString diainicst = QString::number(diainic);
    int mesinic = fech3.date().month();
    QString mesinicst = QString::number(mesinic);
    int aninic = fech3.date().year();
    QString aninicst = QString::number(aninic);
    QString horinicst = tem1.toString(); //Obtenemos la hora incio
    QString inicfechfull = aninicst + "-" + mesinicst + "-" + diainicst + " " + horinicst;
    /*----------------------------------------------------------------------------------------*/

    /*------------------------------------otener fecha fin------------------------------------*/
    QString horfincst = tem2.toString(); //Obtenemos la hora final
    QString finfechfull = aninicst + "-" + mesinicst + "-" + diainicst + " " + horfincst;

        QSqlQuery insfotaut;
        QString queryinsertfot = "INSERT INTO fotos(ruta,imagen) VALUES('" + rutimgauto + "', LOAD_FILE('" + rutimgauto + "'))";

        if(insfotaut.exec(queryinsertfot)){
            QString queryobtmaxfot = "SELECT MAX(idFotos) FROM fotos";
            QSqlQuery maxidfot;

            maxidfot.exec(queryobtmaxfot);
            while ( maxidfot.next()){
                queryobtmaxfot = maxidfot.value(0).toString();
            }

            qDebug()<< queryobtmaxfot;

            QString nombredelus = username;
            qDebug()<< "Nombre de Usuario: " << nombredelus;

            QString obtidusuario = "select * from usuario where usuario = '"+ nombredelus +"'";

            qDebug()<< obtidusuario;

            QSqlQuery obtidus;
            obtidus.exec(obtidusuario);
            obtidus.next();

            QString obtidusua = obtidus.value("idUsuario").toString();

            qDebug()<< "Id usuario: " << obtidusua;

            QSqlQuery insauto;
            QString insertarauto = "INSERT INTO vehiculo(matricula,modelo,descripcion,idCliente,idFotos) VALUES('" + matauto + "','" + modelauto + "','" + descauto + "','" + obtidusua + "','" + queryobtmaxfot + "')";

            if(insauto.exec(insertarauto)){
                QString queryobtmaxaut = "SELECT MAX(idVehiculo) FROM vehiculo";
                QSqlQuery maxidaut;

                maxidaut.exec(queryobtmaxaut);
                while ( maxidaut.next()){
                    queryobtmaxaut = maxidaut.value(0).toString();
                }

                qDebug()<< queryobtmaxaut;


                /*----------------------------Obtenemos el id del cajon-------------------------*/
                QString id3=obtenerIDCajon();

                /*-----------------------------------------------------------------------------*/

                QString insertreserv = "INSERT INTO reservacion(inicio,fin,pagoxhora,idCliente,idCajon,idAuto,estado) VALUES('" + inicfechfull + "','" + finfechfull + "','"+ pagoxhor +"','" + obtidusua + "','"+ id3 +"','" + queryobtmaxaut + "','0')";
                QSqlQuery crearreserv;

                if(crearreserv.exec(insertreserv)){



                    /*------------------------------Creamos la transaccion normal----------------------------------*/

                    /*-----------------Obtenemos el id de la reservacion----------------*/
                    QSqlQuery idReservacion;

                    QString idReserva,dato;
                    dato="select idReservacion from Reservacion where idCliente='"+obtidusua+"'and idCajon='"+id3+"';";


                    if(idReservacion.exec(dato)){

                        qDebug()<<"id encontrado";


                        while(idReservacion.next())
                        {
                            idReserva=idReservacion.value(0).toString();
                        }

                        QSqlQuery transaccionQ;
                        QString dato="insert into transaccion(total_a_pagar,liquidado,idReservacion)values('"+pagoxhor+"','0', '"+idReserva+"');";

                        if(transaccionQ.exec(dato)){
                            qDebug()<<"Transaccion ejecutada";

                            /*Actualizar el cajon*/
                             QSqlQuery llencaj;
                             qDebug() << "Num cajon: " << cajon;
                             QString llenarcaj = "UPDATE cajon SET lleno = '1' WHERE idCajon = '" + id3 + "'";

                             llencaj.exec(llenarcaj);
                             llencaj.next();

                             QMessageBox::about(this, "Confirmacion", "Reservacion creada exitosamente");
                             ui->stackedWidget->setCurrentIndex(4);

                         }else{
                            qDebug() << "Error al crear la reservacion";
                        }

                    }else{
                        qDebug() << "Id de la reservación no encontrado";
                    }
                }else{
                    qDebug() << "Reservacion no creada";
                }

            }else{
                qDebug() << "Error insertando tabla vehiculo";
            }

        }else{
            qDebug() << "Error insertando tabla foto";
        }
}
/******************************************************************************************************************************/

/*--------------------------------------------- INTERFAZ RESERVACION NORMAL---------------------------------------------------*/

void MainWindow::on_regresar_5_clicked(){
    ui->stackedWidget->setCurrentIndex(5);
}
void MainWindow::on_autofoto_2_clicked(){
    auto nombreArchivo = QFileDialog::getOpenFileName(this,"abrir imagen",QDir::rootPath(),"imagenes(.png *.jpg *.jpeg);; cualquier archivo(.*)");
     QString ruta=nombreArchivo;
     QFile archivo(ruta);
     if(!archivo.open(QIODevice::ReadOnly))
     {
         qDebug() << "Imagen no reconocida";
         return;
     }
     QByteArray mostrar=archivo.readAll();
     QPixmap imag;
     imag.loadFromData(mostrar);
     ui->fotouser_2->setPixmap(imag.scaled(180,150));

     ruta1=ruta;
     qDebug() << ruta1;
}
void MainWindow::on_reservarcajon_2_clicked(){
    qDebug() << ruta1;
    QString inic,finic,descauto,matauto,rutimgauto,modelauto;

    rutimgauto = ruta1;
    matauto = ui->Matricula->text();
    modelauto = ui->Modelo->text();
    descauto = ui->descripcion->toPlainText();

    if(matauto.isEmpty() || modelauto.isEmpty() || descauto.isEmpty() || rutimgauto.isEmpty()){
        QMessageBox::about(this, "Error", "Campos Vacios");
        return;
    }

    QDateTime fech1 = ui->inicio->dateTime();
    QDateTime fech2 = ui->fin->dateTime();

    QDateTime fech3 = QDateTime::currentDateTime();

    int eshoy1 = fech3.secsTo(fech1);
    int eshoy2 = fech3.secsTo(fech2);

    qDebug()<<eshoy1;

    if(eshoy1<=0 || eshoy2<=0){
        QMessageBox::about(this, "Error", "Fechas inválidas");
        return;
    }

    int seguns = fech1.secsTo(fech2);
    qDebug() << "Segundos entre: " << seguns;

    if(seguns<=60 || seguns > 82800){
        QMessageBox::about(this, "Error", "La reservacion no cumple el tiempo debido");
        return;
    }

    /*-------------------------------------------------------------*/

    /*Obtener si el tiempo de la reservacion no interrumpe con otra*/

    QString idInterrumpCaj =obtenerIDCajon();
    qDebug() << "ID Cajon: " << idInterrumpCaj;

    QSqlQuery consultacaj111;
    QString concaj111 = "SELECT lleno FROM cajon WHERE idCajon = '" + idInterrumpCaj + "'";
    consultacaj111.exec(concaj111);
    consultacaj111.next();

    int llenocaj111 = consultacaj111.value("lleno").toInt();

    if(llenocaj111 == 1){
        /*obtener si interrumpe una reservacion*/
        QString sifechentre000 = "SELECT inicio,fin,estado FROM reservacion WHERE idCajon = " + idInterrumpCaj;

        QSqlQuery sifechen00;
        sifechen00.exec(sifechentre000);

        while(sifechen00.next()){
            QString iniR00 = sifechen00.value("inicio").toString();
            QString finiR00 = sifechen00.value("fin").toString();

            int estate = sifechen00.value("estado").toInt();

            qDebug() << "Estado: " << estate;

            QDateTime dateIni00 = QLocale(QLocale::English).toDateTime(iniR00,"yyyy-MM-ddThh:mm:ss.zzz");
            QDateTime dateFin00 = QLocale(QLocale::English).toDateTime(finiR00,"yyyy-MM-ddThh:mm:ss.zzz");

            bool withinRange00 = (fech1 >= dateIni00 && fech1 <= dateFin00);
            bool withinRange000 = (fech2 >= dateIni00 && fech2 <= dateFin00);
            bool withinRange0000 = (dateIni00 >= fech1 && dateIni00 <= fech2);
            bool withinRange00000 = (dateFin00 >= fech1 && dateFin00 <= fech2);

            if(estate==0){
                if(withinRange00==true || withinRange000==true || withinRange0000==true || withinRange00000==true){
                    QMessageBox::about(this, "Advertencia", "Cajon no disponible durante ese rango de tiempo");
                    return;
                }
            }
        }

        /*obtener si interrumpe una pension*/
        QDate inicio000 = ui->inicio->date();
        QDate fin000 = ui->fin->date();

        QString sifechentre2000 = "SELECT fecha_inicio,fecha_fin FROM pension WHERE idCajon = " + idInterrumpCaj;

        QSqlQuery sifechen2000;
        sifechen2000.exec(sifechentre2000);

        while(sifechen2000.next()){
            QString iniR200 = sifechen2000.value("fecha_inicio").toString();
            QString finiR200 = sifechen2000.value("fecha_fin").toString();

            QDate dateIni200 = QDate::fromString(iniR200,"yyyy-MM-dd");
            QDate dateFin200 = QDate::fromString(finiR200,"yyyy-MM-dd");

            bool withinRange200 = (inicio000 >= dateIni200 && inicio000 <= dateFin200);
            bool withinRange2000 = (fin000 >= dateIni200 && fin000 <= dateFin200);
            bool withinRange20000 = (dateIni200 >= inicio000 && dateIni200 <= fin000);
            bool withinRange200000 = (dateFin200 >= inicio000 && dateFin200 <= fin000);

            if(withinRange200==true || withinRange2000==true || withinRange20000 == true || withinRange200000 == true){
                QMessageBox::about(this, "Advertencia", "Cajon no disponible durante ese rango de tiempo");
                return;
            }
        }
    }

    /*-------------------------------------------------------------*/

    //obtener fecha inicio
    int diainic = ui->inicio->date().day();
    QString diainicst = QString::number(diainic);
    int mesinic = ui->inicio->date().month();
    QString mesinicst = QString::number(mesinic);
    int aninic = ui->inicio->date().year();
    QString aninicst = QString::number(aninic);
    QTime horinic = ui->inicio->dateTime().time();
    QString horinicst = horinic.toString();
    QString inicfechfull = aninicst + "-" + mesinicst + "-" + diainicst + " " + horinicst;

    //otener fecha fin
    int diafin = ui->fin->date().day();
    QString diafincst = QString::number(diafin);
    int mesfin = ui->fin->date().month();
    QString mesfincst = QString::number(mesfin);
    int anifin = ui->fin->date().year();
    QString anifinst = QString::number(anifin);
    QTime horfin = ui->fin->dateTime().time();
    QString horfincst = horfin.toString();
    QString finfechfull = anifinst + "-" + mesfincst + "-" + diafincst + " " + horfincst;

    qDebug() << inicfechfull << " " << finfechfull;

    QSqlQuery insfotaut;
    QString queryinsertfot = "INSERT INTO fotos(ruta,imagen) VALUES('" + rutimgauto + "', LOAD_FILE('" + rutimgauto + "'))";

    if(insfotaut.exec(queryinsertfot)){
        QString queryobtmaxfot = "SELECT MAX(idFotos) FROM fotos";
        QSqlQuery maxidfot;

        maxidfot.exec(queryobtmaxfot);
        while ( maxidfot.next()){
            queryobtmaxfot = maxidfot.value(0).toString();
        }

        qDebug()<< queryobtmaxfot;

        QString nombredelus = username;

        QString obtidusuario = "select * from usuario where usuario = '"+ nombredelus +"'"; /*ID del usuario*/

        qDebug()<< obtidusuario;

        QSqlQuery obtidus;
        obtidus.exec(obtidusuario);
        obtidus.next();

        QString obtidusua = obtidus.value("idUsuario").toString();

        QSqlQuery insauto;
        QString insertarauto = "INSERT INTO vehiculo(matricula,modelo,descripcion,idCliente,idFotos) VALUES('" + matauto + "','" + modelauto + "','" + descauto + "','" + obtidusua + "','" + queryobtmaxfot + "')"; /*Se inserta el vehiculo*/

        if(insauto.exec(insertarauto)){
            QString queryobtmaxaut = "SELECT MAX(idVehiculo) FROM vehiculo";
            QSqlQuery maxidaut;

            maxidaut.exec(queryobtmaxaut);
            while ( maxidaut.next()){
                queryobtmaxaut = maxidaut.value(0).toString();
            }


            /*----------------------------Obtenemos el id del cajon-------------------------*/
            QString id3=obtenerIDCajon();

            /*------------------- AQUI CHECAR SI LA FECHA ESTA ENTRE OTRAS FECHAS */



            /*--------------------------------------------------------------------*/

            /*-----------------------------------------------------------------------------*/

            QString insertreserv = "INSERT INTO reservacion(inicio,fin,pagoxhora,idCliente,idCajon,idAuto,estado) VALUES('" + inicfechfull + "','" + finfechfull + "','"+ pagoxhor +"', '" + obtidusua + "', '"+ id3 +"','" + queryobtmaxaut + "', '0')";
            QSqlQuery crearreserv;

            if(crearreserv.exec(insertreserv)){

                /*------------------------------Creamos la transaccion normal----------------------------------*/

                /*-----------------Obtenemos el id de la reservacion----------------*/
                QSqlQuery idReservacion;
                QString idReserva, dato;
                dato="select idReservacion from Reservacion where idCliente='"+obtidusua+"'and idCajon='"+id3+"' ;";
                qDebug()<<dato;
                if(!idReservacion.exec(dato)){
                    qDebug()<<"id no encontrado";
                    qDebug()<<dato;
                    return;
                }
                while(idReservacion.next())
                {
                    idReserva=idReservacion.value(0).toString();
                }
                qDebug()<<"id de la reserva: "+idReserva;
                /*------------------------------------------------------------------------*/
                QString transaccion;
                transaccion= "insert into transaccion(total_a_pagar,liquidado,idReservacion)values('"+pagoxhor+"','false','"+idReserva+"');";
                QSqlQuery transaccionQ;
                if(!transaccionQ.exec(transaccion)){
                    qDebug()<<transaccion;
                    qDebug()<<"Transaccion no ejecutada";
                    return;
                }
                qDebug()<<"Transaccion generada";

                //llenar el cajon
                QSqlQuery llencaj;
                qDebug() << "Num cajon: " << id3;
                QString llenarcaj = "UPDATE cajon SET lleno = '1' WHERE idCajon = '" + id3 + "'";
                qDebug()<<llenarcaj;

                llencaj.exec(llenarcaj);
                llencaj.next();

                QMessageBox::about(this, "Confirmacion", "Reservacion creada exitosamente");
                ui->stackedWidget->setCurrentIndex(4);
            }else{
                QMessageBox::about(this, "Error", "Reservacion no creada");
            }

        }else{
            qDebug() << "Error insertando tabla vehiculo";
        }

    }else{
        qDebug() << "Error insertando tabla foto";
    }
}
/*********************************************************************************************************************************/


/*---------------------------------------- Interfaz Reservacion Premium---------------------------------------------------------*/

void MainWindow::on_regresar_6_clicked(){
    ui->Matricula_2->clear();
    ui->Modelo_2->clear();
    ui->descripcion_2->clear();
    ui->fotouser_3->clear();

    ui->stackedWidget->setCurrentIndex(5);
}
void MainWindow::on_autofoto_3_clicked(){
    auto nombreArchivo = QFileDialog::getOpenFileName(this,"abrir imagen",QDir::rootPath(),"imagenes(.png *.jpg *.jpeg);; cualquier archivo(.*)");
     QString ruta=nombreArchivo;
     QFile archivo(ruta);
     if(!archivo.open(QIODevice::ReadOnly))
     {
         qDebug() << "Imagen no reconocida";
         return;
     }
     QByteArray mostrar=archivo.readAll();
     QPixmap imag;
     imag.loadFromData(mostrar);
     ui->fotouser_3->setPixmap(imag.scaled(180,150));

     ruta3=ruta;
     qDebug() << ruta3;
}
void MainWindow::on_reservarcajon_3_clicked(){
    qDebug() << ruta3;
    QString inic,finic,descauto,matauto,rutimgauto,modelauto;

    rutimgauto = ruta3;
    matauto = ui->Matricula_2->text();
    modelauto = ui->Modelo_2->text();
    descauto = ui->descripcion_2->toPlainText();

    if(matauto.isEmpty() || modelauto.isEmpty() || descauto.isEmpty() || rutimgauto.isEmpty()){
        QMessageBox::about(this, "Error", "Campos Vacios");
        return;
    }

    QDate fech1 = ui->dia_inicio->date();
    QDate fech2 = ui->dia_fin->date();

    QDate fech3 = QDate::currentDate();

    int eshoy1 = fech3.daysTo(fech1);
    int eshoy2 = fech3.daysTo(fech2);

    if(eshoy1<0 || eshoy2<0){
        QMessageBox::about(this, "Error", "Fechas inválidas");
        return;
    }

    int dayst = fech1.daysTo(fech2);
    qDebug() << "Dias entre: " << dayst;

    if(dayst<1){
        QMessageBox::about(this, "Error", "La reservacion no cumple el tiempo minimo");
        return;
    }

    /*-------------------Verificar si no choca con otra reservacion----------------------*/

    QString idInterrumpCaj =obtenerIDCajon();
    qDebug() << "ID Cajon OCUPANDO: " << idInterrumpCaj;

    QSqlQuery consultacaj111;
    QString concaj111 = "SELECT lleno FROM cajon WHERE idCajon = '" + idInterrumpCaj + "'";
    consultacaj111.exec(concaj111);
    consultacaj111.next();

    int llenocaj111 = consultacaj111.value("lleno").toInt();

    qDebug() << "CAJON LLENO: " << llenocaj111;

    if(llenocaj111 == 1){
        /*obtener si interrumpe una reservacion*/
        QString sifechentre000 = "SELECT inicio,fin,estado FROM reservacion WHERE idCajon = " + idInterrumpCaj;

        QSqlQuery sifechen00;
        sifechen00.exec(sifechentre000);

        QDateTime tiempoInicio(fech1,QTime(0,0,0));
        QDateTime tiempoFinal(fech2,QTime(23,59,59));

        qDebug() << "Fechas de la reservacion: " << tiempoInicio << " " << tiempoFinal;

        while(sifechen00.next()){
            QString iniR00 = sifechen00.value("inicio").toString();
            QString finiR00 = sifechen00.value("fin").toString();

            int estate = sifechen00.value("estado").toInt();

            qDebug() << "Estado: " << estate;

            QDateTime dateIni00 = QLocale(QLocale::English).toDateTime(iniR00,"yyyy-MM-ddThh:mm:ss.zzz");
            QDateTime dateFin00 = QLocale(QLocale::English).toDateTime(finiR00,"yyyy-MM-ddThh:mm:ss.zzz");

            bool withinRange00 = (tiempoInicio >= dateIni00 && tiempoInicio <= dateFin00);
            bool withinRange000 = (tiempoFinal >= dateIni00 && tiempoFinal <= dateFin00);
            bool withinRange0000 = (dateIni00 >= tiempoInicio && dateIni00 <= tiempoFinal);
            bool withinRange00000 = (dateFin00 >= tiempoInicio && dateFin00 <= tiempoFinal);

            if(estate==0){
                if(withinRange00==true || withinRange000==true || withinRange0000==true || withinRange00000==true){
                    QMessageBox::about(this, "Advertencia", "Cajon no disponible durante ese rango de tiempo");
                    return;
                }
            }
        }

        /*obtener si interrumpe una pension*/
        QString sifechentre2000 = "SELECT fecha_inicio,fecha_fin FROM pension WHERE idCajon = " + idInterrumpCaj;

        QSqlQuery sifechen2000;
        sifechen2000.exec(sifechentre2000);

        while(sifechen2000.next()){
            QString iniR200 = sifechen2000.value("fecha_inicio").toString();
            QString finiR200 = sifechen2000.value("fecha_fin").toString();

            QDate dateIni200 = QDate::fromString(iniR200,"yyyy-MM-dd");
            QDate dateFin200 = QDate::fromString(finiR200,"yyyy-MM-dd");

            bool withinRange200 = (fech1 >= dateIni200 && fech1 <= dateFin200);
            bool withinRange2000 = (fech2 >= dateIni200 && fech2 <= dateFin200);
            bool withinRange20000 = (dateIni200 >= fech1 && dateIni200 <= fech2);
            bool withinRange200000 = (dateFin200 >= fech1 && dateFin200 <= fech2);

            if(withinRange200==true || withinRange2000==true || withinRange20000 == true || withinRange200000 == true){
                QMessageBox::about(this, "Advertencia", "Cajon no disponible durante ese rango de tiempo");
                return;
            }
        }

    }

    /*-----------------------------------------------------------------------------------*/

    //obtener fecha inicio
    int diainic = ui->dia_inicio->date().day();
    QString diainicst = QString::number(diainic);
    int mesinic = ui->dia_inicio->date().month();
    QString mesinicst = QString::number(mesinic);
    int aninic = ui->dia_inicio->date().year();
    QString aninicst = QString::number(aninic);
    QString inicfechfull = aninicst + "-" + mesinicst + "-" + diainicst;

    //otener fecha fin
    int diafin = ui->dia_fin->date().day();
    QString diafincst = QString::number(diafin);
    int mesfin = ui->dia_fin->date().month();
    QString mesfincst = QString::number(mesfin);
    int anifin = ui->dia_fin->date().year();
    QString anifinst = QString::number(anifin);
    QString finfechfull = anifinst + "-" + mesfincst + "-" + diafincst;

    qDebug() << inicfechfull << " " << finfechfull;

    QSqlQuery insfotaut;
    QString queryinsertfot = "INSERT INTO fotos(ruta,imagen) VALUES('" + rutimgauto + "', LOAD_FILE('" + rutimgauto + "'))";

    if(insfotaut.exec(queryinsertfot)){
        QString queryobtmaxfot = "SELECT MAX(idFotos) FROM fotos";
        QSqlQuery maxidfot;

        maxidfot.exec(queryobtmaxfot);
        while ( maxidfot.next()){
            queryobtmaxfot = maxidfot.value(0).toString();
        }

        qDebug()<< queryobtmaxfot;

        QString nombredelus = username;
        qDebug()<< "Nombre de Usuario: " << nombredelus;

        QString obtidusuario = "select * from usuario where usuario = '"+ nombredelus +"'";

        qDebug()<< obtidusuario;

        QSqlQuery obtidus;
        obtidus.exec(obtidusuario);
        obtidus.next();

        QString obtidusua = obtidus.value("idUsuario").toString();

        qDebug()<< "Id usuario: " << obtidusua;

        QSqlQuery insauto;
        QString insertarauto = "INSERT INTO vehiculo(matricula,modelo,descripcion,idCliente,idFotos) VALUES('" + matauto + "','" + modelauto + "','" + descauto + "','" + obtidusua + "','" + queryobtmaxfot + "')";

        if(insauto.exec(insertarauto)){
            QString queryobtmaxaut = "SELECT MAX(idVehiculo) FROM vehiculo";
            QSqlQuery maxidaut;

            maxidaut.exec(queryobtmaxaut);
            while ( maxidaut.next()){
                queryobtmaxaut = maxidaut.value(0).toString();
            }

            qDebug()<< queryobtmaxaut;

            /*-----------------------------------DONDE COMIENZA EL PROCESO-------------------------------------------*/
            /*----------------------------Obtenemos el id del cajon-------------------------*/
            QString id3=obtenerIDCajon();

            /*-----------------------------------------------------------------------------*/

            QString insertreserv = "INSERT INTO pension(fecha_inicio,fecha_fin,precioxdia,idCliente,idCajon,idAuto) VALUES('" + inicfechfull + "','" + finfechfull + "','" + pagoxdia + "','" + obtidusua + "','" + id3 + "','" + queryobtmaxaut + "')";

            QSqlQuery crearreserv;

            if(crearreserv.exec(insertreserv)){
                //llenar el cajon
                QSqlQuery llencaj;
                qDebug() << "Id cajon: " << id3;

                /********************Obtenemos el ID de la pension*****************************/
                QString idPen;
                QSqlQuery obtIDPen;

                idPen="SELECT idPension FROM pension WHERE idCajon='"+id3+"' AND idCliente='"+obtidusua+"'";
                obtIDPen.prepare(idPen);

                if(obtIDPen.exec()){
                    while(obtIDPen.next()){
                        idPen=obtIDPen.value(0).toString();}
                    qDebug() << "Id pension: " << idPen;


                    /********************Calculamos el total a pagar de la pension*****************/

                    //SELECT DATEDIFF(fecha_fin,fecha_inicio) as TDias FROM pension where idPension=1;


                    int tPagarI;
                    QString tPagar;
                    QString totalPagar;
                    QSqlQuery totalDias;
                    tPagar="SELECT DATEDIFF(fecha_fin,fecha_inicio) as tDias FROM pension where idPension = '"+idPen+"'";

                    totalDias.prepare(tPagar);
                    if(totalDias.exec()){
                        while(totalDias.next()){
                        tPagarI=totalDias.value(0).toInt();}

                        qDebug()<<"Total de dias"<<tPagarI;
                        tPagarI*=100; /*Multiplicamos el total de los dias por el pago por dia*/

                        /*-----Creamos la transaccion----*/
                        QString cTransaccion;
                        QSqlQuery crearTransaccion;

                        cTransaccion="INSERT INTO Transaccion(total_a_pagar,liquidado,idPension)VALUES('"+QString::number(tPagarI)+"','0','"+idPen+"')";




                        crearTransaccion.prepare(cTransaccion);
                        if(crearTransaccion.exec()){

                            QString llenarcaj = "UPDATE cajon SET lleno = '1' WHERE idCajon = '" + id3 + "'";

                            llencaj.exec(llenarcaj);
                            llencaj.next();

                            QMessageBox::about(this, "Confirmacion", "Reservacion Premium creada exitosamente");
                            ui->stackedWidget->setCurrentIndex(4);


                        }else{
                            qDebug() << "Error transaccion no creada";
                        }
                    }else{
                        qDebug() << "Error dias no obtenidos";
                    }

                }else{
                    qDebug() << "Error Id pension no obtenido";
                }
            }else{
                QMessageBox::about(this, "Error", "Reservacion Premium no creada");
            }

        }else{
            qDebug() << "Error insertando tabla vehiculo";
        }

    }else{
        qDebug() << "Error insertando tabla foto";
    }
}
/*********************************************************************************************************************************/


/*----------------------------------------------- INTERFAZ PERFIL-------------------------------------------------------------*/
void MainWindow::Datos(){
     QString dato, nombre, a_paterno, a_materno, telefono, correo, id;
     QSqlQuery query;

     id="select idUsuario from usuario where  usuario='"+username+"'"; /*Obtenemos el id del usuario*/
     QSqlQuery insert2;
     insert2.prepare(id);
     insert2.exec();
     while ( insert2.next()){
         id=insert2.value(0).toString(); /*Se obtiene el id*/
     }

     dato="select Nombre, Apellido_p, Apellido_m, correo, telefono from Cliente where idUsuario='"+id+"'";
     if (query.exec(dato)){
         while (query.next()){
             nombre = query.value(0).toString();
             a_paterno = query.value(1).toString();
             a_materno = query.value(2).toString();
              correo = query.value(3).toString();
             telefono = query.value(4).toString();


             ui->nom->setText(nombre);
             ui->apellidop->setText(a_paterno+" "+a_materno);
             ui->tel->setText(telefono);
             ui->correo->setText(correo);
         }
         ui->stackedWidget->setCurrentIndex(9);
     }

}
void MainWindow::DatosDireccion(){
     QString dato, calle,colonia, no_interior, no_exterior, cp, id;
     QSqlQuery query;

     ui->Calle->clear();
     ui->Colonia->clear();
     ui->codigoP->clear();
     ui->no_int->clear();
     ui->no_ext->clear();

     id="select idUsuario from usuario where  usuario='"+username+"'";/*Obtenemos el el id del usuario*/
     QSqlQuery insert2;
     insert2.prepare(id);
     insert2.exec();
     while ( insert2.next()){
         id=insert2.value(0).toString();/*Se obtiene el id*/
     }

    qDebug()<< id;

    dato="select d.calle, d.colonia, d.cp, d.no_exterior, d.no_interior from Domicilio as d, Cliente as c where d.idDomicilio=c.idDomicilio and c.idUsuario='"+id+"'";

    qDebug()<<dato;

     if (query.exec(dato)){
         while (query.next()){
             calle=query.value(0).toString();
             colonia=query.value(1).toString();
             cp=query.value(2).toString();
             no_interior=query.value(3).toString();
             no_exterior=query.value(4).toString();

             ui->Calle->setText(calle);
             ui->Colonia->setText(colonia);
             ui->codigoP->setText(cp);
             ui->no_int->setText(no_interior);
             ui->no_ext->setText(no_exterior);
         }
         ui->stackedWidget->setCurrentIndex(9);
     }
}
void MainWindow::DatosFoto(){
    QPixmap imagen;
    QIcon imagenreal;
    QByteArray img;
    QSqlQuery sql, sacarfoto;


    QString id= "select idUsuario from Usuario where usuario='"+username+"'";
    QSqlQuery insert2;
    insert2.prepare(id);
    insert2.exec();
    while ( insert2.next()){
        id=insert2.value(0).toString();
    }

    qDebug()<< "nombre: "<<username;

    QString id2= "select Nombre from Cliente where idUsuario='"+id+"'";
    QSqlQuery insert;
    insert.prepare(id2);
    insert.exec();
    while ( insert.next()){
        id2=insert.value(0).toString();
        qDebug()<<id2;
    }

    QString insertfoto, idF, propi, llamarFoto;
    llamarFoto="call perfil('"+id2+"');" ;



    sacarfoto.exec(llamarFoto);
    sacarfoto.next();
    img=sacarfoto.value(0).toByteArray();
    imagen.loadFromData(img);
    ui->fotouser->setPixmap(imagen.scaled(191,181));
}
void MainWindow::on_estacionamiento_3_clicked(){
    Cajones();
    QSqlQuery query, query2;
    QMessageBox messageBox;
    messageBox.setText("Eliga el tipo de reservación");
    QPushButton  *Em = messageBox.addButton("Emergencia", QMessageBox::YesRole );
    QPushButton   *Normal = messageBox.addButton("Normal", QMessageBox::AcceptRole);
    QPushButton   *Premium = messageBox.addButton("Premium", QMessageBox::RejectRole);
    Em->setStyleSheet("color: white; background-color: #1a237e; font-size:14px;  border: 1px solid #1a237e;  padding: 2px 4px;");
    Normal->setStyleSheet("color: white; background-color: #1a237e; font-size:14px;  border: 1px solid #1a237e;  padding: 2px 4px;");
    Premium->setStyleSheet("color: white; background-color: #1a237e; font-size:14px;  border: 1px solid #1a237e; padding: 2px 4px;");
    messageBox.exec();

    if(messageBox.clickedButton() == Normal){
        ui->stackedWidget->setCurrentIndex(5);
        ui->tabWidget->setTabEnabled(0,true);
        ui->tabWidget->setTabEnabled(1,true);
        ui->tabWidget->setTabEnabled(2, true);
       Tipo = "Normal";
    }


    if(messageBox.clickedButton() == Premium){
        ui->stackedWidget->setCurrentIndex(5);
        ui->tabWidget->setTabEnabled(0,true);
        ui->tabWidget->setTabEnabled(1,true);
        ui->tabWidget->setTabEnabled(2, true);
        Tipo = "Premium";
    }

    if(messageBox.clickedButton() == Em){
        ui->tabWidget->setTabEnabled(0,true);
        ui->tabWidget->setTabEnabled(1, false);
        ui->tabWidget->setTabEnabled(2, false);
        Tipo = "Emergencia";
    }
    ui->stackedWidget->setCurrentIndex(5);
}
void MainWindow::on_perfil_3_clicked(){
    Datos();
    DatosFoto();
    DatosDireccion();
    ui->stackedWidget->setCurrentIndex(9);
}
void MainWindow::on_reservacion_3_clicked(){
    ui->stackedWidget->setCurrentIndex(10);
    Foto();
    Ofer();
    TablaReservacion();
    DatosTarjeta();
    Piso();
    Tiempo();
}
void MainWindow::on_salir_3_clicked(){
    ui->stackedWidget->setCurrentIndex(0);
}
void MainWindow::on_CambiarFoto_clicked(){
    QPixmap imagen;
    QIcon imagenreal;
    QByteArray img;
    QSqlQuery sql, sacarfoto;
    QString insertfoto, idF, propi, llamarFoto, f;
    auto Archivo=QFileDialog::getOpenFileName(this,"abrir imagen",QDir::rootPath(),"imagenes(*.png *.jpg *.jpeg);; cualquier archivo(*.*)");
    ruta2= Archivo;
    QFile archivo(ruta2);
    if(!archivo.open(QIODevice::ReadOnly)){
        return;
    }

    QByteArray mostrar = archivo.readAll();
    QPixmap imag;
    imag.loadFromData(mostrar);
    ui->fotouser->setPixmap(imag.scaled(191,181));
    ruta3=ruta2;
    imagen2=mostrar;

    insertfoto="insert into Fotos(ruta, imagen)values('"+ruta3+"', :datos);" ;
      QSqlQuery insertfoto1;
      insertfoto1.prepare(insertfoto);
      insertfoto1.bindValue(":datos",imagen2);
      insertfoto1.exec();

      idF="select MAX(idFotos) from fotos";
      QSqlQuery insert3;
      insert3.prepare(idF);
      insert3.exec();
      while ( insert3.next()){
          idF=insert3.value(0).toString();
      }


      QString id= "select idUsuario from Usuario where usuario='"+username+"'";
      QSqlQuery insert2;
      insert2.prepare(id);
      insert2.exec();
      while ( insert2.next()){
          id=insert2.value(0).toString();
      }

      qDebug()<<"id usuario: "+id;

      QString id2= "select Nombre from Cliente where idUsuario='"+id+"'";
      QSqlQuery insert;
      insert.prepare(id2);
      insert.exec();
      while ( insert.next()){
          id2=insert.value(0).toString();
      }
      qDebug()<<"nombre :"+id2;


      propi="update Cliente set idFotos= '"+ idF +"' where Cliente.Nombre='"+id2+"';";
      qDebug()<<propi;
      if(sql.exec(propi)){
           DatosFoto();
          QMessageBox::information(this, "Foto", "Foto Guardada");
      }
}
void MainWindow::on_aceptar_clicked(){
    QString calle,colonia,cp,no_interior,no_exterior, idDom;
    QSqlQuery q1,q2;
    /*Obtenemos los datos de los QLabel*/

    calle=ui->calle->text();
    colonia=ui->colonia->text();
    cp=ui->cp->text();
    no_interior=ui->no_interior->text();
    no_exterior=ui->no_exterior->text();

    if(no_interior.isEmpty()){
        no_interior = "No hay número";
    }

    /*Verificamos que ningun campo este vacio*/
    if(calle.isEmpty()||colonia.isEmpty()||cp.isEmpty()||no_exterior.isEmpty()){
        QMessageBox::about(this, "Error", "Campos Vacios");
        return;
    }
   /*Verificamos que el query funcione adecuadamente, de lo contrario se termina el proceso*/

   if(!q1.exec("select domicilio.idDomicilio from domicilio inner join cliente "
               "on domicilio.idDomicilio=cliente.idDomicilio inner join usuario "
               "on usuario.idUsuario=cliente.idUsuario where usuario like '"+username+"'"
               " and cliente.idDomicilio is not null;")){
       qDebug()<<"No se ejecuto el query exitosamente";
       return;
   }
   else
   {
       /*Si el query q1 funciono obtenemos el id del Domicilio*/
       while(q1.next()){idDom=q1.value(0).toString();}

       /*Verificamos si es una actualizacion o se agregara*/
       if(idDom==NULL)
       {
           qDebug()<<"Se esta ingresando un nuevo domicilio";

           QSqlQuery q3,q4;
           QString nuevoDomicilio;
           if(q3.exec("SELECT idUsuario from usuario where usuario like '"+username+"';")){
               while(q3.next()){
                   nuevoDomicilio=q3.value(0).toString();
               }

               if(q3.exec("INSERT INTO domicilio SET idDomicilio='"+nuevoDomicilio+"', calle='"+calle+"',colonia='"+colonia+"',cp='"+cp+"',no_exterior='"+no_exterior+"',no_interior='"+no_interior+"';")){

                   if(q4.exec("UPDATE Cliente set idDomicilio='"+nuevoDomicilio+"' WHERE Cliente.idCliente='"+nuevoDomicilio+"';"))
                       /*update Cliente set idDomicilio='1' where Cliente.idCliente='1'*/
                   {
                       QMessageBox::about(this, "Datos correctos", "Se agrego el domicilio exitosamente");
                   }else{
                       qDebug()<<"Datos no agregados";
                   }
               }else{
                   qDebug()<<"Cambios no realizados exitosamente 2";
               }

           }
       }
       else
       {
           while(q1.next()){idDom=q1.value(0).toString();} //Obtenemos el idDomicilio

           if(q2.exec("UPDATE domicilio SET calle='"+calle+"',colonia='"+colonia+"',cp='"+cp+"',no_exterior='"+no_exterior+"',no_interior='"+no_interior+"' WHERE idDomicilio='"+idDom+"';")){
                DatosDireccion();
               QMessageBox::about(this, "Datos correctos", "La actualizaciòn del domicilio se realizo exitosamente");
           }else{
                qDebug()<<"Cambios no realizados exitosamente 2";
           }
       }
    }
   ui->calle->clear();
   ui->colonia->clear();
   ui->cp->clear();
   ui->no_interior->clear();
   ui->no_exterior->clear();
   DatosDireccion();

}
/*------------------------------------------ INFORMACION DE LA RESERVACION----------------------------------------------------*/
void MainWindow::Foto(){
    QPixmap imagen;
    QIcon imagenreal;
    QByteArray img;
    QSqlQuery sql, sacarfoto;


    QString id= "select idUsuario from Usuario where usuario='"+username+"'";
    QSqlQuery insert2;
    insert2.prepare(id);
    insert2.exec();
    while ( insert2.next()){
        id=insert2.value(0).toString();
    }
    qDebug()<<"user"<< username;

    QString id2= "select idCliente from Cliente where idUsuario='"+id+"'";
    QSqlQuery insert;
    insert.prepare(id2);
    insert.exec();
    while ( insert.next()){
        id2=insert.value(0).toString();
        qDebug()<<id2;
    }

    QString id3= "select idVehiculo from Vehiculo where idCliente='"+id2+"'";
    QSqlQuery insert3;
    insert3.prepare(id3);
    insert3.exec();
    while ( insert3.next()){
        id3=insert3.value(0).toString();
        qDebug()<<id3;;
    }

    QString insertfoto, idF, propi, llamarFoto;
    llamarFoto="call FotoAuto('"+id3+"');" ;


    sacarfoto.exec(llamarFoto);
    sacarfoto.next();
    img=sacarfoto.value(0).toByteArray();
    imagen.loadFromData(img);
    ui->fotouser_5->setPixmap(imagen.scaled(191,181));
    ui->fotouser_7->setPixmap(imagen.scaled(191,181));
}
void MainWindow::DatosTarjeta(){
    QString dato,  num, tipo, nombre, agno, mes, cvv, id, id2, tipo2;
     QSqlQuery query;

     id="select idUsuario from usuario where  usuario='"+username+"'";
     QSqlQuery insert2;
     insert2.prepare(id);
     insert2.exec();
     while ( insert2.next()){
         id=insert2.value(0).toString();
     }

     id2="select idTarjeta from Cliente where  idUsuario='"+id+"'";
     QSqlQuery insert;
     insert.prepare(id2);
     insert.exec();
     while ( insert.next()){
         id2=insert.value(0).toString();
     }

     dato="select num, tipo, nombre, agno, mes, cvv from Tarjeta where idTarjeta='"+id2+"'";
     if (query.exec(dato)){
         while (query.next()){
             num = query.value(0).toString();
             tipo = query.value(1).toString();
             nombre = query.value(2).toString();
             agno = query.value(3).toString();
             mes = query.value(4).toString();
             cvv = query.value(5).toString();


             if(tipo==0){
                tipo= "Mastercard";
             }
             else{
                 tipo= "Visa";
             }

             ui->dueno_2->setText(nombre);
             ui->no_tarjeta->setText(num);
             ui->tipo->setText(tipo);
             ui->agno_2->setText(agno+"/"+mes);
         }
         ui->stackedWidget->setCurrentIndex(10);
     }
}

void MainWindow::Piso(){
    QString dato,  num, tipo, nombre, agno, mes, cvv, id, id2, id3, tipo2, numPiso,pago;
     QSqlQuery query, query2;

     /*Obtenemos el id del usuario*/
     id="select idUsuario from usuario where  usuario='"+username+"'";
     QSqlQuery insert2;
     insert2.prepare(id);
     insert2.exec();
     while ( insert2.next()){
         id=insert2.value(0).toString(); /*Almacenamos el id del usuario*/
     }
     qDebug()<<"IdUsuario"+id;

     /*Obtenemos el id del Cliente*/
     id2= "select idCliente from Cliente where idUsuario='"+id+"'";
     QSqlQuery insert;
     insert.prepare(id2);
     insert.exec();
     while ( insert.next()){
         id2=insert.value(0).toString();
     }
     qDebug()<<"IdCliente"+id2;

     dato="select idCajon, ((time_to_sec(timediff(fin,inicio))/3600) * pagoxhora) as precio, inicio, fin from Reservacion INNER JOIN transaccion USING(idReservacion) where idCliente='"+id2+"' and estado='0' and liquidado = '0'";
     if (query.exec(dato)){
         while (query.next()){
             num = query.value(0).toString();
             //ui->cajon->setText(num);
             pago = query.value(1).toString();
             ui->lMonto->setText(pago);
             hInicio=query.value(2).toString();
             hInicio.replace("T", " ");
             ui->entrada->setText(hInicio);

             hFin=query.value(3).toString();
             hFin.replace("T", " ");
             ui->salida->setText(hFin);

         }
     }
     else
     {
        return ;
     }


     QString ncajon="select numero from cajon where idCajon='"+num+"'";
     QSqlQuery buscar;
     buscar.prepare(ncajon);
     buscar.exec();
     while(buscar.next()){
         ncajon=buscar.value(0).toString();
         ui->cajon->setText(ncajon);
     }

     id3="select idPiso from cajon where idCajon='"+num+"'";
     if(query2.exec(id3)){
         while (query2.next()){
             numPiso=query2.value(0).toString();
             ui->Piso->setText(numPiso);
         }
         ui->stackedWidget->setCurrentIndex(10);
     }
}

void MainWindow::Tiempo(){
    QString tiempo, dato, fin;
    QDateTime time, d;
    time = QDateTime::currentDateTime();
    tiempo = time.toString("yyyy-MM-dd hh:mm:ss");
    ui->fecha_3->setText(tiempo);


    QDateTime tiempo2;
    tiempo2=QDateTime::currentDateTime();
    dato= tiempo2.toString("dddd, d MMMM yy hh:mm:ss");
    ui->fecha->setText(dato);
    ui->fecha_5->setText(dato);

    //*********************************
    QString fechext = ui->salida->text();

    QDateTime dateFin;
    dateFin = QLocale(QLocale::English).toDateTime(fechext,"yyyy-MM-dd hh:mm:ss.zzz");

    QString tiempo22 = dateFin.toString("yyyy-MM-dd hh:mm:ss");


    if(tiempo==tiempo22){
        timerSound->play();

        QMessageBox::about(this, "Tiempo terminado", "El tiempo de la reservación ha expirado");

        QMessageBox msgBox;
        msgBox.setText("¿Desea culminar la reservación?");
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        int elegido = msgBox.exec();

        if(elegido == 1024){
            /*Aqui deberia desactivar la reservacion*/
            QSqlQuery desactRes;
            desactRes.exec("UPDATE reservacion SET estado = 1 WHERE idReservacion = 4");
            desactRes.next();
            /*---------------------------------------*/

            timerSound->stop();
            ui->stackedWidget->setCurrentIndex(4);
        }
    }
}

void MainWindow::on_estacionamiento_4_clicked(){
    Cajones();

    QSqlQuery query, query2;
    QMessageBox messageBox;
    messageBox.setText("Eliga el tipo de reservación");
    QPushButton  *Em = messageBox.addButton("Emergencia", QMessageBox::YesRole );
    QPushButton   *Normal = messageBox.addButton("Normal", QMessageBox::AcceptRole);
    QPushButton   *Premium = messageBox.addButton("Premium", QMessageBox::RejectRole);
    Em->setStyleSheet("color: white; background-color: #1a237e; font-size:14px;  border: 1px solid #1a237e;  padding: 2px 4px;");
    Normal->setStyleSheet("color: white; background-color: #1a237e; font-size:14px;  border: 1px solid #1a237e;  padding: 2px 4px;");
    Premium->setStyleSheet("color: white; background-color: #1a237e; font-size:14px;  border: 1px solid #1a237e; padding: 2px 4px;");
    messageBox.exec();

    if(messageBox.clickedButton() == Normal){
        ui->stackedWidget->setCurrentIndex(5);
        ui->tabWidget->setTabEnabled(0,true);
        ui->tabWidget->setTabEnabled(1,true);
        ui->tabWidget->setTabEnabled(2, true);
       Tipo = "Normal";
    }


    if(messageBox.clickedButton() == Premium){
        ui->stackedWidget->setCurrentIndex(5);
        ui->tabWidget->setTabEnabled(0,true);
        ui->tabWidget->setTabEnabled(1,true);
        ui->tabWidget->setTabEnabled(2, true);
        Tipo = "Premium";
    }

    if(messageBox.clickedButton() == Em){
        ui->tabWidget->setTabEnabled(0,true);
        ui->tabWidget->setTabEnabled(1, false);
        ui->tabWidget->setTabEnabled(2, false);
        Tipo = "Emergencia";
    }
    ui->stackedWidget->setCurrentIndex(5);
}
void MainWindow::on_perfil_4_clicked(){
    Datos();
    DatosFoto();
    DatosDireccion();
    ui->stackedWidget->setCurrentIndex(9);
}
void MainWindow::on_reservacion_4_clicked(){
    ui->stackedWidget->setCurrentIndex(10);
    Foto();
    Ofer();
    TablaReservacion();
    DatosTarjeta();
    Piso();
    Tiempo();
}
void MainWindow::on_salir_4_clicked(){
    ui->stackedWidget->setCurrentIndex(0);
}
void MainWindow::on_Volver_clicked()
{
    //Piso();
    ui->stackedWidget->setCurrentIndex(10);
}
void MainWindow::on_Pagar_clicked(){

    QLineEdit Pago;
    QString iniciar,transaccion,monto;
    monto=ui->lMonto->text(); /*Obtenemos la cantidad a pagar*/
    bool ok;
    QSqlQuery sql, transaccionQ,idReservacion;


    /*----------------------Obtenemos el id del cliente------------------------*/
     QString  id, id2, id3, id4, cvv, idReserva;
      QSqlQuery query;

      /*Obtenemos el id del usuario*/
      id="select idUsuario from usuario where  usuario='"+username+"'";
      QSqlQuery insert2;
      insert2.prepare(id);
      insert2.exec();
      while ( insert2.next()){
          id=insert2.value(0).toString(); /*Almacenamos el id del usuario*/
      }
      /*Obtenemos el id del Cliente*/
      id2= "select idCliente from Cliente where idUsuario='"+id+"'";
      QSqlQuery insert;
      insert.prepare(id2);
      insert.exec();
      while ( insert.next()){
          id2=insert.value(0).toString();
      }
    /*--------------------------------------------------------------------*/

       id3=obtenerIDCajon();

       qDebug()<< "id Cajon "+id3;

    /*---------------Obtenemos el id de la tarjeta del cliente------------*/
      id4="select idTarjeta from Cliente where idCliente='"+id2+"'";
      QSqlQuery insert3;
      insert3.prepare(id4);
      insert3.exec();
      while(insert3.next()){
          id4=insert3.value(0).toString();
      }

      cvv="select cvv from tarjeta where idTarjeta='"+id4+"'";
      QSqlQuery insert5;
      insert5.prepare(cvv);
      insert5.exec();
      while(insert5.next()){
          cvv=insert5.value(0).toString();
      }
    /*--------------------------------------------------------------------*/


    QString Dialogo = QInputDialog :: getText ( this , " Tarjeta " , tr ( " Escribir cvv " ), Pago.Normal , " " , & ok);
    if (ok){
        qDebug()<<Dialogo;

        if((QString::compare(cvv,Dialogo))!=0){

            qDebug()<<cvv;
            QMessageBox::about(this,"Advetencia", "CVV incorrecto, por favor vuelva a intentarlo");
            ui->stackedWidget->setCurrentIndex(11);
        }
        else{
              /*-----------------Obtenemos el id de la reservacion----------------*/
            QString s = "select idReservacion from Reservacion where idCliente='"+id2+"'and idCajon='"+id3+"' ;";
            qDebug()<<s;
              if(!idReservacion.exec(s)){
                  qDebug()<<"id no encontrado";
                  return;
              }
              while(idReservacion.next())
              {
                  idReserva=idReservacion.value(0).toString();
              }


             if(!transaccionQ.exec("update transaccion set liquidado='1' where idReservacion='"+idReserva+"';")){
                 qDebug()<<"Transaccion no ejecutada";
                 return;
             }
             /*-----------------------------Actualizamos el estado de cajon------------------------------*/
             QSqlQuery cajonE;

             if(!cajonE.exec("update cajon set lleno='1' where idCajon='"+id3+"';")){
                 qDebug()<<"El estado de cajon no se cambio";
                 return;
             }

             QSqlQuery sql;
             QString estado;
             estado="update Reservacion set estado = '0' where idReservacion='"+idReserva+"';";
             qDebug()<< estado;
             if(!sql.exec(estado)){
                 qDebug()<<"No se cambio estado de reservacion";
                 return;
             }

             /*--------------------------------------------------------------------------------------------*/

            QMessageBox::about(this, "Exito", "El pago se ha realizado correctamente");
                ui->Piso->clear();
                ui->cajon->clear();
                ui->lMonto->clear();
                ui->fotouser_5->clear();
                ui->entrada->clear();
                ui->salida->clear();
                TablaReservacion();
            }
    }
}
void MainWindow::on_Ir_clicked()
{
    TablaReservacion();
    ui->stackedWidget->setCurrentIndex(11);
}

/*-----------------------Cancelar la reserva y modificar la tarifa-------------------------*/
void MainWindow::on_btnCancelarReserva_clicked()
{

    QMessageBox msgBox;
    msgBox.setText("Al cancelar la reserva se le aplicara un cargo extra del 50% del total a pagar.");
    msgBox.setInformativeText("¿Desea cancelar la reservación?");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    int elegido = msgBox.exec();

    qDebug()<<elegido;
    if(elegido == 1024){
        ui->lMonto->setText("10"); /*Se actualiza el monto a pagar*/

        QLineEdit Pago;
        QString monto;
        monto=ui->lMonto->text(); /*Obtenemos la cantidad a pagar*/
        bool ok;
        QSqlQuery sql, transaccionQ,idReservacion;


         /*----------------------Obtenemos el id del cliente------------------------*/
         QString  id, id2, id3, id4, cvv, idReserva;
          QSqlQuery query;

          /*Obtenemos el id del usuario*/
          id="select idUsuario from usuario where  usuario='"+username+"'";
          QSqlQuery insert2;
          insert2.prepare(id);
          insert2.exec();
          while ( insert2.next()){
              id=insert2.value(0).toString(); /*Almacenamos el id del usuario*/
          }
          /*Obtenemos el id del Cliente*/
          id2= "select idCliente from Cliente where idUsuario='"+id+"'";
          QSqlQuery insert;
          insert.prepare(id2);
          insert.exec();
          while ( insert.next()){
              id2=insert.value(0).toString();
          }
        /*--------------------------------------------------------------------*/
          /*Obtenemos el id de cajon*/
          id3=obtenerIDCajon();
        /*--------------------------------------------------------------------*/
          /*Obtenemos el id de la tarjeta del cliente*/
          id4="select idTarjeta from Cliente where idCliente='"+id2+"'";
          QSqlQuery insert3;
          insert3.prepare(id4);
          insert3.exec();
          while(insert3.next()){
              id4=insert3.value(0).toString();
          }

          cvv="select cvv from tarjeta where idTarjeta='"+id4+"'";
          QSqlQuery insert5;
          insert5.prepare(cvv);
          insert5.exec();
          while(insert5.next()){
              cvv=insert5.value(0).toString();
          }

        /*El cliente ingresa el cvv*/
        QString Dialogo = QInputDialog :: getText ( this , " Tarjeta " , tr ( " Escribir cvv " ), Pago.Normal , " " , & ok);
        if (ok)
        {
            qDebug()<<Dialogo;

            if((QString::compare(cvv,Dialogo))!=0)
            {
                qDebug()<<cvv;
                QMessageBox::about(this,"Advetencia", "CVV incorrecto, por favor vuelva a intentarlo");
                ui->stackedWidget->setCurrentIndex(11);
            }
            else
            {
                /*-----------------Obtenemos el id de la reservacion----------------*/
                if(idReservacion.exec("select idReservacion from Reservacion where idCliente='"+id2+"'and idCajon='"+id3+"' ;"))
                {
                    qDebug()<<"id encontrado";
                    while(idReservacion.next())
                    {
                      idReserva=idReservacion.value(0).toString();
                    }


                    if(transaccionQ.exec("update transaccion set total_a_pagar='"+ui->lMonto->text()+"', liquidado='1' where idReservacion='"+idReserva+"';")){
                        qDebug()<<"Transaccion ejecutada";
                        /*-----------------------------Actualizamos el estado de cajon------------------------------*/
                        QSqlQuery cajonE, eReserva;

                        if(!cajonE.exec("update cajon set lleno='1' where idCajon='"+id3+"';")){
                        qDebug()<<"El estado de cajon no se cambio";
                        return;
                        }
                        /*--------------------------------------------------------------------------------------------*/
                        /*-----------------------------Actualizamos el estado de la reservacion-----------------------*/

                        if(!eReserva.exec("update reservacion set estado='1' where idReservacion='"+idReserva+"';")){
                            qDebug()<<"El estado de la reserva no se cambio";
                            return;
                        }


                        QMessageBox::about(this, "Aviso", "La reservación ha sido cancelada");
                        ui->Piso->clear();
                        ui->cajon->clear();
                        ui->lMonto->clear();
                        TablaReservacion();
                        ui->stackedWidget->setCurrentIndex(11);

                    }else{qDebug()<<"Transaccion no ejecutada";}

                }else{qDebug()<<"Id de la reservacion no encontrado";}
            }
        }
    }
}

/***********************************Obtener el id del cajon**************************************/
QString MainWindow::obtenerIDCajon(){
    QString id3;
    QSqlQuery query;
    id3="select idCajon from Cajon where idPiso='"+piso+"' and numero='"+cajon+"'";
    qDebug()<<"idCajon: "<< id3;
    query.prepare(id3);
    query.exec();
    while(query.next()){
        id3=query.value(0).toString(); /*Obtenemos el id del cajon*/
    }

    return id3;
}


/*----------------------Realizar la animacion-----------------------------------------*/
void MainWindow::Animacion(int xright){
   QLabel *temp = new QLabel;
   temp->setStyleSheet("image:url(:/Imagenes/car.png); background-color: transparent;");
   temp->setFixedSize(120,90);
   ui->Road->addWidget(temp);

   QPropertyAnimation *animation = new QPropertyAnimation(temp, "geometry");
   animation->setDuration(3000);
   animation->setStartValue(QRect(1,5,111,94));
   animation->setEndValue(QRect(xright,5,111,94));
   QRect();

   animation->start();
   Descansar();
   delete temp;
   delete animation;
}
void MainWindow::Animacion2(int xright){
   QLabel *temp = new QLabel;
   temp->setStyleSheet("image:url(:/Imagenes/car.png); background-color: transparent;");
   temp->setFixedSize(120,90);
   ui->Road_2->addWidget(temp);

   QPropertyAnimation *animation = new QPropertyAnimation(temp, "geometry");
   animation->setDuration(3000);
   animation->setStartValue(QRect(1,5,111,94));
   animation->setEndValue(QRect(xright,5,111,94));
   QRect();

   animation->start();
   Descansar();
   delete temp;
   delete animation;
}
void MainWindow::Animacion3(int xright){
   QLabel *temp = new QLabel;
   temp->setStyleSheet("image:url(:/Imagenes/car.png); background-color: transparent;");
   temp->setFixedSize(120,90);
   ui->Road_3->addWidget(temp);

   QPropertyAnimation *animation = new QPropertyAnimation(temp, "geometry");
   animation->setDuration(3000);
   animation->setStartValue(QRect(1,5,111,94));
   animation->setEndValue(QRect(xright,5,111,94));
   QRect();

   animation->start();
   Descansar();
   delete temp;
   delete animation;
}
void MainWindow::Descansar(){
   QTime reachTime = QTime::currentTime().addSecs(3);

   while (QTime::currentTime() < reachTime){
       QCoreApplication::processEvents(QEventLoop::AllEvents,100);
   }
}
void MainWindow::Calcular(int num){
   int width = 141 - 4.7*num;
   carWidth = width;

   if (num % 2 == 0){
       first = second = num / 2;
   }
   else {
       first = (num + 1) / 2;
       second = (num - 1) / 2;
   }
}

/*----------------------Ver registros (adminsitrador) -----------------------------------------*/

void MainWindow::on_verDetalles_clicked()
{
        ui->stackedWidget->setCurrentIndex(13);

        QString obtusuarios = "SELECT idUsuario,usuario,Nombre,Apellido_p,Apellido_m,correo,telefono FROM usuario INNER JOIN cliente using(idUsuario)";
        int numerodColumnas = ui->Autos->rowCount();

        QList<int> numeros;
                for(int i=1; i<=numerodColumnas; i++){
                    numeros.append(i);
                    ui->Autos->removeRow(numeros.value(i));
                }

        QSqlQuery consultaus;
        consultaus.exec(obtusuarios);

        while(consultaus.next()){
            QString idUsr = consultaus.value("idUsuario").toString();
            QString nomUsr = consultaus.value("usuario").toString();
            QString nombUsr = consultaus.value("Nombre").toString();
            QString appUsr = consultaus.value("Apellido_p").toString();
            QString apmUsr = consultaus.value("Apellido_m").toString();
            QString corrUsr = consultaus.value("correo").toString();
            QString telUsr = consultaus.value("telefono").toString();

            ui->Autos->insertRow(ui->Autos->rowCount());
            int fila = ui->Autos->rowCount()-1;

            //ui->Autos->setItem(fila,0,new QTableWidgetItem(idUsr));
            ui->Autos->setItem(fila,0,new QTableWidgetItem(nomUsr));
            ui->Autos->setItem(fila,1,new QTableWidgetItem(nombUsr));
            ui->Autos->setItem(fila,2,new QTableWidgetItem(appUsr));
            ui->Autos->setItem(fila,3,new QTableWidgetItem(apmUsr));
            ui->Autos->setItem(fila,4,new QTableWidgetItem(corrUsr));
            ui->Autos->setItem(fila,5,new QTableWidgetItem(telUsr));
        }
}
void MainWindow::on_volverregdos_clicked()
{
    ui->stackedWidget->setCurrentIndex(12);
}
void MainWindow::on_pbSalir_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

//Extender reservación ----------------------------------------------

void MainWindow::on_Extender_clicked()
{
    if(ui->entrada->text().isEmpty()){
        QMessageBox::about(this, "Error", "No hay ninguna reservación disponible para extender");
        return;
    }

    QStringList extensiones;
    extensiones << tr("1") << tr("2") << tr("3") << tr("4") << tr("5");

    bool ok;
    QString extensiont = QInputDialog :: getItem( this , " Extensión de tiempo" , tr ("Inserte tiempo que desea extender la reservación (en horas): "), extensiones, 0, false, &ok);

    if(ok){
        QDateTime dateFin,dateFin2;
        QString fechnoext = ui->entrada->text();
        QString fechext = ui->salida->text();

        qDebug()<< fechext;

        dateFin = QLocale(QLocale::English).toDateTime(fechext,"yyyy-MM-dd hh:mm:ss.zzz");
        dateFin2 = QLocale(QLocale::English).toDateTime(fechnoext,"yyyy-MM-dd hh:mm:ss.zzz");

        qDebug() << "Date Fin: " << dateFin2;

        int extn = extensiont.toInt();
        extn = extn * 3600;

        qDebug()<< extn;

        QDateTime NdateFin = dateFin.addSecs(extn);

        QString deb1 = dateFin.toString("yyyy-MM-dd HH:mm:ss.zzz");
        QString deb2 = dateFin2.toString("yyyy-MM-dd HH:mm:ss.zzz");

        QString deb3 = NdateFin.toString("yyyy-MM-dd HH:mm:ss.zzz");

        int versimas = dateFin2.secsTo(NdateFin);

        qDebug()<< versimas;

        if(versimas >= 86400){
            QMessageBox::about(this, "Error", "Extension Inválida");
            return;
        }

        /*Obtenemos el id del usuario*/
        QString dato, id, id2;
        QSqlQuery query, query2;

        id="select idUsuario from usuario where  usuario='"+username+"'";
        QSqlQuery insert2;
        insert2.prepare(id);
        insert2.exec();
        while ( insert2.next()){
            id=insert2.value(0).toString(); /*Almacenamos el id del usuario*/
        }
        qDebug()<<"IdUsuario"+id;

        /*Obtenemos el id del Cliente*/
        id2= "select idCliente from Cliente where idUsuario='"+id+"'";
        QSqlQuery insert;
        insert.prepare(id2);
        insert.exec();
        while ( insert.next()){
            id2=insert.value(0).toString();
        }
        qDebug()<<"IdCliente"+id2;

        /*Obtenemos el numero de cajon y piso en el que esta*/
        QString caj = ui->cajon->text();
        QString pis = ui->Piso->text();

        /*Obtener ID del cajon*/
        QString idcajj = "SELECT idCajon FROM cajon WHERE idPiso = " + pis + " AND numero = " + caj;
        QSqlQuery obtiddcaj;
        obtiddcaj.prepare(idcajj);
        obtiddcaj.exec();
        while ( obtiddcaj.next()){
            idcajj=obtiddcaj.value(0).toString();
        }
        qDebug()<<"IdCajon"+idcajj;

        /*-----------------AQUI SE CONSULTARA SI LA NUEVA EXTENSION NO INTERRUMPE OTRA RESERVACION--------*/

        /*PRIMERO OBTENER EL ID DE LA RESERVACION EN CUESTION PARA EXCEPTUARLA*/

        QString resexcep = "SELECT idReservacion FROM reservacion WHERE inicio = '" + deb2 + "' AND fin = '" + deb1 + "' AND idCajon = '" + idcajj + "' AND estado = '0'";
        QSqlQuery exceptuarres;
        exceptuarres.exec(resexcep);
        exceptuarres.next();

        int reservaexcep = exceptuarres.value("idReservacion").toInt();

        qDebug() << "Id reservacion a exceptuar: " << reservaexcep;

        /*AHORA SI VERIFICAR QUE NO INTERRUMPA OTRA RESERVACION*/

        /*verificar si no interrumpe una reservacion*/
        QString siNfechentre = "SELECT idReservacion,inicio,fin,estado FROM reservacion WHERE idCajon = " + idcajj;

        QSqlQuery siNfechen;
        siNfechen.exec(siNfechentre);

        while(siNfechen.next()){
            QString iniR = siNfechen.value("inicio").toString();
            QString finiR = siNfechen.value("fin").toString();
            int estate = siNfechen.value("estado").toInt();
            int idressex = siNfechen.value("idReservacion").toInt();

            qDebug() << "Estado: " << estate;
            qDebug() << "idReservacion: " << idressex;

            QDateTime dateIni = QLocale(QLocale::English).toDateTime(iniR,"yyyy-MM-ddThh:mm:ss.zzz");
            QDateTime dateFin10 = QLocale(QLocale::English).toDateTime(finiR,"yyyy-MM-ddThh:mm:ss.zzz");

            qDebug() << dateIni << " " << dateFin10;

            bool withinRange = (NdateFin >= dateIni && NdateFin <= dateFin10);

            qDebug() << withinRange;

            if(idressex!=reservaexcep){
                if(estate==0){
                    if(withinRange==true){
                        QMessageBox::about(this, "Error", "La extensión irrumpe el tiempo de otra reservación");
                        return ;
                    }
                }
            }
        }

        /*verificar si no interrumpe una pension*/


        QString siNfechentre2 = "SELECT fecha_inicio,fecha_fin FROM pension WHERE idCajon = " + idcajj;

        QSqlQuery siNfechen2;
        siNfechen2.exec(siNfechentre2);

        while(siNfechen2.next()){
            QString iniR2 = siNfechen2.value("fecha_inicio").toString();
            QString finiR2 = siNfechen2.value("fecha_fin").toString();

            QDate dateIni2 = QDate::fromString(iniR2,"yyyy-MM-dd");
            QDate dateFin2 = QDate::fromString(finiR2,"yyyy-MM-dd");

            QDateTime tiempoInicio(dateIni2,QTime(0,0,0));
            QDateTime tiempoFinal(dateFin2,QTime(23,59,59));

            bool withinRange2 = (NdateFin >= tiempoInicio && NdateFin <= tiempoFinal);

            if(withinRange2==true){
                QMessageBox::about(this, "Error", "La extensión irrumpe el tiempo de otra reservación");
                return ;
            }
        }

        /*--------------------------------------------------------------------*/

        /*------------------------------------------------------------------------------------------------*/

        dato="UPDATE reservacion SET fin = '" + deb3 + "' WHERE inicio = '" + deb2 + "' AND fin = '" + deb1 + "' AND idCajon = '" + idcajj + "' AND estado = '0'";
        if (query.exec(dato)){
            QMessageBox::about(this, "Extendida", "Reservación extendida");
            Piso();
        }
        else
        {
           QMessageBox::about(this, "Error", "Reservación no extendida");
           return ;
        }
    }
}


void MainWindow::Ofer(){
    QString tiempo, id, id2, id3, id4;
    QDateTime time;
    time = QDateTime::currentDateTime();
    tiempo = time.toString("yyyy-MM-dd hh:mm:ss");
    QString oferta, ofer;
    ofer= "Solo hoy "+in+" de descuento";

    /*Obtenemos el id del usuario*/
    id="select idUsuario from usuario where  usuario='"+username+"'";
    QSqlQuery insert2;
    insert2.prepare(id);
    insert2.exec();
    while ( insert2.next()){
        id=insert2.value(0).toString(); /*Almacenamos el id del usuario*/
    }
    /*Obtenemos el id del Cliente*/
    id2= "select idCliente from Cliente where idUsuario='"+id+"'";
    QSqlQuery insert;
    insert.prepare(id2);
    insert.exec();
    while ( insert.next()){
        id2=insert.value(0).toString();
    }


    /*Obtenemos fecha*/
    id3= "select inicio from Reservacion where idCliente='"+id2+"'";
    QSqlQuery insert3;
    insert3.prepare(id2);
    insert3.exec();
    while ( insert3.next()){
        id3=insert3.value(0).toString();
    }

    QSqlQuery sql;
    QString dato, Inicio, Fin, r;
    dato="select idReservacion, inicio, fin from Reservacion where idCliente='"+id2+"' and estado='0'";
    if (sql.exec(dato)){
        while (sql.next()){
            r=sql.value(0).toString();
            Inicio=sql.value(1).toString();
            Inicio.replace("T", " ");
            Fin=sql.value(2).toString();
            Fin.replace("T", " ");
        }
    }


    QSqlQuery query, query2;
    QString dat,dat2, d, s;



    if((tiempo>= aniversario) && (tiempo<=aniversario2)){
        if((Inicio>= aniversario) && (Fin<=aniversario2)){
            if(in=="10%"){
                qDebug()<< "dat: "+ dat;
                d="13.5";
                dat= "update Reservacion set pagoxhora = '"+d+"' where idCliente = '"+id2+"' and idReservacion= '"+r+"';";
                dat2= "update Transaccion set total_a_pagar = '"+d+"' where idReservacion= '"+r+"';";
                if (query.exec(dat)){
                    if (query2.exec(dat2)){
                        ui->lMonto->setText("13.5");
                    }
                }
            }
            if(in=="20%"){
                d="12";
                dat= "update Reservacion set pagoxhora = '"+d+"' where idCliente = '"+id2+"' and idReservacion= '"+r+"';";
                 dat2= "update Transaccion set total_a_pagar = '"+d+"' where idReservacion= '"+r+"';";
                 if (query.exec(dat)){
                     if (query2.exec(dat2)){
                         ui->lMonto->setText("12");
                     }
                 }
            }
            if(in=="30%"){
                d="10.5";
                dat= "update Reservacion set pagoxhora = '"+d+"' where idCliente = '"+id2+"' and idReservacion= '"+r+"';";
                dat2= "update Transaccion set total_a_pagar = '"+d+"' where idReservacion= '"+r+"';";
                if (query.exec(dat)){
                    if (query2.exec(dat2)){
                        ui->lMonto->setText("10.5");
                    }
                }
            }
            if(in=="40%"){
                d="9";
                dat= "update Reservacion set pagoxhora = '"+d+"' where idCliente = '"+id2+"' and idReservacion= '"+r+"';";
                dat2= "update Transaccion set total_a_pagar = '"+d+"' where idReservacion= '"+r+"';";
                qDebug()<< "dat: "+ dat;
                if (query.exec(dat)){
                    if (query2.exec(dat2)){
                        ui->lMonto->setText("9");
                    }
                }
            }
            if(in=="50%"){
                d="7.5";
                dat= "update Reservacion set pagoxhora = '"+d+"' where idCliente = '"+id2+"' and idReservacion= '"+r+"';";
                dat2= "update Transaccion set total_a_pagar = '"+d+"' where idReservacion= '"+r+"';";
                if (query.exec(dat)){
                    if (query2.exec(dat2)){
                        ui->lMonto->setText("7.5");
                    }
                }
            }
            if(in==" "){
                d="15";
                ui->lMonto->setText("15");

            }
        }
    }
    else{
        in= " ";
        if(in==" "){
            d="15";
                ui->lMonto->setText("15");
        }
    }

}
void MainWindow::on_crearOferta_clicked(){

    QString tiempo;
    QString DMuertos, TSantos;

    TSantos="2020-10-01 20:19:30";
    DMuertos="2020-10-02 20:19:30";
    QDateTime time;
    time = QDateTime::currentDateTime();
    tiempo = time.toString("yyyy-MM-dd hh:mm:ss");

    QString oferta, ofer;
    in = ui->descuento->currentText();

    ofer= "Solo hoy "+in+" de descuento";

    if((tiempo!= aniversario) && (tiempo!=aniversario2)){
        ui->img_oferta->setStyleSheet("border-image: url(:/Imagenes/normal.jpg);");
        ui->Moferta->setText("No hay ofertas disponibles");
    }

    if(ui->aniversario->isChecked()){
        if((tiempo>= aniversario) && (tiempo<=aniversario2)){
            ui->img_oferta->setStyleSheet("border-image: url(:/Imagenes/aniversario.jpg);");
            ui->Moferta->setText(ofer);
            QMessageBox::about(this, "Oferta", "Se ha creado la oferta");
        }
        else{
            QMessageBox::about(this, "Error", "Las fechas no coinciden");
        }

    }
    if(ui->agno_nuevo->isChecked()){
        QMessageBox::about(this, "Error", "Las fechas no coinciden");
    }
    if(ui->reyes->isChecked()){
        QMessageBox::about(this, "Error", "Las fechas no coinciden");
    }
    if(ui->sanValentin->isChecked()){
        QMessageBox::about(this, "Error", "Las fechas no coinciden");
    }
    if(ui->dia_nino->isChecked()){
        QMessageBox::about(this, "Error", "Las fechas no coinciden");
    }
    if(ui->batalla->isChecked()){
        QMessageBox::about(this, "Error", "Las fechas no coinciden");
    }
    if(ui->DMadre->isChecked()){
        QMessageBox::about(this, "Error", "Las fechas no coinciden");
    }
    if(ui->todosSantos->isChecked()){
        QMessageBox::about(this, "Error", "Las fechas no coinciden");
    }
    if(ui->DMuertos->isChecked()){
        QMessageBox::about(this, "Error", "Las fechas no coinciden");
    }
    if(ui->Navidad->isChecked()){
        QMessageBox::about(this, "Error", "Las fechas no coinciden");
    }
}

void MainWindow::on_Volver_2_clicked()
{
    ui->stackedWidget->setCurrentIndex(12);
}

void MainWindow::on_Ofertas_clicked()
{
     ui->stackedWidget->setCurrentIndex(14);
}

void MainWindow::on_pushButton_clicked()
{
    ui->Piso_3->clear();
    ui->Cajon_3->clear();
    ui->Dueno_4->clear();//
    ui->no_Tarjeta_3->clear();//
    ui->Tipo_3->clear();//
    ui->precioxd->clear();//
    ui->Entrada_3->clear();//
    ui->Salida_3->clear();//
    ui->lMonto_3->clear();

    /*----Obtenemos el id del cliente----*/
    QString obtIdU="SELECT idUsuario FROM usuario WHERE usuario='"+username+"' ";
    QSqlQuery obtenerUsuario;

    obtenerUsuario.prepare(obtIdU);
    if(obtenerUsuario.exec())
    {
        while(obtenerUsuario.next()){
            obtIdU=obtenerUsuario.value(0).toString();
            qDebug()<<"Id Usuario"<<obtIdU;
        }
        qDebug()<<"Id usuario:"<<obtIdU;

    }else{qDebug()<<"Usuario no encontrado";return;}

    /*----Obtenemos los datos de la tarjeta----*/
    QString numT,tipoT,nombreT;
    QString obtDT="SELECT num,tipo,nombre FROM tarjeta WHERE idTarjeta='"+obtIdU+"'";
    QSqlQuery obtenerDatosTarjeta;
    obtenerDatosTarjeta.prepare(obtDT);
    if(obtenerDatosTarjeta.exec())
    {
        while(obtenerDatosTarjeta.next()){
            numT=obtenerDatosTarjeta.value(0).toString();
            tipoT=obtenerDatosTarjeta.value(1).toString();
            nombreT=obtenerDatosTarjeta.value(2).toString();
        }
        qDebug()<<"Numero de tarjeta:"<<numT;
        qDebug()<<"Tipo de tarjeta:"<<tipoT;
        qDebug()<<"Propietario:"<<nombreT;


    }else{qDebug()<<"Datos de la tarjeta no obtenidos"; return;}


    /*----Obtener los datos de la pension---*/
    QString fIni, fFin, preDia, idCa,idPe;
    QString obtDPen=" SELECT idPension,fecha_inicio,fecha_fin,precioxdia,idCajon FROM pension WHERE idCliente='"+obtIdU+"' ";
    QSqlQuery obtenerPension;
    obtenerPension.prepare(obtDPen);
    if(obtenerPension.exec())
    {
        while(obtenerPension.next())
        {
            idPe=obtenerPension.value(0).toString();
            fIni=obtenerPension.value(1).toString();
            fFin=obtenerPension.value(2).toString();
            preDia=obtenerPension.value(3).toString();
            idCa=obtenerPension.value(4).toString();
        }
        qDebug()<<"Id pension:"<<idPe;
        qDebug()<<"Fecha Inicio:"<<fIni;
        qDebug()<<"Fecha Final:"<<fFin;
        qDebug()<<"Precio x dia:"<<preDia;
        qDebug()<<"Id del cajon:"<<idCa;

    }else{qDebug()<<"Datos de la pension no devueltos"; return;}


    /*-------Obtenemo los datos el cajon-------*/
    QString nPiso, CNumero;
    QString obtCajon="SELECT idPiso,numero FROM cajon WHERE idCajon='"+idCa+"'";
    QSqlQuery obtenerCajon;
    obtenerCajon.prepare(obtCajon);
    if(obtenerCajon.exec())
    {
        while(obtenerCajon.next()){
            nPiso=obtenerCajon.value(0).toString();
            CNumero=obtenerCajon.value(1).toString();
        }
        qDebug()<<"Id del piso:"<<nPiso;
        qDebug()<<"Numeor cajon:"<<CNumero;

    }else{qDebug()<<"Datos del cajon no encontrado";return;}

    /*-------Obtenemos el monto de la transaccion-------*/
   QString tPagarT, liquidadoT;
    QString tPagar="SELECT total_a_pagar,liquidado FROM transaccion WHERE idPension='"+idPe+"'";
    QSqlQuery totalPagar;
    totalPagar.prepare(tPagar);
    if(totalPagar.exec()){
        while(totalPagar.next()){
            tPagarT=totalPagar.value(0).toString();
            liquidadoT=totalPagar.value(1).toString();
        }
    }else{qDebug()<<"Monto no encontrado";return;}

    if(QString::compare(liquidadoT,"1")==0){
        ui->Pagar_3->setEnabled(false);
        ui->Pagar_3->setStyleSheet("background-color: gray");
    }


    ui->Piso_3->setText(nPiso);
    ui->Cajon_3->setText(CNumero);
    ui->Dueno_4->setText(nombreT);//
    ui->no_Tarjeta_3->setText(numT);//
    ui->Tipo_3->setText(tipoT);//
    ui->precioxd->setText(preDia);//
    ui->Entrada_3->setText(fIni);//
    ui->Salida_3->setText(fFin);//
    ui->lMonto_3->setText(tPagarT);


    ui->stackedWidget->setCurrentIndex(15);
}


void MainWindow::on_volverP_clicked()
{
    ui->stackedWidget->setCurrentIndex(10);
}

void MainWindow::on_Pagar_3_clicked()
{
    QString montoPagar;
    QLineEdit Pago;
    bool ok;
    montoPagar=ui->lMonto_3->text(); /*Obtenemo el monto total a pagar*/

    /*-----------Obtenemos el id del cliente/Usuario---------*/

    QString idUser="SELECT idUsuario FROM usuario WHERE usuario='"+username+"'";
    QSqlQuery obteneridUsuario;
    obteneridUsuario.prepare(idUser);
    if(obteneridUsuario.exec()){
        while(obteneridUsuario.next()){
            idUser=obteneridUsuario.value(0).toString();
        }
    }else{qDebug()<<"Id del usuario no obtenido";}

    /*----------Obtenemos el id del cajon---------*/
    QString idP=ui->Piso_3->text(),num=ui->Cajon_3->text();
    QString idCaj="SELECT idCajon FROM cajon WHERE idPiso='"+idP+"' AND numero='"+num+"'";
    QSqlQuery obteneridCa;
    obteneridCa.prepare(idCaj);
    if(obteneridCa.exec()){
        while(obteneridCa.next()){
            idCaj=obteneridCa.value(0).toString();
        }
    }else{qDebug()<<"Id del cajon no obtenido"; return;}

    /*---------------Obtenemos el id de la tarjeta del cliente------------*/
      QString obIdT="select idTarjeta from cliente where idCliente='"+idUser+"'";
      QSqlQuery obtenerIdTarjeta;
      obtenerIdTarjeta.prepare(obIdT);
      obtenerIdTarjeta.exec();

      while(obtenerIdTarjeta.next()){
            obIdT=obtenerIdTarjeta.value(0).toString();
      }


      QString cvv="SELECT cvv FROM tarjeta WHERE idTarjeta='"+obIdT+"'";
      QSqlQuery obtenerCVV;
      obtenerCVV.prepare(cvv);
      if(obtenerCVV.exec()){

          while(obtenerCVV.next()){
              cvv=obtenerCVV.value(0).toString();
          }

      }else{qDebug()<<"CVV no obtenido ";return;}

    QString Dialogo = QInputDialog :: getText ( this , " Tarjeta " , tr ( " Escribir cvv " ), Pago.Normal , " " , & ok);
    if (ok){

        qDebug()<<Dialogo;

        if((QString::compare(cvv,Dialogo))!=0){

            qDebug()<<cvv;
            QMessageBox::about(this,"Advetencia", "CVV incorrecto, por favor vuelva a intentarlo");
            ui->stackedWidget->setCurrentIndex(15);

        }else{

            /*-----Seleccionamos el id de la pension-----*/
            QString idPen="SELECT idPension FROM pension WHERE idCajon='"+idCaj+"'";
            QSqlQuery obtenerIdPension;
            obtenerIdPension.prepare(idPen);
            if(obtenerIdPension.exec()){
                while(obtenerIdPension.next()){
                idPen=obtenerIdPension.value(0).toString();}

            }else{qDebug()<<"idPension no obtenido";}

            /*----Cambiamos el estado de la pension------*/
            QString rPagoPen="UPDATE transaccion SET liquidado='0' WHERE idPension='"+idPen+"'";
            QSqlQuery realizarPagoPension;
            realizarPagoPension.prepare(rPagoPen);
            if(realizarPagoPension.exec()){
                QMessageBox::about(this,"Confirmacion", "Pago realizado exitosamente");

            }else{qDebug()<<"Pago no realizado exitosamente";}
        }

    }else{qDebug()<<"Operacion cancelada";}
}

void MainWindow::on_ruta_clicked()
{
    QString format = ".pdf";
    QString filename = QFileDialog::getSaveFileName(this,
            "Elija el nombre", QDir::homePath(),"*.pdf");

    filename.remove(format);
    filename+=format;
    ui->ruta_2->setText(filename);
}

void MainWindow::on_genera_clicked()
{
    QString nom, pago, piso, cajon;
    pago = ui->lMonto->text();
    nom= ui->ruta_2->text();
    piso = ui->Piso->text();
    cajon = ui->cajon->text();

    QString html ="<div align=right> 10 de Octubre del 2020 </div>"
                  "<h1 align=center>Factura</br></h1> "
                  "<p align=justify>"
                  "<h2>Nombre: Parking Drive</br></h2>"
                  "<h2>Número de factura: 0 </br></h2>"
                  "<h2>RFC: CUPU800825569 </br></h2>"
                  "<h2>25 PTE no. 550 </br></h2>"
                  "<h2>72762 Puebla </br></h2>"
                  "<h2>Mexico</br></h2>"
                  "<h2>Contacto: parking.drive@gmail.com       tel: 2228441155</br></h2>"
                  "<h3>"
                  "<table width='100%' border='1' bordercolor='#000000' cellspacing='10' cellpadding='10'>"
                  "<tr>"
                  "<th>Piso</th>"
                  "<th>Cajón</th>"
                  "<th>Precio</th>"
                  "</tr>"
                  "<tr>"
                  "<th>"+piso+"</th>"
                  "<th>"+cajon+"</th>"
                  "<th>15 la hora</th>"
                  "</tr>"
                  "</br></h3>"
                  "</br></br><h1 align=right> Total: "+pago+"</h1>";

    QTextDocument document;
    document.setHtml(html);

    QPrinter printer(QPrinter::PrinterResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setPaperSize(QPrinter::A4);
    printer.setOutputFileName(nom);
    printer.setPageMargins(QMarginsF(15, 15, 15, 15));

    document.print(&printer);

    ui->ruta_2->clear();

    QMessageBox::about(this,"Factura","Factura descargada correctamente");
}
