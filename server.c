#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h> 

#define SIZE 4096

char Pakli[32][2];

void createPakli()
{
	int index = 0;
	for(int i = 7; i <= 10; i++){
		Pakli[index][0] = 't';
		Pakli[index][1] = i + '0';
		index++;
		Pakli[index][0] = 'm';
		Pakli[index][1] = i + '0';
		index++;
		Pakli[index][0] = 'p';
		Pakli[index][1] = i + '0';
		index++;
		Pakli[index][0] = 'z';
		Pakli[index][1] = i + '0';
		index++;
	}
	
	char szin[4] = {'t','m','p','z'};
	char lap[4] = {'L','F','K','A'};
	
	for(int i = 0; i < 4; i++){
		for(int j = 0; j < 4; j++){
			Pakli[index][0] = szin[i];
			Pakli[index][1] = lap[j];
			index++;
		}
	}
	
}


void pakliKever()
{
	int n = 32;
	size_t i;
	for (i = 0; i < n - 1; i++) 
	{
		size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
		char szin = Pakli[j][0];
		char lap = Pakli[j][1];
		Pakli[j][0] = Pakli[i][0];
		Pakli[j][1] = Pakli[i][1];
		Pakli[i][0] = szin;
		Pakli[i][1] = lap;
	}
}

//lap osztas string
char* createString(char szin, char lap, char *sztring)
{	
	sztring = malloc(SIZE);
	strcpy(sztring, "1:");
	strcat(sztring, &szin);
	strcat(sztring, ",");
	strcat(sztring, &lap);
	return sztring;
}

//lap osztas masik
char* createStringMasikEredmeny(char szin, char lap, char* sztring)
{
	sztring = malloc(SIZE);
	strcpy(sztring, "3:");
	strcat(sztring, &szin);
	strcat(sztring, ",");
	strcat(sztring, &lap);
	return sztring;
}

char* createStringBekeres()
{
	char *jel = "4";
	return jel;
}

char* createStringUjra()
{
	char *jel = "5";
	return jel;
}
char* createStringEnd()
{
	char *jel = "6";
	return jel;
}
char* createStringNew()
{
	char *jel = "7";
	return jel;
}
char* createStringTet()
{
	char *jel = "8";
	return jel;
}

char* createStringToke()
{
	char *jel = "9";
	return jel;
}


int isOkMessage(char *m){
	char ok[] = "OK";
	if( strcmp(m, ok) == 0)
	{
		return 1;
	}
	return 0;
}
int isOkMessageDouble(char *m){
	char ok[] = "OK-DUPLA";
	if( strcmp(m, ok) == 0)
	{
		return 1;
	}
	return 0;
}


int isGiveUpMessage(char *m){
	char felad[] = "FELAD";
	if( strcmp(m, felad) == 0)
	{
		return 1;
	}
	return 0;
}

int isEndMessage(char *m){
	char vege[] = "VEGE";
	if( strcmp(m, vege) == 0)
	{
		return 1;
	}
	return 0;
}
int isNewMessage(char *m){
	char new[] = "UJ";
	if( strcmp(m, new) == 0)
	{
		return 1;
	}
	return 0;
}

//a nyertes megallapitasa
int eredmeny(int p1_eredmeny, int p2_eredmeny){
	if(p1_eredmeny<=21 && p2_eredmeny<=21){
		if(abs(p1_eredmeny-21) < abs(p2_eredmeny-21)){
			return 1;
		}
		else if(abs(p1_eredmeny-21) > abs(p2_eredmeny-21)){
			return 2;
		}
		
	}
	else if(p1_eredmeny<=21 && p2_eredmeny>21 )
	{
		return 1;
	}
	else if(p1_eredmeny>21 && p2_eredmeny<=21 )
	{
		return 2;
	}
	else if(p1_eredmeny==p2_eredmeny && p1_eredmeny<=21){
		return 3; 
	}
		
}
//eredmeny üzenet 
char* eredmenyMessage(int nyertes, int tet1, int tet2){
	if(nyertes == 1){
		return ("2:Az első játékos nyert. Nyereménye: " );
	}
	else if(nyertes == 2)
	{
		return ("2:Az második játékos nyert. Nyereménye: " );
	}
	else if(nyertes == 3){
		return "2:Döntetlen!";
	}
	else
	{
		return "2:Nincs Nyertes";
	}
	
}

