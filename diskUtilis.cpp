#include "vfs.h"
char decToBinary(int n)
{
    char c;
    c = c & 0;
    // array to store binary number
    int binaryNum[8];

    // counter for binary array
    int i = 0;
    while (n > 0)
    {
        // storing remainder in binary array
        binaryNum[i] = n % 2;
        n = n / 2;
        i++;
    }

    // printing binary array in reverse order
    for (int j = i - 1; j >= 0; j--)
    {
        if (binaryNum[j] == 1)
            c = c | 1u << j;
    }
    return c;
};

int binaryToDec(char c)
{
    int out = 0;
    for (unsigned int i = 0; i < 8; i++)
    {

        int bit = (c >> i) & 1u; //right shift with 00000001
        if (bit == 1)
        {
            out += pow(2, i); //multyplay by his place
        }
    }
    return out;
};

void intToBinary(int n , char charArray[] ){
    int i;
    for (i = (sizeof(int)-1); i >= 0; i--){
        int isolatedByte = (n >> (8 * i)) & 255;//11111111
        charArray[i] = decToBinary(isolatedByte);

    }
};

int binaryToInt(char arrayChar[]){
    int a = 0 ; // 00000000 *4
    int i;
    for(i = sizeof(int) ; i > 0; i--) {
        a = (a << 8) | binaryToDec(arrayChar[i-1]);
    }
    return a;
};