drop database if exists Estacionamiento;
create database Estacionamiento;
USE Estacionamiento;

delimiter $$
drop procedure if exists  FotoAuto$$
create procedure FotoAuto(in id int )
begin
   SELECT f.imagen
   FROM 
      fotos as f inner join Vehiculo as V on f.idFotos = v.idFotos
      where v.idVehiculo = id;
      
end $$ 
delimiter ;


delimiter $$
drop procedure if exists  perfil$$
create procedure perfil(in nom varchar(50))
begin
   SELECT f.imagen
   FROM 
      fotos as f inner join cliente as c on f.idFotos=c.idFotos 
      where c.nombre=nom;
end $$
delimiter ;


CREATE TABLE IF NOT EXISTS Usuario(
  idUsuario INT NULL AUTO_INCREMENT,
  usuario NVARCHAR(45) not null,
  contrasena NVARCHAR(45) not null,
  PRIMARY KEY (idUsuario));
  
  CREATE TABLE IF NOT EXISTS Admin(
  idAdmin INT NULL AUTO_INCREMENT,
  idUsuario INT NOT NULL,
  FOREIGN KEY (idUsuario) REFERENCES Usuario(idUsuario) ON DELETE NO ACTION ON UPDATE NO ACTION,
  PRIMARY KEY (idAdmin));
  
   CREATE TABLE IF NOT EXISTS Piso(
  idPiso INT NULL AUTO_INCREMENT,
  num_piso INT NOT NULL,
  num_cajon INT NULL,
  lleno TINYINT NOT NULL,
  PRIMARY KEY (idPiso));

CREATE TABLE IF NOT EXISTS Cajon(
  idCajon INT NULL AUTO_INCREMENT,
  idPiso INT NOT NULL,
  numero INT NOT NULL,
  lleno TINYINT NOT NULL,
  FOREIGN KEY (idPiso) REFERENCES Piso(idPiso) ON DELETE NO ACTION ON UPDATE NO ACTION,
PRIMARY KEY (idCajon));

CREATE TABLE IF NOT EXISTS Fotos(
  idFotos INT NULL AUTO_INCREMENT,
  ruta NVARCHAR(50) NOT NULL,
  imagen BLOB NULL,
  PRIMARY KEY (idFotos));

CREATE TABLE IF NOT EXISTS Domicilio(
  idDomicilio INT NULL AUTO_INCREMENT,
  calle NVARCHAR(45) NOT NULL,
  colonia NVARCHAR(45) NOT NULL,
  cp INT NOT NULL,
  no_exterior INT NOT NULL,
  no_interior INT NULL,
  PRIMARY KEY (idDomicilio));
  
  CREATE TABLE IF NOT EXISTS Tarjeta(
  idTarjeta INT NULL AUTO_INCREMENT,
  num VARCHAR(45) NOT NULL,
  tipo TINYINT NOT NULL,
  nombre NVARCHAR(50) NOT NULL,
  agno INT NOT NULL,
  mes INT NOT NULL,
  cvv INT NOT NULL,
  PRIMARY KEY (idTarjeta));
 
 CREATE TABLE IF NOT EXISTS Cliente(
  idCliente INT NULL AUTO_INCREMENT,
  Nombre NVARCHAR(45) NOT NULL,
  Apellido_p NVARCHAR(45) NOT NULL,
  Apellido_m NVARCHAR(45) NOT NULL,
  correo NVARCHAR(45) NOT NULL,
  telefono NVARCHAR(45) NOT NULL,
  idUsuario INT NULL,
  idDomicilio INT NULL,
  idTarjeta INT NULL,
  idFotos INT NULL,
  FOREIGN KEY(idUsuario) references Usuario (idUsuario) ON DELETE NO ACTION ON UPDATE NO ACTION,
  FOREIGN KEY(idDomicilio) references Domicilio (idDomicilio) ON DELETE NO ACTION ON UPDATE NO ACTION,
  FOREIGN KEY(idTarjeta) references Tarjeta (idTarjeta) ON DELETE NO ACTION ON UPDATE NO ACTION,
  FOREIGN KEY(idFotos) references Fotos(idFotos) ON DELETE NO ACTION ON UPDATE NO ACTION,
  PRIMARY KEY (idCliente));
 
