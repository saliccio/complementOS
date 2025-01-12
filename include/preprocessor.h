#define CONCAT_2(a, b) a##b
#define CONCAT_3(a, b, c) a##b##c
#define CONCAT_4(a, b, c, d) a##b##c##d

#define STRINGIFY(x) #x

#define CONCAT_AND_STRINGIFY(a, b) STRINGIFY(CONCAT(a, b))