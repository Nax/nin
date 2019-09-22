#include <stdio.h>
#include <stdint.h>
#include <locale.h>
#include "ninconv.h"

#if defined(WIN32) || defined(_WIN32)
# define OS_WIN32 1
# include "dirent.h"
# include <direct.h>
#else
# define OS_UNIX 1
# include <dirent.h>
# include <sys/types.h>
# include <sys/stat.h>
#endif

static const NinDbGame kDatabase[] = {
#include "inc/db.inc"
};

static const NinDbGame* lookupCrc(uint32_t crc)
{
    size_t min;
    size_t max;
    size_t target;

    min = 0;
    max = (sizeof(kDatabase) / sizeof(*kDatabase));

    for (;;)
    {
        if (min == max)
            return NULL;

        target = min + (max - min) / 2;
        if (crc == kDatabase[target].crc32)
            return kDatabase + target;

        if (crc > kDatabase[target].crc32)
            min = target + 1;
        else
            max = target;
    }
}

static const NinDbGame* lookupGame(FILE* f)
{
    return lookupCrc(crc32(f));
}

static uint32_t popcnt32(uint32_t x)
{
    x -= ((x >> 1) & 0x55555555);
    x = (((x >> 2) & 0x33333333) + (x & 0x33333333));
    x = (((x >> 4) + x) & 0x0f0f0f0f);
    x += (x >> 8);
    x += (x >> 16);
    return x & 0x0000003f;
}

static uint32_t log2i(uint32_t x)
{
    x |= (x >> 1);
    x |= (x >> 2);
    x |= (x >> 4);
    x |= (x >> 8);
    x |= (x >> 16);

    return popcnt32(x);
}

static void writeHeader(FILE* f, const NinDbGame* game)
{
    uint8_t header[16];

    header[ 0] = 'N';
    header[ 1] = 'E';
    header[ 2] = 'S';
    header[ 3] = 0x1a;
    header[ 4] = (game->prgRomSize / 0x4000) & 0xff;
    header[ 5] = (game->chrRomSize / 0x2000) & 0xff;
    header[ 6] = (game->mirror ? 0x01 : 0x00) | (game->battery ? 0x02 : 0x00) | ((game->mapper & 0xf) << 4);
    header[ 7] = 0x08 | (game->mapper & 0xf0);
    header[ 8] = ((game->mapper & 0xf00) >> 8);
    header[ 9] = (((game->prgRomSize / 0x4000) >> 8) & 0x0f) | ((((game->chrRomSize / 0x2000) >> 8) & 0xf) << 4);
    header[10] = (log2i(game->prgRamSize / 64)) << (game->battery ? 4 : 0);
    header[11] = (log2i(game->chrRamSize / 64));
    header[12] = game->region;
    header[13] = 0;
    header[14] = 0;
    header[15] = 0;

    fwrite(header, 16, 1, f);
}

static void copyGame(FILE* out, FILE* f)
{
    char buffer[4096];
    size_t len;

    for (;;)
    {
        len = fread(buffer, 1, 4096, f);
        if (!len)
            return;
        fwrite(buffer, len, 1, out);
    }
}

static void convertGame(const char* path, const char* outDir)
{
    FILE* f;
    FILE* out;
    const NinDbGame* game;
    char buffer[2048];

    f = fopen(path, "rb");
    if (!f)
        return;
    fseek(f, 16, SEEK_SET);
    game = lookupGame(f);
    if (game)
    {
        printf("%s\n", game->name);
        strcpy(buffer, outDir);
        strcat(buffer, "/");
        strcat(buffer, game->name);
        strcat(buffer, ".nes");
        out = fopen(buffer, "wb");
        if (out)
        {
            writeHeader(out, game);
            copyGame(out, f);
            fclose(out);
        }
    }
    fclose(f);
}

static int massConv(const char* inDir, const char* outDir)
{
    DIR* input;
    struct dirent* ent;
    struct stat dummy;
    char buffer[2048];

    input = opendir(inDir);
    if (!input)
    {
        printf("Could not open input directory\n");
        return 1;
    }

#if OS_WIN32
    _mkdir(outDir);
#else
    mkdir(outDir, 0777);
#endif

    if (stat(outDir, &dummy))
    {
        printf("Could not create output directory\n");
        return 1;
    }

    for (;;)
    {
        ent = readdir(input);
        if (!ent)
            break;
        if (ent->d_name[0] == '.')
            continue;

        strcpy(buffer, inDir);
        strcat(buffer, "/");
        strcat(buffer, ent->d_name);

        convertGame(buffer, outDir);
    }

    closedir(input);
    return 0;
}

int main(int argc, char** argv)
{
#if OS_WIN32
    setlocale(LC_ALL, ".65001");
#endif
    if (argc < 3)
    {
        printf("usage: ninconv <indir> <outdir>\n");
        return 1;
    }

    return massConv(argv[1], argv[2]);
}
