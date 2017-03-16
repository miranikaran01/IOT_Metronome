#include "mbed.h"
#include "utils.hpp"
#include "EthernetInterface.h"
#include "frdm_client.hpp"
#include "Frequency.hpp"
#include "metronome.hpp"

#define IOT_ENABLED
Serial pc(USBTX, USBRX);
namespace active_low
{
	const bool on = false;
	const bool off = true;
}

DigitalOut g_led_red(LED1);
DigitalOut g_led_green(LED2);
DigitalOut g_led_blue(LED3);

InterruptIn g_button_mode(SW3);
InterruptIn g_button_tap(SW2);



bool mode_pressed = false;
bool tapped = false;
bool learn = false;
bool play = true;
int min_bpm = 65536;
int max_bpm = -1;

metronome m = metronome();
Ticker tick;
Frequency freq;

void toggle(){
	learn = !learn;
	play = !play;
	mode_pressed = !mode_pressed;
}
void blinkGreen(){
	g_led_green = active_low::on;
	wait(0.1f);
	g_led_green = active_low::off;	
}

void on_mode()
{
    // Change modes
    mode_pressed = true;
    
}

void blinkRed(){
	g_led_red = active_low::on;
	wait(0.1f);
	g_led_red = active_low::off;
}

void update(){
	int current_bpm = (int)m.get_bpm();
	freq.update_bpm(current_bpm);
	if(current_bpm > max_bpm){
		max_bpm = current_bpm;
		freq.update_max_bpm(max_bpm);	
	}
	if(current_bpm < min_bpm){
		min_bpm = current_bpm;
		freq.update_min_bpm(min_bpm);	
	}
}

void on_tap()
{
    // Receive a tempo tap
    tapped = true;
}

void play_mode(){	
	float bc = m.get_avg_beat_time();
	pc.printf("avg beat interval = %f",bc);
	tick.attach(&blinkGreen,bc);		
}

void learn_mode(){
	tick.detach();
	m.start_timing();
	while(!mode_pressed){
		if(tapped){
			m.tap();
			tapped = !tapped;			
			blinkRed();
		}		
	}	
	m.stop_timing();
	update();
}


int main()
{
	
    unsigned seed = utils::entropy_seed();
    srand(seed);
	freq.set_metronome(&m);
	// LEDs are active LOW - true/1 means off, false/0 means on
	// Use the constants for easier reading
    g_led_red = active_low::off;
    g_led_green = active_low::off;
    g_led_blue = active_low::off;

	// Button falling edge is on push (rising is on release)
    

#ifdef IOT_ENABLED
	// Turn on the blue LED until connected to the network
    g_led_blue = active_low::on;

	// Need to be connected with Ethernet cable for success
    EthernetInterface ethernet;
    if (ethernet.connect() != 0)
        return 1;

	// Pair with the device connector
    
    frdm_client client("coap://api.connector.mbed.com:5684", &ethernet);
    if (client.get_state() == frdm_client::state::error)
        return 1;
	
	// The REST endpoints for this device
//	// Add your own M2MObjects to this list with push_back before client.connect()
    M2MObjectList objects;

    M2MDevice* device = frdm_client::make_device();
    objects.push_back(device);
    objects.push_back(freq.get_object());

	// Publish the RESTful endpoints
    client.connect(objects);

	// Connect complete; turn off blue LED forever
    g_led_blue = active_low::off;
#endif

	g_button_mode.fall(&on_mode);
	g_button_tap.fall(&on_tap);

    while (true)
    {
#ifdef IOT_ENABLED
        if (client.get_state() == frdm_client::state::error)
            break;
#endif		
		
		if(mode_pressed){
			toggle();
			if(play){
				play_mode();
			}			
			else if(learn){
				learn_mode();
			}
		}
		if(play && m.is_updated()){
			update();
			play_mode();
			m.set_updated_bpm(false);
		}  	
		
    }

#ifdef IOT_ENABLED
    client.disconnect();
#endif

    return 1;
}
