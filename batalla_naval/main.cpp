#include <iostream>
#include <ctime>
#include <cstring>

#define  NUM_BARCOS 3
int TAMANIO_TABLERO;
int TAM_BARCOS[NUM_BARCOS] = { 3, 2, 1}; //{ 4, 3, 3, 2, 2, 2, 1, 1}

using namespace std;

class Coordenada{
public:
    char fila;
    int columna;
    Coordenada();
    Coordenada(char, int);
    ~Coordenada();
    string mostrar();
};

Coordenada::Coordenada(){ }

Coordenada::Coordenada(char _fila, int _columna) {
    fila = _fila;
    columna = _columna;
}

string Coordenada::mostrar(){
    string coord = "(";
    coord.append(1,fila);
    coord+= ", ";
    coord.append(1,char(columna + 48));
    coord+=')';
    return coord;
}

Coordenada::~Coordenada(){}

class Barco{
private:
    int tamanio;
    char orientacion; //horizontal (H) o vertical (V)
    int vidas;
    Coordenada* coordIni;
public:
    Barco(int);
    Barco(int, char, Coordenada*);
    ~Barco();
    bool estaHundido();
    int getTamanio();
    char getOrientacion();
    Coordenada* getCoordIni();
    void actualizarVida(int);
    int getVidas();

};

Barco::Barco(int _tamanio){
    tamanio = _tamanio;
    vidas = tamanio;
    orientacion = 'H';
}

Barco::Barco(int _tamanio, char _orientacion, Coordenada* _coordIni){
    tamanio = _tamanio;
    orientacion = _orientacion;
    coordIni = _coordIni;
    vidas = tamanio;
}

void Barco::actualizarVida(int _vidas){ vidas = _vidas;}
int Barco::getVidas(){return vidas;}
Barco::~Barco(){}
bool Barco::estaHundido() { return vidas<=0;}
int Barco::getTamanio() {return tamanio;}
char Barco::getOrientacion() {return orientacion;}
Coordenada* Barco::getCoordIni() { return coordIni; }

class Tablero{
private:
    int tamanio;
    char** matrix;
    Barco* barcos[NUM_BARCOS];
    int num_barcos_flotando;

public:
    Tablero();
    void mostrar();
    bool validarPosicionBarco(Barco*);
    void colocarBarco(Barco*);
    void mostrarFila(char,bool);
    bool realizarDisparo(Coordenada*);
    Barco* buscarBarcoPorCoordenada(Coordenada*);
    void hundirBarco(Barco*);
    bool flotaDestruida();
    char** getMatrix();

};

Tablero :: Tablero(){
    tamanio = TAMANIO_TABLERO;
    matrix = new char* [tamanio];
    for(int i = 0 ; i< tamanio ; i++){
        matrix[i] = new char[tamanio]; //Genera la matriz de manera dinamica, en base al tamanio
    }

    for(int i = 0 ;i< tamanio ; i++) {
        for (int j = 0; j < tamanio; j++)
            matrix[i][j] = '~';     //se inicializa los valores de la matriz
    }

    for(int i = 0 ;i<NUM_BARCOS ; i++){
        barcos[i] = new Barco(TAM_BARCOS[i]);
    }
    num_barcos_flotando = 0;
}

char** Tablero::getMatrix(){
    return matrix;
}

//Metodo para mostrar la matriz
void Tablero::mostrar(){
    printf("  ");
    for(int i = 1 ; i <=tamanio ; i++){
        printf("%d ", i);
    }
    printf("\n");
    for(int i = 0 ;i< tamanio ; i++){
        mostrarFila(char(i+65), false);
        printf("\n");
    }
}

void Tablero::mostrarFila(char fila, bool oculto){
    printf("%c ", fila);
    for(int i = 0 ;i< tamanio ; i++){
        char casilla = matrix[fila-65][i];
        if(oculto){
            if(casilla == '*'){
                casilla = '~';
            }
        }
        if(i < 9){
            printf("%c ",casilla );
        }else{
            printf("%c  ", casilla);
        }
    }
}

bool Tablero::validarPosicionBarco(Barco* barco) {
    Coordenada* coord = barco ->getCoordIni();
    char fila = coord->fila;
    int column = coord->columna;
    int tam_barco = barco->getTamanio();
    bool valido = true;
    if(barco->getOrientacion() == 'H'){
        if((column + tam_barco - 1) > TAMANIO_TABLERO){
            valido = false;
        }else {
            for (int i = column - 1; i < (column + tam_barco - 1); i++) {
                if (matrix[fila - 65][i] == '*') {
                    valido = false;
                    break;
                }
            }
        }
    } else{
        if((fila- 65 + tam_barco) > TAMANIO_TABLERO){
            valido = false;
        }else {
            for (int i = fila - 65; i < (fila - 65 + tam_barco); i++) {
                if (matrix[i][column - 1] == '*') {
                    valido = false;
                    break;
                }
            }
        }
    }
    return valido;
}

