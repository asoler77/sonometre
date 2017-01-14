/****************************************
Example Sound Level Sketch for the
Adafruit Microphone Amplifier
****************************************/

#define Nconst 50 // maxim 800
#define Mconst 100
#define Npote 20 // maxim 800
const int sampleWindow = 100; // Sample window width in mS (50 mS = 20Hz)
unsigned int sample;
int buffer[Nconst];
int superbuffer[Mconst];
//int nivell_vermell=230;
//int nivell_taronja=170;
int nivell_vermell_b[Npote];
int nivell_taronja_b[Npote];
int release_time_b[Npote];
int nivell_vermell=0;
int nivell_taronja=0;
int release_time=0;
String stringtosend;


void setup()
{
	Serial.begin(9600);
	pinMode(5, OUTPUT);
	pinMode(6, OUTPUT);
	pinMode(7, OUTPUT);
	digitalWrite(6, LOW);
	digitalWrite(5, HIGH);
	digitalWrite(7, HIGH);
}

void loop()
{
	unsigned long startMillis= millis();  // Start of sample window
	unsigned int peakToPeak = 0;   // peak-to-peak level

	unsigned int signalMax = 0;
	unsigned int signalMin = 1024;

	//static int N = 150;	// Número de mostres de la mitjana. Maxim 800
	static int i=0;
  static int k=0;
  static int pi=0;
	static int N=Nconst;
  static int M=Mconst;
  static int Mold=Mconst;
  static int Np=Npote;
	static double mean = 0;	// mitjana promitjada en N mostres de sampleWindow ms.
  static double supermean = 0;
	static int state = 0;	// 2 vermell, 1 taronja, 0 verd
	static unsigned long counter = 0;
	//static nivell

	// a 50ms fa unes pren unes 445 mostres, 8,9KHz de mostreig.
	while (millis() - startMillis < sampleWindow)
	{
		sample = analogRead(4);

		if (sample < 1024)  // toss out spurious readings
		{
			if (sample > signalMax)
			{
				signalMax = sample;  // save just the max levels
			}
			else if (sample < signalMin)
			{
				signalMin = sample;  // save just the min levels
			}
		}
	}

	peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
	int volts = peakToPeak;  // convert to volts

	buffer[i] = volts;

  // Aquí podem ajustar el temps d'entrada per la supermean, amb el 8.
  // Compte que M no pot passar de Mconst = 100
  // M=2 10 segons a arribar a la mitjana
  // M=10 40 segons a arribar a la mitjana
  // Ho posem perquè al màxim release_time, 1024, hi hagi una mitjana de 100.
  // Si es vol canviar la relació entrada sortida s'ha de tocar aquest 11. Ara mateix un exemple seria 20 segons entrar, 30 a sortir.
  M = floor(release_time/11)+1;
  // Això evitaria que si es baixa de 11 la constant divisora, quan estigui al màxim el release time, M vagi a mes de 100:
  if(M>100){
      M=100;
  }
  if(M!=Mold){
    supermean=0;
    i=0;
    k=0;
    memset(superbuffer, 0, sizeof(superbuffer));
    Mold=M;
  }

	i=i+1;
	if(i==N){
	    i=0;
      superbuffer[k]=mean;
      supermean = 0;
      for(int l=0;l<=M;l++){
          supermean = supermean + superbuffer[l];
      }
      supermean = supermean/(M);
      k=k+1;
      if(k==M){k=0;}
	}

	//for(int j=0;j<N-1;j++){if(buffer[j]>max0){max0=buffer[j];}}
	mean = 0;
	for(int j=0;j<N-1;j++){mean = mean + buffer[j];}
	mean = mean/(N-1);

  nivell_taronja_b[pi] = analogRead(2);
  nivell_vermell_b[pi] = analogRead(0);

  // Aquí podem ajustar la latència genearl tant d'entrada com de sotida, amb el 1.
  release_time_b[pi] = 1*analogRead(1);

  nivell_taronja = 0;
  for(int j=0;j<Np-1;j++){nivell_taronja = nivell_taronja + nivell_taronja_b[j];}
  nivell_taronja = nivell_taronja/(Np-1);

  nivell_vermell = 0;
  for(int j=0;j<Np-1;j++){nivell_vermell = nivell_vermell + nivell_vermell_b[j];}
  nivell_vermell = nivell_vermell/(Np-1);

  release_time = 0;
  for(int j=0;j<Np-1;j++){release_time = release_time + release_time_b[j];}
  release_time = release_time/(Np-1);

  pi=pi+1;
  if(pi==Np){pi=0;}

	if(supermean>nivell_vermell){	// Nivell de la mitjana per passar a vermell
		digitalWrite(6, HIGH);
		digitalWrite(5, HIGH);
		digitalWrite(7, LOW);
		state = 2;
		counter = millis(); // iniciem el contador per reduir l'estat
	}
	else{
		if(supermean>nivell_taronja && state!=2){ // Nivell de la mitjana per passar a taronja
            digitalWrite(6, HIGH);
            digitalWrite(5, LOW);
            digitalWrite(7, HIGH);
            state = 1;
            counter = millis(); // iniciem el contador per reduir l'estat
		}
	}

	if(state!=0){	// Si hem canviat a taronja o vermell

		if((millis() - counter)/1000 > release_time){ // Temps en segons per baixar el semàfor.
			if(state==1){ // si estavem a taronja i ha passat el temps paseem a verd.
				state=0;
				digitalWrite(6, LOW);
				digitalWrite(5, HIGH);
				digitalWrite(7, HIGH);
			}else	if(state==2){ // si estavem en vermell i ha passat el temps passem a taronja i reinciem el counter.
				state=1;
				digitalWrite(6, HIGH);
				digitalWrite(5, LOW);
				digitalWrite(7, HIGH);
				counter = millis();
			}
		}
	}

	//Serial.println(mean);
	stringtosend = String(supermean) + ":" + String(mean) + ":" + String(nivell_taronja) + ":" + String(nivell_vermell) + ":" + String(release_time);
	Serial.println(stringtosend);

}
