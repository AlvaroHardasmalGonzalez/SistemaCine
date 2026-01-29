#include <iostream>
#include <array>

using namespace std;

const int MAXX=5;
const int MAXY=7;

typedef array<array<char, MAXY>, MAXX> sala;

void Introduccion()
{
    cout << "=============================================" << endl;
    cout << "      SISTEMA DE GESTION DE ENTRADAS" << endl;
    cout << "                 DEL CINE" << endl;
    cout << "=============================================" << endl << endl;
    cout << "SALAS DISPONIBLES" << endl;
    cout << " - Se gestionan 6 salas (1 a 6)." << endl;
    cout << " - Cada sala tiene 5 filas (0-4) y 7 columnas (0-6)." << endl << endl;
    cout << "MENU PRINCIPAL" << endl;
    cout << " A) Comprar" << endl;
    cout << " B) Cancelar" << endl;
    cout << " C) Ver sala" << endl;
    cout << " D) Cambiar sala" << endl;
    cout << " E) Salir" << endl << endl;
    cout << "COMPRAR" << endl;
    cout << " - Dos modos de compra:" << endl;
    cout << "   A) Por filas: elegir rango de filas [0-4] y cantidad (1-35)." << endl;
    cout << "      El sistema busca un bloque contiguo de asientos libres" << endl;
    cout << "      dentro del rango indicado y lo reserva si existe." << endl;
    cout << "   B) Por asiento: indicar fila [0-4] y columna [0-6] para cada asiento." << endl;
    cout << "      Si el asiento esta libre, se reserva; si esta ocupado," << endl;
    cout << "      se solicita otro." << endl << endl;
    cout << "CANCELAR" << endl;
    cout << " - Indicar fila [0-4] y columna [0-6]." << endl;
    cout << " - Si el asiento estaba reservado, se libera;" << endl;
    cout << "   si estaba libre, se informa al usuario." << endl << endl;
    cout << "VER SALA" << endl;
    cout << " - Muestra el plano con columnas 0-6 y filas 0-4." << endl;
    cout << " - 'o' = libre, 'x' = reservado." << endl << endl;
    cout << "CAMBIAR SALA" << endl;
    cout << " - Permite seleccionar otra sala (1 a 6)." << endl;
    cout << " - Si la opcion es invalida, se usa la sala 1 por defecto." << endl << endl;
    cout << "RESTRICCIONES Y VALIDACIONES" << endl;
    cout << " - Filas validas: 0 a 4." << endl;
    cout << " - Columnas validas: 0 a 6." << endl;
    cout << " - Cantidad de asientos: 1 a 35." << endl;
    cout << " - Si se introducen valores invalidos, se solicitan nuevamente." << endl << endl;
    cout << "-----------------------------------------------------------------------------" << endl << endl;
}

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

sala* elegir_sala(sala&s1, sala&s2, sala&s3, sala&s4, sala&s5, sala&s6)
{
    cout << "Que sala quieres ver? (1, 2, 3, 4, 5, 6): ";
    char sala;
    cin >> sala;
    switch (sala)
    {
    case '1':
        return &s1;
        break;
    case '2':
        return &s2;
        break;
    case '3':
        return &s3;
        break;
    case '4':
        return &s4;
        break;
    case '5':
        return &s5;
        break;
    case '6':
        return &s6;
        break;
    default:
        cout << "Sala equivocada, usando sala 1 por defecto." << endl;
        return &s1;
        break;
    }
}

void mostrar(sala s)
{
    cout << "   ";
    for(int k=0; k<MAXY; ++k)
    {
        cout << k << " ";
    }
    cout << endl;

    for(int k=0; k<16; ++k)
    {
        cout << "-";
    }
    cout << endl;
    for(int i=0; i<MAXX; ++i)
    {
        cout << i << ": ";
        for(int j=0; j<MAXY; ++j)
        {
            cout << s[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;
}

void comprar_por_filas(sala&s)
{
    int fila_1, fila_2, n;
    bool reservado=false;
    int cnt=0;
    int cnt1=0;
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

            if(s[i][j]=='o')
            {
                ++cnt;
                if(cnt==n)
                {
                    cout << "Reservado!" << endl;
                    cout << endl;
                    reservado=true;
                    do
                    {
                        s[i][cnt1]='x';
                        --cnt1;
                        --cnt;
                    }
                    while(cnt!=0);
                }
            }
            else if(s[i][j]=='x')
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
    mostrar(s);
}


void comprar_por_asiento(sala&s)
{
    bool ocupado=false;
    int nasientos, fil, col;
    do
    {
        cout << endl;
        cout << "Cuantos asientos desea: ";
        cin >> nasientos;
    }
    while ((nasientos<1)||(nasientos>35));

    for(int i=0; i<nasientos; ++i)
    {
        ocupado=false;
        do
        {
            cout << endl;
            cout << "En que fila y columa quieres tu asiento (uno de ellos): ";
            cin >> fil >> col;
        }
        while((fil<0)||(fil>4)||(col<0)||(col>6));

        while(ocupado==false)
        {
            if(s[fil][col]=='o')
            {
                s[fil][col]='x';
                cout << "Reservado!" << endl;
                cout << endl;
                mostrar(s);
                ocupado=true;
            }
            else
            {
                cout << "El asiento esta ocupado." << endl;
                cout << "Elige otro asiento: ";
                cin >> fil >> col;
            }
        }
    }
}

void comprar_tickets(sala&s)
{
    char c;
    cout << "Como quieres elegir las entradas (A = Por Fila, B = Por Asiento): ";
    do
    {
        cin >> c;
        switch (c)
        {
        case 'A':
            comprar_por_filas(s);
            break;
        case 'B':
            comprar_por_asiento(s);
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

void cancelar_tickets(sala&s)
{
    int fila,columna;
    do
    {
        cout << endl;
        cout << "En que fila y columna esta el asiento que quieres cancelar: ";
        cin >> fila >> columna;
    }
    while((fila<0)||(fila>4)||(columna<0)||(columna>6));
    if(s[fila][columna]=='o')
    {
        cout << "Ese asiento esta libre." << endl;
        cout << endl;
    }
    else
    {
        s[fila][columna]='o';
        cout << "Reserva Cancelada." << endl;
        cout << endl;
    }
}

int main()
{
    Introduccion();
    sala s1, s2, s3, s4, s5, s6;
    inicializar(s1);
    inicializar(s2);
    inicializar(s3);
    inicializar(s4);
    inicializar(s5);
    inicializar(s6);
    sala* s = elegir_sala(s1, s2, s3, s4, s5, s6);
    cout << endl;
    char c;
    while(1)
    {
        cout << "Que desea Hacer (A = Comprar, B = Cancelar, C = Ver sala, D = Cambiar Sala, E = Cancelar): ";
        cin >> c;
        switch (c)
        {
        case 'A':
            cout << endl;
            comprar_tickets(*s);
            break;
        case 'B':
            cancelar_tickets(*s);
            break;
        case 'C':
            cout << endl;
            mostrar(*s);
            break;
        case 'D':
            cout << endl;
            s = elegir_sala(s1, s2, s3, s4, s5, s6);
            break;
        case 'E':
            cout << endl;
            cout << "Programa Finalizado." << endl;
            return 0;
            break;
        default:
            cout << "Elija una opcion valida." << endl;
            cout << endl;
            break;
        }
    }
}