void Tablero::colocarBarco(Barco * barco){
    barcos[num_barcos_flotando] = barco;
    Coordenada* coord = barco ->getCoordIni();
    char fila = coord->fila;
    int column = coord->columna;
    int tam_barco = barco->getTamanio();
    if(barco->getOrientacion() == 'H'){
        for(int i = column-1 ; i<(column + tam_barco -1) ; i++ ) {
            matrix[fila - 65][i] = '*';
        }
    } else{
        for(int i = fila-65 ; i<(fila- 65 + tam_barco) ; i++ ) {
            matrix[i][column - 1] = '*';
        }
    }

    num_barcos_flotando++;
}
bool Tablero::realizarDisparo(Coordenada* disparo){
    char fila = disparo->fila;
    int columna = disparo->columna;
    bool acierto = false;
    if(matrix[fila-65][columna-1] == '*'){
        matrix[fila-65][columna-1] = '+';
        acierto = true;
        Barco* barco = buscarBarcoPorCoordenada(disparo);
        if(barco != nullptr){
            barco->actualizarVida(barco->getVidas() - 1);
            if(barco->estaHundido()){
                hundirBarco(barco);
            }
        }
    }else if(matrix[fila-65][columna -1] == '~'){
        matrix[fila-65][columna -1] = 'o';
    }

    return acierto;
}

void Tablero :: hundirBarco(Barco* barco){
    Coordenada* coord = barco ->getCoordIni();
    char fila = coord->fila;
    int column = coord->columna;
    int tam_barco = barco->getTamanio();
    if(barco->getOrientacion() == 'H'){
        for(int i = column-1 ; i<(column + tam_barco -1) ; i++ ) {
            matrix[fila - 65][i] = 'x';
        }
    } else{
        for(int i = fila-65 ; i<(fila-65 + tam_barco) ; i++ ) {
            matrix[i][column - 1] = 'x';
        }
    }
    num_barcos_flotando--;
}


Barco* Tablero::buscarBarcoPorCoordenada(Coordenada* coord){
    Barco* barco = nullptr;
    char fila = coord->fila;
    int column = coord->columna;
    for(int i = 0 ;i<NUM_BARCOS; i++){
        if(!barcos[i]->estaHundido()) {
            char filaIni = barcos[i]->getCoordIni()->fila;
            int columnIni = barcos[i]->getCoordIni()->columna;
            int tam_barco = barcos[i]->getTamanio();
            if (barcos[i]->getOrientacion() == 'H') {
                if (filaIni == fila &&
                    (columnIni <= column && column <(columnIni + tam_barco ))) { //esta en el rango del barco
                    barco = barcos[i];
                }
            } else {
                if (columnIni == column && (filaIni <= fila && fila < (filaIni + tam_barco))) {
                    barco = barcos[i];
                }
            }
        }
    }
    return barco;
}

bool Tablero ::flotaDestruida(){
    return (num_barcos_flotando<=0);
}

class Jugador{
protected:
    char nombre[50];
    Tablero* tablero;
    Coordenada** jugadas;
    int num_jugadas;

public:
    Jugador(char*);
    ~Jugador();
    bool acertaste;
    char* getNombre();
    Tablero* getTablero();
    void posicionarBarcos(bool);
    void mostrarTablero();
    void mostrarTableroConEnemigo(Jugador*);
    virtual void realizarJugada(Jugador);
    bool validarDisparo(char, int);
    bool pierde();
};

Jugador::Jugador(char* _nombre){
    strcpy(nombre, _nombre);
    tablero =  new Tablero();
    num_jugadas = 0;
    jugadas = new Coordenada*[TAMANIO_TABLERO*TAMANIO_TABLERO];
    acertaste = false;
}

Jugador::~Jugador(){}

char* Jugador::getNombre() {return nombre;}
Tablero* Jugador::getTablero(){return tablero;}

void Jugador::mostrarTablero(){
    printf("Jugador : %s\n",nombre);
    tablero->mostrar();
}
void Jugador::mostrarTableroConEnemigo(Jugador* enemy){
    printf("Enemigo: %s", enemy->nombre);
    for(int i =0 ; i< TAMANIO_TABLERO; i++){
        printf(" ");
        if(i >9);{
            printf(" ");
        }
    }
    printf("\tJugador: %s\n", nombre);

    for(int i = 0; i<2; i++){
        printf("  ");
        for(int j = 1 ; j <=TAMANIO_TABLERO ; j++){
            printf("%d ", j);
        }
        printf("\t\t\t");
    }
    printf("\n");

    for(int i = 0 ;i<TAMANIO_TABLERO; i++){
        enemy->getTablero()->mostrarFila(char(i + 65), true); //El campo true, oculta el mapa
        printf("\t\t\t");
        tablero->mostrarFila(char(i+65), false);
        printf("\n");
    }
}

