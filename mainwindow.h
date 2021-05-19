#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPdfWriter>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QDebug>
#include <QtSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMessageBox>
#include <QDebug>
#include <QTableWidgetItem>
#include<QtCore>
#include<QtGui>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QPixmap>
#include <QLabel>
#include <QInputDialog>
#include <QDialogButtonBox>
#include <QTimer>
#include <qdatetime.h>
#include <QMainWindow>
#include <QTimer>
#include <QSound>
#include <QLocale>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QByteArray imagen1, imagen2;
    QString ruta, ruta1, ruta2, ruta3, iniciar, username, password, admin, in;
    QString   aniversario="2020-10-19 06:00:00";
    QString   aniversario2="2020-10-19 23:59:00";
    int f;
    QString Tipo, hInicio, hFin;
    int carWidth = 120;
    int first = 0;
    int second = 0;
    int max = 12;


    QString pagoxhor = "15", pagoxdia = "100", piso, cajon;

private slots:

    void Ofer();

    void on_regresar_clicked();

    void on_Siguiente_clicked();

    void on_IniciarSesion_clicked();

    void on_registro_clicked();

    void on_OContra_clicked();

    void on_regresar_2_clicked();

    void on_Crear_clicked();

    void on_regresar_3_clicked();

    void on_cambiarC_clicked();

    void on_estacionamiento_clicked();

    void on_perfil_clicked();

    void on_reservacion_clicked();

    void on_salir_clicked();

    void on_estacionamiento_2_clicked();

    void on_perfil_2_clicked();

    void on_reservacion_2_clicked();

    void on_salir_2_clicked();

    void on_numero1_clicked();

    void on_numero2_clicked();

    void on_numero3_clicked();

    void on_numero4_clicked();

    void on_numero5_clicked();

    void on_numero6_clicked();

    void on_numero7_clicked();

    void on_numero8_clicked();

    void on_numero9_clicked();

    void on_numero10_clicked();

    void on_Piso2num1_clicked();

    void on_Piso2num2_clicked();

    void on_Piso2num3_clicked();

    void on_Piso2num4_clicked();

    void on_Piso2num5_clicked();

    void on_Piso2num6_clicked();

    void on_Piso2num7_clicked();

    void on_Piso2num8_clicked();

    void on_Piso2num9_clicked();

    void on_Piso2num10_clicked();

    void on_Piso3num1_clicked();

    void on_Piso3num2_clicked();

    void on_Piso3num3_clicked();

    void on_Piso3num4_clicked();

    void on_Piso3num5_clicked();

    void on_Piso3num6_clicked();

    void on_Piso3num7_clicked();

    void on_Piso3num8_clicked();

    void on_Piso3num9_clicked();

    void on_Piso3num10_clicked();

    void on_regresar_4_clicked();

    void on_autofoto_clicked();

    void on_reservarcajon_clicked();

    void on_regresar_5_clicked();

    void on_autofoto_2_clicked();

    void on_reservarcajon_2_clicked();

    void on_regresar_6_clicked();

    void on_autofoto_3_clicked();

    void on_reservarcajon_3_clicked();

    void on_estacionamiento_3_clicked();

    void on_perfil_3_clicked();

    void on_reservacion_3_clicked();

    void on_salir_3_clicked();

    void on_CambiarFoto_clicked();

    void on_aceptar_clicked();

    void on_estacionamiento_4_clicked();

    void on_perfil_4_clicked();

    void on_reservacion_4_clicked();

    void on_salir_4_clicked();

    void on_Pagar_clicked();

    void on_Volver_clicked();

    void on_Ir_clicked();

    void Datos();

    void DatosDireccion();

    void DatosFoto();

    void Foto();

    void TablaReservacion();

    void DatosTarjeta();

    void Piso();

    void Cajones();

    void on_btnCancelarReserva_clicked();

    /*Obtenemos los ID*/

    QString obtenerIDCajon();

    void Animacion(int xright);
    void Animacion2(int xright);
    void Animacion3(int xright);
    void Descansar();
    void Calcular(int num);

    void Tiempo();


    void on_verDetalles_clicked();

    void on_volverregdos_clicked();

    void on_pbSalir_clicked();

    void on_Extender_clicked();


    void on_crearOferta_clicked();

    void on_Volver_2_clicked();

    void on_Ofertas_clicked();

    void on_pushButton_clicked();

    void on_volverP_clicked();

    void on_Pagar_3_clicked();

    void on_ruta_clicked();

    void on_genera_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
