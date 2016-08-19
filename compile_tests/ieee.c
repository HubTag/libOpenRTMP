/* CMake Test File
 * Description : native IEEE754 doubles
 * Defines : RTMP_ASSUME_IEEE_DOUBLES
 * Must Compile : True
 */

#include "../src/ieee754_double.c"

int main(){
    if(double_is_ieee754()){
        return 1;
    }
    return 0;
}
