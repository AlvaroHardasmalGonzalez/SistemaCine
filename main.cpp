#include <iostream>
#include <array>
#include "sqlite3.h"

using namespace std;

const int MAXX=5;
const int MAXY=7;

sqlite3* db;

typedef array<array<char, MAXY>, MAXX> sala;

// Crea las tablas necesarias (ASIENTOS, PELICULAS, CONFIG, ESTADO_SALAS) e inicializa valores por defecto
void creardatabase()
{
    char* mensajeError = nullptr;
    int salida = sqlite3_open("cine.db", &db);
    if(salida != SQLITE_OK)
    {
        cout << "Error al abrir la base de datos." << endl;
    }
    else
    {
        cout << "Base de datos abierta correctamente." << endl;
    }
    string sqlPelis = "CREATE TABLE IF NOT EXISTS PELICULAS (SALA INT PRIMARY KEY, NOMBRE TEXT);";
    sqlite3_exec(db, sqlPelis.c_str(), NULL, 0, &mensajeError);
    for(int i = 1; i <= 6; i++)
    {
        string sqlInit = "INSERT OR IGNORE INTO PELICULAS (SALA, NOMBRE) VALUES (" + to_string(i) + ", 'Pelicula por definir');";
        sqlite3_exec(db, sqlInit.c_str(), NULL, 0, NULL);
    }
    string sqlSalas = "CREATE TABLE IF NOT EXISTS ESTADO_SALAS (SALA INT PRIMARY KEY, ABIERTA INT);";
    sqlite3_exec(db, sqlSalas.c_str(), NULL, 0, &mensajeError);
    for(int i = 1; i <= 6; i++)
    {
        string sqlInit = "INSERT OR IGNORE INTO ESTADO_SALAS (SALA, ABIERTA) VALUES (" + to_string(i) + ", 1);";
        sqlite3_exec(db, sqlInit.c_str(), NULL, 0, NULL);
    }
    string sql = R"(CREATE TABLE IF NOT EXISTS ASIENTOS(SALA INT NOT NULL, FILA INT NOT NULL, COLUMNA INT NOT NULL, ESTADO CHAR(1) NOT NULL, PRIMARY KEY (SALA, FILA, COLUMNA)))";
    int rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &mensajeError);

    if (rc != SQLITE_OK)
    {
        cout << "Error al crear la tabla: " << mensajeError << endl;
        sqlite3_free(mensajeError);
    }
    else
    {
        cout << "Tabla verificada/creada con exito" << endl;
    }
    string sqlAdmin = "CREATE TABLE IF NOT EXISTS CONFIG (ID INT PRIMARY KEY, CLAVE TEXT);";
    rc = sqlite3_exec(db, sqlAdmin.c_str(), NULL, 0, &mensajeError);
    if (rc != SQLITE_OK)
    {
        cout << "Error al crear la tabla: " << mensajeError << endl;
        sqlite3_free(mensajeError);
    }
    string sqlInsert = "INSERT OR IGNORE INTO CONFIG (ID, CLAVE) VALUES (1, 'admin123');";
    rc = sqlite3_exec(db, sqlInsert.c_str(), NULL, 0, &mensajeError);
    if (rc != SQLITE_OK)
    {
        cout << "Error al crear la tabla: " << mensajeError << endl;
        sqlite3_free(mensajeError);
    }
}

// Callback para recuperar la contraseña de la tabla CONFIG
int callback_password(void* data, int argc, char** argv, char** azColName)
{
    string* pass = (string*)data;
    if (argv[0])
    {
        *pass = argv[0];
    }
    return 0;
}

// Callback para mapear los resultados de la DB a una matriz tipo 'sala' en C++
int callback_cargar_sala(void* data, int argc, char** argv, char** azColName)
{
    sala* s = (sala*)data;
    int fila = stoi(argv[1]);
    int col = stoi(argv[2]);
    char estado = argv[3][0];
    (*s)[fila][col] = estado;
    return 0;
}

// Callback genérico para recibir un número entero (usado para conteos de filas o asientos)
int callback_conteo(void* data, int argc, char** argv, char** azColName)
{
    int* filas = (int*)data;
    if (argv[0])
    {
        *filas = stoi(argv[0]);
    }
    return 0;
}

// Callback para imprimir los asientos en consola con formato de filas y columnas
int callback_mostrar(void* data, int argc, char** argv, char** azColName)
{
    int col = stoi(argv[2]);
    if (col == 0)
    {
        cout << argv[1] << ": ";
    }
    cout << argv[3] << " ";
    if (col == 6)
    {
        cout << endl;
    }
    return 0;
}

