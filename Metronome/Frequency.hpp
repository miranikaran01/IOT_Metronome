#include "metronome.hpp"
class Frequency
{
public:
        Frequency(){
            frequency_object = M2MInterfaceFactory::create_object("3318");
            frequency_inst = frequency_object->create_object_instance();
            bpm = frequency_inst->create_dynamic_resource("5900", "BPM",M2MResourceInstance::INTEGER, true,false);
            bpm->set_operation(M2MBase::GET_PUT_ALLOWED);
            bpm->set_value_updated_function(value_updated_callback(this,&Frequency::value_updated_function));
            update_integer(bpm,65536);
            min_bpm = frequency_inst->create_dynamic_resource("5601", "MIN_BPM",M2MResourceInstance::INTEGER, true,false);
            update_integer(min_bpm,65536);
            min_bpm->set_operation(M2MBase::GET_ALLOWED);
            max_bpm = frequency_inst->create_dynamic_resource("5602", "MAX_BPM",M2MResourceInstance::INTEGER, true,false);
            update_integer(max_bpm,-1);
            max_bpm->set_operation(M2MBase::GET_ALLOWED);
            uint8_t buffer[] = {"BPM"};
            units = frequency_inst->create_static_resource("5701", "UNITS",M2MResourceInstance::STRING,buffer,sizeof(buffer),false);
            units->set_observable(true);
            reset = frequency_inst->create_dynamic_resource("5605","RESET_MIN_MAX",M2MResourceInstance::OPAQUE,true,false);
            reset->set_operation(M2MBase::POST_ALLOWED);
            uint8_t buffer_reset[14] = "RESET MIN MAX";
            reset->set_value(buffer_reset, 14);
            reset->set_execute_function(execute_callback(this,&Frequency::reset_min_max));               
        }
        
        M2MObject* get_object(){
            return frequency_object;
        }
        
        void update_integer(M2MResource* resource,int value){
            char buffer[20];
            resource->set_value((uint8_t*)buffer, sprintf(buffer,"%d",value));
        }
        
        void update_bpm(int val){
            update_integer(bpm,val);    
        }
        
        void update_max_bpm(int val){
            update_integer(max_bpm,val); 
        }
       
        void update_min_bpm(int val){
            update_integer(min_bpm,val); 
        }    
        
        void reset_min_max(void *argument){
            update_min_bpm(65536);
            update_max_bpm(-1);            
        }
        void value_updated_function(const char* name) {
            printf("value updated");
            int new_bpm = bpm->get_value_int();
            m->set_avg_beat_time(new_bpm);
            m->set_bpm(new_bpm);
            m->set_updated_bpm(true);
        }
        
        void set_metronome(metronome* metro){m = metro;}
        
private:
    M2MResource* bpm;
    M2MObject* frequency_object;
    M2MObjectInstance* frequency_inst;
    M2MResource* min_bpm;
    M2MResource* max_bpm;
    M2MResource* units;
    M2MResource* reset;
    metronome* m;
};