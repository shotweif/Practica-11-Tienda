    #include "acerca.h"
#include "ui_acerca.h"
#include "ui_tienda.h"
#include "tienda.h"
#include <QDebug>

Acerca::Acerca(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Acerca)
{
    ui->setupUi(this);
    mostrarFactuira();
}

Acerca::~Acerca()
{
    delete ui;
}

QString Acerca::dato() const
{
    return m_dato;
}

void Acerca::setDato(const QString &dato)
{
    m_dato = dato;
}

void Acerca::mostrarFactuira()
{
    ui->outFact->setText(m_dato);

}