void Jugador::posicionarBarcos(bool aleatorio) {
    if(!aleatorio){
        char ultima_letra = char(TAMANIO_TABLERO + 64);
        int i = 0;
        bool valido = false;

        do {
            char fila;
            int columna;
            int tamanio = TAM_BARCOS[i];
            char orientacion;
            printf("Barco %d de tamanio %d\n", (i + 1), tamanio);
            do {
                printf("Ingrese la fila: (A - %c)\n", (ultima_letra));
                cin >> fila;
            } while (fila < 65 || fila > ultima_letra);
            do {
                printf("Ingrese la columna: (1 - %d)\n", TAMANIO_TABLERO);
                cin >> columna;
            } while (columna < 1 || columna > TAMANIO_TABLERO);

            do {
                printf("Ingrese la orientacion del barco: (V/H)\n");
                cin >> orientacion;
            } while (orientacion != 'H' && orientacion != 'V');

            Coordenada *coord = new Coordenada(fila, columna);
            Barco *barco = new Barco(tamanio, orientacion, coord);
            valido = tablero->validarPosicionBarco(barco);
            if (!valido) {
                printf("Posicion  del barco no valida\n");
            } else {
                tablero->colocarBarco(barco);
                i++;
                tablero->mostrar();
            }
        } while (i < NUM_BARCOS || !valido);
    }else{
        srand(time(NULL));
        int i = 0;
        while( i< NUM_BARCOS){
            char fila = char(rand()%TAMANIO_TABLERO + 65); //Generar fila aleatoria
            int column = rand()%TAMANIO_TABLERO + 1; //Generar columna aleatoria
            char orientaciones[2] = {'V', 'H'};
            int ro = rand()%2;                      //Orientacion aleaotoria
            char orientacion = orientaciones[ro];
            Coordenada* coord = new Coordenada(fila, column);
            Barco* barco = new Barco(TAM_BARCOS[i],orientacion,coord);
            if(tablero ->validarPosicionBarco(barco)){
                tablero ->colocarBarco(barco);
                i++;
            }
        }
    }
}

bool Jugador::validarDisparo(char fila_disp, int column_dips){
    bool valido = true;
    Coordenada* d;
    for(int i = 0 ; i<num_jugadas; i++){
        d = jugadas[i];
        if(fila_disp == d->fila && column_dips == d->columna){
            valido = false;
            break;
        }
    }
    return valido;
}

void Jugador::realizarJugada(Jugador enemy) {
    char ultima_letra = char(TAMANIO_TABLERO + 64);
    char fila;
    int columna;
    bool valido = false;
    Coordenada* disparo;
    do {
        printf("Jugador: %s - Jugada #%d\n", nombre, (num_jugadas + 1));
        do {
            printf("Ingrese la fila: (A - %c)\n", (ultima_letra));
            cin >> fila;
        } while (fila < 65 || fila > ultima_letra);
        do {
            printf("Ingrese la columna: (1 - %d)\n", TAMANIO_TABLERO);
            cin >> columna;
        } while (columna < 1 || columna > TAMANIO_TABLERO);
        valido = validarDisparo(fila, columna);
        if (!valido) {
            printf("Jugada repetida, realice otra vez\n");
        } else {
            disparo = new Coordenada(fila, columna);
            acertaste =  enemy.getTablero()->realizarDisparo(disparo);
            printf("Disparo realizado por %s: (%c, %d)\n",nombre,disparo ->fila, disparo ->columna);
            jugadas[num_jugadas] = disparo;
        }
    } while (!valido);
    num_jugadas++;
}


bool Jugador::pierde(){
    return tablero->flotaDestruida();
}

class Computador:public Jugador{
private:
    Coordenada** posiblesJugadas;
    int ultimaJugada;
    Coordenada* getPosibleJugada();
    void agregarPosibleJugada(Coordenada*);
public:
    Computador(char*);
    void realizarJugada(Jugador);
    void mostrarPosiblesJugadas();
    ~Computador();
};

Computador::Computador(char* _nombre): Jugador(_nombre){
        posiblesJugadas = new Coordenada*[TAMANIO_TABLERO*TAMANIO_TABLERO];
        ultimaJugada = -1;
}

Computador::~Computador(){
}

