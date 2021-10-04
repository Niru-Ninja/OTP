#include <stdio.h>
#include <unistd.h>
#include <string>
#include <sstream>
#include <iomanip>
using namespace std;


void printHelp(int errCode, string text){
switch(errCode){
default:
    printf("\n ENCODE: -e\n");
    printf(" DECODE -d\n");
    printf(" KEY FILE: -k\n");
    printf(" RESULTING FILE : -r\n");
    printf("\n Example: otp -e <file to encode> -k <key file> -r <result file>\n");
break;
case 0:
    printf("\n\n ERROR: The command to encode/decode was used more than once. (-e/-d).\n\n");
break;
case 1:
    printf("\n\n ERROR: The path for the key file (-k) was not specified.\n\n");
break;
case 2:
    printf("\n\n ERROR: The path for the resulting file was not specified (-r).\n\n");
break;
case 3:
    printf("\n\n ERROR: The file \"%s\" could not be found or is inaccessible.\n\n",text.c_str());
break;
case 4:
    printf("\n\n ERROR: The file \"%s\" already exists.\n\n",text.c_str());
break;
case 5:
    printf("\n\n ERROR: The key file (-k) is too small to encode the file (-e).");
break;
}
return;
}

//UTILITY FUNCTIONS:
string charToHex(unsigned char car){
    string retMe;
    stringstream hexString;
    hexString << uppercase << hex << setw(2) << setfill('0') << (int)(unsigned char)car;
    hexString >> retMe;
    return retMe;
}

int hexToInt(char hexChar){
    int retMe = 0;
    switch(hexChar){
    case '0':
        retMe = 0;
    break;
    case '1':
        retMe = 1;
    break;
    case '2':
        retMe = 2;
    break;
    case '3':
        retMe = 3;
    break;
    case '4':
        retMe = 4;
    break;
    case '5':
        retMe = 5;
    break;
    case '6':
        retMe = 6;
    break;
    case '7':
        retMe = 7;
    break;
    case '8':
        retMe = 8;
    break;
    case '9':
        retMe = 9;
    break;
    case 'A':
        retMe = 10;
    break;
    case 'a':
        retMe = 10;
    break;
    case 'B':
        retMe = 11;
    break;
    case 'b':
        retMe = 11;
    break;
    case 'C':
        retMe = 12;
    break;
    case 'c':
        retMe = 12;
    break;
    case 'D':
        retMe = 13;
    break;
    case 'd':
        retMe = 13;
    break;
    case 'E':
        retMe = 14;
    break;
    case 'e':
        retMe = 14;
    break;
    case 'F':
        retMe = 15;
    break;
    case 'f':
        retMe = 15;
    break;
    }

    return retMe;
}

int charToDec(unsigned char byte){
int retMe;
    string hexStr;
    hexStr = charToHex(byte);
    retMe = 16*hexToInt(hexStr[0]) + hexToInt(hexStr[1]);
return retMe;
}

//=0 exists, =-1 doesnt exist.
int checkFile(string checkMe){
    return (access(checkMe.c_str(), F_OK));
}

//Returns size in bytes.
unsigned long long int filesize(FILE* fileToMeasure){
unsigned long long int retMe = 0;
fseek(fileToMeasure,0,SEEK_END);
retMe = ftell(fileToMeasure);
return retMe;
}


void doEncode(FILE* FileToWorkOn, FILE* KeyFile, FILE* OutputFile, unsigned long long int limit){
    char fileChar, keyChar;
    unsigned long long int index = 0;
    int keyNumber, fileNumber, outputNumber;

    fseek(FileToWorkOn,0,SEEK_SET);
    fseek(KeyFile,0,SEEK_SET);
    //We read the bytes of the file to be encrypted and of the key, we pass them to decimal: 
    while(index < limit){

        fread(&fileChar,1,1,FileToWorkOn);
        fileNumber = charToDec(fileChar);

        fread(&keyChar,1,1,KeyFile);
        keyNumber = charToDec(keyChar);

        //We generate the output byte and write it: 
        outputNumber = (fileNumber + keyNumber);
        if(outputNumber > 256) outputNumber -= 256;
        fprintf(OutputFile,"%c",outputNumber);

        index++;
    }
return;
}

void doDecode(FILE* FileToWorkOn, FILE* KeyFile, FILE* OutputFile, unsigned long long int limit){
    char fileChar, keyChar;
    unsigned long long int index = 0;
    int keyNumber, fileNumber, outputNumber;

    fseek(FileToWorkOn,0,SEEK_SET);
    fseek(KeyFile,0,SEEK_SET);

    while(index < limit){

        fread(&fileChar,1,1,FileToWorkOn);
        fileNumber = charToDec(fileChar);

        fread(&keyChar,1,1,KeyFile);
        keyNumber = charToDec(keyChar);

        //We generate the output byte and write it: 
        outputNumber = (fileNumber - keyNumber);
        if(outputNumber < 0) outputNumber += 256;
        fprintf(OutputFile,"%c",outputNumber);

        index++;
    }

return;
}

int main(int argc, char* argv[]){

    //MISCELLANEOUS VARIABLES :
    int counter = 1; //Word counter (argc).
    string STRFileToWorkOn = ""; //Route of the file to encode/decode.
    string STRKeyFile = ""; //Route of the key file.
    string STROutputName = ""; //Name of the final encoded/decoded file.
    int encode = 0;  //0: error. 1: there is an "-e". 2: there is an "-d".  => With this we know if encode/decode or if the user didnt specify anything.

    //LET'S PARSE THE USER COMMAND: 
    if(argc < 6){
        printHelp(-1,"");
        return 1;
    }

    while(counter < argc){
        if(string(argv[counter]) == "-e"){
            if(encode != 0) printHelp(0,"");
            encode = 1;
            STRFileToWorkOn = argv[counter+1];
        }
        if(string(argv[counter]) == "-d"){
            if(encode != 0) printHelp(0,"");
            encode = 2;
            STRFileToWorkOn = argv[counter+1];
        }
        if(string(argv[counter]) == "-k"){
            STRKeyFile = argv[counter+1];
        }
        else if(string(argv[counter]) == "-r"){
            STROutputName = argv[counter+1];
        }
        counter++;
   }

   //WE VERIFY THAT EVERYTHING THE USER TOLD US IS IN ORDER:
    if(STRKeyFile == "")printHelp(1,"");
    if(STROutputName == "")printHelp(2,"");

    if(checkFile(STRFileToWorkOn) == 0){
        FILE* FileToWorkOn = fopen(STRFileToWorkOn.c_str(), "rb");
        if(checkFile(STRKeyFile) == 0){
            FILE* KeyFile = fopen(STRKeyFile.c_str(), "rb");
            if(checkFile(STROutputName) != 0){
                FILE* OutputFile = fopen(STROutputName.c_str(), "wb");

                if(filesize(KeyFile) < filesize(FileToWorkOn)) printHelp(5,"");

                if(encode == 1) doEncode(FileToWorkOn, KeyFile, OutputFile, filesize(FileToWorkOn));
                if(encode == 2) doDecode(FileToWorkOn, KeyFile, OutputFile, filesize(FileToWorkOn));

                fclose(FileToWorkOn); fclose(KeyFile); fclose(OutputFile);
            }
            else{printHelp(4,STROutputName); return 1;}
        }
        else{printHelp(3,STRKeyFile); return 1;}
    }
    else{printHelp(3,STRFileToWorkOn); return 1;}

return 0;
}
