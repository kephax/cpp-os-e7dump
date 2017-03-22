#ifndef swap_endian_h
#define swap_endian_h

template <class T>
T swap_endian(T toBeConverted)
{
    union {
        T realValue;
        char bytes[sizeof(T)];
    } input, output;

    input.realValue = toBeConverted;

    for ( unsigned int i = 0; i < sizeof(T); i++ ) {
        output.bytes[sizeof(T) - i - 1] = input.bytes[i];
    }

    return output.realValue;
}

#endif // swap_endian_h