Coordenada* Computador::getPosibleJugada(){
    Coordenada* c;
    if(ultimaJugada >-1){
        c = posiblesJugadas[ultimaJugada];
        ultimaJugada--;
    }
    return c;
}

void Computador::agregarPosibleJugada(Coordenada* casillaCercana){
    ultimaJugada++;
    posiblesJugadas[ultimaJugada] = casillaCercana;
}

void Computador::mostrarPosiblesJugadas(){
    printf("ultimaJugada: %d\n",ultimaJugada);
    printf("[ ");
    for(int i = 0 ; i<ultimaJugada + 1; i++){
        printf("%s",posiblesJugadas[i]->mostrar());
        printf(" ,");
    }
    printf(" ]\n");
}

void Computador::realizarJugada(Jugador enemy){
    Coordenada* disparo;
    printf("Jugador: %s - Jugada #%d\n", nombre, (num_jugadas + 1));
    printf("ultima jugada%d\n",ultimaJugada);
    if(ultimaJugada < 0){
        srand(time(NULL));
        char fila;
        int column;
        do{
            fila = char(rand()%TAMANIO_TABLERO + 65); //Generar fila aleatoria
            column = rand()%TAMANIO_TABLERO + 1; //Generar columna aleatoria
        }while(!validarDisparo(fila,column));
        disparo =  new Coordenada(fila, column);
        acertaste =  enemy.getTablero()->realizarDisparo(disparo);
    }else{
        do {
            disparo = getPosibleJugada();
        }while(!validarDisparo(disparo->fila,disparo->columna));
        acertaste =  enemy.getTablero()->realizarDisparo(disparo);
    }
    if(acertaste){
        int fila = disparo -> fila - 65;
        int column = disparo -> columna - 1;
        Tablero* tablero_enemy = enemy.getTablero();
        char** matrix = tablero_enemy ->getMatrix();
        //buscar posibles jugadas en la vertical (norte y sur)
        Coordenada* posibleJug;
        for(int i =fila - 1  ; i<fila + 2  ; i++){
            if(0<= i && i< TAMANIO_TABLERO && i == fila){
                printf("%c",matrix[i][column]);
                if(matrix[i][column] != '+' && matrix[i][column]!='o'){
                    posibleJug = new Coordenada(char(i + 65) ,column + 1);
                    agregarPosibleJugada(posibleJug);
                }
            }
        }
        printf(",");

        //buscar posibles jugadas en la horizontal (oeste y este)
        for(int i =column - 1  ; i<column + 2  ; i++){
            if(0<= i && i< TAMANIO_TABLERO && i == column){
                printf("%c",matrix[fila][i]);
                if(matrix[fila][i] != '+' && matrix[fila][i]!='o'){
                    posibleJug = new Coordenada(char(fila + 65), i +1);
                    agregarPosibleJugada(posibleJug);
                }
            }
        }

        printf("\nultima jugada: %d\n", ultimaJugada);
    }

    printf("Disparo realizado por %s: (%c, %d)\n",nombre,disparo -> fila, disparo -> columna);
    jugadas[num_jugadas] = disparo;
    num_jugadas++;

}




int main() {
    do{
        cout<<"Ingrese el tamanio del tablero (>5)"<<endl;
        cin>>TAMANIO_TABLERO;
    }while(TAMANIO_TABLERO < 5); //Se considera que el tamanaño minimo para el tablero es 5, para que puedan entrar los 8 barcos
    Jugador j1((char*)"Jugador1");
    cout<<"Posicionar barcos: Jugador 1"<<endl;
    j1.getTablero() -> mostrar();
    j1.posicionarBarcos(false); //Jugado 1 posiciona sus barcos
    Computador comp((char*)"Computador");
    comp.posicionarBarcos(true); //posicionar barcos aleatoriamente
    printf("Barcos de la computadora posicionados \n");
    printf("Inicio del juego\n");
    bool acierto = false;
    bool pierde = false;
    j1.mostrarTableroConEnemigo(&comp);
    do{
        do{
            j1.realizarJugada(comp);
            j1.mostrarTableroConEnemigo(&comp);
            pierde = comp.pierde();
        }while(j1.acertaste && !pierde);

        if(pierde)
            break;

        printf("Turno del Computador\n");
        do{
            comp.realizarJugada(j1);
            j1.mostrarTableroConEnemigo(&comp);
            pierde = j1.pierde();
        }while(comp.acertaste && !pierde);

    }while(!pierde);

    if(!j1.pierde()){
        printf("El ganador es  el jugador: %s",j1.getNombre());
    }else{
        printf("El ganador es el jugador: %s",comp.getNombre());
    }

    //system("pause");
    return 0;
}