// Callback para capturar el nombre de la película desde la base de datos
int callback_peli(void* data, int argc, char** argv, char** azColName)
{
    string* nombre = (string*)data;
    if (argv[0]) *nombre = argv[0];
    return 0;
}

// Si la tabla ASIENTOS está vacía, genera los 210 asientos (6 salas x 35 asientos) con estado 'o' (libre)
void inicializarAsientos()
{
    int conteo = 0;
    char* mensajeError = nullptr;
    int rc = sqlite3_exec(db, "SELECT COUNT(*) FROM ASIENTOS;", callback_conteo, &conteo, &mensajeError);

    if (rc != SQLITE_OK)
    {
        cout << "Error al contar: " << mensajeError << endl;
        sqlite3_free(mensajeError);
        return;
    }
    if (conteo == 0)
    {
        cout << "Base de datos vacía. Generando asientos por primera vez..." << endl;
        sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, 0, NULL);

        for (int s = 1; s <= 6; s++)
        {
            for (int f = 0; f < MAXX; f++)
            {
                for (int c = 0; c < MAXY; c++)
                {
                    string sql = "INSERT INTO ASIENTOS VALUES (" +
                                 to_string(s) + "," + to_string(f) + "," +
                                 to_string(c) + ", 'o');";

                    sqlite3_exec(db, sql.c_str(), NULL, 0, NULL);
                }
            }
        }
        sqlite3_exec(db, "END TRANSACTION;", NULL, 0, NULL);
        cout << "Cine inicializado con éxito." << endl;
    }
    else
    {
        cout << "Asientos cargados desde la base de datos." << endl;
    }
}

// Muestra el encabezado visual del programa
void Introduccion()
{
    cout << endl;
    cout << "=============================================" << endl;
    cout << "      SISTEMA DE GESTION DE ENTRADAS" << endl;
    cout << "                 DEL CINE" << endl;
    cout << "=============================================" << endl << endl;
}

// Llena una matriz 'sala' con caracteres 'o' (utilizado para resets temporales en memoria)
void inicializar(sala&s)
{
    for(int i=0; i<MAXX; ++i)
    {
        for(int j=0; j<MAXY; ++j)
        {
            s[i][j]= 'o';
        }
    }
}

// Consulta la base de datos y dibuja el plano de asientos de la sala seleccionada
void mostrar(int salaActual)
{
    char* mensajeError = nullptr;
    cout << "   ";
    for(int k = 0; k < MAXY; ++k)
    {
        cout << k << " ";
    }
    cout << endl << "----------------" << endl;
    string sql = "SELECT * FROM ASIENTOS WHERE SALA = " + to_string(salaActual) + " ORDER BY FILA, COLUMNA;";
    sqlite3_exec(db, sql.c_str(), callback_mostrar, NULL, &mensajeError);
    cout << endl;
}

