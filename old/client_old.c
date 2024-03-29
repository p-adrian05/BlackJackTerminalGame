#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h> 

#define SIZE 4096

int main(int argc, char** argv)
{
	int sockfd;
	struct sockaddr_in servaddr;
	char buffer[SIZE];
	int end=1;
	

	if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) <0) {
		printf("Problem in creating the socket");
		exit(2);
	}
	
	char *PORT = argv[1];
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_addr.s_addr= inet_addr("127.0.0.1");
	servaddr.sin_family = AF_INET;
	servaddr.sin_port =  htons(atoi(PORT));


	if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr))<0) {
		printf("Problem in connecting to the server");
		exit(3);
	}

	
	printf("OK - kártya kérése\nOK-DUPLA -kártya kérése és a tét duplázása\nFELAD- feladás\nVEGE - játék vége\nUJ - új játék\n
	15 pontnál vagy kevesebbnél való megállás esetén a tét 2x a büntetés\n");
    printf("Színek:\nt - tök\nm - makk\nz - zold\np - piros\n\n");
	printf("Kártyák:\nA - ász\nK - király\nF - felső\nL - alsó\n7\n8\n9\n10\n");

	for (;;)
	{
		//egy menet
		if(end == 0)
		{
			exit(4);
		}
		int osszeg = 0;
		int osszeg_ellen = 0;
		
		for(;;)
		{
			//a kliens lényegében mindig csak vár egy üzenetre a szervertől, és ha kap valamit, akkor azt értelmezi
			memset(buffer, 0, 256);
			recv(sockfd, buffer, SIZE,0);
			//str = trim(str);
				
			//ha a kapott üzenet 1-el kezdődik, az azt jelenti, hogy kártyát osztottak nekünk, és ezt feldolgozzuk
			if(buffer[0] == '1'){
				//szétbontjuk a sztringet:
				//levágjuk a kezdő számot, majd felbontjuk ',' karakterekre
				char buffer_copy[SIZE];
				//printf("%s\n",buffer);
				memcpy(buffer_copy, buffer, strlen(buffer)+1);
				char *p;
				p = strtok(buffer_copy, ",");
				//printf("%s\n",p);
				char lap;
				int lap10;
				char szin;

				if(p)
				{
					szin = p[2];
				}
				p = strtok(NULL, ",");
				//printf("%s\n",p);
				if(p){
					
						lap = p[0];
			

				}
					
				if(lap==':'){
					printf("Kapott kártya: %c %d\n", szin, 10);
				}
				else
				{
						printf("Kapott kártya: %c %c\n", szin, lap);
				}
				

				if(lap == 'L'){
					osszeg += 2;
				}
				else if(lap == 'F')
					osszeg += 3;
				else if(lap == 'K')
					osszeg += 4;
				else if(lap == 'A'){
					osszeg += 11;
				}
				else if(lap==':'){
					osszeg += 10 ;
				}
				else{
					int ertek = lap - '0';
					osszeg += ertek ;
				}
				printf("Jelenlegi összeg: %d\n", osszeg);
			}
			else if(buffer[0] == '2'){
				//ha a kapott üzenet 2-vel kezdődik, az azt jelenti, lezárult egy kör, és az eredményt kaptuk meg
				// az eredményt küldték
				
				printf("%s\n",buffer);
				printf("=================================\n");
				printf("VÉGE A KÖRNEK\n");
				printf("=================================\n");
				break;
			}
			else if(buffer[0] == '3'){
				//ha a kapott üzenet 3-al kezdődik, az azt jelenti, hogy kártyát osztottak a másik játékosnak, és ezt feldolgozzuk
				//szétbontjuk a sztringet:
				//levágjuk a kezdő számot, majd felbontjuk ',' karakterekre
				char buffer_copy[SIZE];
				
				memcpy(buffer_copy, buffer, strlen(buffer)+1);
				char *p;
				p = strtok(buffer_copy, ",");

				char lap;
				char szin;
				int lap10;
				if(p)
				{
					szin = p[2];
				}
				p = strtok(NULL, ",");
				if(p){
					
						lap = p[0];
					
				}
					
				if(lap==':'){
					printf("A másik játékos kártyája: %c %d\n", szin, 10);
				}
				else
				{
						printf("A másik játékos kártyája: %c %c\n", szin, lap);
				}
				
				if(lap == 'L'){
					osszeg_ellen += 2;
				}
				else if(lap == 'F')
					osszeg_ellen += 3;
				else if(lap == 'K')
					osszeg_ellen += 4;
				else if(lap == 'A'){
					osszeg_ellen += 11;
				}
				else if(lap==':'){
					osszeg_ellen += 10 ;
				}
				else{
					int ertek = lap - '0';
					osszeg_ellen += ertek;
				}
				printf("Jelenlegi összege a másik játékosnak: %d\n", osszeg_ellen);
			}
			else if(buffer[0] == '4')
			{
				//ha a kapott üzenet 4-el kezdődik, az azt jelenti, hogy vár tőlünk választ a szerver
				//hogy passzolni szeretnék, vagy jöhet a következő kártya
				printf("Kér még kártyát vagy feladja?\n");
				char *beolvas = malloc(SIZE);
				memset(buffer, 0, 256);
				scanf("%255s", beolvas);
				memcpy(buffer, beolvas, strlen(beolvas)+1);
				send(sockfd, buffer, SIZE, 0);
			}
			else if(buffer[0] == '5')
			{
				printf("Új kör vagy vége?\n");
				char *beolvas = malloc(SIZE);
				memset(buffer, 0, 256);
				scanf("%255s", beolvas);
				memcpy(buffer, beolvas, strlen(beolvas)+1);
				send(sockfd, buffer, SIZE, 0);

			}
			else if(buffer[0] == '6'){
				end = 0;
				break;
			}
			else if(buffer[0] == '7'){
				end = 1;
				break;
			}
			else if(buffer[0]== '8')
			{
				printf("Adja meg tétjét!\n");
				char *beolvas = malloc(SIZE);
				memset(buffer, 0, 256);
				scanf("%255s", beolvas);
				memcpy(buffer, beolvas, strlen(beolvas)+1);
				send(sockfd, buffer, SIZE, 0);
			}
			else if(buffer[0]== '9'){
				printf("%s\n",buffer);
			}
			

			
			printf("--------------------------\n");
				
		}

			
	}
		
	
	return 0;
}

