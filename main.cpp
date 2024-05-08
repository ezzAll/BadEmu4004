#include <iostream>
#include <vector>
//uint8_t pulse;
uint16_t programm[100]{
    0xD5, 0xB0, 0xA0, 0xB1, 0xA0, 0xF1, 0x83, 0xB3,
    0x1A, 0x0B, 0x62, 0xA1, 0xF8, 0x14, 0x12, 0xB1,
    0x40, 0x04, 0x00
    /*0xD1, //LDM устанавливаем аккум в 1 (0xD*)
    0xA5, //LD аккум в значение (0xA*)
    0xB1, //XCH обмен аккума и регистра (0xB*)
    0x20, 8, //FIM устанавливает значение на 2 регистра (20, число)
    0xF1, //Clc очистка переноса
    0xD1,
    0x84, //ADD сложение аккума и регистра (0x8*)
    0xFB, //DAA перенос из аккума в "ca" 1 если acc > 10
    0xF5 //RAL сдвиг в перенос*/
   /* 0x2E, 0x00, 15, //FIM устанавливаем регистр r0 в 1
    0x8F, 0x00, //ADD = добавляет к аккуму r0
    0xBF, 0x01,  //XCH утсанавливаем r1 в значение acc
    0xAF, 0x01, //LD, умтанавливает регистр в аккум
    0xFB, //daa устанавливаем перенос из аккума
    0xF1, //clc очищает перенос
    0xAF, 1,
    0xDF, 8,
    0xF5, //RAL сдвиг аккума и переноса влево, первый символ аккума заменяет перенос, а перкнос заменяет последний элемент
    0x00 //NOP*/
    
};
class cpu{
    private:
    uint16_t cp=0; //этап на котором идет ввполнение
    uint16_t acc=0; //acumulator
    uint16_t regs[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    uint16_t ca=0; //перенос
    uint16_t dm=0; //адрес памят
    uint16_t mem[2048]={0};
    uint8_t isfast=0; //временная переменная
    std::vector<uint16_t> stack; //jms адреса хранятся тут
    public:
    void tick(){
        for(uint16_t i=0; i<1000; i++){
            //printf("in work\n");
            if(cp>sizeof(programm)/sizeof(programm[0])-1){
                break;
            }
            cpuRun();
        }
        //std::cout<<ca<<"\n";
        std::cout<<"acc: "<<acc<<"\n";
        for(uint16_t i=0; i<16; i++){
            std::cout<<"reg"<<"["<<i<<"]: "<<regs[i]<<"\n";
        }
        for(uint16_t addr=0; addr<2048; addr++){
            if(mem[addr]>0){
                std::cout<<"mem of: "<<addr<<" "<<mem[addr]<<"\n";
            }
        }
        // std::cout<<regs[2]<<"\n";
        //std::cout<<regs[0]<<"\n";
        printf("Stopped\n");
        
    }
    void cpuRun(){
        //pulse=programm[cp]-0xB0;
        for(uint8_t i=0; i<0xF; i++){
            isfast=0;
            if(programm[cp]>=0xA0&&programm[cp]<=0xAF){ //LD аккум в значение (0xA*)
                acc=regs[programm[cp]-0xA0];
                break;
            }
            if(programm[cp]>=0xD0&&programm[cp]<=0xDF){ //LDM устанавливаем аккум в 1 (0xD*)
                acc=programm[cp]-0xD0;
                break;
            }
            if(programm[cp]>=0xB0&&programm[cp]<=0xBF){ //XCH обмен аккума и регистра (0xB*)
                /*isfast=regs[programm[cp]-0xB0];
                regs[programm[cp]-0xB0]=acc;
                //std::cout<<programm[cp]-0xB0<<"\n";
                acc=isfast;
                isfast=0;*/
                regs[programm[cp]-0xB0],acc=acc,regs[programm[cp]-0xB0]=acc;
                break;
            }
            if(programm[cp]>=0x20&&programm[cp]<=0x2E&&programm[cp]%2==0){ //FIM устанавливает значение на 2 регистра (20, число)
               // regs[programm[cp]-(0x20-1)]=programm[cp+1]-15;
                if(programm[cp+1]>=128){
                    regs[(programm[cp]-0x20)/2]+=8;
                    programm[cp+1]-=128;
                }
                if(programm[cp+1]>=64){
                    regs[(programm[cp]-0x20)/2]+=4;
                    programm[cp+1]-=64;
                }
                if(programm[cp+1]>=32){
                    regs[(programm[cp]-0x20)/2]+=2;
                    programm[cp+1]-=32;
                }
                if(programm[cp+1]>=16){
                    regs[(programm[cp]-0x20)/2]+=1;
                    programm[cp+1]-=16;
                }
                if(programm[cp+1]<=8){
                    regs[(programm[cp]-0x20)/2+1]=programm[cp+1];
                }
                cp+=1;
                break;
            }
            if(programm[cp]==0xF1){ //CLC очистка переноса
                ca=0;
                break;
            }
            if(programm[cp]>=0x80&&programm[cp]<=0x8F){ //ADD сложение аккума и регистра (0x8*)
                acc+=regs[programm[cp]-0x80]+ca;
                if(acc>=15){
                    ca=1;
                } else {

                }
                acc&=0xF;
                break;
            }
            if(programm[cp]>=0x90&&programm[cp]<=0x9F){ //SUB вычитание аккума и регистра (0x8*)
                acc-=regs[programm[cp]-0x80]+ca;
                ca=0;
                if(acc>15){
                    ca=1;
                }
                break;
            }
            if(programm[cp]==0xFB){ //daa перенос
                if(acc>10){
                    acc-=10;
                    ca=1;
                }
                break;
            }
            if(programm[cp]==0xF5){ //ral сдвиг
                acc=acc<<1;
                acc+=ca;
                if(acc>=16){
                    ca=1;
                    acc-=16;
                }
                break;
            }
            if(programm[cp]>0x10&&programm[cp]<0x1F){
                //stack.push_back(cp);
                if(programm[cp]==0x1C){ //an
                    if(acc>0){
                        cp=programm[cp+1];
                    } else{
                        cp+=1;
                    }
                    //cp+=1;
                    break;
            }
                if(programm[cp]==0x14){ //az
                    if(acc==0){
                        cp=programm[cp+1];
                    } else{
                        cp+=1;
                    }
                    //cp+=1;
                    break;
            }
                if(programm[cp]==0x1A){ //c0
                    if(ca==0){
                        cp=programm[cp+1];
                    } else{
                        cp+=1;
                    }
                    //cp+=1;
                    break;
            }
                if(programm[cp]==0x12){ //c1
                    if(ca==1){
                        cp=programm[cp+1];
                    } else{
                        cp+=1;
                    }
                    //cp+=1;
                    break;
                }
                break;
            }
            if(programm[cp]>=0x50&&programm[cp]<=0x5F){ //jms
                stack.push_back(cp);
                cp=programm[cp+1];
                break;
            }
            if(programm[cp]==0x40){ //jun 
                cp=programm[cp+1];
                //std::cout<<programm[cp+1]<<"\n";
                break;
            }
            if(programm[cp]>=0x20&&programm[cp]<=0x2F&&programm[cp]%2!=0){ //SRC сохраняет регистры, как адрес
                dm=(regs[programm[cp]-0x20-1]<<4)+regs[programm[cp]-0x20];
                break;
            }
            if(programm[cp]>=0x30&&programm[cp]<=0x3E){ //FIN извлекает из адреса(регистры 0 и 1) данные в два других регистра
                if(mem[(regs[0]<<4)+regs[1]]>=128){
                    mem[(regs[0]<<4)+regs[1]]-=128;
                    regs[programm[cp]-0x30]+=8;
                }
                if(mem[(regs[0]<<4)+regs[1]]>=64){
                    mem[(regs[0]<<4)+regs[1]]-=64;
                    regs[programm[cp]-0x30]+=4;
                }
                if(mem[(regs[0]<<4)+regs[1]]>=32){
                    mem[(regs[0]<<4)+regs[1]]-=32;
                    regs[programm[cp]-0x30]+=2;
                }
                if(mem[(regs[0]<<4)+regs[1]]>=16){
                    mem[(regs[0]<<4)+regs[1]]-=16;
                    regs[programm[cp]-0x30]+=1;
                }
                if(mem[(regs[0]<<4)+regs[1]]<=15){
                    regs[programm[cp]-0x30+1]=mem[(regs[0]<<4)+regs[1]];
                }
                break;
            }
            if(programm[cp]>=0x60&&programm[cp]<=0x6F){ //INC прибавление к регистру 1
                regs[programm[cp]-0x60]=(regs[programm[cp]-0x60]+1)&0XF;
                break;
            }
            if(programm[cp]>=0x70&&programm[cp]<=0x7F){ //ISZ прибавление к регистру 1, до того момента пока 4бит не будут 1111
                regs[programm[cp]-0x70]+=1;
                //std::cout<<regs[programm[cp]-0x70]<<"\n"<<"\n";
                if(regs[programm[cp]-0x70]>=16){
                    regs[programm[cp]-0x70]=0;
                    cp+=1;
                    break;
                } else{
                    cp=programm[cp+1];
                    break;
                }
                break;
            }
            if(programm[cp]>=0xC0&&programm[cp]<=0xCF){ //bbl загрузка в аккум числа и переход к stack
                acc=programm[cp]-0xC0;
                cp=stack[stack.size()-1]+1;
                break;
            }
            if(programm[cp]==0xE0){ //WRM записывает в определенный адресс (заданный src) значение в ОЗУ
                mem[dm]=acc;
                break;
            }
            if(programm[cp]==0xE8){ //SBM из аккума выситают память
                acc=acc-mem[dm]-ca;
                ca=acc>>4;
                break;
            }
            if(programm[cp]==0xE9){ //RDM
                acc=mem[dm];
                break;
            }
            if(programm[cp]==0xE9){ //ADM к аккуму память (если большн 15=перенос)
                acc+=mem[dm];
                if(acc>15){
                    acc-=15;
                    ca=1;
                }
                break; 
            }
            if(programm[cp]==0xF0){ //CLB очистка переноса и аккума
                acc=0;
                ca=0;
                break;
            }
            if(programm[cp]==0xF2){ //IAC прибавляет к аккуму 1
                acc+=1;
                break;
            }
            if(programm[cp]==0xF3){ //CMC перенос заменяется противоположным значением
                switch(ca){
                    case 0: ca=1; break;
                    case 1: ca=0; break;
                }
                break;
            }
            if(programm[cp]==0xF4){ //CMA аналогично CMC для аккума
                if(acc>=8){
                    acc-=8;
                } else {
                    isfast+=8;
                }
                if(acc>=4){
                    acc-=4;
                } else {
                    isfast+=4;
                }
                if(acc>=2){
                    acc-=2;
                } else {
                    isfast+=2;
                }
                if(acc>=1){
                    acc-=1;
                } else {
                    isfast+=1;
                }
                acc=isfast;
                isfast=0;
                break;
            }
            if(programm[cp]==0xF6){ //RAR аналогично RAL
                if(acc>=8){
                    isfast=1;
                }
                if(ca==1){
                    acc=acc<<1;
                    acc+=1;
                    ca=isfast;
                }
                isfast=0;
                break;
            }
            if(programm[cp]==0xF7){ //TTC
                switch(ca){
                    case 0: acc=0; break;
                    case 1: acc=1; break;
                }
                ca=0;
                break;
            }
            if(programm[cp]==0xF8){ //DAC
                acc=(acc-1)&0xF;
                if(acc!=15){
                    ca=1;
                } else {
                    ca=0;
                }
                break;
            }
            if(programm[cp]==0xF9){
                switch(ca){
                    case 0: acc=9; break;
                    case 1: acc=10; break;
                }
                ca=0;
                break;
            }
            if(programm[cp]==0xFA){ //STC ставит перенос
                ca=1;
                break;
            }
            if(programm[cp]==0xFB){ //DAA перенос = 1 если аккум > 9
                if(acc>9&&acc<=15){
                    ca=1;
                    acc+=6;
                }
                if(acc>=16){
                    acc=0;
                    ca=1;
                }
                break;
            }
            if(programm[cp]==0xFC){
                if(acc==4){
                    acc=3;
                } else if(acc==8){
                    acc=4;
                } else if(acc==0){
                    acc=0;
                } else {
                    acc=15;
                }
                break;
            }
            if(programm[cp]==0x00){
                break;
            }
            std::cout<<"error in "<<cp<<"\n";
    }
        
        for(uint8_t i=0; i<0xF; i++){
            if(regs[i]>15){
                regs[i]=0;
            }
        }
        cp+=1;
    }
};
int main() {
    cpu i4004;
    i4004.tick();
    system("pause");
    return 0;
}