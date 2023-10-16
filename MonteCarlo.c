
/*gcc Tarea4.c -o a.out -lpthread -lm 
./pruebaF 14 archivo.txt -h (historico)
./pruebaF 14 archivo.txt -u (no historico)*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include<time.h>
#include<math.h>
#include<time.h>

pthread_mutex_t candado = PTHREAD_MUTEX_INITIALIZER;


int totalP[20];
int circuloP = 0;
int insideP[20];
float aprox[5];
int cant = 0;


int NParam(int cant){ //numero de parametros
    if(cant !=4){// 
        return -1;
    }else{
        return 0;
    }
}

float average;
float desvEstandar;
int sumP = 0;
int sumPCirculo = 0;


void *recPtos(void *datos){ //Recoleccion de puntos

    float pX;//Punto de Y
	float pY; //Punto de X
	float hip; //Hipotenusa
    pthread_mutex_lock(&candado);//uso de candado, bloqueo accion critica
    
    
    for (int i = 0; i < cant; i++){// random entre 1500 a 2000
        int value =  1500 + rand()%(2000+1-1500);
        totalP[i] = value;
    }

    for(int i = 0; i < cant; i++){//cant es a cantdad de hilos ingresados
        for(int j = 0; j < totalP[i]; j++){
            pX = (float)rand() / RAND_MAX;
            pY = (float)rand() / RAND_MAX;
            hip = pX * pX + pY * pY;
            if (hip <= 1){// Punto interno del circulo con radio 1 si hip==radio > 1 esta fuera del circulo
                circuloP++;
            }
            insideP[i] = circuloP;
        }
        circuloP = 0;
    }
    pthread_mutex_unlock(&candado);//se se desbloquea la accion critica
}

void recAprox(){
    float aproxPI;
    for (int i = 0; i < 5; i++){// obtiene las 5 aprox
        aproxPI = 4 * (insideP[i]) / ((float)totalP[i]);
        aprox[i] = aproxPI;
    } 
}

void procAverage(){
    float sum = 0;
    for(int i = 0; i < 5; i++){
        sum = sum+aprox[i];
    }
    average = sum/5;
}

void arrayPuntosT(){
    for(int i = 0; i < cant;i++){
        sumP = sumP + totalP[i];
    }
}

void arrayPuntosTCirculo(){
    for(int i = 0; i < cant;i++){
        sumPCirculo = sumPCirculo + insideP[i];
    }
}

void obtDesvE(){
    float sum = 0;
	for (int i = 0; i < 5; ++i)
	{
		sum += pow(aprox[i]-average, 2);
	}
	float varTotal = sum / 5; //Numero total de varianza
    desvEstandar = sqrt(varTotal);
}

void threads(){
    srand(time(NULL));
    pthread_t threads[cant];
    for (int i = 0; i < cant; i++){
        pthread_create(&threads[i], NULL, recPtos, NULL);
    }
    for (int i = 0; i < cant; i++){
        pthread_join(threads[i], NULL);
    }
}

int valData(char *param[]){// valida la cantidad de hilos y el nombre del archivo

    char nomArc[15];
    char *path;
    int threads = atoi(param[1]);
    strcpy(nomArc,param[2]);
    path = strstr(nomArc, ".txt");
    if (!path){
        return -1;
    }
    if(threads < 10 || threads > 15){
        return -2;
    }
    return 0;
}

//Metodo principal para llamar a todas las funciones
int main(int argc ,char *argv[]){
    cant = atoi(argv[1]);
    int res = NParam(argc);
    if(res == 0){
        int resDatos = valData(argv);
        if(resDatos == 0){
            threads();
            recAprox();
            arrayPuntosT();
            arrayPuntosTCirculo();
            procAverage();
            obtDesvE();
            
            //Función de escritura en archivo
            char paramHistoric[5];
            char arcSal[15];
            strcpy(arcSal,argv[2]);
            strcpy(paramHistoric,argv[3]);
            FILE *arc;
            if(strcmp(paramHistoric,"-h") == 0){
                arc = fopen(arcSal,"a");// agrega en el archvo al final
                if(arc != NULL){
                    fprintf(arc,"Hilos generados: %d \n",cant);
                    fprintf(arc,"Vector cantidad de puntos: ");
                    for(int i = 0; i < cant; i++){
                        fprintf(arc,"%d ", totalP[i]);
                    }
                    fprintf(arc,"\n");

                    fprintf(arc,"Puntos dentro del circulo (5ta aprox.) = %d \n", sumP);
                    fprintf(arc, "Puntos totales generados (5ta aprox.) = %d \n" , sumPCirculo);
                    for(int i = 0; i < 5; i++){
                        fprintf(arc,"Aproximacion numero %d: %.4f \n", i+1 , aprox[i]);
                    }
                    fprintf(arc,"Promedio del calculo: %.4f \n", average);
                    fprintf(arc, "Desviacion Estandar: %.4f\n", desvEstandar);
                    fprintf(arc,"\n");
                    fprintf(arc,"\n");
                    fclose(arc);
                }else{
                    printf("Fallo al abrir el archivo \n");
                }

            }else if(strcmp(paramHistoric,"-u") == 0){// remplaza la informacion
                arc = fopen(arcSal,"w");
                if(arc != NULL){
                    fprintf(arc,"Hilos generados %d \n",cant);
                    fprintf(arc,"Vector cantidad de puntos: ");
                    for(int i = 0; i < cant; i++){
                        fprintf(arc,"%d ", totalP[i]);
                    }
                    fprintf(arc,"\n");

                    fprintf(arc,"Puntos dentro del circulo (5ta aprox.) = %d \n", sumP);
                    fprintf(arc,"Puntos totales generados (5ta aprox.) = %d \n" , sumPCirculo);
                    for(int i = 0; i < 5; i++){
                        fprintf(arc,"Aproximacion numero %d: %.4f \n", i+1 , aprox[i]);
                    }
                    fprintf(arc,"Promedio del calculo: %.4f \n", average);
                    fprintf(arc, "Desviacion Estandar: %.4f\n", desvEstandar);
                    fprintf(arc,"\n");
                    fprintf(arc,"\n");
                    fclose(arc);
                }else{
                    printf("Fallo al abrir el archivo \n");
                }
            }
            
            printf("Hilos generados: %d \n", cant);
            printf("Vector cantidad de puntos: ");
            for(int i = 0; i < cant; i++){
                printf("%d ", totalP[i]);
            }
            printf("\n");
            printf("Puntos dentro del circulo (5ta aprox.) =  %d \n", sumPCirculo);
            printf("Puntos totales generados (5ta aprox.) =  %d \n", sumP);
            for(int i = 0; i < 5; i++){
                printf("Aproximacion numero %d: %.4f \n", i+1 , aprox[i]);
            }
            printf("Promedio del calculo: %.4f\n",average);
            printf("Desviacion Estandar: %.4f\n", desvEstandar);
        }else if(resDatos == -2){
            printf("La cantidad de threads solo pude ser de 10 a 15 \n");
        }
    }else{
        printf("Cantidad de parametros no valida \n");
    }
}

