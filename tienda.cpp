#include "tienda.h"
#include "ui_tienda.h"
#include "acerca.h"
#include "ui_acerca.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QDir>
#include <QDebug>
#include <cstdlib>


Tienda::Tienda(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Tienda)
{
    ui->setupUi(this);

    m_subtotal = 0;

    cargarDatos();
    inicilizarWidgets();

}

Tienda::~Tienda()
{
    delete ui;
}

void Tienda::mostrarPrecio(int indice)
{
    // obtener el producto que se ha seleccionado en el combo
    float precioProducto = m_productos.at(indice)->precio();

    // Mostrar el precio en el widget correspondiente
    ui->outPrecio->setText("$ " + QString::number(precioProducto));
}

void Tienda::cargarDatos()
{
    // Crear y agregar productos a la lista
    //m_productos.append(new Producto(1,"Pan", 0.15));
    //m_productos.append(new Producto(2,"Leche", 0.80));
    //m_productos.append(new Producto(3,"Huevos", 0.12));

    QString pathActual = QDir::currentPath();
    QFile archivo(pathActual + "/productos.csv");

    bool primeraLinea = true;
    if (archivo.open(QFile::ReadOnly)){
        QTextStream in(&archivo);
        while(!in.atEnd()){
            QString linea = in.readLine();
            if (primeraLinea){
                primeraLinea = false;
                continue;
            }
            // separar los datos por ','
            QStringList datos = linea.split(";");
            int codigo = datos[0].toInt();
            float precio = datos[2].toFloat();
            // Crear producto
            m_productos.append(new Producto(codigo,datos[1],precio));
        }
    }else{
        QMessageBox msgBox;
        msgBox.setText("El archivo de prodfuctos no se puede abrir.");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
    }

}

void Tienda::inicilizarWidgets()
{
    // Agrega los productos al combo box
    for(int i=0; i < m_productos.length(); i++){
        ui->inProducto->addItem(m_productos.at(i)->nombre());
    }

    // Colocar cabecera a la tabla
    QStringList cabecera = {"Cantidad", "Nombre", "Sutotal"};
    ui->outDetalle->setColumnCount(3);
    ui->outDetalle->setHorizontalHeaderLabels(cabecera);

    // Eventos
    connect(ui->inProducto, SIGNAL(currentIndexChanged(int)),
            this, SLOT(mostrarPrecio(int)));

    // Mostrr el precio del primer producto
    mostrarPrecio(0);
}

void Tienda::calcular(float subtotal)
{
    // Calcular valores
    //fixed(qSetRealNumberPrecision(2));
    m_subtotal += subtotal;
    float iva = m_subtotal * IVA;
    float total = m_subtotal + iva;
    // Mostrar valores en UI
    ui->outSubtotal->setText(QString::number(m_subtotal,'f',2));
    ui->outIva->setText(QString::number(iva,'f',2));
    ui->outTotal->setText(QString::number(total,'f',2));
}

void Tienda::validarCedula()
{
    QString cedula = ui->inCedula->text();
    if(ui->inCedula->text() != ""){
        if (cedula.length() != 10){
            ui->inCedula->setStyleSheet("background-color: rgb(239, 41, 41);");
        }else{
            ui->inCedula->setStyleSheet("background-color: rgb(132, 225, 113);");
        }
    }
}

void Tienda::validarNombre()
{
    QString nombre = ui->inNombre->text();
    if(ui->inNombre->text() != ""){
        if(nombre.length() > 3){
            ui->inNombre->setStyleSheet("background-color: rgb(239, 41, 41);");
        }else{
            ui->inNombre->setStyleSheet("background-color: rgb(132, 225, 113);");
        }
    }
}

void Tienda::on_cmdAgregar_clicked()
{
    // Obtener el producto seleccionado
    int indice = ui->inProducto->currentIndex();
    Producto *p = m_productos.at(indice);
    // Obtener la cantidad
    int cantidad = ui->inCantidad->value();

    // Validar: si la cantidad es 0, no hace nada
    if (cantidad == 0){
        return;
    }
    // Calcular el subtotal
    float subtotal = cantidad * p->precio();

    // Agregar a la tabla
    int posicion = ui->outDetalle->rowCount();



    if(posicion !=0){
        for(int x=0;x<posicion;x++) {
            if(p->nombre() == ui->outDetalle->item(x,1)->text()){
                ui->outDetalle->item(x,1)->text();

                QString ds=ui->outDetalle->item(x,2)->text();
                float value = ds.toFloat();

                ui->outDetalle->item(x,2)->setText(QString::number(subtotal+value,'f',2));

                ui->inCantidad->setValue(0);
                ui->inProducto->setFocus();
                calcular(subtotal);
                return;
            }
        }
    }
    ui->outDetalle->insertRow(posicion);
    ui->outDetalle->setItem(posicion,0,new QTableWidgetItem(QString::number(cantidad)));
    ui->outDetalle->setItem(posicion,1,new QTableWidgetItem(p->nombre()));
    ui->outDetalle->setItem(posicion,2,new QTableWidgetItem(QString::number(subtotal,'f',2)));

    // Limpiar widgets
    ui->inCantidad->setValue(0);
    ui->inProducto->setFocus();

    // calcular
    calcular(subtotal);


}
//------------------------------------
void Tienda::on_cmdFinalizar_clicked()
{
    if(ui->inNombre->text() == ""){
        QMessageBox::information(this, tr("Error"),tr("Nombre no ingresado"));
        return;
    }else if(ui->inCedula->text() == ""){
        QMessageBox::information(this, tr("Error"),tr("Cedula no ingresada"));
        return;
    }

    int posicion = ui->outDetalle->rowCount();
    QString DateFact = "";

    for (int x=0;x<posicion;x++) {
        DateFact += ui->outDetalle->item(x,0)->text()+"\t"+
                    ui->outDetalle->item(x,1)->text()+"\t"+
                    ui->outDetalle->item(x,2)->text()+"\n";
    }
        QString direccion = ui->inDirection->toPlainText();
        QString Datos = "\nCI: " + ui->inCedula->text()+
                    "\nNombre: " + ui->inNombre->text()+
                    "\nTelefono: " + ui->inTelefono->text()+
                    "\nCorreo: " + ui->inEmail->text()+
                    "\nDireccion: "+ direccion +"\n"
                    "\nPriducto\tCantidad\tValor\n"+
                    DateFact +
                    "\n"
                    "\n\tSubtotal:\t" + ui->outSubtotal->text()+
                    "\n\tIVA:\t" + ui->outIva->text()+
                    "\n\tTOTAL:\t" + ui->outTotal->text();

    Acerca *dialogAcerca= new Acerca(this);
    dialogAcerca->setDato(Datos);
    dialogAcerca->mostrarFactuira();
    dialogAcerca->show();
}
//------------------------------------
void Tienda::on_inCedula_editingFinished()
{
    validarCedula();
    validarNombre();
}
