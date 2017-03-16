#pragma once

#include "mbed.h"



class metronome
{


public:
    metronome()
    : m_timing(false), bpm(65535) {updated_bpm=false,m_beat_count=0,current_beat_time=0.0,prev_beat_time=0,avg_beat_time=0;}
    ~metronome() {}

public:
	// Call when entering "learn" mode
    void start_timing();
	// Call when leaving "learn" mode
    void stop_timing();
	void resetMinMax();
	int get_minBpm() const{return min_bpm;}
	int get_maxBpm() const{return max_bpm;}	
	// Should only record the current time when timing
	// Insert the time at the next free position of m_beats
    void tap();

    float get_avg_beat_time() const{return avg_beat_time;}
    void  set_avg_beat_time(int val){avg_beat_time = 60/val;}
   	
   	bool is_updated() const{return updated_bpm;}
	// Return 0 if there are not enough samples
    float get_bpm() const{	
    	return bpm;
    }
    
    void set_bpm(int val);
    void set_updated_bpm(bool val){updated_bpm = val;}

private:
    bool m_timing;
    Timer m_timer;
	int min_bpm;
	int max_bpm;
	void reset();
    int m_beat_count;
    int bpm;    
    float current_beat_time;
    float prev_beat_time;
    float avg_beat_time;
    bool updated_bpm;
};
