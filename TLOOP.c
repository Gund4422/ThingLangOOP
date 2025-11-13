// ThingLang OOP, a superset of thinglang, by intiha. thinglang was
// made by Raya8877 at github.com.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 256
#define MAX_VARS 256
#define MAX_CLASSES 16
#define MAX_OBJECTS 32
#define MAX_FIELDS 32
#define MAX_METHODS 32
#define MAX_EVENTS 32
#define MAX_LOOP 1024

// -------------------- Variables --------------------
typedef struct {
    char name[64];
    char value[256];
} Variable;

Variable global_vars[MAX_VARS];
int var_count = 0;

void set_var(const char* name,const char* val){
    int i;
    for(i=0;i<var_count;i++){
        if(strcmp(global_vars[i].name,name)==0){
            strncpy(global_vars[i].value,val,256);
            return;
        }
    }
    strncpy(global_vars[var_count].name,name,64);
    strncpy(global_vars[var_count].value,val,256);
    var_count++;
}

char* get_var(const char* name){
    int i;
    for(i=0;i<var_count;i++){
        if(strcmp(global_vars[i].name,name)==0) return global_vars[i].value;
    }
    return NULL;
}

// -------------------- Events --------------------
typedef void (*EventCallback)();

typedef struct {
    char name[64];
    EventCallback callbacks[MAX_EVENTS];
    int count;
} Event;

Event events[MAX_EVENTS];
int event_count=0;

void add_event_callback(char* name, EventCallback cb){
    int i;
    for(i=0;i<event_count;i++){
        if(strcmp(events[i].name,name)==0){
            events[i].callbacks[events[i].count++]=cb;
            return;
        }
    }
    strncpy(events[event_count].name,name,64);
    events[event_count].callbacks[0]=cb;
    events[event_count].count=1;
    event_count++;
}

void trigger_event(char* name){
    int i,j;
    for(i=0;i<event_count;i++){
        if(strcmp(events[i].name,name)==0){
            for(j=0;j<events[i].count;j++){
                events[i].callbacks[j]();
            }
        }
    }
}

// -------------------- Bytecode --------------------
enum {
    OP_SET_VAR=0x01,
    OP_PRINT=0x02,
    OP_CALL_EVENT=0x03,
    OP_LOOP=0x04,
    OP_BLOCK_END=0xFF
};

void write_byte(FILE* f,unsigned char b){ fwrite(&b,1,1,f);}
void write_string(FILE* f,char* s){
    unsigned char len=(unsigned char)strlen(s);
    write_byte(f,len);
    fwrite(s,1,len,f);
}

// -------------------- Compiler --------------------
void compile_line(FILE* out,char* line){
    if(line[0]=='!' || line[0]=='\0') return;

    if(strncmp(line,"THIS ",5)==0){
        char var[64], val[256];
        if(sscanf(line,"THIS %s IS %s",var,val)==2){
            write_byte(out,OP_SET_VAR);
            write_string(out,var);
            write_string(out,val);
        }
    } else if(strncmp(line,"PRINT ",6)==0){
        char val[256];
        sscanf(line,"PRINT %s",val);
        write_byte(out,OP_PRINT);
        write_string(out,val);
    } else if(strncmp(line,"TRIGGER ",8)==0){
        char val[256];
        sscanf(line,"TRIGGER %s",val);
        write_byte(out,OP_CALL_EVENT);
        write_string(out,val);
    } else if(strncmp(line,"NOTAGAIN ",9)==0){
        int times;
        sscanf(line,"NOTAGAIN %d",&times);
        write_byte(out,OP_LOOP);
        write_byte(out,(unsigned char)times);
    }
}