// Busca un grupo de asientos contiguos libres dentro de un rango de filas específico
void comprar_por_filas(int salaActual)
{
    int fila_1, fila_2, n;
    char* mensajeError = nullptr;
    sala s_temp;
    bool reservado=false;
    int cnt=0;
    int cnt1=0;
    string sql_load = "SELECT * FROM ASIENTOS WHERE SALA = " + to_string(salaActual) + ";";
    inicializar(s_temp);
    int rc = sqlite3_exec(db, sql_load.c_str(), callback_cargar_sala, &s_temp, &mensajeError);
    if (rc != SQLITE_OK)
    {
        cout << "Error al Cargar Datos: " << mensajeError << endl;
        sqlite3_free(mensajeError);
        return;
    }
    do
    {
        cout << endl;
        cout << "Entre que filas desea los asientos: ";
        cin >> fila_1 >> fila_2;
    }
    while((fila_1<0)||(fila_1>4)||(fila_2<0)||(fila_2>4)||(fila_1>fila_2));
    do
    {
        cout << endl;
        cout << "Cuantos asientos desea: ";
        cin >> n;
    }
    while ((n<1)||(n>35));
    for(int i=fila_1; (i<fila_2+1)&&(reservado==false); ++i)
    {
        cnt=0;
        cnt1=0;
        for(int j=0; (j<MAXY)&&(reservado==false); ++j)
        {

            if(s_temp[i][j]=='o')
            {
                ++cnt;
                if(cnt==n)
                {
                    cout << endl;
                    reservado=true;
                    if(cnt == n)
                    {
                        int rc = sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, 0, &mensajeError);
                        if (rc != SQLITE_OK)
                        {
                            cout << "Error al Iniciar Transaccion: " << mensajeError << endl;
                            sqlite3_free(mensajeError);
                            break;
                        }
                        for(int k = j; k > j - n; --k)
                        {
                            string sql_upd = "UPDATE ASIENTOS SET ESTADO = 'x' WHERE SALA = " + to_string(salaActual) + " AND FILA = " + to_string(i) + " AND COLUMNA = " + to_string(k) + ";";
                            rc = sqlite3_exec(db, sql_upd.c_str(), NULL, 0, &mensajeError);
                            if (rc != SQLITE_OK)
                            {
                                cout << "Error al Actualizar Asiento: " << mensajeError << endl;
                                sqlite3_free(mensajeError);
                                break;
                            }
                        }
                        rc = sqlite3_exec(db, "END TRANSACTION;", NULL, 0, &mensajeError);
                        if (rc != SQLITE_OK)
                        {
                            cout << "Error al Guardar Cambios: " << mensajeError << endl;
                            sqlite3_free(mensajeError);
                            break;
                        }
                        reservado = true;
                        cout << "Reserva de " << n << " asientos realizada!" << endl;
                    }
                }
            }
            else if(s_temp[i][j]=='x')
            {
                cnt=0;
            }
            ++cnt1;
        }
    }
    if(reservado==false)
    {
        cout << "No quedan " << n << " sitios libres entre esas filas" << endl;
    }
    mostrar(salaActual);
}

// Permite al usuario elegir asientos específicos uno por uno indicando fila y columna
void comprar_por_asiento(int salaActual)
{
    int nasientos, fil, col;
    char* mensajeError = nullptr;
    cout << "Cuantos asientos desea: ";
    cin >> nasientos;
    cout << endl;

    for(int i = 0; i < nasientos; i++)
    {
        bool exito = false;
        while(!exito)
        {
            cout << "Asiento " << i + 1 << " - Fila y Columna: ";
            cin >> fil >> col;

            if(fil < 0 || fil > 4 || col < 0 || col > 6)
            {
                cout << "Coordenadas invalidas." << endl;
                cout << endl;
                continue;
            }

            string sql = "UPDATE ASIENTOS SET ESTADO = 'x' WHERE SALA = " + to_string(salaActual) + " AND FILA = " + to_string(fil) + " AND COLUMNA = " + to_string(col) + " AND ESTADO = 'o';";
            int rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &mensajeError);
            if (rc != SQLITE_OK)
            {
                cout << "Error al Comprar Asiento: " << mensajeError << endl;
                sqlite3_free(mensajeError);
            }
            else if (sqlite3_changes(db) > 0)
            {
                cout << "Reservado!" << endl;
                cout << endl;
                exito = true;
            }
            else
            {
                cout << "Asiento ocupado. Elige otro." << endl;
                cout << endl;
            }
        }
    }
    mostrar(salaActual);
}

// Submenú para elegir el método de compra (por filas o por asiento individual)
void comprar_tickets(int salaActual)
{
    char c;
    cout << "Como quieres elegir las entradas (A = Por Fila, B = Por Asiento): ";
    do
    {
        cin >> c;
        switch (c)
        {
        case 'A':
            comprar_por_filas(salaActual);
            break;
        case 'B':
            comprar_por_asiento(salaActual);
            break;
        default:
            cout << "Elige una opcion correcta." << endl;
            cout << endl;
            cout << "Como quieres elegir las entradas (A = Por Fila, B = Por Asiento): ";
            break;
        }
    }
    while((c!='A')&&(c!='B'));
}

// Cambia el estado de un asiento de 'x' (reservado) a 'o' (libre) en la base de datos
void cancelar_tickets(int salaActual)
{
    int fila,columna;
    char* mensajeError = nullptr;
    do
    {
        cout << endl;
        cout << "Fila (0-4) y columna (0-6) a cancelar: ";
        cin >> fila >> columna;
    }
    while((fila<0)||(fila>4)||(columna<0)||(columna>6));
    string sql = "UPDATE ASIENTOS SET ESTADO = 'o' WHERE SALA = " + to_string(salaActual) + " AND FILA = " + to_string(fila) + " AND COLUMNA = " + to_string(columna) + " AND ESTADO = 'x';";
    int rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &mensajeError);
    if (rc != SQLITE_OK)
    {
        cout << "Error al Cancelar Reserva: " << mensajeError << endl;
        sqlite3_free(mensajeError);
    }
    else if (sqlite3_changes(db) > 0)
    {
        cout << "Reserva cancelada con Exito." << endl;
        cout << endl;
    }
    else
    {
        cout << "Ese asiento ya estaba libre o no existe." << endl;
        cancelar_tickets(salaActual);
    }
}