//eredmeny üzenethez hozzafüzes a nyeremenyt
char * createEredmenyMessage(char * eredmeny, int tet){
	char str[12];
	char *eredmeny2 = malloc(SIZE);
	sprintf(str, "%d", tet);
	strcpy(eredmeny2, eredmeny);
	strcat(eredmeny2, str);
	return eredmeny2;

}

//aktualis eredmeny üzenet
char * createTokeMessagePlayerOne(int toke1){
	char toke[32];
	char *allas = malloc(SIZE);
	sprintf(toke, "%d", toke1);
	strcpy(allas, "9:Az első játékos egyenlege: " );
	strcat(allas, toke);
	return allas;

}
char * createTokeMessagePlayerTwo(int toke1){
	char toke[32];
	char *allas = malloc(SIZE);
	sprintf(toke, "%d", toke1);
	strcpy(allas, "9:A második játékos egyenlege: " );
	strcat(allas, toke);
	return allas;

}

int player1_toke=1000;
int player2_toke=1000;
int renoszon1 = 1;
int renoszon2 =1;

//toke szamitasa
int eredmenyTet(int nyertes, int tet1, int tet2){
	int nyert1=tet1*2;
	int nyert2=tet2*2;

	if(nyertes == 1 && renoszon1 == 1){
		player1_toke+=nyert1;
		player2_toke-=tet2;
		return (nyert1);
	}
	else if(nyertes == 1 && renoszon1 == 0){
		player1_toke-=nyert1;
	}
	else if(nyertes == 2 && renoszon2 == 1)
	{
		player1_toke-=tet1;
		player2_toke+=nyert2;
		return (nyert2);
	}
	else if(nyertes == 2 && renoszon2 == 0){
		player2_toke-=nyert2;
	}
	else
	{
		return 0;
	}
	
}

