#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h> 

#define LISTENQ 2
#define SIZE 4096

char Pakli[32][2];

//belerakjuk a kártyákat
void initPakli()
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
			int ertek;
			if(lap[j] == 'A')
				ertek = 11;
			if(lap[j] == 'L')
				ertek = 2;
			if(lap[j] == 'F')
				ertek = 3;
			if(lap[j] == 'K')
				ertek = 4;
				
			Pakli[index][0] = szin[i];
			Pakli[index][1] = lap[j];
			index++;
		}
	}
	
}

//megkeverjük a paklit
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

//egy függvény, amiben egy olyan sztringet csinálunk, amit majd küldünk a kliensnek mikor "osztjuk" neki a lapot
char* createString(char szin, char lap, char *sztring)
{	
	sztring = malloc(SIZE);
	strcpy(sztring, "1:");
	strcat(sztring, &szin);
	strcat(sztring, ",");
	strcat(sztring, &lap);
	return sztring;
}

//egy függvény, amiben egy olyan sztringet csinálunk, amit majd küldünk a kliensnek mikor "osztjuk" a másik játékosnak a lapot
char* createStringMasikEredmeny(char szin, char lap, char* sztring)
{
	sztring = malloc(SIZE);
	strcpy(sztring, "3:");
	strcat(sztring, &szin);
	strcat(sztring, ",");
	strcat(sztring, &lap);
	return sztring;
}

//ezzel jelezzük a kliensnek, hogy most kérünk tőle választ
char* createStringBekeres()
{
	char *jel = "4";
	return jel;
}

//csak ellenőrizzük a kapott üzenetet
int isOkMessage(char *m){
	char ok[] = "OK";
	if( strcmp(m, ok) == 0)
	{
		return 1;
	}
	return 0;
}

int isPassMessage(char *m){
	char passz[] = "PASSZ";
	if( strcmp(m, passz) == 0)
	{
		return 1;
	}
	return 0;
}

//a két összeg alapján az eredményt adja vissza
//ebben a programban most az nyer, aki "közelebb" végzett a 21-hez
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
		return 3; //döntetlen
	}
		
}

//megcsináljuk a sztringet, amit akkor küldjük a klienseknek mikor az eredményeket küldjük
char* eredmenyMessage(int nyertes, int pont1, int pont2){
	if(nyertes == 1){
		return ("2:Az első játékos nyert ");
	}
	else if(nyertes == 2)
	{
		return ("2:Az második játékos nyert ");
	}
	else if(nyertes == 3){
		return "2:Döntetlen!";
	}
	else
	{
		return "2:Nincs Nyertes";
	}
	
}

