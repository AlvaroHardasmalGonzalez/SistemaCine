# Sistema Cine
Este programa implementa un sistema integral de gestión de entradas para un cine con seis salas, utilizando SQLite3 para que los datos se guarden permanentemente. Cada sala se modela como una cuadrícula de 5 filas por 7 columnas (35 asientos), donde cada asiento puede estar libre ('o') o reservado ('x'). El sistema incluye un panel de administración protegido por contraseña para gestionar la cartelera, el estado de las salas y las finanzas del cine.

# Utilidades y Menú
*SALAS DISPONIBLES*
 - Se gestionan 6 salas (1 a 6).
 - Cada sala tiene su propia película asignada y un estado (Abierta/Cerrada).
 - Dimensiones: 5 filas (0-4) y 7 columnas (0-6).

*MENU PRINCIPAL*
 - 1. Comprar: Accede al submenú de reservas (solo si la sala está abierta).
 - 2. Cancelar: Libera asientos previamente reservados.
 - 3. Ver sala: Muestra el plano de asientos y la película actual.
 - 4. Cambiar sala: Permite navegar entre las salas 1 a 6.
 - 5. Menu Admin: Panel de control protegido por contraseña (admin123 por defecto).
 - 6. Salir: Cierra el programa y la conexión a la base de datos.

*COMPRAR*
 - Dos modos de compra:
   A) Por filas: El sistema busca un bloque contiguo de asientos libres dentro de un rango de filas.
   B) Por asiento: Selección manual mediante coordenadas exactas de fila y columna.

*CANCELAR*
 - Se indica fila [0-4] y columna [0-6].
 - El sistema verifica que el asiento esté realmente reservado antes de liberarlo.

*MENU ADMIN (PANEL DE CONTROL)*
 - 1. Resetear asientos: Vacía por completo todos los asientos de una sala elegida.
 - 2. Abrir/Cerrar Sala: Permite bloquear una sala por mantenimiento, impidiendo ventas.
 - 3. Revisar Ingresos: Calcula la recaudación total basada en los asientos vendidos (Precio: 5.50 eur).
 - 4. Actualizar Película: Permite cambiar el título del filme proyectado en cada sala.

*RESTRICCIONES Y VALIDACIONES*
 - Seguridad: Solo se puede acceder al Menú Admin con la clave correcta almacenada en la base de datos.
 - Validación: Filas válidas (0 a 4), Columnas válidas (0 a 6), Cantidad (1 a 35).
 - Disponibilidad: Si una sala está cerrada por el admin, los usuarios no pueden realizar compras en ella.
 - Persistencia: Todos los cambios (ventas, nombres de películas, bloqueos) se guardan automáticamente en cine.db.

-----------------------------------------------------------------------------