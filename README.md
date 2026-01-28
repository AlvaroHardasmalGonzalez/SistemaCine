# Sistema Cine
Este programa implementa un sistema básico de gestión de entradas para un cine con seis salas. Cada sala se modela como una cuadrícula de 5 filas por 7 columnas (35 asientos), donde cada asiento puede estar libre ('o') o reservado ('x'). El objetivo es permitir al usuario comprar, cancelar y visualizar reservas de manera interactiva desde la consola, así como cambiar de sala cuando lo necesite.
# Utildades y Menú
*SALAS DISPONIBLES*
 - Se gestionan 6 salas (1 a 6).  
 - Cada sala tiene 5 filas (0-4) y 7 columnas (0-6).  

*MENU PRINCIPAL*
 - Comprar
 - Cancelar
 - Ver sala
 - Cambiar sala
 - Salir  

*COMPRAR*
 - Dos modos de compra:  
   A) Por filas: elegir rango de filas [0-4] y cantidad (1-35).  
      El sistema busca un bloque contiguo de asientos libres  
      dentro del rango indicado y lo reserva si existe.  
   B) Por asiento: indicar fila [0-4] y columna [0-6] para cada asiento.  
      Si el asiento está libre, se reserva; si está ocupado,  
      se solicita otro.  

*CANCELAR* 
 - Indicar fila [0-4] y columna [0-6].  
 - Si el asiento estaba reservado, se libera;  
   si estaba libre, se informa al usuario.  

*VER SALA* 
 - Muestra el plano con columnas 0-6 y filas 0-4.  
 - 'o' = libre, 'x' = reservado.  

*CAMBIAR SALA*  
 - Permite seleccionar otra sala (1 a 6).  
 - Si la opción es inválida, se usa la sala 1 por defecto.  

*RESTRICCIONES Y VALIDACIONES*  
 - Filas válidas: 0 a 4.  
 - Columnas válidas: 0 a 6.  
 - Cantidad de asientos: 1 a 35.  
 - Si se introducen valores inválidos, se solicitan nuevamente.  

-----------------------------------------------------------------------------