CREATE TABLE IF NOT EXISTS Vehiculo(
  idVehiculo INT NULL AUTO_INCREMENT,
  matricula NVARCHAR(45) NOT NULL,
  modelo NVARCHAR(45) NOT NULL,
  descripcion MEDIUMTEXT NOT NULL,
  idCliente INT NOT NULL,
  idFotos	INT NULL,
  FOREIGN KEY (idCliente) REFERENCES Cliente(idCliente) ON DELETE NO ACTION ON UPDATE NO ACTION,
  FOREIGN KEY(idFotos) references Fotos(idFotos) ON DELETE NO ACTION ON UPDATE NO ACTION,
  PRIMARY KEY (idVehiculo)); 
  
   CREATE TABLE IF NOT EXISTS Pension(
  idPension INT NULL AUTO_INCREMENT,
  fecha_inicio DATE,
  fecha_fin DATE,
  precioxdia DOUBLE NOT NULL,
  idCliente INT NOT NULL,
  idCajon	INT NOT NULL,
  idAuto INT NOT NULL,
  FOREIGN KEY (idCliente) REFERENCES Cliente(idCliente) ON DELETE NO ACTION ON UPDATE NO ACTION,
   FOREIGN KEY (idCajon) REFERENCES Cajon(idCajon) ON DELETE NO ACTION ON UPDATE NO ACTION,
   FOREIGN KEY (idAuto) REFERENCES Vehiculo(idVehiculo) ON DELETE NO ACTION ON UPDATE NO ACTION,
   PRIMARY KEY (idPension));

CREATE TABLE IF NOT EXISTS Reservacion(
  idReservacion INT NULL AUTO_INCREMENT,
  inicio DATETIME NOT NULL,
  fin DATETIME NOT NULL,
  pagoxhora DOUBLE NOT NULL,
  idCliente INT NOT NULL,
  idCajon	INT NOT NULL,
  idAuto INT NOT NULL,
  estado TINYINT NULL,
  FOREIGN KEY (idCliente) REFERENCES Cliente(idCliente) ON DELETE NO ACTION ON UPDATE NO ACTION,
   FOREIGN KEY (idCajon) REFERENCES Cajon(idCajon) ON DELETE NO ACTION ON UPDATE NO ACTION,
  FOREIGN KEY (idAuto) REFERENCES Vehiculo(idVehiculo) ON DELETE NO ACTION ON UPDATE NO ACTION,
  PRIMARY KEY (idReservacion));
  
 CREATE TABLE IF NOT EXISTS Transaccion (
  idtransaccion INT NULL AUTO_INCREMENT,
  total_a_pagar DOUBLE NULL,
  liquidado TINYINT NOT NULL,
  idAdmin INT NULL,
  idReservacion INT NULL,
  idPension INT NULL,
  FOREIGN KEY(idAdmin) references Admin(idAdmin) ON DELETE NO ACTION ON UPDATE NO ACTION,
  FOREIGN KEY(idReservacion) references Reservacion(idReservacion) ON DELETE NO ACTION ON UPDATE NO ACTION,
  FOREIGN KEY(idPension) references Pension(idPension) ON DELETE NO ACTION ON UPDATE NO ACTION,
  PRIMARY KEY (idtransaccion));
  
  -- insertar pisos
INSERT INTO piso(num_piso,num_cajon,lleno) VALUES(1,10,0),(2,10,0),(3,10,0);
-- insertar cajones
INSERT INTO cajon(idPiso,numero,lleno) 
VALUES(1,1,0),(1,2,0),(1,3,0),(1,4,0),(1,5,0),(1,6,0),(1,7,0),(1,8,0),(1,9,0),(1,10,0),
(2,1,0),(2,2,0),(2,3,0),(2,4,0),(2,5,0),(2,6,0),(2,7,0),(2,8,0),(2,9,0),(2,10,0),
(3,1,0),(3,2,0),(3,3,0),(3,4,0),(3,5,0),(3,6,0),(3,7,0),(3,8,0),(3,9,0),(3,10,0);