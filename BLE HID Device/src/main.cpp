//need to add license 
#include <Arduino.h>
#include <bluefruit.h>
#include <Adafruit_LSM6DSOX.h>
#include <Bounce2.h>
#include <array>

BLEDis bledis;
BLEHidAdafruit blehid;

boolean gyro_connected= false;

Adafruit_LSM6DSOX sox;
sensors_event_t accel;
sensors_event_t gyro;
sensors_event_t temp;

const double averageX = -0.013;//gyro drift compensation 
const double averageY = 0;
const double averageZ = 0.013;
int8_t gx, gy;//calculated mouse movement from gyro sensor

const int NUM_BUTTONS = 4;
Bounce *buttons = new Bounce[NUM_BUTTONS];//set up 4 buttons with denouncing(left click, right click, scroll, dwell click toggle)
int BUTTON_PINS[NUM_BUTTONS] = { A2, A3, A4, A5};

//mouse control options 
boolean dwell = false;
boolean dwellActive = false;
unsigned long dwellTimer;
unsigned long dwellLength = 650;

bool button_active[NUM_BUTTONS] = {false, false, false, false};
bool long_press_active[NUM_BUTTONS] = {false, false, false, false};
unsigned long button_timer[4];
unsigned long long_press_time[4] = {1000000, 100000, 500, 1000000};
int8_t click_action[4][4] = {{1,0,0,0},{2,0,0,0},{0,0,0,0},{0,0,0,0}};//for mousereport function: button, x, y, scroll
//short_click_action and long_click_action are for buttons that act differently on short vs long press. Scroll button in our case
int8_t short_click_action[4][4] = {{0,0,0,0},{0,0,0,0},{0,0,0,-5},{0,0,0,0}};//for short press mousereport function: button, x, y, scroll
int8_t long_click_action[4][4] = {{0,0,0,0},{0,0,0,0},{0,0,0,1},{0,0,0,0}};//for long press mousereport function: button, x, y, scroll

//header functions located after setup and loop functions 
void dwell_click_control();
void mouse_click_control();
void startAdv(void);

void setup() {
  
  Serial.begin(115200);
  Bluefruit.begin();
  // HID Device can have a min connection interval of 9*1.25 = 11.25 ms
  Bluefruit.Periph.setConnInterval(9, 16); // min = 9*1.25=11.25 ms, max = 16*1.25=20ms
  Bluefruit.setTxPower(4);    // Check bluefruit.h for supported values
  //Bluefruit.setName("ICAN HID Device");
  Bluefruit.setName("ICAN HID Device");

  // Configure and Start Device Information Service
  bledis.setManufacturer("Adafruit Industries");
  bledis.setModel("Bluefruit Feather 52");
  bledis.begin();

  // BLE HID
  blehid.begin();

  // Set up and start advertising
  startAdv();

  if (!sox.begin_I2C()) {
    Serial.println("Failed to find IMU chip");
  }
  else gyro_connected = true;

  for (int i = 0; i < NUM_BUTTONS; i++)
  {
    buttons[i].attach(BUTTON_PINS[i], INPUT_PULLUP); //setup the bounce instance for the current button
    buttons[i].interval(30);                         // interval in ms
  }
  
  
}

void loop() {
 
 if (gyro_connected == true){
  // Get a new normalized sensor event 
  sox.getEvent(&accel, &gyro, &temp);
  
  gx = -((gyro.gyro.x - averageX) / 0.03);//change value as needed for mouse sensitivity
  gy = -((gyro.gyro.z - averageZ) / 0.03);//change value as needed for mouse sensitivity
  if{gx!=0 || gy!=0}{
    blehid.mouseMove(gx*10, gy*10);//change value as needed for mouse sensitivity
  }

  
 // Serial.println(gyro.gyro.x);
  //Serial.println(gyro.gyro.z - averageZ);
  //Serial.println(gy);
 // Serial.println("end");
  
  delay(5);//adjust the delay for smooth pointer movement 
 }
  for (int i = 0; i < NUM_BUTTONS; i++){
    // Update button state after debouncing
    buttons[i].update();

  }
  //check if dwell click has been toggled on and click if appropriate 
  if(dwell && gyro_connected == true) dwell_click_control();
  //check for left, right scroll and dwell toggle and perform appropriate action 
  mouse_click_control();

}


void dwell_click_control()
{//check if pointer has met movement and time conditions and that the scroll button isn't pressed
    if (-1 <= gx && gx <= 1 && -1 <= gy && gy <= 1 && buttons[3].read() == HIGH)
    {
        if (dwellActive == false)
        {
            dwellActive = true;
            dwellTimer = millis();
        }
        if ((millis() - dwellTimer > dwellLength))
        {
            blehid.mouseButtonPress(1);//left click 
            delay(50);//this delay can be adjusted if the click is not responding appropriately  
            blehid.mouseButtonRelease();
            dwellActive = false;
        }
    }
    else
    {
        dwellActive = false;
    }
    mouse_click_control();
}

void mouse_click_control(){
    
    for (int i = 0; i < NUM_BUTTONS; i++)
    {
        if(buttons[i].read() == LOW){
            if(button_active[i] == false){
                button_active[i]  = true;
                button_timer[i] = millis();
                //press left or right button and start timing the press for scroll button 
                 blehid.mouseReport(click_action[i][0], click_action[i][1],
                    click_action[i][2], click_action[i][3]);

                    Serial.println(i);
                    
            }
            if(millis() - button_timer[i] > long_press_time[i]){
                long_press_active[i] = true;
                blehid.mouseReport(long_click_action[i][0], long_click_action[i][1],
                long_click_action[i][2], long_click_action[i][3]);
            }

        }
        else if(buttons[i].read() == HIGH){//button is not pressed
            if(button_active[i]  == true){//button was pressed in the prior loop  
                if(long_press_active[i] == true){//button was long pressed in prior loop
                    long_press_active[i] = false;//set long press activated to false(i.e. Reset)
                }
                else{//execute short button press action
                  if(i != 3){//not the dwell click toggle switch 
                    blehid.mouseReport(short_click_action[i][0], short_click_action[i][1],
                    short_click_action[i][2], short_click_action[i][3]);
                  }
                  else dwell =!dwell;//togge dwell click or off
                  Serial.println("dwell"); 
                }
            }
            button_active[i]  = false;
        }
    }
    
} 

void startAdv(void)
{  
  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addAppearance(BLE_APPEARANCE_HID_MOUSE);
  
  // Include BLE HID service
  Bluefruit.Advertising.addService(blehid);

  // There is enough room for 'Name' in the advertising packet
  Bluefruit.Advertising.addName();
  
  /* Start Advertising
   * - Enable auto advertising if disconnected
   * - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
   * - Timeout for fast mode is 30 seconds
   * - Start(timeout) with timeout = 0 will advertise forever (until connected)
   * 
   * For recommended advertising interval
   * https://developer.apple.com/library/content/qa/qa1931/_index.html   
   */
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds  
}
