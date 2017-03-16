#include "metronome.hpp"


void metronome:: start_timing(){
       reset();
       m_timer.start();
       m_timing = true;
}

void metronome:: set_bpm(int val){
    bpm = val;    
}

void metronome:: reset(){
    m_timer.reset();
    avg_beat_time = 0;
    prev_beat_time = 0;
    current_beat_time = 0;
    m_beat_count = 0;
}

void metronome::stop_timing(){
    m_timer.stop();
    m_timing = false;    
    if(m_beat_count >= 4){
        avg_beat_time /= --m_beat_count;
        bpm = (int)(60.0/avg_beat_time);
    }
    else bpm = 0;
        
}

void metronome::resetMinMax(){
    min_bpm = -1;
    max_bpm = -1;    
}

void metronome:: tap(){
    current_beat_time = m_timer.read();
    avg_beat_time += current_beat_time - prev_beat_time;
    prev_beat_time = current_beat_time;
    m_beat_count++;   
}