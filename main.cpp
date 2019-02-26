#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
using namespace std;

#define ERROR_INVALID_ARG -1
#define ERROR_INVALID_OP  -2
#define ERROR_INVALID_SYN -3
#define ERROR_OPEN_FILE   -4
#define OPERATION_SUCCESS 0

//https://github.com/timothychan29475600/base64_encode_decode

const char base64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
FILE *input01, *output01;
FILE *input02, *output02;
int encode64(FILE *input_file, FILE* output_file);
int decode64(FILE *input_file, FILE* output_file);

void pause()
{
    printf("Press Enter key to continue...");
    fgetc(stdin);
}
int main(int argc, char* argv[])
{
    input01 = fopen("input.png","rb");
    output01 = fopen("output.txt","w");
    encode64(input01,output01);
    fclose(input01);
    fclose(output01);
    input02 = fopen("output.txt","rb");
    output02 = fopen("output.png","wb");
    decode64(input02,output02);
    pause();
    return 0;
}

int encode64(FILE* input_file, FILE* output_file){
    //Variables Declaration
    int decode_buffer = 0;
    int byteCount = 1; int temp;
    char raw_byte = 0; char out_byte = 0;

    //Read  and encode
    while(fread(&raw_byte,sizeof(char),1,input_file)){
        temp = raw_byte & ~(-1<<8);
        decode_buffer = (decode_buffer << 8) + temp;

        if(byteCount % 3 == 0){ //Every 3 byte -> 4 base64 char
            for(temp = 0; temp < 4; temp++){
                out_byte = base64_table[(decode_buffer>>(6*(3-temp)) & 0b00111111)];
                fprintf(output_file,"%c",out_byte);
            }
            decode_buffer = 0;
        }
        byteCount ++;
    }

    if(byteCount % 3){ //If padding is needed
        for(temp = 0;temp < byteCount %3; temp++)
            decode_buffer = (decode_buffer << 8) ; //Fill the missing 1 or 2 byte

        for(temp = 0; temp < 4-(byteCount%3); temp++){ //If fill 1 byte -> Get 3 char; if fill 2 byte -> get 2 char => 4-fill_no
            out_byte = base64_table[(decode_buffer>>(6*(3-temp)) & 0b00111111)];
            fprintf(output_file,"%c",out_byte);
        }
        for(temp = 0;temp < byteCount %3; temp++)
            fprintf(output_file,"=");
    }

    return OPERATION_SUCCESS;
}

int decode64(FILE* input_file, FILE* output_file){
    int decode_buffer = 0;
    int byteCount = 1; int paddingCount = 0; int i;
    char raw_byte=0; char resolved_byte = 0; unsigned char temp;

    //Read and decode
    while(fread(&raw_byte,sizeof(char),1,input_file)){

        if(raw_byte == '='){ //If a padding is encountered
            raw_byte = 'A'; //Turn it into 0
            paddingCount ++;
        }

        //Decode the character
        char* pt = strchr(base64_table,(unsigned char)raw_byte);

        if(!pt){//Unable to resolve character
            return ERROR_INVALID_SYN;
        }

        resolved_byte = pt-base64_table;
        resolved_byte = resolved_byte & ~(-1<<6); //Mask the byte to ensure all the bit except the last 6 is zero
        decode_buffer = (decode_buffer<<6) + resolved_byte;

        if(byteCount%4 == 0){ //Every 4 base64 char -> 3 byte
            for(i=0;i<3-paddingCount;i++){
                temp = (decode_buffer>>(8*(2-i))) & ~(-1<<8);

                fwrite(&temp,sizeof(char),1,output_file);
            }

            decode_buffer = 0;
        }

        byteCount++;
    }

    return OPERATION_SUCCESS;
}