// -------------------- Runner --------------------
void run_text(FILE* f){
    char line[MAX_LINE];
    int loop_count=0;
    char loop_lines[MAX_LOOP][MAX_LINE];
    int loop_index=0;
    int in_loop=0;

    while(fgets(line,MAX_LINE,f)){
        line[strcspn(line,"\n")]=0;
        if(line[0]=='!' || line[0]=='\0') continue;

        if(strncmp(line,"NOTAGAIN ",9)==0){
            in_loop=1;
            loop_index=0;
            sscanf(line,"NOTAGAIN %d",&loop_count);
        } else if(strncmp(line,"TIMESEND",8)==0){
            int i,j;
            for(i=0;i<loop_count;i++){
                for(j=0;j<loop_index;j++){
                    char* l=loop_lines[j];
                    if(strncmp(l,"THIS ",5)==0){
                        char var[64], val[256];
                        if(sscanf(l,"THIS %s IS %s",var,val)==2)
                            set_var(var,val);
                    } else if(strncmp(l,"PRINT ",6)==0){
                        char val[256];
                        sscanf(l,"PRINT %s",val);
                        char* v=get_var(val);
                        if(v) printf("%s\n",v);
                        else printf("%s\n",val);
                    } else if(strncmp(l,"TRIGGER ",8)==0){
                        char val[256];
                        sscanf(l,"TRIGGER %s",val);
                        trigger_event(val);
                    }
                }
            }
            in_loop=0;
        } else {
            if(in_loop){
                strcpy(loop_lines[loop_index++],line);
            } else {
                if(strncmp(line,"THIS ",5)==0){
                    char var[64], val[256];
                    if(sscanf(line,"THIS %s IS %s",var,val)==2)
                        set_var(var,val);
                } else if(strncmp(line,"PRINT ",6)==0){
                    char val[256];
                    sscanf(line,"PRINT %s",val);
                    char* v=get_var(val);
                    if(v) printf("%s\n",v);
                    else printf("%s\n",val);
                } else if(strncmp(line,"TRIGGER ",8)==0){
                    char val[256];
                    sscanf(line,"TRIGGER %s",val);
                    trigger_event(val);
                }
            }
        }
    }
}

void run_bytecode(FILE* f){
    unsigned char opcode;
    while(fread(&opcode,1,1,f)==1){
        if(opcode==OP_SET_VAR){
            unsigned char len;
            char var[64], val[256];
            fread(&len,1,1,f); fread(var,1,len,f); var[len]=0;
            fread(&len,1,1,f); fread(val,1,len,f); val[len]=0;
            set_var(var,val);
        } else if(opcode==OP_PRINT){
            unsigned char len;
            char val[256];
            fread(&len,1,1,f); fread(val,1,len,f); val[len]=0;
            char* v=get_var(val);
            if(v) printf("%s\n",v);
            else printf("%s\n",val);
        } else if(opcode==OP_CALL_EVENT){
            unsigned char len;
            char val[256];
            fread(&len,1,1,f); fread(val,1,len,f); val[len]=0;
            trigger_event(val);
        } else if(opcode==OP_LOOP){
            unsigned char times;
            fread(&times,1,1,f);
            // For simplicity, loops in bytecode will execute next instruction X times
        } else if(opcode==OP_BLOCK_END) return;
    }
}

// -------------------- Main --------------------
int main(int argc,char** argv){
    if(argc<2){ printf("Usage: -compile=file.tlo | -run=file.tlo/.tloc\n"); return 1;}

    for(int i=1;i<argc;i++){
        if(strncmp(argv[i],"-compile=",9)==0){
            char* infile=argv[i]+9;
            char outfile[512];
            snprintf(outfile,512,"%s.tloc",infile);
            FILE* in=fopen(infile,"r");
            FILE* out=fopen(outfile,"wb");
            if(!in || !out){ printf("File error\n"); return 1;}
            char line[MAX_LINE];
            while(fgets(line,MAX_LINE,in)){
                line[strcspn(line,"\n")]=0;
                compile_line(out,line);
            }
            fclose(in); fclose(out);
            printf("Compiled to %s\n",outfile);
        } else if(strncmp(argv[i],"-run=",5)==0){
            char* file=argv[i]+5;
            FILE* f=fopen(file,"rb");
            if(!f){ printf("File error\n"); return 1;}
            add_event_callback("LowHealth",&low_health); // sample event
            char* dot=strrchr(file,'.');
            if(dot && strcmp(dot,".tlo")==0) run_text(f);
            else if(dot && strcmp(dot,".tloc")==0) run_bytecode(f);
            else printf("Unknown file type\n");
            fclose(f);
        }
    }
    return 0;
}