int main(int argc, char* argv[])
{

	createPakli();
	int player1_tet, player2_tet;

	int fd;
	int player1_socket, player2_socket;
	
	socklen_t clilen_p1;
	socklen_t clilen_p2;
	char buffer[SIZE];
	struct sockaddr_in servaddr;
	struct sockaddr_in cliaddr_p1;
	struct sockaddr_in cliaddr_p2;

	fd = socket (AF_INET, SOCK_STREAM, 0);

	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(atoi(argv[1]));

	bind (fd, (struct sockaddr *) &servaddr, sizeof(servaddr));

	listen (fd, 2);

	printf("%s\n","Waiting for connections.");
	
	clilen_p1 = sizeof(cliaddr_p1);
	player1_socket = accept (fd, (struct sockaddr *) &cliaddr_p1, &clilen_p1);
	printf("%s\n","Player one connected.");
	
	printf("%s\n","Waiting for player two connection.");

	clilen_p2 = sizeof(cliaddr_p2);
	player2_socket = accept (fd, (struct sockaddr *) &cliaddr_p2, &clilen_p2);
	printf("%s\n","Player two connected.");
	
	

	for (;;)
	{	
		
		int legfelsoLapIndex = 0; 
		pakliKever(); 

		//Jaték kezdese
		char startMessage[] = "Játékosok csatlakoztak, a játék kezdődik.";
		memcpy(buffer, startMessage, strlen(startMessage)+1);
		printf("%s \n", buffer);

		send(player1_socket, buffer, SIZE, 0);
		send(player2_socket, buffer, SIZE, 0);

		//tet kerese
		player1_tet=0;
		player2_tet=0;	

		memset(buffer, 0, 256);
		char *messageK = createStringTet();
		memcpy(buffer, messageK, strlen(messageK)+1);
		send(player1_socket, buffer, SIZE, 0);
		send(player2_socket, buffer, SIZE, 0);

		recv(player1_socket, buffer, SIZE,0);
		player1_tet = atoi(buffer);
		printf("%d\n",player1_tet);
		memset(buffer, 0, 256);

		recv(player2_socket, buffer, SIZE,0);
		player2_tet = atoi(buffer);
		memset(buffer, 0, 256);
		printf("%d\n",player2_tet);
		memset(buffer, 0, 256);

		
		int player1_osszeg = 0;
		int player2_osszeg = 0;
		
		int voltPassz_player1 = 0;
		int voltPassz_player2 = 0;
			
		for(;;)
		{
				
			//ellenőrizzük hogy vége e a körne
			
			if(voltPassz_player1 && voltPassz_player2){
				break;
			}
			
		
			if(player1_osszeg == 21 || player2_osszeg == 21){
				break;
			}

			if(player1_osszeg >= 21 && player2_osszeg >= 21){
				break;
			}
				
			if((player1_osszeg >= 21 && voltPassz_player2) && (player2_osszeg >= 21 && voltPassz_player1)){
				break;
			}

			if(voltPassz_player1 && player2_osszeg>=21){
				break;
			}
			if(voltPassz_player2 && player1_osszeg>=21){
				break;
			}
			if(player1_osszeg<15 && voltPassz_player1){
				player1_tet = player1_tet*2;
				renoszon1 =0;
			}
			if(player2_osszeg<15 && voltPassz_player2){
				player1_tet = player1_tet*2;
				renoszon2 =0;
			}	
					
			//eredmeny szamolas és küldes elso jatekosnak
			if(!voltPassz_player1 && player1_osszeg < 21){
				int lapIndex = legfelsoLapIndex;
				legfelsoLapIndex++;
				if(Pakli[lapIndex][1] == 'L'){
					player1_osszeg += 2;
				}
				else{
					if(Pakli[lapIndex][1] == 'F'){
						player1_osszeg += 3;
					}
					else{
						if(Pakli[lapIndex][1] == 'K'){
							player1_osszeg += 4;
						}
						else{
							if(Pakli[lapIndex][1] == 'A'){
								player1_osszeg += 11;
							}
							else{
								int ertek = Pakli[lapIndex][1] - '0';
								player1_osszeg += ertek ;
							}
						}
					}
				}
					
				printf("%d \n",player1_osszeg);

				memset(buffer, 0, SIZE);
				char *message = createString(Pakli[lapIndex][0],Pakli[lapIndex][1],message);
				memcpy(buffer, message, SIZE);
					
				
				send(player1_socket, buffer, SIZE, 0);
				memset(buffer, 0, 256);
				message = createStringMasikEredmeny(Pakli[lapIndex][0],Pakli[lapIndex][1],message);
				memcpy(buffer, message, strlen(message)+1);
				send(player2_socket, buffer, SIZE, 0);
			}

			//eredmeny szamolas és küldes masodik jatekosnak
			if(!voltPassz_player2 && player2_osszeg < 21){
				int lapIndex = legfelsoLapIndex;
				legfelsoLapIndex++;
				if(Pakli[lapIndex][1] == 'L'){
					player2_osszeg += 2;
				}
				else{
					if(Pakli[lapIndex][1] == 'F'){
						player2_osszeg += 3;
					}
					else{
						if(Pakli[lapIndex][1] == 'K'){
							player2_osszeg += 4;
						}
						else{
							if(Pakli[lapIndex][1] == 'A'){
								player2_osszeg += 11;
							}
							else{
								int ertek = Pakli[lapIndex][1] - '0';
								player2_osszeg += ertek ;
							}
						}
					}
				}
					
				printf("%d \n",player2_osszeg);
				memset(buffer, 0, SIZE);
				char *message = createString(Pakli[lapIndex][0],Pakli[lapIndex][1],message);
				memcpy(buffer, message, SIZE);
					
				send(player2_socket, buffer, SIZE, 0);
				memset(buffer, 0, 256);
				message = createStringMasikEredmeny(Pakli[lapIndex][0],Pakli[lapIndex][1],message);
				memcpy(buffer, message, strlen(message)+1);
				send(player1_socket, buffer, SIZE, 0);
			}
			else{
					
			}
				
			//továbbmenet kérése és ellenőrzése elso jatekos
			if(!voltPassz_player1 && player1_osszeg < 21){
				memset(buffer, 0, 256);
				char *messageKeres = createStringBekeres();
				memcpy(buffer, messageKeres, strlen(messageKeres)+1);
				send(player1_socket, buffer, SIZE, 0);
				
				memset(buffer, 0, 256);
				recv(player1_socket, buffer, SIZE,0);
				
				printf("%s \n", buffer);

				if(isGiveUpMessage(buffer)){
					voltPassz_player1 = 1;
				}
				else if(isOkMessage(buffer)){
					;
				}
				else if(isOkMessageDouble(buffer)){
					player1_tet *= 2;
				}
				else{
					break;
				}
					
			}
				
			//továbbmenet kérése és ellenőrzése elso jatekos
			if(!voltPassz_player2 && player2_osszeg < 21){
				memset(buffer, 0, 256);
				char *messageKeres = createStringBekeres();
				memcpy(buffer, messageKeres, strlen(messageKeres)+1);
				send(player2_socket, buffer, SIZE, 0);
				
				
				memset(buffer, 0, 256);
				recv(player2_socket, buffer, SIZE,0);
				printf("%s \n", buffer);
		
				if(isGiveUpMessage(buffer)){
					voltPassz_player2 = 1;
				}
				else if(isOkMessage(buffer)){
					;
				}
				else if(isOkMessageDouble(buffer)){
					player2_tet *= 2;
				}
				else{
					break;
				}
			}
			
		}
			
		//kor vege, eredmenyek kuldese 
		memset(buffer, 0, 256);
		int nyertes = eredmeny(player1_osszeg, player2_osszeg);
		char *message = createEredmenyMessage(eredmenyMessage(nyertes,player1_osszeg, player2_osszeg),eredmenyTet(nyertes,player1_tet, player2_tet));
	    memcpy(buffer, message, strlen(message)+1);

		send(player1_socket, buffer, SIZE, 0);
		send(player2_socket, buffer, SIZE, 0);

		//Egyenleg kuldese
		memset(buffer, 0, 256);
		char *messageToke1 = createTokeMessagePlayerOne(player1_toke);
		memcpy(buffer, messageToke1, strlen(messageToke1)+1);
		send(player1_socket, buffer, SIZE, 0);	
		send(player2_socket, buffer, SIZE, 0);
		memset(buffer, 0, 256);

		char *messageToke2 = createTokeMessagePlayerTwo(player2_toke);
		memcpy(buffer, messageToke2, strlen(messageToke2)+1);
		send(player1_socket, buffer, SIZE, 0);	
		send(player2_socket, buffer, SIZE, 0);
		memset(buffer, 0, 256);


		//Uj kor vagy vege valasz kuldese és valasz feldolgozas
		char * messageKeres = createStringUjra();
		memcpy(buffer, messageKeres, strlen(messageKeres)+1);
		send(player1_socket, buffer, SIZE, 0);
		memset(buffer, 0, 256);
		recv(player1_socket, buffer, SIZE,0);
		printf("%s \n", buffer);
		

		if(isEndMessage(buffer))
		{
			memset(buffer, 0, 256);
			char *messageKeres = createStringEnd();
			memcpy(buffer, messageKeres, strlen(messageKeres)+1);
			send(player1_socket, buffer, SIZE, 0);
			send(player2_socket, buffer, SIZE, 0);
			memset(buffer, 0, 256);
		}
		if(isNewMessage(buffer)){
			memset(buffer, 0, 256);
			char *messageKeres = createStringNew();
			memcpy(buffer, messageKeres, strlen(messageKeres)+1);
			send(player1_socket, buffer, SIZE, 0);
			send(player2_socket, buffer, SIZE, 0);
			memset(buffer, 0, 256);
		}

	}


	return 0;
}

