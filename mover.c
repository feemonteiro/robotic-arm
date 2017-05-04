/*************************************************************/
/*       UNIVERSIDADE FEDERAL DO RIO GRANDE DO NORTE         */
/*   DEPARTAMENTO DE ENGENHARIA DE COMPUTAÇÃO E AUTOMAÇÃO    */
/*							                                 */
/*   DRIVER DO BRAÇO ROBÓTICO LYNX AL5D PARA A PORTA SERIAL  */
/*							                                 */
/*   DESENVOLVEDORES:					                     */
/*	- ENG. M.SC. DESNES AUGUSTO NUNES DO ROSÁRIO	         */
/*	- ENG. DANILO CHAVES DE SOUSA ICHIHARA		             */
/*  - FERNANDA MONTEIRO DE ALMEIDA                           */
/*  - JUNINHO LOPES 									     */
/*************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <opencv2/core/core_c.h>
#include <opencv2/imgcodecs/imgcodecs_c.h>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/highgui/highgui_c.h>

#include <string.h>

#include "ufrn_al5d.h"


//Posicao inicial para todos os servos
#define HOME_POS "#0P1500#1P1500#2P1500#3P1500#4P1500"

int pos[5] = {1500, 1500, 1500, 1500, 1500};

void plot(int *pos, int argc, char** argv){
	float t[4], x, y, L1 = 1, L2 = 1, L3 = 1;
	int i;
	int stepSize = 65;

	for(i = 0; i<4;i++){
		t[i] = 0.09*pos[i+1];
	}

	x = L2*cos(t[1])*cos(t[2]+t[3]) + sin(t[1])*cos(t[1]) + L3*(cos(t[1] - t[4])*cos(t[2]+t[3]));
	y = L3*(sin(t[1] - t[4])*sin(t[2]+t[3])) + L2*cos(t[1])*cos(t[2]+t[3]) - L1*cos(t[1]);


	//! [load]
    char* imageName = "sample.png"; // by default
    if( argc > 1)
    {
        imageName = argv[1];
    }
    //! [load]

    //! [mat]
    Mat mtx(image);
    //! [mat]

    //! [imread]
    image = imread( imageName, IMREAD_COLOR ); // Read the file
    //! [imread]

    if( image.empty() )                      // Check for invalid input
    {
        cout <<  "Could not open or find the image" << std::endl ;
        return;
    }

    Mat mat_img(image);
	

	int width = mat_img.size().width;
	int height = mat_img.size().height;

	for (int i = 0; i<height; i += stepSize)
	    cvLine(mat_img, Point(0, i), Point(width, i), Scalar(0, 255, 255));

	for (int i = 0; i<width; i += stepSize)
	    cvLine(mat_img, Point(i, 0), Point(i, height), Scalar(255, 0, 255));

	cvPutText(mat_img, "(x, y)", cvPoint(x,y), 
    FONT_HERSHEY_COMPLEX_SMALL, cvScalar(200,200,250));
    //! [window]
    namedWindow( "Display window", WINDOW_AUTOSIZE ); // Create a window for display.
    //! [window]

    //! [imshow]
    imshow( "Display window", image );                // Show our image inside it.
    //! [imshow]

    //! [wait]
    waitKey(0); // Wait for a keystroke in the window
}

void movimento(int a, char mov, char* comando){

	switch(mov){
		case 'q':
			pos[a] = trava(a, pos[a] + 5);
			printf("%d\n", pos[a]);
			break;
		case 'w':
			pos[a] = trava(a, pos[a] - 5);
			printf("%d\n", pos[a]);
			break;
	}
	sprintf(comando, "#%dP%d", a, pos[a]);
	printf("[0]%d, [1]%d, [2]%d, [3]%d, [4]%d", pos[0], pos[1], pos[2], pos[3], pos[4]);
}

void pos_direto(char* comando){
	int p0, p1, p2 , p3 ,p4;
	if( scanf("%d %d %d %d %d", &p0, &p1, &p2, &p3, &p4) == 5){
		p0 = trava(0, p0);
		p1 = trava(1, p1);
		p2 = trava(2, p2);
		p3 = trava(3, p3);
		p4 = trava(4, p4);
		sprintf(comando,"#0P%d S500#1P%d S500#2P%d S500#3P%d S500#4P%d S500", p0, p1, p2, p3, p4);
		pos[0]=p0; pos[1]=p1; pos[2] = p2; pos[3]=p3; pos[4]=p4;
	}else{
		printf("Tá loco mermão\n");
	}
}

int main(int argc, char** argv)
{
	ufrn_header();

	int serial_fd;
	char *comando;
	char in; int jnt=0;

	// INICIO DO PROGRAMA DEMO //

	printf("PROGRAMA DEMONSTRACAO INICIADO\n\n");
	
	serial_fd = abrir_porta();

	if(serial_fd == -1)
	{
		printf("Erro abrindo a porta serial /dev/ttyS0\nAbortando o programa...");
		return -1;
	}
	else
	{
		printf("Porta serial /dev/ttyS0 aberta com sucesso\n");

		if(configurar_porta(serial_fd) == -1)
		{
		    printf("Erro inicializando a porta\n");
		    close(serial_fd);
		    return -1;
		}

		comando = (char*) malloc(sizeof(char)*BUFSIZE);

		sprintf(comando,"%s",HOME_POS);

		//////////////////////
		// PRIMEIRO COMANDO //
		//////////////////////
		printf("Números de 0-4 modificam a junta");
		printf("Para movimentar, utilize Q e W");
		printf("Pressione n para sair");
		printf("Use q para movimentar para direita e w para esquerda\n");
		printf("Insira o canal\n");
		//system("/bin/stty raw");
		in = getchar();
		while((in = getchar())!=-1){
			if (in == '0'){
				jnt = 0; continue;
			}
			else if (in == '1'){
				jnt = 1; continue;
			}
			else if (in == '2'){
				jnt = 2; continue;
			}
			else if (in == '3'){
				jnt = 3; continue;
			}
			else if (in == '4'){
				jnt = 4; continue;
			}else if(in == 'q' || in == 'w'){
				movimento(jnt, in, comando);
				enviar_comando(comando,serial_fd);
				memset(comando, 0, BUFSIZE);
			}else if(in == 'n'){
				break;
			}else if(in == 'c'){
				pos_direto(comando);
				enviar_comando(comando,serial_fd);
				memset(comando, 0, BUFSIZE);
			}else if(in == 'h'){
				sprintf(comando,"%s S500",HOME_POS);
				enviar_comando(comando,serial_fd);
				memset(comando, 0, BUFSIZE);
			}
		}
		
		

		// FIM DO PROGRAMA DEMO //
	    fechar_porta(serial_fd);
		printf("\nAcesso a porta serial /dev/ttyS0 finalizado\n");

    	}

	printf("\nPROGRAMA FINALIZADO\n\n");

	return 0;
}
