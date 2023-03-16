//RECEIVER NodeMCU

//library buat nrf dan esp8266
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <ESP8266WiFi.h>

//variabel untuk mengoneksi esp8266 dengan internet (hotspot)
const char* ssid = "Galaxy A71F588";
const char* password = "iloveyou";

//fungsi untuk memasuki esp/arduino dalam pinout-nya
RF24 radio(D4, D2);                       // CE, CSN (buat esp apke D4, D2; arduino langsung pin)

//variabel untuk kode unik 2 nrf, dipastikan untuk ke 2 mikrokontroler sama 
const byte address[6] = "00001";

void setup() {
  Serial.begin(9600);                   //memanggil fungsi dari Serial.begin(baud)
  radio.begin();                          //memanggil fungsi pada nrf
  //WiFi.mode(WIFI_OFF);
  WiFi.mode(WIFI_STA);                    //membaca wifi ke hotspot atau station
  WiFi.begin(ssid, password);             // memanggil fungsi 
  Serial.println("\nConnecting to WiFi");
  
  while (WiFi.status() != WL_CONNECTED) { //keadaan saat WiFi terhubung maka akan menampilkan "." di serial monitor
    Serial.print(".");
    delay(1000);
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
  radio.openReadingPipe(0, address);      // memasukkan address pada nrf agar terhubung
  radio.setPALevel(RF24_PA_MIN);          //PA level untuk mengukur kuat PA pada nrf (RF24_PA_MIN untuk jarak dekat)
  radio.startListening();                 // mikrokontroller akan menjadi Receiver dan mulai membaca data yang dikirim
}

void loop() {
  if (radio.available()) {                // Fungsi ketika nrf sudah terkoneksi
    char text[32] = "";                   // nrf sendiri hanya bisa mengirim data berbentuk char dan hanya dibatasi 32 bit
    radio.read(&text, sizeof(text));      // radio.read(&void, sizeof(sub void))

    String stringOne = String(text);      // Mengubah tipe data dari char ke string
    Serial.println(stringOne);

    int cur1 = stringOne.indexOf("!");    // indexOf berfungsi untuk menemukan sebuah tanda atau huruf sebagai penanda
    int cur2 = stringOne.indexOf("@");
    int cur3 = stringOne.indexOf("#");
    int cur4 = stringOne.indexOf("$");
    int cur5 = stringOne.indexOf("%");

    String Temperature = stringOne.substring(0, cur1);          //substring digunakan untuk mengambil string sesuai yang diinginkan 
    String Current     = stringOne.substring(cur1 + 1, cur2);
    String Voltage     = stringOne.substring(cur2 + 1, cur3);
    String Power       = stringOne.substring(cur3 + 1, cur4);
    String pwm         = stringOne.substring(cur4 + 1, cur5);

    float temperature = Temperature.toFloat();
    float current     = Current.toFloat();
    float voltage     = Voltage.toFloat();
    float power       = Power.toFloat();
    int PWM           = pwm.toInt();

    /*int suhu = Suhu.toInt();
      int arus = Arus.toInt();
      int tegangan = Tegangan.toInt();
      int daya = Daya.toInt();*/

    Serial.println("========================================");

    Serial.println("---- Data terkirim ke WEBSITE ----");
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print(", Current:");
    Serial.print(current);
    Serial.print(", Power:");
    Serial.print(power);
    Serial.print(", Voltage:");
    Serial.print(voltage);
    Serial.print(", PWM:");
    Serial.println(PWM);
  }
  delay(1000);
}