int main(int argc, char* argv[])
{
	int seed = time(NULL);
    srand(seed);
	
	initPakli();
	
	int listenfd, n;
	int player1_socket, player2_socket;
	socklen_t clilen_p1;
	socklen_t clilen_p2;
	char buffer[SIZE];
	struct sockaddr_in servaddr;
	struct sockaddr_in cliaddr_p1;
	struct sockaddr_in cliaddr_p2;

	//AF_INET -> IPv4
	//SOCK_STREAM,0 -> TCP
	listenfd = socket (AF_INET, SOCK_STREAM, 0);

	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(atoi(argv[1]));

	bind (listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

	listen (listenfd, 2);

	printf("%s\n","Server running...waiting for connections.");
	
	clilen_p1 = sizeof(cliaddr_p1);
	player1_socket = accept (listenfd, (struct sockaddr *) &cliaddr_p1, &clilen_p1);
	printf("%s\n","Player one has connected.");
	
	clilen_p2 = sizeof(cliaddr_p2);
	player2_socket = accept (listenfd, (struct sockaddr *) &cliaddr_p2, &clilen_p2);
	printf("%s\n","Player two has connected.");
	
	for (;;)
	{	
		int legfelsoLapIndex = 0; //ebben a változóban tartjuk néhány hogy melyik lap van legfelül
		pakliKever(); //megkeverjük a paklit minden kör elején

		//Kezdődik a játék, elküldjük üzenetben
		char startMessage[] = "Játékosok csatlakoztak, a játék kezdődik.";
		memcpy(buffer, startMessage, strlen(startMessage)+1);
		printf("%s \n", buffer);
			
		//írunk a játékosoknak, de itt most nem várunk választ
		send(player1_socket, buffer, SIZE, 0);
		
		send(player2_socket, buffer, SIZE, 0);
			
		//minden kör elején nullázuk az összegeket
		int player1_osszeg = 0;
		int player2_osszeg = 0;
			
		//ebben a bool változóban tartjuk nyílván, hogy történt-e passz, ha történt, true-ra állítjuk
		//ha történt passz, nem kell bizonyos küldéseknek lefutnia
		int voltPassz_player1 = 0;
		int voltPassz_player2 = 0;
			
		for(;;)
		{
				
			//ellenőrizzük hogy vége van-e a körnek a játék szabályai szerint
			//ha mindketten passzoltak legyen vége a körnek
			if(voltPassz_player1 && voltPassz_player2){
				break;
			}
			
			//ha valamelyikük pont 21-et kap legyen vége a körnek
			if(player1_osszeg == 21 || player2_osszeg == 21){
				break;
			}
				
				
			//ha mindketten meghaladják vagy elérik a 21-et	legyen vége a körnek
			if(player1_osszeg >= 21 && player2_osszeg >= 21){
				break;
			}
				
					
			//ha az egyik játékos passzolt és a másik is elérte a 21-et legyen vége a körnek
			if((player1_osszeg >= 21 && voltPassz_player2) && (player2_osszeg >= 21 && voltPassz_player1)){
				break;
			}

			if(voltPassz_player1 && player2_osszeg>=21){
				break;
			}
			if(voltPassz_player2 && player1_osszeg>=21){
				break;
			}

				
					
			//ebben az if-ben küldi a szerver a "kihúzott" lapjait az első kliensnek
			//nem lép be ha az első kliens passzolt, tehát nem kap új lapot
			if(!voltPassz_player1 && player1_osszeg < 21){
				//"levesszük" a legfelső lapot, és hozzáadjuk az első játékos összegéhez
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
				//a buffer memseteljük mindig küldések és fogadások között
				//azaz "kitöröljük" a tartalmát, hogy ne legyen probléma
				memset(buffer, 0, SIZE);
				char *message = createString(Pakli[lapIndex][0],Pakli[lapIndex][1],message);
				memcpy(buffer, message, SIZE);
					
				//elküldjük az eredményt az első játékosnak
				send(player1_socket, buffer, SIZE, 0);
				//elküldjük az eredményt a másik játékosnak is
				memset(buffer, 0, 256);
				message = createStringMasikEredmeny(Pakli[lapIndex][0],Pakli[lapIndex][1],message);
				memcpy(buffer, message, strlen(message)+1);
				send(player2_socket, buffer, SIZE, 0);
			}
				
			//ebben az if-ben küldi a szerver a "kihúzott" lapjait a második kliensnek
			//nem lép be ha a második kliens passzolt, tehát nem kap új lapot
			//ugyanaz mint az előző if, csak player 2-re
			if(!voltPassz_player2 && player2_osszeg < 21){
				//"levesszük" a legfelső lapot, és hozzáadjuk az első játékos összegéhez
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
				//a buffer memseteljük mindig küldések és fogadások között
				//azaz "kitöröljük" a tartalmát, hogy ne legyen probléma
				memset(buffer, 0, SIZE);
				char *message = createString(Pakli[lapIndex][0],Pakli[lapIndex][1],message);
				memcpy(buffer, message, SIZE);
					
				//elküldjük az eredményt az első játékosnak
				send(player2_socket, buffer, SIZE, 0);
				//elküldjük az eredményt a másik játékosnak is
				memset(buffer, 0, 256);
				message = createStringMasikEredmeny(Pakli[lapIndex][0],Pakli[lapIndex][1],message);
				memcpy(buffer, message, strlen(message)+1);
				send(player1_socket, buffer, SIZE, 0);
			}
			else{
					
			}
				
			//ebben az ifben kéri a szerver az első játékost hogy mondja meg, kér-e még lapot vagy passzol
			//ha passzolt az első játékos, akkor nem kér tőle semmit már ebben a körben
			if(!voltPassz_player1 && player1_osszeg < 21){
				//elküldjük a kérést
				memset(buffer, 0, 256);
				char *messageKeres = createStringBekeres();
				memcpy(buffer, messageKeres, strlen(messageKeres)+1);
				send(player1_socket, buffer, SIZE, 0);
				
				//várjuk a választ
				memset(buffer, 0, 256);
				recv(player1_socket, buffer, SIZE,0);
				
				printf("%s \n", buffer);
				//feldolgozzuk a választ
				if(isPassMessage(buffer))
					voltPassz_player1 = 1;
				else if(isOkMessage(buffer))
					;
				else
					break; //ha valami nem olyan üzenetet kap amit kellene csak kilépünk a körből, nem kezeljük le
			}
				
			//ebben az ifben kéri a szerver a második játékost hogy mondja meg, kér-e még lapot vagy passzol
			//ha passzolt az első játékos, akkor nem kér tőle semmit már ebben a körben
			if(!voltPassz_player2 && player2_osszeg < 21){
				//elküldjük a kérést
				memset(buffer, 0, 256);
				char *messageKeres = createStringBekeres();
				memcpy(buffer, messageKeres, strlen(messageKeres)+1);
				send(player2_socket, buffer, SIZE, 0);
				
				//várjuk a választ
				memset(buffer, 0, 256);
				recv(player2_socket, buffer, SIZE,0);
				printf("%s \n", buffer);
				//feldolgozzuk a választ
				if(isPassMessage(buffer))
					voltPassz_player2 = 1;
				else if(isOkMessage(buffer))
					;
				else
					break; //ha valami nem olyan üzenetet kap amit kellene csak kilépünk a körből, nem kezeljük le
			}
			
		}
			
		//vége egy körnek, elküldük az eredményeket a játékosoknak, és kezdjük az új kört
		memset(buffer, 0, 256);
		int nyertes = eredmeny(player1_osszeg, player2_osszeg);
		char *message = eredmenyMessage(nyertes,player1_osszeg, player2_osszeg);
		memcpy(buffer, message, strlen(message)+1);
		
		//Eredmények elküldése
		send(player1_socket, buffer, SIZE, 0);
		send(player2_socket, buffer, SIZE, 0);
		
		memset(buffer, 0, 256);
	}


	return 0;
}

