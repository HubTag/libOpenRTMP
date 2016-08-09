#include "../src/ieee754_double.c"

int main(){
    if(double_is_ieee754()){
        return 1;
    }
    return 0;
}
