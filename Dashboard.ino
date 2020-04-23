
int rpm_raw = 0;
int rpm = 0;
int rpm_old = 0;

int temp_raw = 0;
int temp = 0;
int temp_old = 0;
int oel_raw = 0;
int oel = 0;
int oel_old = 0;
int volt_raw = 0;
int volt = 0;
int volt_old = 0;

int fuel_raw = 0;
int fuel = 0;
int fuel_old = 0;

#define filterSamples   9              // filterSamples should  be an odd number, no smaller than 3
int sensSmoothArray1 [filterSamples];   // array for rpm
int sensSmoothArray2 [filterSamples];   // array for temp
int sensSmoothArray3 [filterSamples];   // array for oel
int sensSmoothArray4 [filterSamples];   // array for volt
int sensSmoothArray5 [filterSamples];   // array for fuel

// Umdrehung Motor

unsigned long last=0;
unsigned long zeit=30000;
unsigned long zeituebergabe = 15000000;
int MotorPIN = 2;


void setup() {
  
Serial.begin(115200);
pinMode(MotorPIN, INPUT); // Digital pin als Eingang definieren
digitalWrite(MotorPIN,HIGH);

for (int i = 0; i <= 10; i++) {
    rpm = i*18  ; // 0 - 8000
    temp = i*9+45 ;//316 -26
    oel = i*9+45;
    volt = i*9+45 ;
    fuel = i*9+45;
    delay (50);
    sendData();
  }
for (int i = 10; i >= 0; i--) {
    rpm = i*18  ; // 0 - 8000
    temp = i*9+45 ;//316 -26
    oel = i*9+45;
    volt = i*9+45 ;
    fuel = i*9+45;
     delay (50);
    sendData();
  }
attachInterrupt(0, Motor, FALLING);
}

void loop() {

rpm_raw = digitalSmooth(15000000/zeituebergabe, sensSmoothArray1);
temp_raw = digitalSmooth(analogRead(4), sensSmoothArray2) ; 
oel_raw = digitalSmooth(analogRead(1), sensSmoothArray3) ; 
volt_raw = digitalSmooth(analogRead(5), sensSmoothArray4) ; 
fuel_raw = digitalSmooth(analogRead(2), sensSmoothArray5) ; 

mapData();
sendData();
delay (100);
}

int digitalSmooth(int rawIn, int *sensSmoothArray) {    // "int *sensSmoothArray" passes an array to the function - the asterisk indicates the array name is a pointer
  int j, k, temp, top, bottom;
  long total;
  static int i;
  // static int raw[filterSamples];
  static int sorted[filterSamples];
  boolean done;

  i = (i + 1) % filterSamples;    // increment counter and roll over if necc. -  % (modulo operator) rolls over variable
  sensSmoothArray[i] = rawIn;                 // input new data into the oldest slot

  // Serial.print("raw = ");

  for (j = 0; j < filterSamples; j++) { // transfer data array into anther array for sorting and averaging
    sorted[j] = sensSmoothArray[j];
  }

  done = 0;                // flag to know when we're done sorting
  while (done != 1) {      // simple swap sort, sorts numbers from lowest to highest
    done = 1;
    for (j = 0; j < (filterSamples - 1); j++) {
      if (sorted[j] > sorted[j + 1]) {    // numbers are out of order - swap
        temp = sorted[j + 1];
        sorted [j + 1] =  sorted[j] ;
        sorted [j] = temp;
        done = 0;
      }
    }
  }
  // throw out top and bottom 15% of samples - limit to throw out at least one from top and bottom
  bottom = max(((filterSamples * 15)  / 100), 1);
  top = min((((filterSamples * 85) / 100) + 1  ), (filterSamples - 1));   // the + 1 is to make up for asymmetry caused by integer rounding
  k = 0;
  total = 0;
  for ( j = bottom; j < top; j++) {
    total += sorted[j];  // total remaining indices
    k++;
  }

  return total / k;    // divide by number of samples
}



void Motor(){ 
      detachInterrupt(0);  // Interrupt ausschalten damit er uns nicht beißt
      unsigned long m = micros();                 // Microsekundenzähler auslesen
      unsigned long v = m - last;        // Differenz zum letzten Durchlauf berechnen
      if (v > 3000 ) {        // ignorieren wenn <= 3 ms (Kontaktpreller)
      zeit = v;                                // Wert in dauer übernehmen
      last = m;         // und wieder den letzten Wert merken
       zeituebergabe = zeit;    
      }  
      
      attachInterrupt(0, Motor, FALLING );   
     // Interrupt wieder einschalten.
   }

 void mapData() {

  if (rpm_raw <= 1333) {  rpm =map( rpm_raw ,0,1333,315,360);}
else
{rpm =map( rpm_raw ,1334,8000,0,225);}

if (temp_raw >= 298) {temp_raw = 297;}
temp =949.72*( pow (temp_raw,-0.539));

if (oel_raw >= 196) {oel_raw = 195;}
oel = 168.81 * pow (exp(1),oel_raw*(-0.0066));

if (volt_raw <= 329) {volt_raw = 329 ;} 
volt =map( (volt_raw) ,329,657,45,135);  // 0.02431611V  pro 1/1024


if (fuel_raw >= 196) {fuel_raw = 195;}
fuel = 168.81 * pow (exp(1),fuel_raw*(-0.0066));

 }


 void sendData(){  

if (rpm != rpm_old) {
Serial.print("rpm.val=");
Serial.print(rpm);
Serial.write(0xff);
Serial.write(0xff);
Serial.write(0xff);
rpm_old= rpm;
}


if (temp != temp_old) {
Serial.print("temp.val=");
Serial.print(temp);
Serial.write(0xff);
Serial.write(0xff);
Serial.write(0xff);
temp_old= temp;

}

if (oel != oel_old) {
Serial.print("oel.val=");
Serial.print(oel);
Serial.write(0xff);
Serial.write(0xff);
Serial.write(0xff);
oel_old= oel;
}


if (volt != volt_old) {
Serial.print("volt.val=");
Serial.print(volt);
Serial.write(0xff);
Serial.write(0xff);
Serial.write(0xff);
volt_old= volt;
}

if (fuel != fuel_old) {
Serial.print("fuel.val=");
Serial.print(fuel);
Serial.write(0xff);
Serial.write(0xff);
Serial.write(0xff);
fuel_old= fuel;

}
 }
 