// Borra todas las reservas de una sala específica poniendo todos sus asientos en 'o'
void reset_asientos()
{
    int salaAResetear;
    char* mensajeError = nullptr;
    cout << "Que sala desea resetear completamente? (1-6): ";
    cin >> salaAResetear;
    if (salaAResetear < 1 || salaAResetear > 6)
    {
        cout << "Error: Sala no valida. Operacion cancelada." << endl;
        return;
    }
    string sql = "UPDATE ASIENTOS SET ESTADO = 'o' WHERE SALA = " + to_string(salaAResetear) + ";";
    int rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &mensajeError);
    if (rc != SQLITE_OK)
    {
        cout << "Error al resetear la sala: " << mensajeError << endl;
        sqlite3_free(mensajeError);
    }
    else
    {
        if (sqlite3_changes(db) > 0)
        {
            cout << "SALA " << salaAResetear << " ha sido reseteada con exito. Todos los asientos estan libres." << endl;
        }
        else
        {
            cout << "La sala ya estaba vacía o no se encontraron asientos." << endl;
        }
    }
}

// Verifica en la tabla ESTADO_SALAS si una sala está habilitada para la venta
bool esta_sala_abierta(int salaId)
{
    int abierta = 1; // Por defecto abierta
    string sql = "SELECT ABIERTA FROM ESTADO_SALAS WHERE SALA = " + to_string(salaId) + ";";

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        abierta = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    return abierta == 1;
}

// Permite al administrador bloquear (0) o abrir (1) el acceso a una sala
void gestionar_bloqueo_sala()
{
    int salaId, nuevoEstado;
    cout << "Que sala quieres gestionar (1-6)?: ";
    cin >> salaId;
    cout << "Qu quieres hacer? (0 = Bloquear, 1 = Abrir): ";
    cin >> nuevoEstado;

    string sql = "UPDATE ESTADO_SALAS SET ABIERTA = " + to_string(nuevoEstado) + " WHERE SALA = " + to_string(salaId) + ";";
    if (sqlite3_exec(db, sql.c_str(), NULL, 0, NULL) == SQLITE_OK)
    {
        cout << "Sala " << salaId << (nuevoEstado == 0 ? " BLOQUEADA" : " ABIERTA") << " con exito." << endl;
    }
}

// Calcula el dinero recaudado multiplicando los asientos con estado 'x' por el precio de la entrada
void revisar_ingresos()
{
    int ocupados = 0;
    char* mensajeError = nullptr;
    double precioEntrada = 5.50;
    string sql = "SELECT COUNT(*) FROM ASIENTOS WHERE ESTADO = 'x';";
    int rc = sqlite3_exec(db, sql.c_str(), callback_conteo, &ocupados, &mensajeError);
    if (rc != SQLITE_OK)
    {
        cout << "Error al consultar ingresos: " << mensajeError << endl;
        sqlite3_free(mensajeError);
    }
    else
    {
        double ingresosTotales = ocupados * precioEntrada;
        cout << endl;
        cout << "=============================================" << endl;
        cout << "           REPORTE DE INGRESOS" << endl;
        cout << "=============================================" << endl;
        cout << " Asientos vendidos (totales): " << ocupados << endl;
        cout << " Precio por asiento:         " << precioEntrada << " eur" << endl;
        cout << "---------------------------------------------" << endl;
        cout << " INGRESOS TOTALES:           " << ingresosTotales << " eur" << endl;
        cout << "=============================================" << endl;
    }
}

// Actualiza el nombre de la película proyectada en una sala determinada
void cambiar_pelicula() {
    int numSala;
    string nuevoNombre;
    cout << "Ingrese el numero de sala (1-6): ";
    cin >> numSala;
    cin.ignore(); // Limpiar el buffer para usar getline
    cout << "Ingrese el nuevo nombre de la pelicula: ";
    getline(cin, nuevoNombre);
    string sql = "UPDATE PELICULAS SET NOMBRE = '" + nuevoNombre + "' WHERE SALA = " + to_string(numSala) + ";";
    if (sqlite3_exec(db, sql.c_str(), NULL, 0, NULL) == SQLITE_OK) {
        cout << "Cartelera actualizada con exito." << endl;
    }
}

// Panel de control protegido por contraseña para gestionar la configuración del cine
void menu_admin()
{
    string password_db = "";
    char c;
    char* mensajeError = nullptr;
    string sql = "SELECT CLAVE FROM CONFIG WHERE ID = 1;";
    sqlite3_exec(db, sql.c_str(), callback_password, &password_db, &mensajeError);
    string intento_usuario;
    cout << endl;
    cout << "Ingrese clave de administrador: ";
    cin >> intento_usuario;
    if (intento_usuario == password_db)
    {
        cout << endl;
        cout << "ACCESO TOTAL CONCEDIDO" << endl;
        cout << "1. Resetear todos los asientos" << endl;
        cout << "2. Abrir/Cerrar Sala" << endl;
        cout << "3. Revisar Ingresos" << endl;
        cout << "4. Actualizar Pelicula" << endl;
        cout << "5. Salir" << endl << endl;
        cout << "Elige una opcion: ";
        cin >> c;
        switch (c)
        {
        case '1':
            char confirmar;
            cout << "ESTA SEGURO? Esta accion no se puede deshacer (S/N): ";
            cin >> confirmar;
            if (confirmar == 'S' || confirmar == 's')
            {
                reset_asientos();
            }
            else
            {
                cout << "Operacion cancelada." << endl;
            }
            break;
        case '2':
            gestionar_bloqueo_sala();
            break;
        case '3':
            revisar_ingresos();
            break;
        case '4':
            cambiar_pelicula();
            break;
        case '5':
            break;
        default:
            cout << "Opcion incorrecta, saliendo del menu de admin." << endl;
            break;
        }
    }
    else
    {
        cout << "Error: Clave incorrecta." << endl;
    }
}

// Recupera el nombre de la película de una sala para mostrarlo en el menú principal
string obtener_nombre_peli(int salaActual)
{
    string nombre = "";
    string sql = "SELECT NOMBRE FROM PELICULAS WHERE SALA = " + to_string(salaActual) + ";";
    sqlite3_exec(db, sql.c_str(), callback_peli, &nombre, NULL);
    return nombre;
}

// Inicializacion de tablas y bucle del menú principal
int main()
{
    cout << "Mensajes del Sistema:" << endl;
    creardatabase();
    inicializarAsientos();
    Introduccion();
    int salaActual = 1;
    char c;
    while(1)
    {
        string peliActual = obtener_nombre_peli(salaActual);
        cout << "---------------------------------------------" << endl;
        cout << "SALA: " << salaActual << " | PELICULA: " << peliActual << endl;
        cout << "---------------------------------------------" << endl;
        cout << "Menu Principal:" << endl;
        cout << "1. Comprar" << endl;
        cout << "2. Cancelar" << endl;
        cout << "3. Ver Sala" << endl;
        cout << "4. Cambiar Sala" << endl;
        cout << "5. Menu Admin" << endl;
        cout << "6. Salir" << endl << endl;
        cout << "Elige una opcion: ";
        cin >> c;
        switch (c)
        {
        case '1':
            cout << endl;
            if (!esta_sala_abierta(salaActual))
            {
                cout << "ERROR! La sala " << salaActual << " esta cerrada por mantenimiento." << endl;
            }
            else
            {
                comprar_tickets(salaActual);
            }
            break;
            break;
        case '2':
            cancelar_tickets(salaActual);
            break;
        case '3':
            cout << endl << "Sala Actual: " << salaActual << endl;
            mostrar(salaActual);
            break;
        case '4':
            cout << "A que sala quieres cambiar (1-6): ";
            cin >> salaActual;
            cout << endl;
            if(salaActual < 1 || salaActual > 6)
            {
                cout << "Sala invalida, volviendo a sala 1." << endl;
                cout << endl;
                salaActual = 1;
            }
            break;
        case '5':
            menu_admin();
            cout << endl;

            break;
        case '6':
            cout << endl;
            cout << "Programa Finalizado." << endl;
            sqlite3_close(db);
            return 0;
            break;
        default:
            cout << "Elija una opcion valida." << endl;
            cout << endl;
            break;
        }
    }
}
